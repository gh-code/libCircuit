#include "celllibrary.h"
#include "circuit.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <qatomic.h>
#include "../parser/liberty/driver.h"
#include "../parser/liberty/expression.h"

class CellLibraryPrivate
{
public:
    CellLibraryPrivate();
    CellLibraryPrivate(const std::string&);

    void createTwoInputCell(const std::string&, const std::string&, const std::string&, const std::string&);
    bool hasCell(const std::string&) const;
    Cell cell(const std::string&) const;

    bool load(std::fstream&, const std::string &path);

    std::string name;
    std::string time_unit;
    std::string leakage_power_unit;
    std::string voltage_unit;
    std::string current_unit;
    std::string pulling_resistance_unit;
    std::string capacitive_load_unit;
    double nom_process;
    double nom_temperature;
    double nom_voltage;

    bool isDefault;

    std::map<std::string,Cell> cells;

    QAtomicInt ref;
};

/**************************************************************
 *
 * CellLibraryPrivate
 *
 **************************************************************/

CellLibraryPrivate::CellLibraryPrivate() : isDefault(true)
{
    Cell INV_X1("INV_X1");
    INV_X1.addInputPinName("A");
    INV_X1.addOutputPinName("ZN");
    cells["INV_X1"] = INV_X1;

    createTwoInputCell("NAND2_X1", "A1", "A2", "ZN");
    createTwoInputCell( "AND2_X1", "A1", "A2", "ZN");
    createTwoInputCell( "NOR2_X1", "A1", "A2", "ZN");
    createTwoInputCell(  "OR2_X1", "A1", "A2", "ZN");
    createTwoInputCell( "XOR2_X1", "A",  "B",  "Z");
}

CellLibraryPrivate::CellLibraryPrivate(const std::string &path) : isDefault(false)
{
    // Open a Liberty file
    std::fstream infile(path.c_str());
    if (!infile.good())
    {
        std::cerr << "Could not open file: " << path << std::endl;
        return;
    }
    load(infile, path);
}

static void tableRowStringToDouble(std::vector<double> *out, const std::string &data)
{
    std::istringstream iss(data);
    double item;
    while (iss >> item)
    {
        out->push_back(item);
        if (iss.peek() == ',')
            iss.ignore();
    }
    while (iss);
}

static void handleTimingTable(LibertyContext &liberty, Cell &cell, LNPin *pin, const std::string &type, Signal::Transition trans, LNTiming *timing, LNTimingTable* timingTable)
{
    std::vector<std::string> &t_table = timingTable->table;
    std::vector<double> x;
    std::vector<double> y;
    LNLuTableTemplate &tableTemplate = liberty.lu_table_templates[timingTable->lu_table_template];
    if (tableTemplate.variable_1 == "input_net_transition" &&
        tableTemplate.variable_2 == "total_output_net_capacitance")
    {
        // NanGate
        tableRowStringToDouble(&x, tableTemplate.index_2);
        tableRowStringToDouble(&y, tableTemplate.index_1);

        std::vector<std::vector<double> > table;
        for (size_t i = 0; i < y.size(); i++)
        {
            std::vector<double> temp;
            tableRowStringToDouble(&temp, t_table[i]);
            assert(x.size() == temp.size());
            table.push_back(temp);
        }

        cell.addTimingTable(type,
            pin->name,
            timing->related_pin,
            timing->timing_sense,
            trans,
            x, y, table);
    }
    else if (tableTemplate.variable_1 == "total_output_net_capacitance" &&
             tableTemplate.variable_2 == "input_net_transition")
    {
        // ISPD
        tableRowStringToDouble(&x, tableTemplate.index_1);
        tableRowStringToDouble(&y, tableTemplate.index_2);

        std::vector<std::vector<double> > table;
        for (size_t i = 0; i < y.size(); i++)
        {
            std::vector<double> temp;
            table.push_back(temp);
        }

        for (size_t i = 0; i < y.size(); i++)
        {
            std::vector<double> temp;
            tableRowStringToDouble(&temp, t_table[i]);
            for (size_t j = 0; j < y.size(); j++)
                table[j].push_back(temp[j]);
        }

        cell.addTimingTable(type,
            pin->name,
            timing->related_pin,
            timing->timing_sense,
            trans,
            x, y, table);
    }
    else
    {
        // temporarily ignore Tristate_Disable and other things
    }
}

static void handleTimings(LibertyContext &liberty, Cell &cell, LNPin *pin)
{
    for (size_t j = 0; j < pin->timings.size(); j++)
    {
        LNTiming *timing = pin->timings[j];
        if (timing->cell_fall)
            handleTimingTable(liberty, cell, pin, "delay", Signal::Fall, timing, timing->cell_fall);
        if (timing->cell_rise)
            handleTimingTable(liberty, cell, pin, "delay", Signal::Rise, timing, timing->cell_rise);
        if (timing->fall_transition)
            handleTimingTable(liberty, cell, pin, "trans", Signal::Fall, timing, timing->fall_transition);
        if (timing->rise_transition)
            handleTimingTable(liberty, cell, pin, "trans", Signal::Rise, timing, timing->rise_transition);
        // otherwise, temporarily ignore
    }
}

bool CellLibraryPrivate::load(std::fstream &infile, const std::string &path)
{
    LibertyContext liberty;
    Liberty::Driver driver(liberty);
    // driver.trace_scanning = true;
    // driver.trace_parsing = true;

    // Start parsing the Verilog file
    liberty.clearExpressions();
    bool result = driver.parse_stream(infile, path);
    if (!result || liberty.expressions.size() == 0)
        return false;

    // Copy properties
    name = liberty.name;
    time_unit = liberty.time_unit;
    leakage_power_unit = liberty.leakage_power_unit;
    voltage_unit = liberty.voltage_unit;
    current_unit = liberty.current_unit;
    pulling_resistance_unit = liberty.pulling_resistance_unit;
    capacitive_load_unit = liberty.capacitive_load_unit;
    nom_process = liberty.nom_process;
    nom_temperature = liberty.nom_temperature;
    nom_voltage = liberty.nom_voltage;

    // Build cell library

    // 1. lu_table_templates
    // 2. cells

    std::map<std::string,LNCell>::iterator it;
    for (it = liberty.cells.begin(); it != liberty.cells.end(); ++it)
    {
        LNCell tmp_cell = it->second;

        Cell cell(tmp_cell.name);
        cell.setArea(tmp_cell.area);

        for (size_t i = 0; i < tmp_cell.pins.size(); i++)
        {
            LNPin *pin = tmp_cell.pins[i];
            if (pin->direction == "input")
            {
                cell.setInputCapacitance(pin->name, pin->capacitance);
                cell.setInputCapacitanceRise(pin->name, pin->rise_capacitance);
                cell.setInputCapacitanceFall(pin->name, pin->fall_capacitance);
                cell.addInputPinName(pin->name);
            }
            else if (pin->direction == "output")
            {
                cell.setFunction(pin->function);
                cell.setOutputMaxCapacitance(pin->name, pin->max_capacitance);
                cell.setOutputMaxTransition(pin->name, pin->max_transition);
                cell.addOutputPinName(pin->name);
                handleTimings(liberty, cell, pin);
            }
            else if (pin->direction == "internal")
            { /* do nothing */ }
            else
            {
                std::cerr << "WARNING: something went wrong: "
                          << tmp_cell.name << ":" << pin->name << "'s "
                          << "pin direction is " << pin->direction << std::endl;
            }
        }
        cells[tmp_cell.name] = cell;
    }

    return true;

    // for (std::map<std::string,LNLuTableTemplate>::iterator it = liberty.lu_table_templates.begin();
    //         it != liberty.lu_table_templates.end(); ++it)
    // {
    //     std::cout << it->first << std::endl;
    //     std::cout << it->second.variable_1 << " ";
    //     std::cout << it->second.variable_2 << std::endl;
    //     std::cout << it->second.index_1 << std::endl;
    //     std::cout << it->second.index_2 << std::endl;
    // }
    for (std::map<std::string,LNCell>::iterator it = liberty.cells.begin();
            it != liberty.cells.end(); ++it)
    {
        LNCell tmp_cell = it->second;
        std::cout << tmp_cell.name << std::endl;
        for (size_t i = 0; i < tmp_cell.pins.size(); i++)
        {
            LNPin *pin = tmp_cell.pins[i];
            std::cout << "  " << pin->name << std::endl;
            for (size_t j = 0; j < pin->timings.size(); j++)
            {
                if (pin->timings[j]->cell_fall)
                {
                    std::vector<std::string> &table = pin->timings[j]->cell_fall->table;
                    std::cout << "    " << pin->timings[j]->cell_fall->name << std::endl;
                    // for (size_t i = 0; i < table.size(); i++)
                    // {
                    //     std::cout << "      ";
                    //     std::cout << table[i];
                    //     std::cout << std::endl;
                    // }
                }
                else if (pin->timings[j]->fall_constraint) {
                    std::vector<std::string> &table = pin->timings[j]->fall_constraint->table;
                    for (size_t i = 0; i < table.size(); i++)
                    {
                        std::cout << "      ";
                        std::cout << table[i];
                        std::cout << std::endl;
                    }
                } else {
                    std::cout << "    " << "0" << std::endl;
                }
                std::cout << "    " << pin->timings[j]->related_pin << std::endl;
            }
        }
    }

    return true;
}

void CellLibraryPrivate::createTwoInputCell(const std::string &name, const std::string &a, const std::string &b, const std::string &out)
{
    Cell cell(name);
    cell.addInputPinName(a);
    cell.addInputPinName(b);
    cell.addOutputPinName(out);
    cells[name] = cell;
}

bool CellLibraryPrivate::hasCell(const std::string &type) const
{
    return (cells.find(type) != cells.end());
}

Cell CellLibraryPrivate::cell(const std::string &type) const
{
    if (hasCell(type))
        return cells.at(type).cloneNode().toCell();
    return Cell();
}


/**************************************************************
 *
 * CellLibrary
 *
 **************************************************************/

CellLibrary::CellLibrary()
{
    impl = new CellLibraryPrivate;
}

CellLibrary::CellLibrary(const std::string &path)
{
    impl = new CellLibraryPrivate(path);
}

CellLibrary::CellLibrary(CellLibraryPrivate *p)
{
    impl = p;
}

CellLibrary::CellLibrary(const CellLibrary &n)
{
    impl = n.impl;
    if (impl)
        impl->ref.ref();
}

CellLibrary& CellLibrary::operator= (const CellLibrary &n)
{
    if (n.impl)
        n.impl->ref.ref();
    if (impl && !impl->ref.deref())
        delete impl;
    impl = n.impl;
    return *this;
}

bool CellLibrary::operator== (const CellLibrary &n) const
{
    return (impl == n.impl);
}

bool CellLibrary::operator!= (const CellLibrary &n) const
{
    return (impl != n.impl);
}

CellLibrary::~CellLibrary()
{
    if (impl && !impl->ref.deref())
        delete impl;
}

std::string CellLibrary::name() const
{
    if (!impl)
        return std::string();
    return impl->name;
}

std::string CellLibrary::time_unit() const
{
    if (!impl)
        return std::string();
    return impl->time_unit;
}

std::string CellLibrary::leakage_power_unit() const
{
    if (!impl)
        return std::string();
    return impl->leakage_power_unit;
}

std::string CellLibrary::voltage_unit() const
{
    if (!impl)
        return std::string();
    return impl->voltage_unit;
}

std::string CellLibrary::current_unit() const
{
    if (!impl)
        return std::string();
    return impl->current_unit;
}

std::string CellLibrary::pulling_resistance_unit() const
{
    if (!impl)
        return std::string();
    return impl->pulling_resistance_unit;
}

std::string CellLibrary::capacitive_load_unit() const
{
    if (!impl)
        return std::string();
    return impl->capacitive_load_unit;
}

double CellLibrary::nom_process() const
{
    if (!impl)
        return 0.0;
    return impl->nom_process;
}

double CellLibrary::nom_temperature() const
{
    if (!impl)
        return 0.0;
    return impl->nom_temperature;
}

double CellLibrary::nom_voltage() const
{
    if (!impl)
        return 0.0;
    return impl->nom_voltage;
}

bool CellLibrary::isDefault() const
{
    return impl->isDefault;
}

bool CellLibrary::isNull() const
{
    return (impl == 0);
}

bool CellLibrary::load(std::fstream &infile, const std::string &path)
{
    if (!impl)
        return false;
    return impl->load(infile, path);
}

size_t CellLibrary::cellCount() const
{
    if (!impl)
        return 0;
    return impl->cells.size();
}

bool CellLibrary::hasCell(const std::string &type) const
{
    if (!impl)
        return false;
    return impl->hasCell(type);
}

Cell CellLibrary::cell(const std::string &type) const
{
    if (!impl)
        return Cell();
    return impl->cell(type);
}