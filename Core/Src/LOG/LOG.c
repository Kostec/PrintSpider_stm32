#include "LOG.h"
#include <stdio.h>
#include <stdarg.h>

static LOG_tenLogLevel LOG_ActiveLogLevel;

static const char* LOG_LogLevelString[] = 
{
    "CRITICAL", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE"
};

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

void LOG_Log(LOG_tenLogLevel logLevel, const char *format, ...)
{
    if (logLevel > LOG_ActiveLogLevel || logLevel == LOG_enLogLevelOff)
    {
        return;
    }
    printf("%s: ", LogLevelString(logLevel));
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\r\n");
}

void LOG_Error(const char *format, ...)
{
    LOG_Log(LOG_enLogLevelError, format);
}

void LOG_Critical(const char *format, ...)
{
    LOG_Log(LOG_enLogLevelCritical, format);
}

void LOG_Warning(const char *format, ...)
{
    LOG_Log(LOG_enLogLevelWarning, format);
}

void LOG_Info(const char *format, ...)
{
    LOG_Log(LOG_enLogLevelInfo, format);
}

void LOG_Debug(const char *format, ...)
{
    LOG_Log(LOG_enLogLevelDebug, format);
}

void LOG_Trace(const char *format, ...)
{
    LOG_Log(LOG_enLogLevelTrace, format);
}

LOG_tenLogLevel LOG_GetLogLevel()
{
    return LOG_ActiveLogLevel;
}

void LOG_SetLogLevel(LOG_tenLogLevel logLevel)
{
    LOG_ActiveLogLevel = logLevel;
}