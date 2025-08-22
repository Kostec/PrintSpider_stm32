#ifndef LOG_H
#define LOG_H

#define LOG_Critical(format, ...) LOG_Log(LOG_enLogLevelCritical, format, ##__VA_ARGS__)
#define LOG_Error(format, ...) LOG_Log(LOG_enLogLevelError, format, ##__VA_ARGS__)
#define LOG_Warning(format, ...) LOG_Log(LOG_enLogLevelWarning, format, ##__VA_ARGS__)
#define LOG_Info(format, ...) LOG_Log(LOG_enLogLevelInfo, format, ##__VA_ARGS__)
#define LOG_Debug(format, ...) LOG_Log(LOG_enLogLevelDebug, format, ##__VA_ARGS__)
#define LOG_Trace(format, ...) LOG_Log(LOG_enLogLevelTrace, format, ##__VA_ARGS__)

// Max log string length
#define LOG_MSG_MAX_LEN   128
// Max log qeue depth
#define LOG_QUEUE_DEPTH   16

typedef struct {
    char buf[LOG_MSG_MAX_LEN];
} LogMessage_t;

/* !TODO!
    Further Ideas:
    - Use timestamps
    - Use file logging
*/

/// @brief Default log levels
/// User can define and set own log levels in free range
typedef enum
{
    LOG_enLogLevelOff = 0,
    LOG_enLogLevelCritical = 10,
    LOG_enLogLevelError = 100,
    LOG_enLogLevelWarning = 200,
    LOG_enLogLevelInfo = 300,
    LOG_enLogLevelDebug = 400,
    LOG_enLogLevelTrace = 500,
} LOG_tenLogLevel;

void LOG_Init(void);

/// @brief Basic function for logging
/// @param logLevel Level of log message
/// @param format message format
/// @param 
/// @note This wraps printf calls from stdio
/// @return None
void LOG_Log(LOG_tenLogLevel logLevel, const char *format, ...);

/// @brief Get current active log level
/// @return LOG_tenLogLevel
LOG_tenLogLevel LOG_GetLogLevel();

/// @brief Set current active log level
/// @param logLevel new log level
/// @return None
void LOG_SetLogLevel(LOG_tenLogLevel logLevel);

/// @brief Logger loop task
/// @param pvParameters task parameters
/// @return None
void LoggerTask(void *pvParameters);

#endif  //LOG_H
