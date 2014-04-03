# ----------------------------------------------------------------------
# Numenta Platform for Intelligent Computing (NuPIC)
# Copyright (C) 2013, Numenta, Inc.  Unless you have an agreement
# with Numenta, Inc., for a separate license for this software code, the
# following terms and conditions apply:
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see http://www.gnu.org/licenses.
#
# http://numenta.org/licenses/
# ----------------------------------------------------------------------

from collections import deque
import time
import random

from nupic.data.inference_shifter import InferenceShifter
from nupic.frameworks.opf.modelfactory import ModelFactory
from nupic.frameworks.opf.clamodel import CLAModel

import imp
modelPath = "../test/py/model_params.py" #relative path, assuming we are in the ./build directory when running the program
model_params = imp.load_module("model_params",open(modelPath),modelPath,('py','U',imp.PY_SOURCE))

###################################################################################################################

model = None
shifter = None

###################################################################################################################

def initialize():
    global model, shifter
    
    model = ModelFactory.create(model_params.MODEL_PARAMS)
    model.enableInference({'predictedField': 'x'})
    print "model initialized !"
    
    shifter = InferenceShifter()
    print "shifter initialized !"
    

# Get the Game Event usage.
def run(modelInput):
    global model, shifter
    
    result = model.run(modelInput)
    
    # Update the trailing predicted and actual value deques.
    inference = shifter.shift(result).inferences['multiStepBestPredictions'][5]
    
    if inference is None:
        inference = 0.0
    
    return inference
    

###################################################################################################################

if __name__ == "__main__":
    print "CLATemporal __main__"
    
