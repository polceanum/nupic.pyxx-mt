cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)

INCLUDE_DIRECTORIES(
    ./include
    ./test/include #for compilation of test

    /usr/include/python2.7
)

LINK_DIRECTORIES(
	
)

LINK_LIBRARIES(
    
    #-- BOOST --#
    boost_thread-mt
    
    #-- Python Wrapper --#
    boost_python-mt-py27
    python2.7
)

ADD_EXECUTABLE(testNupicWrapper
    
    ## PyWrapper sources ##
    ./src/pyWrapper.cpp
    
    ## Test sources
    ./test/src/myController.cpp
    ./test/testNupicWrapper.cpp
)
