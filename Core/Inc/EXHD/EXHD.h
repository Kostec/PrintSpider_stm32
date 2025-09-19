#ifndef EXHD_H
#define EXHD_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    EXHD_DIO_Read,
    EXHD_DIO_Write,
    EXHD_DIO_Timeout,
    EXHD_SD_NotMount,

    EXHD_numberOfExceptions
} EXHD_tenException;

void EXHD_Init();
bool EXHD_isRaised(EXHD_tenException ex);
void EXHD_Raise(EXHD_tenException ex);
void EXHD_Reset(EXHD_tenException ex);

#endif // EXHD_H
