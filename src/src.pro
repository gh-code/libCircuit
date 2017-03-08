TEMPLATE = subdirs
SUBDIRS += \
    verilogparser \
    circuit

circuit.subdir = circuit
verilogparser.subdir = parser/verilog

circuit.depends = verilogparser
