#include "hello_pybind11/oop.h"

namespace py = pybind11;
// to be able to use "arg"_a shorthand
using namespace pybind11::literals;



// Base class
struct Pet {
    Pet(const std::string &name) : name(name) { }
    void setName(const std::string &name_) { name = name_; }
    const std::string &getName() const { return name; }

    std::string name;
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


// Custom Constructors 
// 1: private constructors, happens quite rarely :
// acccording to https://www.geeksforgeeks.org/can-constructor-private-cpp/
// usecases involve Using Friend Classes (only some classes can create this object class), Singleton design pattern, Named Constructor design pattern 
// Possible to bind factory function as static function

class ExampleCCprivate {
    private:
        ExampleCCprivate(int i) : i_(i) {} // private constructor, original example

    public:
        // Factory function:
        static ExampleCCprivate create(int a) { return ExampleCCprivate(a);} 
        int i_; // public just to avoid defining a getter
};

// NOT possible because ExampleCCprivate::ExampleCCprivate constructor is private!
// ExampleCCprivate create(int a) { return ExampleCCprivate(a);} 

// If constructor is public, constructor function should be available outside
class ExampleCCpublic {
    public:
        ExampleCCpublic(int i) : i_(i) {} // public constructor

        int i_; // public just to avoid defining a getter
};

// implements a static cast in the function and then pass to the constructor (is it bad practice?)
ExampleCCpublic create_pub(double a) { return ExampleCCpublic((int)a);} 


// // Even if constructor is private, we can create a base class
// class PySimpleHack: public ExampleCCprivate {
//     public:
//         PySimpleHack(double d) {
//             ExampleCCprivate::ExampleCCprivate((int)i);
//         }
// };


// When a constructor with appropriate arguments does not exist on C++ side
void custom_constructors(py::module &m){
    py::class_<ExampleCCprivate>(m, "ExampleCCprivate")
        .def(py::init(&ExampleCCprivate::create))
        .def_readwrite("i", &ExampleCCprivate::i_);
        // .def(py::init(&create));  // NOPE

    py::class_<ExampleCCpublic>(m, "ExampleCCpublic")
        .def(py::init(&create_pub))
        .def_readwrite("i", &ExampleCCpublic::i_);
        // .def(py::init(&create));  // NOPE

    // py::class_<PySimpleHack>(m, "ExampleCC")
    //     .def(py::init<double>())
    //     .def_readwrite("i", &PySimpleHack::i_);

};

void def_examples_oop(py::module &m) {
    basic_class_def(m);
    checking_inheritance_polymorphism(m);
    overloading_functions(m);
    internal_types(m);
    custom_constructors(m);
}