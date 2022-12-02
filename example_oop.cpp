#include <pybind11/pybind11.h>

/*
Building from CLI:
c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3-config --includes) -Iextern/pybind11/include example_oop.cpp -o example_oop$(python3-config --extension-suffix)
-> requires cpython includes and pybin11 header only (here gotten as a git submodule) 
*/

namespace py = pybind11;

// to be able to use "arg"_a shorthand
using namespace pybind11::literals;


struct Pet {
    Pet(const std::string &name) : name(name) { }
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }

    std::string name;
};

class Pet2 {
public:
    Pet2(const std::string &name) : name(name) { }
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }
private:
    std::string name;
};

struct Dog : Pet {
    Dog(const std::string &name) : Pet(name) { }
    std::string bark() const { return "woof!"; }
};

PYBIND11_MODULE(example_oop, m) {
    py::class_<Pet>(m, "Pet")
        .def(py::init<const std::string &>())
        .def("setName", &Pet::setName)
        .def("getName", &Pet::getName)
        .def("__repr__",
        [](const Pet &a) {
            return "<example.Pet named '" + a.name + "'>";
        })  // possible to bind lambda functions!
        .def_readwrite("name", &Pet::name)  // works only for public variables, also def_readonly for const attributes
        ;

    // py::dynamic_attr() makes instance attributes dynamic as usually the case in Python 
    py::class_<Pet2>(m, "Pet2", py::dynamic_attr())
        .def(py::init<const std::string &>())
        .def_property("name", &Pet2::getName, &Pet2::setName);

    // inheritance
    py::class_<Dog, Pet>(m, "Dog")
        .def(py::init<const std::string &>())
        .def("bark", &Dog::bark);
}