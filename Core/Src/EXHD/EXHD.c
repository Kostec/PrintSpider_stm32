#include "EXHD/EXHD.h"
#include "LOG/LOG.h"

/*
    TODO List:
    - Store exceptions in EEPROM or SD card
    - Add more smart handling like: raised_but_released and so on
    - Add separated output for EXHD independent of LOG (command via UART, USB or smt else)
*/

static bool EXHD__exceptions[EXHD_numberOfExceptions] = {0};

void EXHD_Init()
{

}

bool EXHD_isRaised(EXHD_tenException ex)
{
    return EXHD__exceptions[ex];
}

void EXHD_Raise(EXHD_tenException ex)
{
    if (!EXHD_isRaised(ex))
    {
        EXHD__exceptions[ex] = true;
        LOG_Error("%s: %d", __FUNCTION__, ex);
    }
}

void EXHD_Reset(EXHD_tenException ex)
{
    if (EXHD_isRaised(ex))
    {
        EXHD__exceptions[ex] = false;
        LOG_Info("%s: %d", __FUNCTION__, ex);
    }
}
