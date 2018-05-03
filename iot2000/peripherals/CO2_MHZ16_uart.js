'use strict'

const config = require('../config/default')
const logger = require('../services/logger')
const utils = require('../services/utils')
const scheduler = require('../services/scheduler-client')

const mraa = require('/usr/lib/node_modules/mraa')

logger.init('co2')
logger.log('MRAA Version: %s ', mraa.getVersion())

var uart = new mraa.Uart(0)

uart.setBaudRate(9600)
uart.setMode(8, 0, 1)
uart.setFlowcontrol(false, false)
utils.sleep(200)

var ppm = 0

const cmd_calibrateZero = [0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78]

function CO2_calibrate () {
  var cmd_calibrateZero = new Buffer('FF0187000000000078', 'hex')
  var i = 0
  uart.flush()
  uart.write(cmd_calibrateZero)
  utils.sleep(200)

  logger.log('CO2_calibrate ')
}

function CO2_measurement_error (error) {
  scheduler.hasRun()
  logger.log(error)
  _co2_supply_off() // for security reason
}

function CO2_measure () {
  var i = 0

  var cmd_measure = new Buffer('FF019C000000000063', 'hex')

  // logger.log("CO2_measure");

  // ask co2 sensor to prepare a measurment
  uart.flush()
  uart.write(cmd_measure)

  utils.sleep(200)
  /*
  for (i = 0; i < 9; i++) {
    uart.write(cmd_measure[i]);
  } */
  // logger.log("CO2_measure cmd sent");

  var buf = new Buffer(9)

  // logger.log("CO2_measure update");
  if (uart.dataAvailable(400)) {
    buf = uart.read(9)

    //  logger.log("CO2_measure - " + buf.toString('hex'));
    // if (buf[1] == 134)
    {
      var checksum = new Buffer('00', 'hex')

      for (i = 0; i < 9; i++) {
        checksum[0] += buf[i]
      }

      if (buf[0] == 0xFF && buf[1] == 0x9C) {
        if (checksum[0] == 0xFF) {
          ppm = buf[2] << 24 | buf[3] << 16 | buf[4] << 8 | buf[5]
          if (ppm > 3990) CO2_measurement_error('too high !')
          else {
            scheduler.hasRun()
            _send_data(ppm)
            logger.log('CO2_measure --> ' + ppm)
            // temperature = buf[4] - 40;
          }
          return true
        } else {
          CO2_measurement_error('bad checksum :' + checksum.toString('hex'))
          return false
        }
      } else {
        CO2_measurement_error('bad buf')
        return false
      }
    }
    /* else {
      CO2_measurement_error("bad buf");
      return false;
    } */
  } else {
    CO2_measurement_error('timeout')
  }
}

function _send_data (value) {
  if (sendDataEnabled && scheduler.getDeviceId()) {
    var sensorValue = value

    if (isNaN(sensorValue) || Number(sensorValue) == 0) {
      logger.log(sensorValue + 'is not a valid CO2 number , exit')
      return
    }

    var msg2send = JSON.stringify({
      device_id: scheduler.getDeviceId().toString(),
      sensor_type: 'air_co2',
      sensor_value: sensorValue.toString()
    })

    // logger.log(msg2send)
    scheduler.sendData(msg2send)
  }
}

function _co2_supply_off () {
  if (sendDataEnabled && scheduler.getDeviceId()) {
    var msg2send = JSON.stringify({
      device_id: scheduler.getDeviceId().toString(),
      sensor_type: 'relay1',
      sensor_value: '0'
    })

    logger.log(msg2send)
    scheduler.sendData(msg2send)
  }
}

var sendDataEnabled = false

var local_service = scheduler.init({
  module_name: 'air_co2',
  onInit: () => { sendDataEnabled = true },
  onDisconnect: () => { sendDataEnabled = false },
  onSchedule: CO2_measure
})

/*
// DEBUG ONLY
var mcu_loop = setInterval(function () {
  CO2_measure();
}, 2000)
*/
