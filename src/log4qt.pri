QT       += widgets sql
CONFIG   += c++11

INCLUDEPATH += \
    $$PWD/../include

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
    $$PWD/../include\log4qt.h \
    $$PWD/../include/processors/LogDebugOutput.h \
    $$PWD/../include/processors/LogFileSaver.h \
    $$PWD/../include/processors/LogDisplayWidget.h \
    $$PWD/../include/processors/LogSqlSaver.h \
    $$PWD/Common.h \
    $$PWD/LogFileSaver/LogFileSaveTask.h \
    $$PWD/LogFileSaver/LogFileSaverPrivate.h \
    $$PWD/LogDisplay/LogDisplayModel.h \
    $$PWD/LogDisplay/LogDisplayFilter.h \
    $$PWD/LogDisplay/LogDisplayWidgetPrivate.h

SOURCES += \
    $$PWD/log4qt.cpp \
    $$PWD/LogDebugOutput/LogDebugOutput.cpp \
    $$PWD/LogFileSaver/LogFileSaver.cpp \
    $$PWD/LogFileSaver/LogFileSaveTask.cpp \
    $$PWD/LogDisplay/LogDisplayModel.cpp \
    $$PWD/LogDisplay/LogDisplayFilter.cpp \
    $$PWD/LogDisplay/LogDisplayWidget.cpp \
    $$PWD/LogSqlSaver/LogSqlSaver.cpp

TRANSLATIONS += \
    $$PWD/log4qt.ts
