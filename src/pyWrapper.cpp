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

#include "pyWrapper.h"

PyWrapper::PyWrapper()
{
	
}

PyWrapper::~PyWrapper()
{
	
}

void PyWrapper::_workerThreadLoop()
{
    _initialize();
    
    while (true)
    {
        _mutex.lock();
        
        if (_working)
        {
            _step();
        }
        else
        {
            _mutex.unlock(); //don't forget to leave the door open
            break; //stops thread loop
        }
        
        _mutex.unlock();
        
        boost::this_thread::sleep(boost::posix_time::milliseconds(5));
    }
}

void PyWrapper::start()
{
    bool working = false;
	_mutex.lock();
        working = _working;
    _mutex.unlock();
    
    if (!working)
    {
        _working = true;
        _thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&PyWrapper::_workerThreadLoop, shared_from_this()))); //spawns thread that starts executing _workerThreadLoop
    }
    else
    {
        std::cerr << "Cannot start thread: already running" << std::endl;
    }
}

void PyWrapper::stop()
{
	_mutex.lock();
		_working = false;
	_mutex.unlock();
	if (_thread)
	{
		_thread->join(); //wait for thread to finish
	}
}

boost::shared_ptr<PyWrapper::PythonContextManager> PyWrapper::PythonContextManager::_instance = boost::shared_ptr<PythonContextManager>();

boost::shared_ptr<PyWrapper::PythonContextManager> PyWrapper::PythonContextManager::getInstance()
{
    if (!_instance)
    {
        _instance = boost::shared_ptr<PythonContextManager>(new PythonContextManager());
    }
    
    return _instance;
}

PyWrapper::PythonContextManager::PythonContextManager()
{
    Py_Initialize();
    PyEval_InitThreads();
    
    try
    {
        _main_module = boost::python::import("__main__");
        _globalDict = boost::python::extract<boost::python::dict>(_main_module.attr("__dict__"));
        
        _state = PyEval_SaveThread();
        
        _interpreterState = _state->interp;
    }
    catch(const std::exception &e)
    {
        std::cerr << "Exception in script: " << e.what() << "\n";
    }
}

PyWrapper::PythonContextManager::~PythonContextManager()
{
    //Py_Finalize(); //seems to crash if used
}

boost::python::dict PyWrapper::PythonContextManager::createContextFromFile(std::string fileName)
{
    boost::python::dict localDict = _globalDict.copy();
    try
    {
        PyEval_RestoreThread(_state);
        
        try
        {
            boost::python::exec_file(fileName.c_str(), localDict, localDict);
        }
        catch(const boost::python::error_already_set &e)
        {
            std::cerr << "Exception in script: ";
            print_py_error();
        }
        
        _state = PyEval_SaveThread();
    }
    catch(const std::exception &e)
    {
        std::cerr << "Exception in script: " << e.what() << "\n";
    }
    
    return localDict;
}

void PyWrapper::PythonContextManager::print_py_error()
{
    try
    {
        PyErr_Print();
        boost::python::object sys(boost::python::handle<>(PyImport_ImportModule("sys")));
        boost::python::object err = sys.attr("stderr");
        std::string err_text = boost::python::extract<std::string>(err.attr("getvalue")());
        std::cerr << err_text << "\n";
    }
    catch (...)
    {
        std::cerr << "Failed to parse python error\n";
    }
    PyErr_Clear();
}

void PyWrapper::py_call(std::string func)
{
    PyExternalUser::Use use(*_pyUser);
    try
    {
        try
        {
            boost::python::object f = boost::python::extract<boost::python::object>(_context[func.c_str()]);
            if(f)
            {
                f(); //call the function !
            }
            else
            {
                std::cerr << "Script did not have a " << func << " function!\n";
            }
        }
        catch(const boost::python::error_already_set &e)
        {
            std::cerr << "Exception in script: ";
            PyWrapper::PythonContextManager::getInstance()->print_py_error();
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << "Exception in script: " << e.what() << "\n";
    }
}

void PyWrapper::py_call(std::string func, std::map<std::string, float> args)
{
    PyExternalUser::Use use(*_pyUser);
    try
    {
        try
        {
            boost::python::object f = boost::python::extract<boost::python::object>(_context[func.c_str()]);
            if(f)
            {
                boost::python::dict dictData;
                std::map<std::string, float>::iterator iter;
                for (iter = args.begin(); iter != args.end(); ++iter)
                {
                    dictData[iter->first] = iter->second;
                }
                
                f(dictData); //call the function !
            }
            else
            {
                std::cerr << "Script did not have a " << func << " function!\n";
            }
        }
        catch(const boost::python::error_already_set &e)
        {
            std::cerr << "Exception in script: ";
            PyWrapper::PythonContextManager::getInstance()->print_py_error();
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << "Exception in script: " << e.what() << "\n";
    }
}
