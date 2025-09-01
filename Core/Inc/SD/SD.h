#ifndef SD_H
#define SD_H
#include "stdint.h"
#include <string.h>
#include <stdbool.h>

void SD_Init();
void SD_Deinit();
void SD_Task(void *pvParameters);
int SD_readDir(void (*clb)(uint8_t idx, char* str, size_t len), char* dirPath, uint8_t start, uint8_t end);
bool SD_isDir(char* path);
bool SD_dirUp(char* path);
#endif  //SD_H
