#ifndef SD_H
#define SD_H

#include "cmsis_os.h"      // FreeRTOS-семафор
#include "main.h"          // SD_CS_GPIO_Port, SD_CS_Pin
#include <string.h>
#include <integer.h>

HAL_StatusTypeDef SD_ReadSector_DMA(DWORD sector, uint8_t *buff);
HAL_StatusTypeDef SD_WriteSector_DMA(DWORD sector, uint8_t *buff);
uint32_t SD_GetSectorCount(void);
uint32_t SD_GetEraseBlockSize(void);

#endif  //SD_H
