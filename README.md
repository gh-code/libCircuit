# libCircuit

libCircuit is a C++ Library for EDA software development

*WARNING: Alpha version.*

## Requirement
* Qt 5.2 (Untested on lower version but may be fine)
* Bison and Flex

## Usage
Get the source code:
```sh
$ git clone git@cialab.nctu.me:/EDA/libCircuit.git
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

## Examples

Check circuit is created successfully
```C++
Circuit circuit("c17.v");
if (!circuit.isNull())
{
    // You can use circuit now
}
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
* Primitive gate is not handled yet
* Multiple module is not handled yet
* Module/cell call-by-port-order is not handled yet
* Reference counting issue

## License
GNU Lesser General Public License (LGPL) version 3
Copyright (c) 2017 Gary Huang
