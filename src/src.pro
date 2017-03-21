TEMPLATE = lib
TARGET = Circuit

DESTDIR = ../lib
CONFIG += staticlib debug
CONFIG -= debug_and_release debug_and_release_target

INCLUDEPATH += circuit celllibrary

include(parser/verilog/verilog.pri)
include(parser/liberty/liberty.pri)
include(celllibrary/celllibrary.pri)
include(circuit/circuit.pri)
//include(EDAUtils/EDAUtils.pri)

POST_TARGETDEPS += copy_headers
QMAKE_EXTRA_TARGETS += copy_headers extraclean

copy_headers.depends = FORCE
win32: copy_headers.commands = \
    mkdir ..\\include & \
    copy celllibrary\\celllibrary.h ..\\include & \
    copy circuit\\circuit.h ..\\include & \
    copy EDAUtils\\EDAUtils.h ..\\include
unix: copy_headers.commands = \
    mkdir ../include; \
    cp celllibrary/celllibrary.h ../include; \
    cp circuit/circuit.h ../include; \
    cp EDAUtils/EDAUtils.h ../include

clean.depends += extraclean
distclean.depends += extraclean
win32: extraclean.commands = del /q /f ..\\include\\*.h & rmdir ..\\include
unix: extraclean.commands = rm -f ../include/*.h; rmdir ../include