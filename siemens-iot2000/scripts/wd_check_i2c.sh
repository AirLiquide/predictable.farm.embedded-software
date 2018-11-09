#!/bin/sh

# Copyright (C) Air Liquide S.A,  2017
# Author: Sébastien Lalaurette and Cyril Chapellier, La Factory, Creative Foundry
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

# while true
# do
if mraa-i2c get 0 0x27 0 | grep "Register 00 = 00" > /dev/null
then
  echo "i2c ok"
  # exit 0
else
  echo "i2c ko"
  # Cycling power on pin IO 4 (gpio6)
  # Make sure pin is an output
  echo "out" > /sys/class/gpio/gpio6/direction
  # Set to 0, wait 2 seconds, back to 1
  echo 0 > /sys/class/gpio/gpio6/value
  sleep 2 # secs
  echo 1 > /sys/class/gpio/gpio6/value
  sleep 2 # secs
  # I2C should be back up
  # exit 255 #reboot
fi
# done