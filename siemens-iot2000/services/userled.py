#!/usr/bin/python

# Copyright (C) Air Liquide S.A,  2017
# Author: Sébastien Lalaurette, La Factory, Creative Foundry
# This file is part of Predictable Farm project.
# 
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# 
# See the LICENSE.txt file in this repository for more information.

import mraa
import sys
import os

def redled(state):
 redLedFile = "/sys/class/leds/mpio_uart_led:red:user/brightness"
 if(not os.path.isfile(redLedFile)):
  print("Red LED not available")
  return
 file = open(redLedFile,'w')
 if(1==state):
  file.write("1")
 else:
  file.write("0")
 file.close()

def greenled(state):
 ledpin = mraa.Gpio(13)
 ledpin.dir(mraa.DIR_OUT)
 if(1==state):
  ledpin.write(1)
 else:
  ledpin.write(0)

if(len(sys.argv)<2):
 print("Usage: " + sys.argv[0] + " color")
 print("color:")
 print("0\t\tout")
 print("1\t\tgreen")
 print("2\t\tred")
 print("3\t\torange")
 sys.exit()

x = int(sys.argv[1])

if(x%2 == 0):
 greenled(0)
else:
 greenled(1)

if(x/2 == 0):
 redled(0)
else:
 redled(1)
