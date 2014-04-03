//----------------------------------------------------------------------------
// The MIT License (MIT)
// 
// Copyright (c) 2014 Mihai Polceanu
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//----------------------------------------------------------------------------

#include "myController.h"

#include <boost/thread.hpp>

MyController::MyController()
{
    _context = PyWrapper::PythonContextManager::getInstance()->createContextFromFile("../test/py/myController.py"); //relative path, assuming we are in the ./build directory when running the program
}

MyController::~MyController()
{
    py_call("stop");
}

void MyController::_initialize()
{
    _pyUser = new PyExternalUser(PyWrapper::PythonContextManager::getInstance()->getState()); //initialize user for this thread
    
    py_call("init"); //call the python init function in 'myController.py' which starts a new process to be used later
    
    while (py_rcall<int>("isReady") == 0) //wait to finish
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(5));
    }
}

void MyController::_step()
{
    //generate some test input for the model
    float input = time(0)%2; //repeating sequence of 0's and 1's
    
    std::map<std::string, float> toSend;
    toSend["x"] = input;
    
    py_call("step", boost::ref(toSend)); //call the python step function in 'myController.py'
    
    while (py_rcall<int>("isReady") == 0) //wait to finish -- guarantees that getStepResult will return a valid value
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(5));
    }
    
    float result = py_rcall<float>("getStepResult"); //retrieve the result (prediction) of the step
    
    std::cerr << "Thread id " << boost::this_thread::get_id() << ": Input=" << input << " : Prediction=" << result << std::endl; //the text output is not synced, so it will probably be mangled when more threads are running.
}
