##----------------------------------------------------------------------------
## The MIT License (MIT)
## 
## Copyright (c) 2014 Mihai Polceanu
## 
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to deal
## in the Software without restriction, including without limitation the rights
## to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
## copies of the Software, and to permit persons to whom the Software is
## furnished to do so, subject to the following conditions:
## 
## The above copyright notice and this permission notice shall be included in all
## copies or substantial portions of the Software.
## 
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
## AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
## OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
## SOFTWARE.
##----------------------------------------------------------------------------

import sys
import signal
import time
from multiprocessing import Process, Queue, Value

import imp
modelPath = "../test/py/claTemporal.py" #relative path, assuming we are in the ./build directory when running the program
claTemporal = imp.load_module("claTemporal",open(modelPath),modelPath,('py','U',imp.PY_SOURCE))

########################################################################################################
## This side is run in a separate process ##############################################################

proc_ready = Value('i', 0)

def proc_run(qIn, qOut):
    global proc_initialized, proc_stepFinished
    
    while True:
        with proc_ready.get_lock():
            proc_ready.value = 1
            
        cmd = qIn.get()
        
        with proc_ready.get_lock():
            proc_ready.value = 0
        
        #print "Python process: ", cmd
        
        if cmd == 'quit' or cmd == 'fquit': ## normal exit ## forced exit
            return
            
        elif cmd == 'init': ## initialize
            claTemporal.initialize() ## initalize CLA model
            
        elif cmd == 'step': ## process step
            args = qIn.get() ## wait for command arguments (a dictionary)
            if args == 'quit' or args == 'fquit': ## in case arguments are never received due to program crash
                return
            proc_step(args, qOut)
            

def proc_step(args, qOut):
    result = claTemporal.run(args)
    qOut.put(result)
    

########################################################################################################
## This side is run in the main thread #################################################################

commandQueue = Queue()
resultQueue = Queue()
proc = Process(target=proc_run, args=(commandQueue,resultQueue))

#####################################################

def init(): ## requests initialization
    proc.daemon = True
    proc.start()
    commandQueue.put('init')

def stop(): ## request process end
    commandQueue.put('quit')
    proc.join()

def step(args): ## requests the process to execute step on the model
    commandQueue.put('step') # send command
    commandQueue.put(args) # send command arguments

def getStepResult(): ## returns step result
    result = resultQueue.get()
    return result

def isReady():
    result = 0
    with proc_ready.get_lock():
        result = proc_ready.value
    return result


#####################################################

########################################################################################################
## Some help for the process to exit when the main program is aborted ##################################

def signal_handler(signal, frame):
    print('You pressed Ctrl+C!')
    commandQueue.put('fquit')
    proc.join()
    sys.exit("Python module terminated")
signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)


if __name__ == '__main__':
    print "Hello PyProcess world !"
