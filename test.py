import example

print('Functions')
print('add:')
print(example.add(1,3))
print('attributes:')
print(example.the_answer)
print(example.what)
print('mult:')
print(example.mult())
print(example.mult(2,3))

import example_oop

print()
print('OOP')
p = example_oop.Pet('Jamy')
print(p.getName())
print(p.setName('Jo'))
print(p.getName())
p.name = 'Josy'  # name is explicitely bound and is public
print(p.name)
print(p)  # calls

p2 = example_oop.Pet2('Jose')
p2.name  # name is a property here
print(p2.name)

# Dynamic attributes
# p.random_att_name = 'hey'  # NOPE: dynamic attr not valid by default contrary to python objects 
p2.random_att_name = 'hey'  # OK cause p2 has dynamic attr
# p2.name = 234324  # NOPE: name is declared as string, not dynamic
print(p2.__dict__)