#-------------------------------------------------
#
# Project created by QtCreator 2017-12-29T10:14:23
#
#-------------------------------------------------

QT       += widgets sql
CONFIG   += c++17

TEMPLATE = lib
CONFIG(release, debug|release): TARGET = log4qt
CONFIG(debug, debug|release): TARGET = log4qtd

DESTDIR = $${_PRO_FILE_PWD_}/../lib
DLLDESTDIR = $${_PRO_FILE_PWD_}/../lib
CONFIG(debug, debug|release) {
    OBJECTS_DIR = $${_PRO_FILE_PWD_}/../build/log4qt/debug/obj
    MOC_DIR = $${_PRO_FILE_PWD_}/../build/log4qt/debug/moc
    RCC_DIR = $${_PRO_FILE_PWD_}/../build/log4qt/debug/rcc
    UI_DIR = $${_PRO_FILE_PWD_}/../build/log4qt/debug/uic
} else {
    OBJECTS_DIR = $${_PRO_FILE_PWD_}/../build/log4qt/release/obj
    MOC_DIR = $${_PRO_FILE_PWD_}/../build/log4qt/release/moc
    RCC_DIR = $${_PRO_FILE_PWD_}/../build/log4qt/release/rcc
    UI_DIR = $${_PRO_FILE_PWD_}/../build/log4qt/release/uic
}

DEFINES += LOG4QT_LIBRARY

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

unix {
    target.path = /usr/lib
    INSTALLS += target
}
