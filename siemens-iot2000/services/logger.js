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

const fs = require('fs')
const util = require('util')
const path = require('path')

// Where do we put log files relative to this folder
const LOG_DIR = 'logs'

let origlog = console.log
let log_stdout = process.stdout

var module_name = null
var log_file = null

console.log = function (obj, ...placeholders) {
  if (module_name === null) {
    return
  }

  const prefix = '[' + (new Date()).toISOString() + '] ' + module_name + ': '

  if (typeof obj === 'string') {
    placeholders.unshift(prefix + obj)
  } else {
    // This handles console.log( object )
    placeholders.unshift(obj)
    placeholders.unshift(prefix + ' %j')
  }

  origlog.apply(this, placeholders)

  // Log to file too
  log_file.write(util.format.apply(null, placeholders) + '\n')
}

let init = (module_name_var) => {
  module_name = module_name_var

  const logfilePath = path.join(__dirname, '..', LOG_DIR, module_name + '.log')
  log_file = fs.createWriteStream(logfilePath, {flags: 'a'})

  // console.log('Registering module')
}

module.exports = {
  init: init,
  log: console.log
}
