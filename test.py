import example as ex

print('## Functions')
print('add:')
print(ex.add(1,3))
print('attributes:')
print(ex.the_answer)
print(ex.what)
print('mult:')
print(ex.mult())
print(ex.mult(2,3))




import example_oop as exoop

print()
print('## OOP')
p = exoop.Pet('Jamy')
print(p.getName())
# print(p.setName('Jo'))
print(p.getName())
p.name = 'Josy'  # name is explicitely bound and is public
print(p.name)
print(p)  # calls

p2 = exoop.Pet2('Jose')
p2.name  # name is a property here
print(p2.name)

# Dynamic attributes
# p.random_att_name = 'hey'  # NOPE: dynamic attr not valid by default contrary to python objects 
p2.random_att_name = 'hey'  # OK cause p2 has dynamic attr
# p2.name = 234324  # NOPE: name is declared as string, not dynamic
print(p2.__dict__)

# Inheritance
d = exoop.Dog("Milou")
print(d.getName())
d.setName('Loumi')
print(d.name)
print(d.bark())

# Default in C++: inheritance is non polymormic
# So a Pet pointer to a Dog object is seen only as a Pet on python side 
p = exoop.pet_store()
print(type(d))
print(type(p))

# Polymorphic inheritance: just need one virtual function in Base class to change the behavior to a polymorphic type!
# Usual behavior for C++: for polymorphic classes, a Base pointer to a Derived class has access to Derived class functions
pp = exoop.pet_store2()
print(type(pp))
print(pp.bark())

# Overloading
o = exoop.Overlord()
print(o.getName(), o.getAge())
# overloading in action! dispatch based on the type, not usual thing in python
o.set('Sauron') 
o.set(42)
print(o.getName(), o.getAge())

