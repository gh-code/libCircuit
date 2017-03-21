OBJECTS += \
    $$PWD/parser.o \
    $$PWD/scanner.o \
    $$PWD/driver.o

QMAKE_EXTRA_TARGETS += liberty distclean libertyclean
PRE_TARGETDEPS += liberty

clean.depends += libertyclean
distclean.depends += libertyclean
libertyclean.commands += cd parser/liberty && make distclean

liberty.depends = FORCE
liberty.commands = cd parser/liberty && qmake && make