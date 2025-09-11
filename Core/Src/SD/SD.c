/*
 Copyright 2025 Kostec
 SPDX-License-Identifier: Apache-2.0
*/

#include "SD/SD.h"
#include "LOG/LOG.h"

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include <semphr.h>
#include "LOG/LOG.h"
#include "fatfs.h"

typedef enum
{
    SD_opDirItemsCount,
    SD_opReadDir,
    SD_opIsDir,
    SD_opDirUp,
    SD_opInfo
} SD_tenOpCode;

typedef struct
{
    SD_tenOpCode opCode;
    char* path;
    uint8_t start;
    uint8_t end;
    void* resClb;
    fpSD_fileReadClb fileReadClb;
} SD_tstMessage;


extern FATFS USERFatFS;
osThreadId_t SD__Thread;
osMessageQueueId_t SD__Queue;

int SD__dirItemsCount(char* path)
{
    DIR dir;
    FILINFO fno;
    LOG_Debug("%s: %s", __FUNCTION__, path);
    FRESULT res = f_opendir(&dir, path);
    osDelay(10);
    uint32_t count = 0;
    if (res == FR_OK)
    {
        // LOG_Debug("%s: readdir", __FUNCTION__);
        while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0])
        {
            osDelay(10);
            count++;
        }
        osDelay(10);
        f_closedir(&dir);
        osDelay(10);
        return count;
    }
    else 
    {
        return -1;
    }
}

int SD__readDir(fpSD_fileReadClb clb, char* dirPath, uint8_t start, uint8_t end)
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
            if (count < start)
            {
                count++;
                continue;
            }
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

bool SD__isDir(char* path)
{
    FILINFO fno;
    if (f_stat(path, &fno) != FR_OK)
    {
        LOG_Error("%s: f_stat return error", __FUNCTION__);
        return false;
    }
    
    return fno.fattrib & AM_DIR;
}

bool SD__dirUp(char* path)
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

tstSD_info SD__info(char* path)
{
    DWORD freeClusters;
    tstSD_info sdInfo;
    FATFS* pFATFS = &USERFatFS;
    f_getfree(path, &freeClusters, &pFATFS);
    sdInfo.totalSize = (uint32_t)((USERFatFS.n_fatent - 2) * USERFatFS.csize * 0.5);
    sdInfo.freeSpace = (uint32_t)(freeClusters * USERFatFS.csize * 0.5);

    return sdInfo;
}

void SD_Init()
{   
    const osMessageQueueAttr_t SD_QueueAttr = {
        .name = SD__Queue,
    };
    SD__Queue = osMessageQueueNew(16, sizeof(SD_tstMessage), &SD_QueueAttr);

    const osThreadAttr_t SD_attributes = {
        .name = "SD_Task",
        .stack_size = 256 * 4,
        .priority = (osPriority_t) osPriorityHigh,
      };
      SD__Thread = osThreadNew(SD_Task, NULL, &SD_attributes);
    configASSERT(SD__Thread);
}

void SD_Deinit()
{

}

void SD_Task(void *pvParameters)
{
    LOG_Debug("%s", __FUNCTION__);
    static uint8_t mounted = 0;
    SD_tstMessage msg = {0};
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
        if (osMessageQueueGet(SD__Queue, &msg, NULL, 10) == osOK)
        {
            switch (msg.opCode)
            {
                case SD_opDirItemsCount:
                {
                    fpSD_dirItemsCountClb clb = (fpSD_dirItemsCountClb) msg.resClb;
                    int res = SD__dirItemsCount(msg.path);
                    clb(res);
                    break;
                }
                case SD_opReadDir:
                {
                    fpSD_readDirClb clb = (fpSD_readDirClb) msg.resClb;
                    int res = SD__readDir(msg.fileReadClb, msg.path, msg.start, msg.end);
                    clb(res);
                    break;
                }
                case  SD_opIsDir:
                {
                    fpSD_isDirClb clb = (fpSD_isDirClb) msg.resClb;
                    bool res = SD__isDir(msg.path);
                    clb(res);
                    break;
                }
                case SD_opDirUp:
                {
                    fpSD_dirUpClb clb = (fpSD_dirUpClb) msg.resClb;
                    bool res = SD__dirUp(msg.path);
                    clb(res);
                    break;
                }
                case SD_opInfo:
                {
                    fpSD_InfoClb clb = (fpSD_InfoClb) msg.resClb;
                    tstSD_info res = SD__info(msg.path);
                    clb(res);
                    break;
                }
            
            default:
                break;
            }
        }

        osDelay(100);
    }
}

void SD_dirItemsCount(fpSD_dirItemsCountClb resClb, char* dirPath)
{
    SD_tstMessage msg;
    msg.opCode = SD_opDirItemsCount;
    msg.resClb = resClb;
    msg.path = dirPath;

    osMessageQueuePut(SD__Queue, &msg, 0, 0);
}

void SD_readDir(fpSD_readDirClb resClb, fpSD_fileReadClb clb, char* dirPath, uint8_t start, uint8_t end)
{
   SD_tstMessage msg;
   msg.opCode = SD_opReadDir;
   msg.resClb = resClb;
   msg.fileReadClb = clb;
   msg.path = dirPath;
   msg.start = start;
   msg.end = end;

   osMessageQueuePut(SD__Queue, &msg, 0, 0);
}

void SD_isDir(fpSD_isDirClb resClb, char* path)
{
    SD_tstMessage msg;
    msg.opCode = SD_opIsDir;
    msg.resClb = resClb;
    msg.path = path;
    osMessageQueuePut(SD__Queue, &msg, 0, 0);
}

void SD_dirUp(fpSD_dirUpClb resClb, char* path)
{
    SD_tstMessage msg;
    msg.opCode = SD_opDirUp;
    msg.resClb = resClb;
    msg.path = path;
    osMessageQueuePut(SD__Queue, &msg, 0, 0);
}

void SD_info(fpSD_InfoClb resClb, char* path)
{
    SD_tstMessage msg;
    msg.opCode = SD_opInfo;
    msg.resClb = resClb;
    msg.path = path;
    osMessageQueuePut(SD__Queue, &msg, 0, 0);
}
