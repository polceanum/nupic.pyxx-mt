nupic.pyxx-mt
=============

This code allows you to access and use python code from a multithreaded C++ program. Its main purpose is to provide a temporary way to use the nupic library (https://github.com/numenta/nupic) from C++, until the official nupic.core (https://github.com/numenta/nupic.core) is extracted.

Dependencies
============
libboost-thread-dev (tested with version 1.46 of boost)
libboost-python-dev (tested with version 1.46 of boost)
libpython2.7

Usage
=====
Go to download directory (where CMakeLists.txt is located) and:

    mkdir build
    cd build
    cmake ..
    make
    ./testNupicWrapper

Important note: testNupicWrapper assumes it is being run in the build folder, so it searches for the python files in ../test/py/ folder
