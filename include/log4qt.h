#ifndef LOG4QT_H
#define LOG4QT_H

#include <log4qtDefs.h>

namespace log4qt {
// log level, accepts integer ranges 0-100
enum LogLevel
{
    Debug = 0,          // 0-24 for debug
    Infomation = 25,    // 25-49 for infomation
    Warning = 50,       // 50-74 for warning
    Critical = 75,      // 75-99 for critical
    Fatal = 100         // 100 for fatal
};
} // namespace log4qt

/* Get output stream for logging, has same syntax as qDebug()
 * C format:
 *     qtDebug("0x%04X", 0xabcd);
 * C++ style:
 *     qtDebug() << "Hello World";
 */
#define qtLog(logLevel, ...) \
    log4qt::impl::getLogEngine()->log(QString(), logLevel, __FILE__, __LINE__, Q_FUNC_INFO).log(__VA_ARGS__)
#define qtDebug(...) qtLog(log4qt::Debug, __VA_ARGS__)
#define qtInfomation(...) qtLog(log4qt::Infomation, __VA_ARGS__)
#define qtWarning(...) qtLog(log4qt::Warning, __VA_ARGS__)
#define qtCritical(...) qtLog(log4qt::Critical, __VA_ARGS__)
#define qtFatal(...) qtLog(log4qt::Fatal, __VA_ARGS__)


/* The logging category represents a category, or 'area' in the logging infrastructure.
 * It is identified by a string - at runtime.
 * A category can be configured to filter messages by level.
 * When a message is not accepted by filter, it will not be recorded, generate none performance costs
 */
#define qtCLog(category, logLevel, ...) \
    if(log4qt::impl::getLogEngine()->categoryFilter(category) >= logLevel) \
        log4qt::impl::getLogEngine()->log(category, logLevel, __FILE__, __LINE__, Q_FUNC_INFO).log(__VA_ARGS__)
#define qtCDebug(category, ...) qtCLog(category, log4qt::Debug, __VA_ARGS__)
#define qtCInfomation(category, ...) qtCLog(category, log4qt::Infomation, __VA_ARGS__)
#define qtCWarning(category, ...) qtCLog(category, log4qt::Warning, __VA_ARGS__)
#define qtCCritical(category, ...) qtCLog(category, log4qt::Critical, __VA_ARGS__)
#define qtCFatal(category, ...) qtCLog(category, log4qt::Fatal, __VA_ARGS__)

#endif // LOG4QT_H
