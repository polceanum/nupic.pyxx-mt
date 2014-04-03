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

int main()
{
    int numberOfThreads = 3;
    
    std::vector<boost::shared_ptr<PyWrapper> > nupics;
    
    for (int i=0; i<numberOfThreads; ++i)
    {
        boost::shared_ptr<PyWrapper> nu = boost::shared_ptr<MyController>(new MyController()); //create a controller object
        
        nu->start();
        
        nupics.push_back(nu);
    }
    
    //wait a while before stopping the threads
    boost::this_thread::sleep(boost::posix_time::milliseconds(5000)); //5 seconds
    
    for (int i=0; i<numberOfThreads; ++i)
    {
        nupics[i]->stop();
    }
    
    return 0;
}
