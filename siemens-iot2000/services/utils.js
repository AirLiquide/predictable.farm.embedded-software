/*
  Copyright (C) Air Liquide S.A,  2017
  Author: Sébastien Lalaurette and Cyril Chapellier, La Factory, Creative Foundry
  This file is part of Predictable Farm project.

  The MIT License (MIT)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
   
  See the LICENSE.txt file in this repository for more information.
*/

'use strict'

const exec = require('child_process').exec

const GREEN = 1
const RED = 2
const ORANGE = 3

let sleep = (delay) => {
  delay += new Date().getTime()
  while (new Date() < delay) { }
}

let led = (color) => {
  if (color == GREEN || color == RED || color == ORANGE) {
    exec(__dirname + '/userled.py ' + color)
  }
}

module.exports = {
  colors: {
    GREEN: GREEN,
    RED: RED,
    ORANGE: ORANGE
  },
  sleep: sleep,
  led: led
}
