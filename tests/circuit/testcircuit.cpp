#include <QtTest/QtTest>
#include "circuit.h"

class TestCircuit : public QObject
{
    Q_OBJECT;
private slots:
    void testCircuitProperties_data();
    void testCircuitProperties();
};

void TestCircuit::testCircuitProperties_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QString>("name");
    QTest::addColumn<size_t>("inputNum");
    QTest::addColumn<size_t>("outputNum");
    QTest::addColumn<size_t>("gateCount");
    //                           filename       name       inputNum    outputNum gateCount
    QTest::newRow("adder")    << "data/adder.v"   << "adder" << 4ul      << 3ul    << 14ul;
    QTest::newRow("c17_syn")  << "data/c17_syn.v" << "c17"   << 5ul      << 2ul    << 9ul;
    QTest::newRow("c7552")    << "data/c7552.v"   << "c7552" << 207ul    << 108ul  << 3513ul;
    QTest::newRow("empty")    << "data/empty.v"   << "empty" << 0ul      << 0ul    << 0ul;
    QTest::newRow("gate")     << "data/gate.v"    << "Gate"  << 0ul      << 0ul    << 1ul;
    QTest::newRow("ethernet") << "data/ethernet_SYN.v" << "ethernet_syn_comb" << 10469ul << 10625ul << 42787ul;
}

void TestCircuit::testCircuitProperties()
{
    QFETCH(QString, filename);
    QFETCH(QString, name);
    QFETCH(size_t, inputNum);
    QFETCH(size_t, outputNum);
    QFETCH(size_t, gateCount);

    Circuit circuit(filename.toLocal8Bit().constData());
    QVERIFY(QString::fromStdString(circuit.name()) == name);
    QCOMPARE(circuit.inputSize(), inputNum);
    QCOMPARE(circuit.outputSize(), outputNum);
    QCOMPARE(circuit.gateCount(), gateCount);
}

QTEST_MAIN(TestCircuit)
#include "testcircuit.moc"
