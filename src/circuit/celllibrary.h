#ifndef CELLLIBRARY_H
#define CELLLIBRARY_H

#include "circuit.h"
#include <string>

// Forward declaration
class CellLibraryPrivate;

class CellLibrary
{
public:
    CellLibrary();
    CellLibrary(const std::string &path);
    CellLibrary(const CellLibrary&);
    CellLibrary& operator= (const CellLibrary&);
    bool operator== (const CellLibrary&) const;
    bool operator!= (const CellLibrary&) const;
    ~CellLibrary();

    // Properties
    std::string name() const;
    std::string time_unit() const;
    std::string leakage_power_unit() const;
    std::string voltage_unit() const;
    std::string current_unit() const;
    std::string pulling_resistance_unit() const;
    std::string capacitive_load_unit() const;
    double nom_process() const;
    double nom_temperature() const;
    double nom_voltage() const;

    bool hasCell(const std::string &type) const;
    Cell cell(const std::string &type) const;

    bool isNull() const;

    bool load(std::fstream &infile, const std::string &path);

private:
    CellLibraryPrivate *impl;
    CellLibrary(CellLibraryPrivate*);
};

#endif // CELLLIBRARY_H