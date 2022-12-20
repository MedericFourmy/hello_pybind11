#include "hello_pybind11/eigen_conv.h"
#include "hello_pybind11/type_casters_utils.h"

#include <pybind11/eigen.h>

#include <Eigen/Dense>

namespace py = pybind11;
// to be able to use "arg"_a shorthand
using namespace pybind11::literals;
using namespace Eigen;

Matrix3d eig_add_mat3d(Matrix3d m1, Matrix3d m2){
    return m1 + m2;
}

Matrix4d eig_compose_affine_mat(Matrix4d t1, Matrix4d t2){
    return (Affine3d(t1) * Affine3d(t2)).matrix();
}

Affine3d eig_compose_affine(Affine3d t1, Affine3d t2){
    return t1 * t2;
}

Vector3f eig_cref(Ref<const Vector3f> v, double x){
    return v * x;
}

const Vector3f eig_ccref(Ref<const Vector3f> v, double x){
    return v * x;
}

void eig_inplace_multiply_f(Ref<Vector3f> v, double x){
    v *= x;
}

void eig_inplace_multiply_d(Ref<VectorXd> v, double x){
    v *= x;
}

class ClassEigen {
    MatrixXd big_mat = MatrixXd::Zero(10000, 10000);
public:
    MatrixXd &getMatrix() { return big_mat; }
    const MatrixXd &viewMatrix() { return big_mat; }
};


Quaterniond eig_quatd_mult(Quaterniond q1, Quaterniond q2){
    return q1 * q2;
}

Quaternionf eig_quatf_mult(Quaternionf q1, Quaternionf q2){
    return q1 * q2;
}



void def_examples_eigen_conv(py::module &m) {
    m.def("eig_add_mat3d", &eig_add_mat3d, "A function that adds two 3x3 matrices");
    m.def("eig_compose_affine", &eig_compose_affine, "Compose Eigen transformations -> Compiles but bug on python side because not in/out implicit comversion!");
    m.def("eig_compose_affine_mat", &eig_compose_affine_mat, "Compose Eigen transformations");
    m.def("eig_cref", &eig_cref, "Checking const ref");
    m.def("eig_ccref", &eig_ccref, "Checking const ref");
    m.def("eig_inplace_multiply_f", &eig_inplace_multiply_f, "Inplace multiply float");
    m.def("eig_inplace_multiply_d", &eig_inplace_multiply_d, "Inplace multiply double");

    py::class_<ClassEigen>(m, "ClassEigen")
        .def(py::init<>())
        .def("copy_matrix", &ClassEigen::getMatrix) // Makes a copy!
        .def("get_matrix", &ClassEigen::getMatrix, py::return_value_policy::reference_internal)
        .def("view_matrix", &ClassEigen::viewMatrix, py::return_value_policy::reference_internal)
        ;
    
    m.def("eig_quatd_mult", &eig_quatd_mult, "Multiply two float quaternions");
    m.def("eig_quatf_mult", &eig_quatf_mult, "Multiply two double quaternions");

}
