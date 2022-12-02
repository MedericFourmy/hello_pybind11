import example

print('add:')
print(example.add(1,3))
print('attributes:')
print(example.the_answer)
print(example.what)
print('mult:')
print(example.mult())
print(example.mult(2,3))

import example_oop

p = example_oop.Pet('jamy')
print(p.getName())
print(p.setName('Jo'))
print(p.getName())
