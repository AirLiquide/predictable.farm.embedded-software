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
  log_file.write(prefix + util.format.apply(null, placeholders) + '\n')
}

let init = (module_name_var) => {
  module_name = module_name_var

  const logfilePath = path.join(__dirname, '..', LOG_DIR, module_name + '.log')
  log_file = fs.createWriteStream(logfilePath, {flags: 'a'})

  console.log('Registering module')
}

module.exports = {
  init: init,
  log: console.log
}
