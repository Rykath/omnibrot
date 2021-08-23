# setup.py 
#  python package installation script using setuptools / cython
#  see also: setup.cfg & pyproject.toml
# 
# https://cython.readthedocs.io/en/latest/src/userguide/source_files_and_compilation.html
from setuptools import setup
from Cython.Build import cythonize

if __name__ == "__main__":
    setup(
    #    ext_modules = cythonize("deepixel/pyFPHPN.pyx")
    )

