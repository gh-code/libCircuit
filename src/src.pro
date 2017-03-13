TEMPLATE = subdirs
SUBDIRS += \
    verilogparser \
    libertyparser \
    circuit

circuit.subdir = circuit
verilogparser.subdir = parser/verilog
libertyparser.subdir = parser/liberty

circuit.depends = verilogparser
