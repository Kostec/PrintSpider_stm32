#ifndef SD_H
#define SD_H
#include "stdint.h"
#include <string.h>
#include <stdbool.h>

typedef struct {
    uint32_t totalSize;
    uint32_t freeSpace;
} tstSD_info;

typedef void (*SD_FileReadClb) (uint8_t idx, char* str, size_t len);

typedef void (*SD_intClb) (int res);
typedef SD_intClb SD_readDirClb;

typedef SD_intClb SD_dirItemsCountClb;

typedef void (*SD_boolClb) (bool res);
typedef SD_boolClb SD_isDirClb;
typedef SD_boolClb SD_dirUpClb;

typedef void (*SD_InfoClb) (tstSD_info res);

void SD_Init();
void SD_Deinit();
void SD_Task(void *pvParameters);
void SD_dirItemsCount(SD_dirItemsCountClb resClb, char* dirPath);
void SD_readDir(SD_readDirClb resClb, SD_FileReadClb clb, char* dirPath, uint8_t start, uint8_t end);
void SD_isDir(SD_isDirClb resClb, char* path);
void SD_dirUp(SD_dirUpClb resClb, char* path);
void SD_info(SD_InfoClb resClb, char* path);

#endif  //SD_H
