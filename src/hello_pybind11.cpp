
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <Eigen/Dense>
#include <Eigen/Geometry> 



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


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////


// Base class
struct Pet {
    Pet(const std::string &name) : name(name) { }
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }

    std::string name;
};

// Non polymorphic inheritance
class Pet2 {
public:
    Pet2(const std::string &name) : name(name) { }
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }
private:
    std::string name;
};

struct Dog : Pet {
    Dog() : Pet("AtomicDog") { }
    Dog(const std::string &name) : Pet(name) { }
    std::string bark() const { return "woof!"; }
};


// Polymorphic inheritance 
struct PolymorphicPet {
    virtual ~PolymorphicPet() = default;
};

struct PolymorphicDog : PolymorphicPet {
    std::string bark() const { return "woof!"; }
};


// Overloading functions
struct Overlord {
    Overlord(const std::string &name, int age) : name(name), age(age) { }

    void set(int age_) { age = age_; }
    void set(const std::string &name_) { name = name_; }
    std::string getName() {return name;}
    int getAge() {return age;}
    int getAge() const {return age;}

    std::string name;
    int age;
};


// Internal types
struct Bird {
    enum Kind {
        Crow = 0,
        Goose
    };

    struct Attributes {
        float age = 0;
    };

    Bird(const std::string &name, Kind type) : name(name), type(type) { }

    std::string name;
    Kind type;
    Attributes attr;
};


void basic_class_def(py::module &m){
    // biding class methods is similar to binding 
    // Init has a special syntax to map itself to the constructor
    // py::init<const std::string &>() is kind of expanding to: "__init__", &Pet::Pet
    // except that C++98 Standard $12.1/10: “The address of a constructor shall not be taken.” -> this syntax is not possible
    py::class_<Pet>(m, "Pet")
        .def(py::init<const std::string &>())  // if not: TypeError: Pet: No constructor defined!
        .def("getName", &Pet::getName)
        .def("setName", &Pet::setName)
        .def("__repr__",
        [](const Pet &a) {
            return "<example.Pet named '" + a.name + "'>";
        })  // possible to bind lambda functions!
        .def_readwrite("name", &Pet::name)  // works only for public variables, also def_readonly for const attributes
        ;
}

void checking_inheritance_polymorphism(py::module &m){
    // Non polymorphic inheritance
    // py::dynamic_attr() makes instance attributes dynamic as usually the case in Python 
    py::class_<Pet2>(m, "Pet2", py::dynamic_attr())
        .def(py::init<const std::string &>())
        .def_property("name", &Pet2::getName, &Pet2::setName);

    // Inheritance: specify base class as extra template parameter
    // Base class bound methods/attributes are also inherited by the derived bindings
    py::class_<Dog, Pet>(m, "Dog")
        .def(py::init<>())
        .def(py::init<const std::string &>())
        .def("bark", &Dog::bark);
    m.def("pet_store", []() { return std::unique_ptr<Pet>(new Dog("Molly")); });  // just to show non-polymorphic inheritance resulting behaviour
    
    // Polymorphic inheritance
    // Same binding code
    py::class_<PolymorphicPet>(m, "PolymorphicPet");
    py::class_<PolymorphicDog, PolymorphicPet>(m, "PolymorphicDog")
        .def(py::init<>())
        .def("bark", &PolymorphicDog::bark);

    // Again, return a base pointer to a derived instance
    m.def("pet_store2", []() { return std::unique_ptr<PolymorphicPet>(new PolymorphicDog); });
}


void overloading_functions(py::module &m){
    // // Heavy notation for c++11 compatibility: have to specify the return type
    // py::class_<Overlord>(m, "Overlord")
    //     .def(py::init<const std::string &, int>())
    //     .def("set", static_cast<void (Overlord::*)(int)>(&Overlord::set), "Set the Overlord's age")
    //     .def("set", static_cast<void (Overlord::*)(const std::string &)>(&Overlord::set), "Set the Overlord's name");
    // from C++14: return type is automatically deduced
    // "name"_a="Morgoth", "age"_a=141
    py::class_<Overlord>(m, "Overlord")
        .def(py::init<const std::string &, int>(), "name"_a="Morgoth", "age"_a=141)
        .def("set", py::overload_cast<int>(&Overlord::set), "Set the overlord's age")
        .def("set", py::overload_cast<const std::string &>(&Overlord::set), "Set the overlord's name")
        .def("getName", py::overload_cast<>(&Overlord::getName), "Get the overlord's name")
        .def("getAge", py::overload_cast<>(&Overlord::getAge), "Get the overlord's age")
        .def("getAge", py::overload_cast<>(&Overlord::getAge, py::const_), "Get the overlord's age");  // const attibute overloading must be specified

}

void internal_types(py::module &m){
    // Internal types
    py::class_<Bird> bird(m, "Bird");
    bird.def(py::init<const std::string &, Bird::Kind>())
        .def_readwrite("name", &Bird::name)
        .def_readwrite("type", &Bird::type)
        .def_readwrite("attr", &Bird::attr);

    py::enum_<Bird::Kind>(bird, "Kind")
        .value("Crow", Bird::Kind::Crow)
        .value("Goose", Bird::Kind::Goose)
        .export_values();

    py::class_<Bird::Attributes>(bird, "Attributes")
        .def(py::init<>())
        .def_readwrite("age", &Bird::Attributes::age);
}

void def_examples_oop(py::module &m) {
    basic_class_def(m);
    checking_inheritance_polymorphism(m);
    overloading_functions(m);
    internal_types(m);
}

////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////

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
    Eigen::MatrixXd big_mat = Eigen::MatrixXd::Zero(10000, 10000);
public:
    Eigen::MatrixXd &getMatrix() { return big_mat; }
    const Eigen::MatrixXd &viewMatrix() { return big_mat; }
};


void def_examples_eigen(py::module &m) {
    m.def("eig_add_mat3d", &eig_add_mat3d, "A function that adds two 3x3 matrices");
    m.def("eig_compose_affine", &eig_compose_affine, "Compose Eigen transformations -> Compiles but bug on python side!");
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
}

///////////////////////////////////////// 
///////////////////////////////////////// 
///////////////////////////////////////// 


PYBIND11_MODULE(hello_pybind11, m) {
    def_examples_func(m);
    def_examples_oop(m);
    def_examples_eigen(m);
}
