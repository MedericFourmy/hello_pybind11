#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
// #include <pybind11/numpy.h>

namespace py = pybind11;

// To be able to use std::shared_ptr class_ holder type (default is std::unique_ptr -> "the object is deallocated when Python’s reference count goes to zero.") 
// PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)

namespace pybind11 {
namespace detail {

template <typename Scalar>
class type_caster<Eigen::Quaternion<Scalar>> {
 public:
  /**
   * This macro establishes the name 'Quaternion' in
   * function signatures and declares a local variable
   * 'value' of type Eigen::Quaternion<Scalar>.
   * 
   * Note: "A type_caster<T> defined with PYBIND11_TYPE_CASTER(T, ...) requires that T is default-constructible (value is first default constructed and then load() assigns to it)."
   */
  PYBIND11_TYPE_CASTER(Eigen::Quaternion<Scalar>, _("Eigen::Quaternion<Scalar>"));

  /**
   * Conversion part 1 (Python->C++): convert a PyObject
   * into a Eigen::Quaternion<Scalar> instance
   */
  bool load(py::handle src, bool) {
    // If src is a quaternion, extract its values
    if (py::isinstance<py::array>(src)) {
      // Scalar toto = 2.0; // OK!
    //   py::array_t<Scalar> array = src.cast<py::array_t<Scalar>>();  // NOT OK! -> Check some examples from pydrake
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
   * Conversion part 2 (C++ -> Python): convert a Eigen::Quaternion<Scalar>
   * instance into a PyObject
   */
  static py::handle cast(const Eigen::Quaternion<Scalar>& src,
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

}  // namespace detail
}  // namespace pybind11