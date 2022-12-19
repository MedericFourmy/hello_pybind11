import os
import sys

# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = '0.0.1'

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

src_files = ['functions.cpp', 'oop.cpp', 'eigen_conv.cpp', 'hello_pybind11.cpp']
sources = [os.path.join('hello_pybind11/src', s) for s in src_files]

ext_modules = [
    Pybind11Extension(
        name='hello_pybind11',  # FUN FACT: if name != module name defined by pybind11 macro -> installs 2 .so file, one being invalid
        sources=sources,
        include_dirs=[
            'include',
            '/usr/include/eigen3',  # Meh
        ]),
]


setup(
    name='hello_pybind11',
    version=__version__,
    author='Mederic Fourmy',
    description='A test project using pybind11',
    long_description='',
    ext_modules=ext_modules,
    # Currently, build_ext only provides an optional 'highest supported C++
    # level' feature, but in the future it may provide more features.
    cmdclass={'build_ext': build_ext},
    zip_safe=False,
    python_requires='>=3.7',
)


# TODO: add runtime dependencies
# numpy-quaternion