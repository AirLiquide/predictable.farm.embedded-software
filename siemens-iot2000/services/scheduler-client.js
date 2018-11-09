'use strict'

const config = require('../config/default')
const logger = require('./logger')

const client = require('/usr/lib/node_modules/socket.io-client')

const MODE_QUEUE = 'queue'

// The connection
var local_service
var isInited = false

// Keep the module name and device id
var device_id
var module_name

// Array to hold all the commands that are given to this module
var cmd_process = []

// Sensible defaults
const defaults = {
  onInit: () => {},
  onConnect: () => {},
  onData: () => {},
  onDisconnect: () => {},
  onSchedule: () => {}
}

const options = {
  path: '/socket.io/socket.io.js', // Limit ECONNRESET errors
  transports: [ 'websocket' ] // Limit to websocket (and not xhr for instance)
}

let init = (options) => {
  options = Object.assign({}, defaults, options)

  module_name = options.module_name
  logger.init('scheduler-' + module_name)

  local_service = client('http://127.0.0.1:' + config.socket.port, options)

  local_service.on('connect', () => {
    logger.log('%s is connected', module_name)
    options.onConnect()
  })

  local_service.on('disconnect', (reason) => {
    logger.log('%s is disconnected : %s', module_name, reason)
    options.onDisconnect()
  })

  local_service.on('device_id', (data) => {
    // logger.log(data)
    device_id = data
    local_service.emit('io', module_name)
    options.onInit()
  })

  local_service.on(module_name, (data) => {
    // logger.log(data)
    try {
      var cmd = JSON.parse(data)
      if (options.mode === MODE_QUEUE) {
        cmd_process.push(cmd)
      }
      options.onData(data)
    } catch (e) {
      logger.log('Bad JSON, ignoring')
      logger.log(cmd)
      // TODO do things based on parameters
      return false // console.error(e);
    }
  })

  local_service.on('scheduler', (data) => {
    // logger.log(data)
    if (data == module_name) {
      // logger.log('%s is required to run', module_name)
      if (options.mode === MODE_QUEUE) {
        while (cmd_process.length > 0) {
          options.onSchedule(cmd_process)
        }
      } else {
        options.onSchedule()
        cmd_process = []
      }

      hasRun()
    }
  })

  isInited = true
  return local_service
}

let hasRun = () => {
  if (isInited) {
    local_service.emit('scheduled', module_name)
  }
}

let sendData = (data, message) => {
  if (isInited) {
    local_service.emit('data', {data: data, message: message})
  }
}

let getDeviceId = () => {
  if (isInited) {
    return device_id
  }
  return null
}

module.exports = {
  constants: {
    MODE_QUEUE: MODE_QUEUE
  },
  init: init,
  hasRun: hasRun,
  sendData: sendData,
  getDeviceId: getDeviceId
}
