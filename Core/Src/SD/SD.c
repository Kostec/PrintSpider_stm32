#include "SD/SD.h"
#include "LOG/LOG.h"

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <semphr.h>
#include "LOG/LOG.h"
#include "fatfs.h"

osThreadId_t SD_Thread;

void SD_Init()
{
    const osThreadAttr_t DIO_attributes = {
        .name = "SD_Task",
        .stack_size = 256 * 4,
        .priority = (osPriority_t) osPriorityHigh,
      };
      SD_Thread = osThreadNew(SD_Task, NULL, &DIO_attributes);
    configASSERT(SD_Thread);
}

void SD_Deinit()
{

}

void SD_Task(void *pvParameters)
{
    LOG_Debug("%s", __FUNCTION__);
    static uint8_t mounted = 0;
    for(;;)
    {
        if (!mounted)
        {
            FRESULT res = f_mount(&USERFatFS, USERPath, 1);
            if (res != FR_OK)
            {
                LOG_Debug("%s: SD card not found. Error: %d", __FUNCTION__, res);
            }
            else
            {
                LOG_Debug("%s: SD card is available", __FUNCTION__);
                mounted = 1;
            }
        }
        osDelay(1000);
    }
}

int SD_readDir(void (*clb)(uint8_t idx, char* str, size_t len), char* dirPath, uint8_t start, uint8_t end)
{
    DIR dir;
    FILINFO fno;
    LOG_Debug("%s: start=%u end=%u", __FUNCTION__, start, end);
    FRESULT res = f_opendir(&dir, dirPath);
    osDelay(10);
    uint8_t count = 0;
    if (res == FR_OK)
    {
        // LOG_Debug("%s: readdir", __FUNCTION__);
        while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0])
        {
            if (count < start) continue;
            if (count > end) break;
            size_t len = strlen(fno.fname);
            clb(count-start, fno.fname, len);
            // LOG_Debug("%s: filename=%s", __FUNCTION__, fno.fname);
            osDelay(10);
            count++;
        }
        osDelay(10);
        f_closedir(&dir);
        osDelay(10);
        return count - start;
    }
    else 
    {
        return -1;
    }
}

bool SD_isDir(char* path)
{
    FILINFO fno;
    if (f_stat(path, &fno) != FR_OK)
    {
        LOG_Error("%s: f_stat return error", __FUNCTION__);
        return false;
    }
    
    return fno.fattrib & AM_DIR;
}

bool SD_dirUp(char* path)
{
    char _path[64] = {0};
    osDelay(10);
    strcat(_path, path);
    osDelay(10);
    FRESULT res = f_chdir(_path);
    if (res != FR_OK)
    {
       return false;
    }

    osDelay(10);
    res = f_chdir("..");
    osDelay(10);
    if (res != FR_OK)
    {
       return false;
    }

    if (f_getcwd(path, sizeof(_path)) != FR_OK)
    {
        return false;
    }

    return true;
}
