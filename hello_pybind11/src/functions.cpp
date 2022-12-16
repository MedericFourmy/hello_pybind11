#include <pybind11/pybind11.h>

#include "hello_pybind11/functions.h"

namespace py = pybind11;
// to be able to use "arg"_a shorthand
using namespace pybind11::literals;


int add(int i, int j) {
    return i + j;
}

// check default parameter behavior
int mult(int i, int j=2);  // !! default C++ arguments are only syntactic sugar: pybind11 cannot handle them!
int mult(int i, int j) {
    return i * j;
}

void def_add(py::module &m) {
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
}

void def_mult(py::module &m) {
    m.def("mult", &mult, "Multiply 2 stuffs");
    m.def("mult", &mult, "Multiply 2 stuffs", "i"_a, "j"_a);
    m.def("mult", &mult, "Multiply 2 stuffs", "i"_a=2, "j"_a=4);
}

void def_module_attributes(py::module &m) {
    // exporting variables as module attributes
    m.attr("the_answer") = 42;
    py::object world = py::cast("World");
    m.attr("what") = world;
}
    
void def_examples_func(py::module &m) {
    def_add(m);
    def_mult(m);
    def_module_attributes(m);
}