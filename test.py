import time
import numpy as np
import quaternion  # use as np.quaternion
import hello_pybind11 as hpb

____ = '--------------'

print(____ + '\n# Functions' + ____)
print('add:')
print(hpb.add(1,3))
print('attributes:')
print(hpb.the_answer)
print(hpb.what)
print('mult (overloads):')
print(hpb.mult())
print(hpb.mult(2,3))
print(hpb.mult(2.0,3.0))


print('\n' + ____ + '\n# OOP' + ____)
pet = hpb.Pet('Jamy')
print(pet.getName())
# print(p.setName('Jo'))
print(pet.getName())
pet.name = 'Josy'  # name is explicitely bound and is public
print(pet.name)
print(pet)  # calls

pet2 = hpb.Pet2('Jose')
pet2.name  # name is a property here
print(pet2.name)

# Dynamic attributes
# p.random_att_name = 'hey'  # NOPE: dynamic attr not valid by default contrary to python objects 
pet2.random_att_name = 'hey'  # OK cause p2 has dynamic attr
# p2.name = 234324  # NOPE: name is declared as string, not dynamic
print(pet2.__dict__)

# Inheritance
dog = hpb.Dog()
print(dog.getName())
dog = hpb.Dog("Milou")
print(dog.getName())
dog.setName('Loumi')
print(dog.name)
print(dog.bark())

# Default in C++: inheritance is non polymormic
# So a Pet pointer to a Dog object is seen only as a Pet on python side 
pet = hpb.factory_dog_nonpoly()
dog = hpb.Dog()
print(type(pet))
print(type(dog))

# Polymorphic inheritance: just need one virtual function in Base class to change the behavior to a polymorphic type!
# Usual behavior for C++: for polymorphic classes, a Base pointer to a Derived class has access to Derived class functions
poly_pet = hpb.PolymorphicPet('PolyPetty')
poly_dog = hpb.factory_dog_poly()
print(type(poly_pet))
print(type(poly_dog))
print(poly_dog.bark())

# calling functions with
print(hpb.get_pet_couple(poly_pet, poly_pet))
# print(hpb.get_dog_couple(poly_pet, poly_pet))  # NOPE (normal)
print(hpb.get_dog_couple(poly_dog, poly_dog))
print(hpb.get_pet_couple(poly_dog, poly_dog))

# Overloading
o = hpb.Overlord()
print(o.getName(), o.getAge())
# overloading in action! dispatch based on the type, not usual thing in python
o.set('Sauron') 
o.set(42)
print(o.getName(), o.getAge())

# Internal types
b = hpb.Bird("Lucy", hpb.Bird.Goose)
print(b.type)
print(int(b.type))
print(b.Kind.__members__)  # enumeration types are exposed with "__"
print(b.name)
print(b.attr)

# Custom constructors
epri = hpb.ExampleCCprivate(12)
epub = hpb.ExampleCCpublic(34)
print(epri.i)
print(epub.i)





print('\n' + ____ + 'Eigen' + ____)
# type does not matter when passing by value: casting of array elements is done during copy
print(hpb.eig_add_mat3d(np.eye(3, dtype=np.int32), np.eye(3, dtype=np.float128)))
# print(hpb.compose_affine(np.eye(4), np.eye(4)))  # no implicit casting from numpy array to Eigen::Transform
print(hpb.eig_compose_affine_mat(np.eye(4), np.eye(4)))
# pass by reference, careful of the type!! float64 <-> double, float32 <-> float
arr = np.arange(3, dtype=np.float32)
# behavior of the returned array depends on the return type (const or not)
arr_cref = hpb.eig_cref(arr, -2)
arr_ccref = hpb.eig_ccref(arr, -2)
print(arr_cref)
print(arr.flags.owndata)
print(arr_cref.flags.owndata)
print(arr_cref.flags.writeable)
print(arr_ccref.flags.writeable)  # arr_ccref[0] += 1 forbidden!
hpb.eig_inplace_multiply_f(arr, 2.0)
print(arr)
arr = np.arange(5, dtype=np.float64)
hpb.eig_inplace_multiply_d(arr, 3.0)
print(arr)

a = hpb.ClassEigen()
m = a.get_matrix(); print('m: ', m.flags.owndata, m.flags.writeable)
v = a.view_matrix(); print('v: ', v.flags.owndata, v.flags.writeable)
t = time.time()
# ClassEigen defines a 1e4 x 1e4 = 1e8 size matrix -> copy takes almost a second, normal
c = a.copy_matrix(); print('c: ', c.flags.owndata, c.flags.writeable)
print('a.copy_matrix() took (s): ', time.time() - t)
# m[5,6] and v[5,6] refer to the same element, c[5,6] does not.

print('\n' + ____ + "Quaternions" + ____)
t1 = time.time()
q1 = np.arange(4)
q2 = np.arange(3,-1,step=-1)

# np.quaternion uses the same convention as Eigen for constructing quaternion
# from individual elements BUT also for internal storage (?) / viewing
# np.quaternion: w,x,y,z
qq1 = np.quaternion(q1[3], q1[0], q1[1], q1[2])
qq2 = np.quaternion(q2[3], q2[0], q2[1], q2[2])

print('q1: ', q1, q1.dtype)
qe3 = hpb.eig_quat_mult(q1, q2)
print('qe3: ', qe3, qe3.dtype)
qq3 = qq1 * qq2
print('Check computation is ok')
print('qq3:', qq3.x, qq3.y, qq3.z, qq3.w)

# print('Cast q1, q2 to float32 (C++ floats)')
qf1 = np.array(q1, dtype=np.float32)
print('qf1: ', qf1, qf1.dtype)
qf11 = hpb.eig_quat_mult(qf1, qf1)
print('qf11: ', qf11, qf11.dtype)

print('\n' + ____ + "Raw array" + ____)
# Accessing raw array buffer and modifying it
a = np.arange(8, dtype=np.float64).reshape((2,2,2))
a = np.arange(8).reshape((2,2,2))
print(hpb.sum_3d(a))
a = a.astype(np.float64)  # we have enforced increment_3d to work only with double == float64
hpb.increment_3d(a)
print(hpb.sum_3d(a))

print('\n' + ____ + "Transforms" + ____)
# numpy to Eigen::Transform
m1 = np.arange(16, dtype=np.float64).reshape((4,4))
print("m1\n", m1)
# print("m1 flags\n", m1.flags)
m2 = hpb.pass_through(m1)
print("m2\n", m2)
# print("m2 flags\n", m2.flags)
print("m2.dtype", m2.dtype)

m1 = np.arange(16, dtype=np.float32).reshape((4,4))
m2 = hpb.pass_through(m1)
print("m2\n", m2)
print("m2.dtype", m2.dtype)
