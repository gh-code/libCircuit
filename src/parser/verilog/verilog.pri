OBJECTS += \
    $$PWD/parser.o \
    $$PWD/scanner.o \
    $$PWD/driver.o

QMAKE_EXTRA_TARGETS += verilog distclean verilogclean
PRE_TARGETDEPS += verilog

clean.depends += verilogclean
distclean.depends += verilogclean
verilogclean.commands = cd parser/verilog && make distclean

verilog.depends = FORCE
verilog.commands = cd parser/verilog && qmake && make