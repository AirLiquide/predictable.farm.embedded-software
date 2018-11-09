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

WORKDIR=/home/root/predictable-farm

cd $WORKDIR

echo "Setting orange LED"
./services/userled.py 3 #set led to orange

# See https://link.springer.com/chapter/10.1007/978-1-4302-6838-3_14
# Pin IO4 => BUS I2C
echo "Exporting and setting PIN gpio6 (IO4) - Bus I2C"
mraa-gpio set 04 0 # Deactivate bus I2C
sleep 2
mraa-gpio set 04 01 # Activate bus I2C

# Pin IOA1 => 24V/VIN
echo "Exporting and setting PIN gpio50 (IO15 / ADC A1) - 24V/VIN"
# mraa-gpio set 15 01

echo "Starting forever with configuration file"
# Why setting home here ? See https://github.com/foreverjs/forever/issues/415
HOME=/home/root/ forever -p /home/root/.forever start forever.json


# Info : to force deactivation of PIN gpio6 (IO4) - Bus I2C
# echo "out" > /sys/class/gpio/gpio6/direction
# echo 0 > /sys/class/gpio/gpio6/value
