#include "circuit.h"

Signal::Signal() : value(0)
{
}

Signal::Signal(unsigned s) : value(s)
{
    if (s >= SignalType::Last)
        value = X;
}

Signal Signal::operator& (const Signal &other)
{
    if (this->value == Z || other.value == Z)
        return Z;
    if (this->value == X || other.value == X)
        return X;
    return this->value & other.value;
}

Signal Signal::operator&= (const Signal &other)
{
    this->value = this->value & other.value;
    return this->value;
}

Signal Signal::operator| (const Signal &other)
{
    if (this->value == Z || other.value == Z)
        return Z;
    if (this->value == X || other.value == X)
        return X;
    return this->value | other.value;
}

Signal Signal::operator|= (const Signal &other)
{
    this->value = this->value | other.value;
    return this->value;
}

Signal Signal::operator~ () {
    if (this->value == Z)
        return Z;
    if (this->value == X)
        return X;
    return ~this->value & 1;
}

Signal Signal::operator! () {
    if (this->value == Z)
        return Z;
    if (this->value == X)
        return X;
    return operator~();
}

std::ostream& operator << (std::ostream &os, const Signal &s)
{
    os << s.value;
    return os;
}