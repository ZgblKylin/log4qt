TEMPLATE = lib
CONFIG(release, debug|release): TARGET = log4qt
CONFIG(debug, debug|release): TARGET = log4qtd

DEFINES += LOG4QT_DYNAMIC

DESTDIR = $${_PRO_FILE_PWD_}/../lib
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

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include(src.pri)
