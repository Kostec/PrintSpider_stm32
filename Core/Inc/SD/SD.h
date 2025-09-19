#ifndef SD_H
#define SD_H
#include "stdint.h"
#include <string.h>
#include <stdbool.h>

/*
 * The module handles sd card requests
*/

typedef struct {
    uint32_t totalSize;
    uint32_t freeSpace;
} tstSD_info;

typedef void (*fpSD_fileReadClb) (uint8_t idx, char* str, size_t len);

typedef void (*fpSD_intClb) (int res);
typedef fpSD_intClb fpSD_readDirClb;

typedef fpSD_intClb fpSD_dirItemsCountClb;

typedef void (*fpSD_boolClb) (bool res);
typedef fpSD_boolClb fpSD_isDirClb;
typedef fpSD_boolClb fpSD_dirUpClb;

typedef void (*fpSD_InfoClb) (tstSD_info res);

void SD_Init();
void SD_Deinit();
void SD_Task(void *pvParameters);
bool SD_isAvailable();
void SD_dirItemsCount(fpSD_dirItemsCountClb resClb, char* dirPath);
void SD_readDir(fpSD_readDirClb resClb, fpSD_fileReadClb clb, char* dirPath, uint8_t start, uint8_t end);
void SD_isDir(fpSD_isDirClb resClb, char* path);
void SD_dirUp(fpSD_dirUpClb resClb, char* path);
void SD_info(fpSD_InfoClb resClb, char* path);

#endif  //SD_H
