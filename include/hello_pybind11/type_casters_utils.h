#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
// #include <pybind11/numpy.h>

#include <Eigen/Dense>
#include <Eigen/Geometry>


// TOREMOVE
#include <iostream>
//



// To be able to use std::shared_ptr class_ holder type (default is std::unique_ptr -> "the object is deallocated when Python’s reference count goes to zero.") 
// PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)

namespace pybind11 {
namespace detail {

namespace py = pybind11;
namespace E = Eigen;
using py_farray_t = typename py::array_t<double, py::array::f_style | py::array::forcecast>;


/* !! using a templatized type_caster is actually very important to support all functions that require Quaternions
 * If we only use E::Quaterniond, then we will have the following error when calling `eig_quatf_mult`
 *       TypeError: eig_quatf_mult(): incompatible function arguments. The following argument types are supported:
 *           1. (arg0: E::Quaternion<float, 0>, arg1: E::Quaternion<float, 0>) -> E::Quaternion<float, 0>
 * 
 **/
template <typename Scalar>
class type_caster<E::Quaternion<Scalar>> {
 public:
  /**
   * 
   * This macro establishes the name 'E::Quaternion<Scalar>' in
   * function signatures, displayed when calling help(function), and declares a local variable
   * 'value' of type E::Quaternion<Scalar> to be used in "load" method.
   * 
   * Note: "A type_caster<T> defined with PYBIND11_TYPE_CASTER(T, ...) requires that T is default-constructible (value is first default constructed and then load() assigns to it)."
   * 
   * See https://github.com/pybind/pybind11/blob/0694ec6a15863bff2e0ea5efe07c78de39b9a33c/include/pybind11/cast.h#L85
   * for macro definition
   */
  PYBIND11_TYPE_CASTER(E::Quaternion<Scalar>, _("E::Quaternion<Scalar>"));

  /**
   * Conversion part 1 (Python->C++): convert a PyObject
   * into a E::Quaternion<Scalar> instance
   * The second argument indicates whether implicit conversions should be applied.
   */
  bool load(py::handle src, bool convert) {
    if (py::isinstance<py::array>(src)) {

      py::array_t<double> array = src.cast<py::array_t<double>>();
      if (array.ndim() == 1 && array.shape(0) == 4) {
        auto r = array.unchecked<1>(); // static check of ndim

        value.x() = r(0);
        value.y() = r(1);
        value.z() = r(2);
        value.w() = r(3);
        return true;
      }
    }
    return false;
  }

  /**
   * Conversion part 2 (C++ -> Python): convert a E::Quaternion<Scalar>
   * instance into a PyObject
   */
  static py::handle cast(const E::Quaternion<Scalar>& src,
                         py::return_value_policy /* policy */,
                         py::handle /* parent */) {
    py::array_t<double> array(4);
    array.mutable_at(0) = src.x();
    array.mutable_at(1) = src.y();
    array.mutable_at(2) = src.z();
    array.mutable_at(3) = src.w();
    return array.release();
  }
};



// For instance, if we need a typedef E::Transform<float, 3, E::Affine> Transform3fA; to be used:
// We can templatize the scalar type but why not the _Mode as well 

template <typename Scalar>
class type_caster<E::Transform<Scalar, 3, E::Affine>> {
 public:
  using TransformTplt = E::Transform<Scalar, 3, E::Affine>;

  PYBIND11_TYPE_CASTER(TransformTplt, _("E::Transform<Scalar, 3, E::Affine>"));

  /**
   * Python array->C++ E::Transform)
   */
  bool load(py::handle src, bool convert) {
    if (py::isinstance<py::array>(src)) {
      // takes a 4x4 matrix
      py::array_t<double> array = src.cast<py::array_t<double>>();
      if (array.ndim() == 2 && array.shape(0) == 4 && array.shape(1) == 4) {

        py::buffer_info buff_info = array.request();
        // buf.ptr is a "void *" type -> needs to be casted to "double *" before use
        double *ptr = static_cast<double *>(buff_info.ptr);

        E::Map<const E::Matrix<double,4,4,E::RowMajor>> matmap(ptr);

        value.matrix() = matmap;
        // 
        std::cout << "Loading Transform\n";
        std::cout << value.matrix() << "\n";

        return true;
      }
    }
    return false;
  }

  /**
   * Conversion part 2 (C++ -> Python)
   */
  static py::handle cast(const E::Transform<Scalar, 3, E::Affine>& src,
                         py::return_value_policy /* policy */,
                         py::handle /* parent */) {
    // Test SIMPLE idea
    // Problem with array ordering -> Eigen by default is column-major (Fortran style) while numpy default is row-major (C style)
    py_farray_t array({4,4}, src.data());

    return array.release();
  }
};


}  // namespace detail
}  // namespace pybind11