'use strict'

const config = require('../config/default')
const logger = require('../services/logger')
const scheduler = require('../services/scheduler-client')

const adc = require('../lib/ADS1015.js')

logger.init('par')

function _send_data (value) {
  if (sendDataEnabled && scheduler.getDeviceId()) {
    light_par = Number(value) * 0.0078125 * 99.7
    var sensorValue = Number(light_par).toFixed(0)

    if (isNaN(sensorValue) || Number(sensorValue) < 0) {
      logger.log(sensorValue + ' is not a valid light_par , exit')
      return
    }

    var msg2send = JSON.stringify({
      device_id: scheduler.getDeviceId().toString(),
      sensor_type: 'light_par',
      sensor_value: sensorValue.toString()
    })

    logger.log(msg2send)
    scheduler.sendData(msg2send)
  }
}

function _dataReadCallback (val) {
  scheduler.hasRun()
  _send_data(val)
}

var sendDataEnabled = false

var local_service = scheduler.init({
  module_name: 'light_par',
  onInit: () => { sendDataEnabled = true },
  onDisconnect: () => { sendDataEnabled = false },
  onSchedule: () => {
    adc.readChannel(3, _dataReadCallback)
  }
})

/*
// DEBUG ONLY
var mcu_loop = setInterval(function () {
    //retrieve raw value from ADC
    adc.readChannel(3, _send_data);
}, 3000);
*/
