nupic.pyxx-mt
=============

This code allows you to access and use python code from a multithreaded C++ program. Its main purpose is to provide a temporary way to use the nupic library (https://github.com/numenta/nupic) from C++, until the official nupic.core (https://github.com/numenta/nupic.core) is extracted.

In order to enable true multithreading with python, a special interface is used (implemented in myController.py) which manages a separate python process where the nupic algorithm is run. This works using polling the python interpreter from the C++ side to know when the python process has finished a job; each C++ thread will wait until their assigned process is ready in order to pass other commands.

Dependencies
============
 * libboost-thread-dev (tested with version 1.46 of boost)
 * libboost-python-dev (tested with version 1.46 of boost)
 * libpython2.7

Usage
=====
Go to the download directory (where CMakeLists.txt is located) and:

    mkdir build
    cd build
    cmake ..
    make
    ./testNupicWrapper

Important note: testNupicWrapper assumes it is being run in the build folder, so it searches for the python files in ../test/py/ folder
