#include <pybind11/pybind11.h>

/*
Building from CLI:
c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3-config --includes) -Iextern/pybind11/include example.cpp -o example$(python3-config --extension-suffix)
-> requires cpython includes and pybin11 header only (here gotten as a git submodule) 
*/

namespace py = pybind11;

// to be able to use "arg"_a shorthand
using namespace pybind11::literals;


int add(int i, int j) {
    return i + j;
}

// refactoring the definitions
int mult(int i, int j) {
    return i * j;
}
void def_mult(py::module &m) {
    m.def("mult", &mult, "Multiply 2 stuffs");
    m.def("mult", &mult, "Multiply 2 stuffs", "i"_a, "j"_a);
    m.def("mult", &mult, "Multiply 2 stuffs", "i"_a=2, "j"_a=4);
}

// example: module name
// m: variable of type py::module_, main interface for creating bindings
// module_::def(): creates binding code exposing the add function
PYBIND11_MODULE(example, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    // arguments without any name
    m.def("add", &add, "A function that adds two numbers");
    // name arguments
    m.def("add", &add, "A function that adds two numbers", py::arg("i"), py::arg("j"));
    // name arguments with shorthand (requires pybind11::literals namespace)
    m.def("add", &add, "A function that adds two numbers", "i"_a, "j"_a);
    // default argument values
    m.def("add", &add, "A function that adds two numbers", py::arg("i")=1, py::arg("j")=2);
    m.def("add", &add, "A function that adds two numbers", "i"_a=1, "j"_a=2);
    // Here, we have overloaded the add functions, which is not normally possible in python! 

    def_mult(m);

    // exporting variables as module attributes
    m.attr("the_answer") = 42;
    py::object world = py::cast("World");
    m.attr("what") = world;
}
