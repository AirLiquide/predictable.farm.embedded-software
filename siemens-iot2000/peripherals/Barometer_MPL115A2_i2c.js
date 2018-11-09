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

const config = require('../config/default')
const logger = require('../services/logger')
const utils = require('../services/utils')
const scheduler = require('../services/scheduler-client')

const mraa = require('/usr/lib/node_modules/mraa')

logger.init('barometer')
logger.log('MRAA Version: %s ', mraa.getVersion())

/* =========================================================================
    I2C ADDRESS/BITS
    ----------------------------------------------------------------------- */
const MPL115A2_ADDRESS = 0x60 // 1100000
/* ========================================================================= */

/* =========================================================================
    REGISTERS
    ----------------------------------------------------------------------- */
const MPL115A2_REGISTER_PRESSURE_MSB = 0x00
const MPL115A2_REGISTER_PRESSURE_LSB = 0x01
const MPL115A2_REGISTER_TEMP_MSB = 0x02
const MPL115A2_REGISTER_TEMP_LSB = 0x03
const MPL115A2_REGISTER_A0_COEFF_MSB = 0x04
const MPL115A2_REGISTER_A0_COEFF_LSB = 0x05
const MPL115A2_REGISTER_B1_COEFF_MSB = 0x06
const MPL115A2_REGISTER_B1_COEFF_LSB = 0x07
const MPL115A2_REGISTER_B2_COEFF_MSB = 0x08
const MPL115A2_REGISTER_B2_COEFF_LSB = 0x09
const MPL115A2_REGISTER_C12_COEFF_MSB = 0x0A
const MPL115A2_REGISTER_C12_COEFF_LSB = 0x0B
const MPL115A2_REGISTER_STARTCONVERSION = 0x12
/* ========================================================================= */

var i2cDevice = new mraa.I2c(0)
i2cDevice.address(MPL115A2_ADDRESS)

var _mpl115a2_a0 = 0.0
var _mpl115a2_b1 = 0.0
var _mpl115a2_b2 = 0.0
var _mpl115a2_c12 = 0.0

/**************************************************************************/
/*!
    @brief  Gets the factory-set coefficients for this particular sensor
*/
/**************************************************************************/
function Barometer_readCoefficients () {
  var a0coeff
  var b1coeff
  var b2coeff
  var c12coeff

  a0coeff = i2cDevice.readWordReg(MPL115A2_REGISTER_A0_COEFF_MSB)
  b1coeff = i2cDevice.readWordReg(MPL115A2_REGISTER_B1_COEFF_MSB)
  b2coeff = i2cDevice.readWordReg(MPL115A2_REGISTER_B2_COEFF_MSB)
  c12coeff = i2cDevice.readWordReg(MPL115A2_REGISTER_C12_COEFF_MSB)

  _mpl115a2_a0 = Number(a0coeff).toFixed(8) / 8
  _mpl115a2_b1 = Number(b1coeff).toFixed(8) / 8192
  _mpl115a2_b2 = Number(b2coeff).toFixed(8) / 16384
  _mpl115a2_c12 = Number(c12coeff).toFixed(8)
  _mpl115a2_c12 /= 4194304.0
}

var pressurekPa
var tempCelcius
var tempRAW = 0

function getP () {
  var pressureRAW
  var pressureComp

  // pressureRAW = (i2cDevice.readWordReg(MPL115A2_REGISTER_PRESSURE_MSB) >> 6);

  pressureRAW = (i2cDevice.readReg(MPL115A2_REGISTER_PRESSURE_MSB) << 8)
  pressureRAW |= i2cDevice.readReg(MPL115A2_REGISTER_PRESSURE_LSB)
  pressureRAW = pressureRAW >> 6

  // See datasheet p.6 for evaluation sequence
  pressureComp = _mpl115a2_a0 + (_mpl115a2_b1 + _mpl115a2_c12 * tempRAW) * pressureRAW + _mpl115a2_b2 * tempRAW

  // Return pressure and temperature as floating point values
  pressurekPa = ((65.0 / 1023.0) * pressureComp) + 50.0 // kPa
  return pressurekPa.toFixed(1)
}

function Barometer_getPressure () {
  i2cDevice.writeReg(MPL115A2_REGISTER_STARTCONVERSION, 0x00)
  utils.sleep(10)
  var p = getP()
  if (p < 600) // meaning atmospheric pressure @ 4000m above see we don't send it , sensor may have an issue
  { send_pressure(p) }
}

function getT () {
  tempRAW = (i2cDevice.readReg(MPL115A2_REGISTER_TEMP_MSB) << 8)
  tempRAW |= i2cDevice.readReg(MPL115A2_REGISTER_TEMP_LSB)
  tempRAW = tempRAW >> 6

  // convert
  tempCelcius = (Number(tempRAW).toFixed(2) - 498.0) / -5.35 + 25.0 // C
  return tempCelcius.toFixed(1)
}

function Barometer_getTemperature () {
  var tempCelcius
  var tempRAW
  i2cDevice.writeReg(MPL115A2_REGISTER_STARTCONVERSION, 0x00)
  utils.sleep(10)
  var t = getT()
  send_temperature(t)
}

Barometer_readCoefficients()

function _send_data (type, value) {
  if (sendDataEnabled && scheduler.getDeviceId()) {
    var sensorValue = value

    if (isNaN(sensorValue) || Number(sensorValue) == 0) {
      logger.log(sensorValue + ' is not a valid ' + type + ' , exit')
      return
    }

    var msg2send = JSON.stringify({
      device_id: scheduler.getDeviceId().toString(),
      sensor_type: type,
      sensor_value: sensorValue.toString()
    })

    // logger.log(msg2send)
    scheduler.sendData(msg2send)
  }
}

function send_pressure (pressure) {
  _send_data('air_pressure', pressure)
}

function send_temperature (temp) {
  _send_data('air_temperature', temp)
}

var sendDataEnabled = false

var local_service = scheduler.init({
  module_name: 'air_pressure',
  onInit: () => { local_service.emit('io', 'air_temperature'); sendDataEnabled = true },
  onDisconnect: () => { sendDataEnabled = false },
  onSchedule: () => {
    Barometer_getPressure()
    Barometer_getTemperature()
  }
})

/*
// DEBUG ONLY
var loop = setInterval(function () {
    Barometer_getPressure();
}, 5000);
*/
