TEMPLATE = subdirs
SUBDIRS += \
    verilogparser \
    libertyparser \
    circuit \
    EDAUtils

circuit.subdir = circuit
verilogparser.subdir = parser/verilog
libertyparser.subdir = parser/liberty

circuit.depends = verilogparser
