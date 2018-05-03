'use strict'

const config = require('../config/default')
const logger = require('../services/logger')
const scheduler = require('../services/scheduler-client')

const mraa = require('/usr/lib/node_modules/mraa')
const fs = require('fs')

logger.init('relay_native')
logger.log('MRAA Version: %s ', mraa.getVersion())

const RELAY_MODE_AUTO = '0'
const RELAY_MODE_MANUAL = '1'

const MODE_LOCAL = 0
const MODE_REMOTE = 1


var current_relay_state = '0'
var relay_state = '0'
var relay_mode = '0'
var relay = new mraa.Gpio(8)// setup digital read on pin 8
var extBtn = new mraa.Gpio(7)// setup digital read on pin 8

logger.log(' * init relay ')
relay.dir(mraa.DIR_OUT)
relay.write(0)

extBtn.dir(mraa.DIR_IN)

function _send_data () {
  if (sendDataEnabled && scheduler.getDeviceId()) {
    var msg2send = JSON.stringify({
      device_id: scheduler.getDeviceId().toString(),
      sensor_type: 'relay1',
      sensor_value: relay_state.toString(),
      sensor_mode: relay_mode.toString()
    })

    // logger.log('send :' + msg2send)
    scheduler.sendData(msg2send)
  }
}

function isUserButtonPressed () {
  var contents = fs.readFileSync('/sys/class/gpio/gpio63/value', 'utf8')
  var value = parseInt(contents.toString()) ? 0 : 1
  // logger.log('User = ' + value)
  return (value == 1)
}

function isExtBtnButtonPressed () {
  // var value = parseInt(extBtn.read()) ? 0 : 1
  // logger.log('EXT = ' + value)
  // return (value == 1)
  return false
}

function toggleRelay () {
  // logger.log("relay : set to " + relay_state+ " mode = " + relay_mode);
  relay.write(Number(relay_state))
  if((current_relay_state != relay_state))
  {
    current_relay_state = relay_state
    _send_data() // send data msg on change
  }
}

var loop = 0
var btnState = 0
var auto_off = null

function auto_off_start()
{
  logger.log('auto off start')
  if (!auto_off) {
    logger.log('auto off started')
    auto_off = setTimeout(function () {
      relay_state = 0
      logger.log('auto off stopped after completion')
      auto_off = null
      _send_data() // auto off change broadcast
    }, 3000)
  }
}
function auto_off_stop()
{
  logger.log('auto off stop')
  if (auto_off) {
    clearTimeout(auto_off)
    logger.log('auto off canceled')
  }
}

function isRelay (relayNumber, command) {
  try {
    var cmd = JSON.parse(command)
  } catch (e) {
    logger.log('isRelay   Bad JSON, ignoring')
    logger.log(cmd)
    return false // console.error(e);
  }
  if ((cmd.sensor_type.slice(0, -1) == 'relay') && (Number(cmd.sensor_type.split('relay').pop()) == Number(relayNumber))) {
    return true
  } else {
    logger.log(cmd.sensor_type.slice(0, -1) + '---' + Number(cmd.sensor_type.split('relay').pop()))
    return false
  }
}

function getRelayState (command) {
  try {
    var cmd = JSON.parse(command)
  } catch (e) {
    logger.log('getRelayState   Bad JSON, ignoring')
    logger.log(cmd)
    return -1 // console.error(e);
  }
  // logger.log(command+" --- state = "+Number(cmd.sensor_value)+ " raw ="+cmd.sensor_value);

  return Number(cmd.sensor_value)
}

function getRelayMode (command) {
  try {
    var cmd = JSON.parse(command)
  } catch (e) {
    logger.log('getRelayMode   Bad JSON, ignoring')
    logger.log(cmd)
    return -1 // console.error(e);
  }
  if (cmd.hasOwnProperty('sensor_mode')) {
    var mode = Number(cmd.sensor_mode)
    // logger.log(command+" --- mode = "+Number(cmd.sensor_mode)+ " raw ="+cmd.sensor_mode);
    return mode
  } else return -1
}

var sendDataEnabled = false

var local_service = scheduler.init({
  module_name: 'relay',
  onInit: () => { sendDataEnabled = true },
  onData: (data) => {
    logger.log('relay command from cloud : ' + data)
    if (isRelay(1, data) == true) {
      var rstate = getRelayState(data)
      if (rstate != relay_state) _send_data() // change broadcast
      var mode = getRelayMode(data)
      if (mode < 0 || mode ==  0) // command is comming from automation engine
      {
        if (relay_mode == RELAY_MODE_MANUAL) {
          logger.log('relay : automation engine command discarded as relay is in manual mode')
          return
        } else // auto mode
        {
          relay_state = rstate
          if (relay_state == 1) // if auto mode set relay to on, set it back to off automatically after 3 sec
          {
            auto_off_start()
          }
        }
      } else // manual command from dashboard
      {
        relay_state = rstate
      }
      if(relay_mode != mode)
      {
        relay_mode = mode
        _send_data()
      }

      if (relay_mode == RELAY_MODE_MANUAL) {
        auto_off_stop()
      }
    }
  },
  onDisconnect: () => { sendDataEnabled = false },
  onSchedule: () => {
    if (loop == 50) {
      _send_data() // send periodically relay state outside of change events
      loop = 0
    } else {
      loop++
      if (isUserButtonPressed() || isExtBtnButtonPressed()) {
        // logger.log("btn down");
        btnState = 1
      } else {
        // logger.log("btn up");
        if (btnState) {
          logger.log('btn click')
          btnState = 0
          relay_mode = RELAY_MODE_MANUAL // force manual mode
          auto_off_stop();
          if (relay_state == '0') relay_state = '1'
          else relay_state = '0'
        }
      }
    }
    toggleRelay()
  }
})
