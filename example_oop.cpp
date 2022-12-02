#include <pybind11/pybind11.h>

/*
Building from CLI:
c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3-config --includes) -Iextern/pybind11/include example_oop.cpp -o example_oop$(python3-config --extension-suffix)
-> requires cpython includes and pybin11 header only (here gotten as a git submodule) 
*/

namespace py = pybind11;

// to be able to use "arg"_a shorthand
using namespace pybind11::literals;


// Second module: OOP

struct Pet {
    Pet(const std::string &name) : name(name) { }
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }

    std::string name;
};


PYBIND11_MODULE(example_oop, m) {
    py::class_<Pet>(m, "Pet")
        .def(py::init<const std::string &>())
        .def("setName", &Pet::setName)
        .def("getName", &Pet::getName);
}