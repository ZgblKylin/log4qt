#ifndef LOG4QT_H
#define LOG4QT_H

#include <log4qtDefs.h>

namespace log4qt {
// log level, accepts integer ranges 0-100
enum LogLevel
{
    Debug = 0,          // 0-24 for debug
    Infomation = 25,    // 25-49 for information
    Warning = 50,       // 50-74 for warning
    Critical = 75,      // 75-99 for critical
    Fatal = 100         // 100 for fatal
};
} // namespace log4qt

/* Get output stream for logging, has same syntax as qDebug()
 * C format:
 *     qtDebug(%0x04X", 0xabcd);
 * C++ style:
 *     qtDebug() << "Hello World";
 */
// logging for custom log level
#define qtLog(logLevel, ...) log4qt::impl::LogEngine::log(logLevel, __FILE__, __LINE__, Q_FUNC_INFO).log(__VA_ARGS__)
// logging for debug message
#define qtDebug(...) qtLog(log4qt::Debug, __VA_ARGS__)
// logging for information message
#define qtInfomation(...) qtLog(log4qt::Infomation, __VA_ARGS__)
// logging for warning message
#define qtWarning(...) qtLog(log4qt::Warning, __VA_ARGS__)
// logging for critical message
#define qtCritical(...) qtLog(log4qt::Critical, __VA_ARGS__)
// logging for fatal message
#define qtFatal(...) qtLog(log4qt::Fatal, __VA_ARGS__)

#endif // LOG4QT_H
