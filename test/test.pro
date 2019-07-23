QT += testlib
QT += gui widgets
CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app
DEFINES += QT_WIDGETS_LIB

CONFIG(release, debug|release): TARGET = test
CONFIG(debug, debug|release): TARGET = testd

DESTDIR = $${_PRO_FILE_PWD_}/../bin
CONFIG(debug, debug|release) {
    OBJECTS_DIR = $${_PRO_FILE_PWD_}/../build/test/debug/obj
    MOC_DIR = $${_PRO_FILE_PWD_}/../build/test/debug/moc
    RCC_DIR = $${_PRO_FILE_PWD_}/../build/test/debug/rcc
    UI_DIR = $${_PRO_FILE_PWD_}/../build/test/debug/uic
} else {
    OBJECTS_DIR = $${_PRO_FILE_PWD_}/../build/test/release/obj
    MOC_DIR = $${_PRO_FILE_PWD_}/../build/test/release/moc
    RCC_DIR = $${_PRO_FILE_PWD_}/../build/test/release/rcc
    UI_DIR = $${_PRO_FILE_PWD_}/../build/test/release/uic
}

SOURCES +=  \
    Log4QtTest.cpp

HEADERS += \
    Log4QtTest.h

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
