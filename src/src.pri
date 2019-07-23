QT       += widgets sql
CONFIG   += c++11

INCLUDEPATH += \
    $$_PRO_FILE_PWD_/../include

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
HEADERS += \
    ../include\log4qt.h \
    ../include/processors/LogDebugOutput.h \
    ../include/processors/LogFileSaver.h \
    ../include/processors/LogDisplayWidget.h \
    ../include/processors/LogSqlSaver.h \
    Common.h \
    LogFileSaver/LogFileSaveTask.h \
    LogFileSaver/LogFileSaverPrivate.h \
    LogDisplay/LogDisplayModel.h \
    LogDisplay/LogDisplayFilter.h \
    LogDisplay/LogDisplayWidgetPrivate.h

SOURCES += \
    log4qt.cpp \
    LogDebugOutput/LogDebugOutput.cpp \
    LogFileSaver/LogFileSaver.cpp \
    LogFileSaver/LogFileSaveTask.cpp \
    LogDisplay/LogDisplayModel.cpp \
    LogDisplay/LogDisplayFilter.cpp \
    LogDisplay/LogDisplayWidget.cpp \
    LogSqlSaver/LogSqlSaver.cpp

DISTFILES += \
    log4qt.ts

TRANSLATIONS += log4qt.ts
