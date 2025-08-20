#ifndef LOG_H
#define LOG_H

/* !TODO!
    Further Ideas:
    - Use separated task for logging
    - Use queue
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

/// @brief Basic function for logging
/// @param logLevel Level of log message
/// @param format message format
/// @param 
/// @note This wraps printf calls from stdio
void LOG_Log(LOG_tenLogLevel logLevel, const char *format, ...);

/// @brief Wrap log for Critical
/// @param format message format
/// @param 
void LOG_Critical(const char *format, ...);

/// @brief Wrap log for Error
/// @param format message format
/// @param 
void LOG_Error(const char *format, ...);

/// @brief Wrap log for Warning
/// @param format message format
/// @param 
void LOG_Warning(const char *format, ...);

/// @brief Wrap log for Info
/// @param format message format
/// @param 
void LOG_Info(const char *format, ...);

/// @brief Wrap log for Debug
/// @param format message format
/// @param 
void LOG_Debug(const char *format, ...);

/// @brief Wrap log for Trace
/// @param format message format
/// @param 
void LOG_Trace(const char *format, ...);

/// @brief Get current active log level
/// @return 
LOG_tenLogLevel LOG_GetLogLevel();

/// @brief Set current active log level
/// @param logLevel new log level
void LOG_SetLogLevel(LOG_tenLogLevel logLevel);

#endif  //LOG_H
