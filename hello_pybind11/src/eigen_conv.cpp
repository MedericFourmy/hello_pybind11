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

// templatized version -> not possible to bind! https://github.com/pybind/pybind11/issues/281#issuecomment-232655034
template <typename Scalar>
Quaternion<Scalar> eig_quat_mult(Quaternion<Scalar> q1, Quaternion<Scalar> q2){
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
    
    // we need to instantiate template functions
    m.def("eig_quat_mult", &eig_quat_mult<double>, "Multiply two float quaternions");
    m.def("eig_quat_mult", &eig_quat_mult<float>, "Multiply two double quaternions");


    m.def("sum_3d", [](py::array_t<double> x) {
        auto r = x.unchecked<3>(); // x must have ndim = 3; can be non-writeable
        double sum = 0;
        for (py::ssize_t i = 0; i < r.shape(0); i++)
            for (py::ssize_t j = 0; j < r.shape(1); j++)
                for (py::ssize_t k = 0; k < r.shape(2); k++)
                    sum += r(i, j, k);
        return sum;
    }, "Sum elements of a 3 dimensional tensfor");
    m.def("increment_3d", [](py::array_t<double> x) {
        auto r = x.mutable_unchecked<3>(); // Will throw if ndim != 3 or flags.writeable is false
        for (py::ssize_t i = 0; i < r.shape(0); i++)
            for (py::ssize_t j = 0; j < r.shape(1); j++)
                for (py::ssize_t k = 0; k < r.shape(2); k++)
                    r(i, j, k) += 1.0;
    }, "Increment a 3 dimensional tensfor", py::arg().noconvert());  // FORBID implicit convesions in array type (e.g. int->double)

}
