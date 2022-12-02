#include <pybind11/pybind11.h>

/*
Building from CLI:
c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3-config --includes) -Iextern/pybind11/include example.cpp -o example$(python3-config --extension-suffix)
-> requires cpython includes and pybin11 header only (here gotten as a git submodule) 
*/

namespace py = pybind11;

int add(int i, int j) {
    return i + j;
}

// example: module name
// m: variable of type py::module_, main interface for creating bindings
// module_::def(): creates binding code exposing the add function
PYBIND11_MODULE(example, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    m.def("add", &add, "A function that adds two numbers");
}