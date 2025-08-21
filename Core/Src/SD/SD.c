#include "SD.h"
#include "semphr.h"
#include "LOG.h"

extern SPI_HandleTypeDef hspi3;
extern osSemaphoreId_t spi3_semHandle;
extern HAL_StatusTypeDef SPI3_TransmitReceive_DMA(uint8_t*, uint8_t*, uint16_t, uint32_t);

// Линейка для вызова DMA и ожидания завершения
static HAL_StatusTypeDef SD_DMA_TXRX(uint8_t* txBuf, uint8_t* rxBuf, uint16_t len)
{
    if (HAL_SPI_TransmitReceive_DMA(&hspi3, txBuf, rxBuf, len) == HAL_OK)
    {
        return xSemaphoreTake(spi3_semHandle, pdMS_TO_TICKS(500) == pdTRUE ? HAL_OK : HAL_TIMEOUT);
    }
    return HAL_ERROR;
}

#define SD_CS_LOW()  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET)
#define SD_CS_HIGH() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)

// Отправка простой команды (R1)
static HAL_StatusTypeDef SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t *resp)
{
    uint8_t buf[6] = {
        0x40 | cmd,
        (uint8_t)(arg >> 24),
        (uint8_t)(arg >> 16),
        (uint8_t)(arg >>  8),
        (uint8_t)(arg >>  0),
        crc
    };
    // Один полный цикл SCK перед CS_LOW – карта должна быть deselect
    SD_CS_HIGH();
    for (int i = 0; i < 8; i++)
    {
        uint8_t dummy = 0xFF;
        HAL_SPI_Transmit(&hspi3, &dummy, 1, 10); 
    }
    SD_CS_LOW();

    // Посылаем пакет команды
    if (HAL_SPI_Transmit(&hspi3, buf, 6, 10) != HAL_OK)
    {
      // LOGError("%s: cmd=%u; arg=%lu;", __FUNCTION__, cmd, arg);
      return HAL_ERROR;
    }

    // Ждём R1 (MSB=0)
    for (int i = 0; i < 8; i++)
    {
        HAL_SPI_Receive(&hspi3, resp, 1, 10);
        if (!(*resp & 0x80))
        {
            return HAL_OK;
        }
    }

    // LOGError("%s: Timeout", __FUNCTION__);
    return HAL_TIMEOUT;
}

// Ожидание готовности (карта возвращает 0xFF)
static HAL_StatusTypeDef SD_WaitReady(uint32_t timeout)
{
  uint8_t b;
  uint32_t tick = HAL_GetTick();
  do {
    HAL_SPI_Receive(&hspi3, &b, 1, 10);
    if (b == 0xFF)
    {
        return HAL_OK;
    }
  } while ((HAL_GetTick() - tick) < timeout);
  // LOGError("%s: timeout=%lu", __FUNCTION__, timeout);
  return HAL_TIMEOUT;
}

// Поиск токена 0xFE
static HAL_StatusTypeDef SD_WaitDataToken(uint8_t token, uint32_t timeout)
{
    uint8_t b;
    uint32_t tick = HAL_GetTick();
    do {
        HAL_SPI_Receive(&hspi3, &b, 1, 10);
        if (b == token)
        {
            return HAL_OK;
        }
    } while ((HAL_GetTick() - tick) < timeout);
    // LOGError("%s: Timeout token=%u; timeout=%lu", __FUNCTION__, token, timeout);
    return HAL_TIMEOUT;
}

HAL_StatusTypeDef SD_ReadSector_DMA(DWORD sector, uint8_t *buff)
{
    uint8_t r1;
    // Для SDHC/SDXC передаём аргумент в секторах, для SDSC – sector*512
    uint32_t arg = sector;

    // 1. Отправляем CMD17
    if (SD_SendCmd(17, arg, 0xFF, &r1) != HAL_OK || r1 != 0x00)
    {
        SD_CS_HIGH();
        // LOGError("%s: Timeout sectod=%lu", __FUNCTION__, sector);
        return HAL_ERROR;
    }

    // 2. Ждём начала блока 0xFE
    if (SD_WaitDataToken(0xFE, 100) != HAL_OK)
    {
        SD_CS_HIGH();
        // LOGError("%s: 0xFE send timeout", __FUNCTION__);
        return HAL_TIMEOUT;
    }

    // 3. Читаем 512 байт через DMA
    uint8_t txFF[512];
    memset(txFF, 0xFF, sizeof(txFF));
    if (SD_DMA_TXRX(txFF, buff, 512) != HAL_OK)
    {
        SD_CS_HIGH();
        // LOGError("%s: reading timeout", __FUNCTION__);
        return HAL_ERROR;
    }

    // 4. Считываем 2-байтный CRC (игнорируем)
    uint8_t crc[2];
    HAL_SPI_Receive(&hspi3, crc, 2, 10);

    // 5. Завершаем транзакцию
    SD_CS_HIGH();
    HAL_SPI_Transmit(&hspi3, crc, 1, 10); // один такт шины для де-селекта
    return HAL_OK;
}

HAL_StatusTypeDef SD_WriteSector_DMA(DWORD sector, uint8_t *buff)
{
  uint8_t r1, resp;
  uint32_t arg = sector;

  // 1. Отправляем CMD24
  if (SD_SendCmd(24, arg, 0xFF, &r1) != HAL_OK || r1 != 0x00)
  {
    SD_CS_HIGH();
    return HAL_ERROR;
  }

  // 2. Шлём старт-токен 0xFE
  uint8_t token = 0xFE;
  HAL_SPI_Transmit(&hspi3, &token, 1, 10);

  // 3. Передача 512 байт через DMA
  if (SD_DMA_TXRX(buff, NULL, 512) != HAL_OK)
  {
    SD_CS_HIGH();
    return HAL_ERROR;
  }

  // 4. CRC dummy
  uint8_t crc[2] = { 0xFF, 0xFF };
  HAL_SPI_Transmit(&hspi3, crc, 2, 10);

  // 5. Ждём одобрительный ответ D-token (0bXXX0AAA1)
  HAL_SPI_Receive(&hspi3, &resp, 1, 10);
  if ((resp & 0x1F) != 0x05)
  {
    SD_CS_HIGH();
    return HAL_ERROR;
  }

  // 6. Дождаться, пока карта завершит запись (CS=LOW, MISO=0 until busy=1)
  if (SD_WaitReady(500) != HAL_OK)
  {
    SD_CS_HIGH();
    return HAL_TIMEOUT;
  }

  // 7. Завершаем
  SD_CS_HIGH();
  return HAL_OK;
}

uint32_t SD_GetSectorCount(void)
{
  uint8_t r1;
  uint8_t csd[16];
  uint32_t sectors = 0;

  // 1. Запрос CSD
  if (SD_SendCmd(9, 0, 0xFF, &r1) != HAL_OK || r1 != 0x00)
  {
    SD_CS_HIGH();
    return 0;
  }
  // Ждём токен 0xFE
  if (SD_WaitDataToken(0xFE, 100) != HAL_OK) 
  {
    SD_CS_HIGH();
    return 0;
  }
  // Считываем CSD-регистр
  if (SD_DMA_TXRX((uint8_t[]){ [0 ... 15] = 0xFF }, csd, 16) != HAL_OK)
  {
    SD_CS_HIGH();
    return 0;
  }
  // Считываем и отбрасываем CRC
  uint8_t crc2[2];
  HAL_SPI_Receive(&hspi3, crc2, 2, 10);
  SD_CS_HIGH();

  // 2. Парсим CSD
  uint8_t csd_struct = (csd[0] >> 6) & 0x03;

  if (csd_struct == 0)
  {
    // SDSC: C_SIZE = bits [73:62]
    uint32_t c_size = ((uint32_t)(csd[6] & 0x03) << 10)
                    | ((uint32_t)csd[7]        << 2)
                    | ((csd[8] & 0xC0) >> 6);
    uint32_t c_size_mult = ((csd[9] & 0x03) << 1) | ((csd[10] & 0x80) >> 7);
    uint32_t block_len = 1 << (csd[5] & 0x0F);
    uint32_t mult = 1 << (c_size_mult + 2);
    uint32_t capacity = (c_size + 1) * mult * block_len;
    sectors = capacity / 512;
  } 
  else if (csd_struct == 1) 
  {
    // SDHC/SDXC: C_SIZE = bits [69:48]
    uint32_t c_size = ((uint32_t)(csd[7] & 0x3F) << 16)
                    | ((uint32_t)csd[8] << 8)
                    |  csd[9];
    sectors = (c_size + 1) * 1024;
  }

  return sectors;
}

uint32_t SD_GetEraseBlockSize(void)
{
  // Большинство карт по SPI стирают группой в 32 сектора (16 KiB)
  // Можно делать CMD23 перед много-блочной записью, но для простоты:
  return 32;
}
