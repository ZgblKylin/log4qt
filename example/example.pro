#-------------------------------------------------
#
# Project created by QtCreator 2018-01-02T11:49:35
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(release, debug|release): TARGET = example
CONFIG(debug, debug|release): TARGET = exampled
TEMPLATE = app

DESTDIR = $${_PRO_FILE_PWD_}/../bin
CONFIG(debug, debug|release) {
    OBJECTS_DIR = $${_PRO_FILE_PWD_}/../build/example/debug/obj
    MOC_DIR = $${_PRO_FILE_PWD_}/../build/example/debug/moc
    RCC_DIR = $${_PRO_FILE_PWD_}/../build/example/debug/rcc
    UI_DIR = $${_PRO_FILE_PWD_}/../build/example/debug/uic
} else {
    OBJECTS_DIR = $${_PRO_FILE_PWD_}/../build/example/release/obj
    MOC_DIR = $${_PRO_FILE_PWD_}/../build/example/release/moc
    RCC_DIR = $${_PRO_FILE_PWD_}/../build/example/release/rcc
    UI_DIR = $${_PRO_FILE_PWD_}/../build/example/release/uic
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        MainWindow.cpp

HEADERS += \
        MainWindow.h

FORMS += \
        MainWindow.ui

# Link log4qt library
win32:CONFIG(release, debug|release): LIBS += -L$$_PRO_FILE_PWD_/../lib -llog4qt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$_PRO_FILE_PWD_/../lib -llog4qtd
else:unix: LIBS += -L$$OUT_PWD/../src/ -llog4qtd
INCLUDEPATH += $$_PRO_FILE_PWD_/../include
DEPENDPATH += $$_PRO_FILE_PWD_/../src


# Copy log4qt library to bin
copy_dir.target = $$_PRO_FILE_PWD_/../bin
win32: copy_dir.source = $$_PRO_FILE_PWD_/../lib/*.dll
else:unix: copy_dir.source = $$_PRO_FILE_PWD_/../lib/*.so
win32 {
    copy_dir.target ~= s,/,\\\\,g
    copy_dir.source ~= s,/,\\\\,g
}
QMAKE_POST_LINK = copy $${copy_dir.source} $${copy_dir.target}
