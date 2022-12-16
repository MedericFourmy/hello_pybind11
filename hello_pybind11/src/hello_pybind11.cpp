#include "hello_pybind11/functions.h"
#include "hello_pybind11/oop.h"
#include "hello_pybind11/eigen_conv.h"


PYBIND11_MODULE(hello_pybind11, m) {
    def_examples_func(m);
    def_examples_oop(m);
    def_examples_eigen_conv(m);
}
