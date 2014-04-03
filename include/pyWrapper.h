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

#ifndef PYTHON_WRAPPER_H
#define PYTHON_WRAPPER_H

#include <boost/python.hpp>
#include <boost/thread/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <map>

class PyWrapper : public boost::enable_shared_from_this<PyWrapper>
{
public:
    PyWrapper();
	virtual ~PyWrapper();
	
    void start();
    void stop();
    
protected:
    void _workerThreadLoop();
    virtual void _initialize()=0; //gets called by _workerThreadLoop once at the begining
    virtual void _step()=0; //gets called in a loop by _workerThreadLoop while the thread is running
    
    bool _working;
    boost::mutex _mutex;
    boost::shared_ptr<boost::thread> _thread;
    
    //--------------------------------------------------------------//
    // Python function call interface
    void py_call(std::string);
    void py_call(std::string, std::map<std::string, float>);
    
    template<class T>
    T py_rcall(std::string);
    
    template<class T>
    T py_rcall(std::string, std::map<std::string, float>);
    //--------------------------------------------------------------//
    
    // <<Singleton>>
    class PythonContextManager
    {
    public:
        virtual ~PythonContextManager();
        
        boost::python::dict createContextFromFile(std::string);
        
        static boost::shared_ptr<PythonContextManager> getInstance();
        
        void print_py_error();
        
        PyInterpreterState* getState() { return _interpreterState; }
        
    private:
        PythonContextManager();
        static boost::shared_ptr<PythonContextManager> _instance;
        boost::python::dict _globalDict;
        boost::python::object _main_module;
        PyThreadState *_state;
        PyInterpreterState* _interpreterState;
    };
    
    //--------------------------------------------------------------//
        
    class PyExternalUser
    {
        /**
         ** ************************************************************************************************************************* **
         ** The PyExternalUser class was created by Daniel Pocock (see: http://danielpocock.com/embedding-python-multi-threaded-cpp)  **
         ** ************************************************************************************************************************* **
         ** Copyright 2013 Daniel Pocock http://danielpocock.com  All rights reserved.                                                **
         ** ************************************************************************************************************************* **
         */
    public:
        PyExternalUser(PyInterpreterState* interpreterState) : mInterpreterState(interpreterState),mThreadState(PyThreadState_New(mInterpreterState)) {}
        
        class Use
        {
        public:
            Use(PyExternalUser& user) : mUser(user)
            {
                PyEval_RestoreThread(mUser.getThreadState());
            }
            ~Use()
            {
                mUser.setThreadState(PyEval_SaveThread());
            }
        private:
            PyExternalUser& mUser;
        };
        
        friend class Use;
        
    protected:
        PyThreadState* getThreadState()
        {
            return mThreadState;
        }
        void setThreadState(PyThreadState* threadState)
        {
            mThreadState = threadState;
        }
        
    private:
        PyInterpreterState* mInterpreterState;
        PyThreadState* mThreadState;
    };
    
    //--------------------------------------------------------------//
    PyExternalUser* _pyUser;
    boost::python::dict _context;
	
private:
	
};

template<class T>
T PyWrapper::py_rcall(std::string func)
{
    T result;
    PyExternalUser::Use use(*_pyUser);
    try
    {
        try
        {
            boost::python::object f = boost::python::extract<boost::python::object>(_context[func.c_str()]);
            if(f)
            {
               result = boost::python::extract<T>(f()); //call the function !
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
    
    return result;
}

template<class T>
T PyWrapper::py_rcall(std::string func, std::map<std::string, float> args)
{
    T result;
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
                
                result = boost::python::extract<T>(f(dictData)); //call the function !
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
    
    return result;
}

#endif
