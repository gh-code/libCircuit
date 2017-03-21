// DEPRECATED, see ../src.pro
include( ../common.pri )
TEMPLATE = lib
TARGET = circuit
HEADERS = circuit.h ../celllibrary/celllibrary.h
SOURCES = circuit.cpp ../celllibrary/celllibrary.cpp
CONFIG += debug
POST_TARGETDEPS += copy_headers
QMAKE_EXTRA_TARGETS += copy_headers

copy_headers.depends = FORCE
win32: copy_headers.commands = mkdir ..\\..\\include & copy *.h ..\\..\\include
unix: copy_headers.commands = mkdir ../../include; cp *.h ../../include