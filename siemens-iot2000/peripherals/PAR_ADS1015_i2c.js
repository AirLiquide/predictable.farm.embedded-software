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
const scheduler = require('../services/scheduler-client')

logger.init('par')

const adc = require('../lib/ADS1015.js')

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

    // logger.log(msg2send)
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
