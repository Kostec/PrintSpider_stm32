#include "LOG/LOG.h"
#include <stdio.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include "stm32f4xx_hal.h"
#include <string.h>

#define UART_TX_BUF_SIZE 256
static uint8_t uart_tx_buf[UART_TX_BUF_SIZE];
static LOG_tenLogLevel LOG_ActiveLogLevel;
static const char* LOG_LogLevelString[] = 
{
    "CRITICAL", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE"
};

SemaphoreHandle_t uartTxSem;
SemaphoreHandle_t uartMutex;
static QueueHandle_t logQueueHandle;
extern UART_HandleTypeDef huart2;

int _write(int file, char *ptr, int len)
{
  len = len > UART_TX_BUF_SIZE ? UART_TX_BUF_SIZE : len;
  
  // guarantee only one call
  xSemaphoreTake(uartMutex, portMAX_DELAY);
  memcpy(uart_tx_buf, ptr, len);

  // waiting for UART ready state
  while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY)
  {
    taskYIELD();
  }

  // Transmit error
  if (HAL_UART_Transmit_DMA(&huart2, uart_tx_buf, len) != HAL_OK)
  {
    xSemaphoreGive(uartMutex);
    return -1;
  }

  xSemaphoreTake(uartTxSem, portMAX_DELAY);
  xSemaphoreGive(uartMutex);
  return len;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(uartTxSem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


void LOG_Init(void)
{
    uartMutex = xSemaphoreCreateMutex();
    configASSERT(uartMutex);

    uartTxSem = xSemaphoreCreateBinary();
    configASSERT(uartTxSem);
    xSemaphoreGive(uartTxSem);

    LOG_SetLogLevel(LOG_enLogLevelTrace);

    logQueueHandle = xQueueCreate(LOG_QUEUE_DEPTH, sizeof(LogMessage_t));
    configASSERT(logQueueHandle);

    xTaskCreate(LoggerTask, 
        "Logger", 
        1024, 
        NULL, 
        tskIDLE_PRIORITY + 1, 
        NULL);
}

const char* LogLevelString(LOG_tenLogLevel logLevel)
{
    if (logLevel > LOG_enLogLevelOff && logLevel < LOG_enLogLevelError)
    {
        return LOG_LogLevelString[0]; // Criticall
    }
    else if (logLevel >= LOG_enLogLevelError && logLevel < LOG_enLogLevelWarning)
    {
        return LOG_LogLevelString[1]; // Error
    }
    else if (logLevel >= LOG_enLogLevelWarning && logLevel < LOG_enLogLevelInfo)
    {
        return LOG_LogLevelString[2]; // Warning
    }
    else if (logLevel >= LOG_enLogLevelInfo && logLevel < LOG_enLogLevelDebug)
    {
        return LOG_LogLevelString[3]; // Info
    }
    else if (logLevel >= LOG_enLogLevelDebug && logLevel < LOG_enLogLevelTrace)
    {
        return LOG_LogLevelString[4]; // Debug
    }
    else if (logLevel >= LOG_enLogLevelTrace)
    {
        return LOG_LogLevelString[5]; // Trace
    }
    
    return "";
}

void LOG_Log(LOG_tenLogLevel level, const char *format, ...)
{
    if (level > LOG_ActiveLogLevel || level == LOG_enLogLevelOff) {
        return;
    }

    LogMessage_t msg;
    // 1) Префикс
    size_t max = sizeof(msg.buf);
    size_t prefix = snprintf(msg.buf, max, "[%s] ", LogLevelString(level));
    if (prefix >= max - 3) {
        return; // нет места даже под "\r\n\0"
    }

    // 2) Текст
    size_t avail = max - prefix;
    va_list args;
    va_start(args, format);
    int ret = vsnprintf(msg.buf + prefix, avail, format, args);
    va_end(args);

    if (ret < 0) {
        return;
    }
    size_t used = (size_t)ret < (avail - 1) ? (size_t)ret : (avail - 1);
    size_t pos  = prefix + used;

    // 3) Добавляем CRLF
    if (pos + 2 < max) {
        msg.buf[pos++] = '\r';
        msg.buf[pos++] = '\n';
        msg.buf[pos]   = '\0';
    } else {
        msg.buf[max - 1] = '\0';
    }

    // 4) Отправляем
    xQueueSend(logQueueHandle, &msg, 0);
}

LOG_tenLogLevel LOG_GetLogLevel()
{
    return LOG_ActiveLogLevel;
}

void LOG_SetLogLevel(LOG_tenLogLevel logLevel)
{
    LOG_ActiveLogLevel = logLevel;
}

void LoggerTask(void *pvParameters)
{
    LogMessage_t msg;
    // size_t len;

    for (;;) {
        // ждём новый лог (блокируемся здесь)
        if (xQueueReceive(logQueueHandle, &msg, portMAX_DELAY) == pdPASS) {
            printf(msg.buf);
        }
    }
}
