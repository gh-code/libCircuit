#include "circuit.h"
#include "celllibrary.h"
#include "EDAUtils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <utility>

using namespace std;

void usage()
{
    cout << "./circuit <verilog>" << endl;
}

class STA
{
public:
    struct STAData {
        double loading;
        double slew;
        double maxDelay;
        double wireDelay;
        Node node;
        std::string trace;
        Signal::Transition transIn;
        Signal::Transition transOut;
    };

    STA(const Circuit &c, const CellLibrary &lib)
        : circuit(c), library(lib)
    {
        // Set input information
        for (size_t i = 0; i < circuit.inputSize(); i++)
        {
            Port inputPort = circuit.inputPort(i);
            STAData fall = { .loading = 0.0, .slew = 0.0, .maxDelay = 0.0, .wireDelay = 0.0, .node = inputPort };
            STAData rise = { .loading = 0.0, .slew = 0.0, .maxDelay = 0.0, .wireDelay = 0.0, .node = inputPort };
            STAMap[inputPort.name()] = std::make_pair(fall, rise);
        }
    }

    void whenInputRise(STAData *fall, STAData *rise, const std::pair<STAData,STAData> &inputData, Cell &cell, const std::string &fromPin, const std::string &toPin)
    {
        double inputSlew = inputData.second.slew;
        double wireDelay = library.inputWireDelayRiseMax(cell, fromPin);
        double loading = cell.loadingMax(fromPin, toPin, Signal::Rise);
        double slew = cell.slew(fromPin, toPin, Signal::Rise, inputSlew, loading);
        double delay = cell.delay(fromPin, toPin, Signal::Rise, inputSlew, loading) + wireDelay;

        if (cell.timingSense(fromPin, toPin) == PositiveUnate)
        {
            if (slew > rise->slew) { rise->slew = slew; }
            if (inputData.second.maxDelay + delay > rise->maxDelay)
            {
                rise->maxDelay = inputData.second.maxDelay + delay;
                rise->wireDelay = wireDelay;
                rise->loading = loading;
                rise->trace = inputData.second.node.name();
                rise->transIn = Signal::Rise;
                rise->transOut = Signal::Rise;
            }
        }
        else
        {
            if (slew > fall->slew) { fall->slew = slew; }
            if (inputData.second.maxDelay + delay > fall->maxDelay)
            {
                fall->maxDelay = inputData.second.maxDelay + delay;
                fall->wireDelay = wireDelay;
                fall->loading = loading;
                fall->trace = inputData.second.node.name();
                fall->transIn = Signal::Rise;
                fall->transOut = Signal::Fall;
            }
        }
    }

    void whenInputFall(STAData *fall, STAData *rise, const std::pair<STAData,STAData> &inputData, Cell &cell, const std::string &fromPin, const std::string &toPin)
    {
        double inputSlew = inputData.first.slew;
        double wireDelay = library.inputWireDelayFallMax(cell, fromPin);
        double loading = cell.loadingMax(fromPin, toPin, Signal::Fall);
        double slew = cell.slew(fromPin, toPin, Signal::Fall, inputSlew, loading);
        double delay = cell.delay(fromPin, toPin, Signal::Fall, inputSlew, loading) + wireDelay;

        if (cell.timingSense(fromPin, toPin) == PositiveUnate)
        {
            if (slew > fall->slew) { fall->slew = slew; }
            if (inputData.first.maxDelay + delay > fall->maxDelay)
            {
                fall->maxDelay = inputData.first.maxDelay + delay;
                fall->wireDelay = wireDelay;
                fall->loading = loading;
                fall->trace = inputData.second.node.name();
                fall->transIn = Signal::Fall;
                fall->transOut = Signal::Fall;
            }
        }
        else
        {
            if (slew > rise->slew) { rise->slew = slew; }
            if (inputData.first.maxDelay + delay > rise->maxDelay)
            {
                rise->maxDelay = inputData.first.maxDelay + delay;
                rise->wireDelay = wireDelay;
                rise->loading = loading;
                rise->trace = inputData.second.node.name();
                rise->transIn = Signal::Fall;
                rise->transOut = Signal::Rise;
            }
        }
    }

    bool run()
    {
        vector<Cell> levelized;
        EDAUtils::orderByLevel(circuit, levelized);
        if (levelized.empty())
        {
            cerr << "Only allow to simulate synthesized circuit" << endl;
            return false;
        }

        for (size_t i = 0; i < levelized.size(); i++)
        {
            Cell cell = levelized[i];
            
            STAData fall = { .loading = 0.0, .slew = 0.0, .maxDelay = 0.0, .wireDelay = 0.0, .node = cell };
            STAData rise = { .loading = 0.0, .slew = 0.0, .maxDelay = 0.0, .wireDelay = 0.0, .node = cell };

            for (size_t j = 0; j < cell.inputSize(); j++)
            {
                Node input = cell.input(j);

                // bypass wire
                if (input.isWire())
                    input = input.input(0);

                std::string fromPin = cell.inputPinName(j);
                std::string toPin = cell.outputPinName(0);

                std::pair<STAData,STAData> inputData = STAMap[input.name()];
                whenInputRise(&fall, &rise, inputData, cell, fromPin, toPin);
                whenInputFall(&fall, &rise, inputData, cell, fromPin, toPin);
            }

            STAMap[cell.name()] = std::make_pair(fall, rise);
        }

        return true;
    }

    STAData getData(const std::string &cellName, Signal::Transition trans)
    {
        return (trans == Signal::Rise) ? STAMap[cellName].second : STAMap[cellName].first;
    }

private:
    const Circuit &circuit;
    const CellLibrary &library;
    map<std::string,std::pair<STAData,STAData> > STAMap;
};


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        usage();
        return 1;
    }

    CellLibrary library("NangateOpenCellLibrary_typical_conditional_nldm.lib");
    Circuit circuit(argv[1], library);
    if (circuit.isNull())
    {
        cout << "Circuit is empty\n";
        return 1;
    }

    STA sta(circuit, library);
    sta.run();

    // Find global maximum delay
    double globalMaxDelay = 0.0;
    std::string globalMaxDelayPort;
    STA::STAData globalMaxDelayCell;
    for (size_t i = 0; i < circuit.POSize(); i++)
    {
        Port port = circuit.PO(i);
        for (size_t j = 0; j < port.inputSize(); j++)
        {
            Node node = port.input(j);

            if (node.isWire())
                node = node.input(0);

            STA::STAData data = sta.getData(node.name(), Signal::Rise);
            if (data.maxDelay > globalMaxDelay)
            {
                globalMaxDelay = data.maxDelay;
                globalMaxDelayCell = data;
                globalMaxDelayPort = port.name();
            }

            data = sta.getData(node.name(), Signal::Fall);
            if (data.maxDelay > globalMaxDelay)
            {
                globalMaxDelay = data.maxDelay;
                globalMaxDelayCell = data;
                globalMaxDelayPort = port.name();
            }
        }
    }

    std::vector<std::string> lines;
    STA::STAData current = globalMaxDelayCell;
    lines.push_back(globalMaxDelayPort + " (out)");
    while (!current.node.isNull())
    {
        std::stringstream out;
        if (current.node.isPort())
            out << current.node.name() << " (in)\t";
        else
            out << current.node.name() << "     \t";
        out << std::fixed << std::setprecision(6) << current.loading << "  ";
        out << std::fixed << std::setprecision(6) << current.slew << "  ";
        out << std::fixed << std::setprecision(6) << current.maxDelay << " ";
        out << (current.transOut == Signal::Rise ? 'r' : 'f');
        current = sta.getData(current.trace, current.transIn);
        lines.push_back(out.str());
    }

    std::cout << "Point\t             Cap     Trans      Path" << std::endl;
    std::cout << std::string(46, '-') << std::endl;
    for (std::vector<std::string>::reverse_iterator it = lines.rbegin();
        it != lines.rend(); ++it)
    {
        std::cout << (*it) << std::endl;
    }
    std::cout << std::string(46, '-') << std::endl;

    return 0;
}
