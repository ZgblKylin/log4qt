#-------------------------------------------------
#
# Project created by QtCreator 2017-12-29T10:14:23
#
#-------------------------------------------------

QT       += widgets sql
CONFIG   += c++11

TARGET = log4qt
TEMPLATE = lib

DEFINES += LOG4QT_LIBRARY

INCLUDEPATH += $$_PRO_FILE_PWD_/../include

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
    ../include/log4qtDefs.h \
    log4qtLogEngine.h \
    ../include/processors/log4qtDebugOutput.h \
    ../include/processors/log4qtDisplayWidget.h \
    ../include/processors/log4qtFileSaver.h \
    LogDisplay/log4qtDisplayFilter.h \
    LogDisplay/log4qtDisplayModel.h \
    ../include/processors/log4qtSqlSaver.h \
    LogDisplay/log4qtDisplayPage.h \
    LogFileSaver/log4qtFileSaveTask.h

SOURCES += \
    log4qtDefs.cpp \
    log4qtLogEngine.cpp \
    LogDebugOutput/log4qtDebugOutput.cpp \
    LogDisplay/log4qtDisplayFilter.cpp \
    LogDisplay/log4qtDisplayModel.cpp \
    LogDisplay/log4qtDisplayWidget.cpp \
    LogFileSaver/log4qtFileSaver.cpp \
    log4qtSql/log4qtSqlSaver.cpp \
    LogDisplay/log4qtDisplayPage.cpp \
    LogFileSaver/log4qtFileSaveTask.cpp

DISTFILES += \
    log4qt.ts

TRANSLATIONS += log4qt.ts

unix {
    target.path = /usr/lib
    INSTALLS += target
}
