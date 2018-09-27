# libCircuit

libCircuit is a C++ Library for EDA software development

*WARNING: Alpha version is not suitable for practical usage*

## Features
* Support Verilog circuit format (*.v)
* Support Liberty format (*.lib)
* Simple to use
* Easy to compile

## Requirement
* Qt 5.8 (Untested on lower version but it may be fine)
* Bison and Flex

## Usage
Get the source code:
```sh
$ git clone https://github.com/gh-code/libCircuit.git
```

Compile:
```sh
$ cd libCircuit/
$ qmake
$ make
```

Run:
```sh
$ ./circuit c17.v
```

## Integrate into your project

```sh
$ cp -r libCircuit/include/ libCircuit/lib/ your_project/
$ cd your_project/
```

Assume there's a main.cpp in your_project directory.
Add a line at the top of main.cpp
```c++
#include <circuit.h>
```

Compile your Qt program by simply adding
```
-I./include -L./lib -lCircuit
```
For example
```sh
$ g++ -std=c++11 -o main main.cpp -L/opt/Qt/5.8/gcc_64/lib -lQt5Core -I./include -L./lib -lCircuit
```

## Examples

Check circuit is created successfully
```C++
Circuit circuit("c17.v");
if (!circuit.isNull())
{
    // You can use circuit now
}
```

If circuit is synthesized, Cell Library has to be considered
```C++
CellLibrary library("NangateOpenCellLibrary_typical_conditional.lib");
Circuit circuit("c17_syn.v", library);
if (circuit.isNull())
{
    cerr << "Fail to load the circuit" << endl;
    return 1;
}
// You can use circuit now
```

Print circuit information
```C++
int main()
{
    Circuit circuit("c17.v");
    cout << "Circuit: " << circuit.name() << endl;
    cout << "#input: " << circuit.inputSize() << endl;
    cout << "#output: " << circuit.outputSize() << endl;
    cout << "Gate count: " << circuit.gateCount() << endl;
    return 0;
}
```

Forward traverse all the circuit elements
```C++
void forward(const Node &node)
{
    static int level = 0;
    string spaces(2 * level, ' ');
    cout << spaces << node.nodeName() << endl;
    for (size_t i = 0; i < node.outputSize(); i++)
    {
        level++;
        forward(node.output(i));
        level--;
    }
}

int main()
{
    Circuit circuit("c17.v");
    for (size_t i = 0; i < circuit.inputSize(); i++)
        forward(circuit.inputPort(i));
    return 0;
}

```

Backward traverse all the circuit elements
```C++
void backward(const Node &node)
{
    static int level = 0;
    string spaces(2 * level, ' ');
    cout << spaces << node.nodeName() << endl;
    for (size_t i = 0; i < node.inputSize(); i++)
    {
        level++;
        backward(node.input(i));
        level--;
    }
}

int main()
{
    Circuit circuit("c17.v");
    for (size_t i = 0; i < circuit.outputSize(); i++)
        backward(circuit.outputPort(i));
    return 0;
}

```

## Changelog

## Issues
* Multiple module is not handled yet
* Reference counting issue

## License
GNU Lesser General Public License (LGPL) version 3  
Copyright (c) 2017 Gary Huang
