'use strict'

const config = require('../config/default')
const logger = require('../services/logger')
const utils = require('../services/utils')
const scheduler = require('../services/scheduler-client')

const mraa = require('/usr/lib/node_modules/mraa')

logger.init('rh')
logger.log('MRAA Version: %s ', mraa.getVersion())

var temp_unit_celcius = true
const SHT3X_I2C_ADDRESS = 0x44
const SENSOR_div = 65535 // 2^16 -1
const RH_mult = 100
const TcShift = -45
const TfShift = -49
const TcMult = 175
const TfMult = 315

var mTemperature
var mHumidity

var i2cDevice = new mraa.I2c(0)
i2cDevice.address(SHT3X_I2C_ADDRESS)

function _send_data (value) {
  if (sendDataEnabled && scheduler.getDeviceId()) {
    var sensorValue = value

    if (isNaN(sensorValue) || Number(sensorValue) == 0) {
      logger.log(sensorValue + 'is not a valid RH number , exit')
      return
    }

    var msg2send = JSON.stringify({
      device_id: scheduler.getDeviceId().toString(),
      sensor_type: 'air_humidity',
      sensor_value: sensorValue.toString()
    })

    // logger.log(msg2send)
    scheduler.sendData(msg2send)
  }
}

function periodic_data_acquisition (startStop) {
  i2cDevice.writeReg(0x30, 0x66) // stop internal heater
  if (startStop == 'start') i2cDevice.writeReg(0x20, 0x24) // periodic data acquisition every 500ms // medium
  else i2cDevice.writeReg(0x30, 0x93) // stop periodic data acquisition
}

periodic_data_acquisition('start')

function fetch_data () {
  var data = null
  i2cDevice.writeReg(0xE0, 0x00) // fetch data
  try {
    // var s = get_status();
    // logger.log("status :"+s);
    data = i2cDevice.read(6)
  } catch (err) {
    // logger.log("measurement not ready :"+err);
  }

  return data
}

function get_status () {
  i2cDevice.writeReg(0xf3, 0x2d)
  return i2cDevice.read(2)
}

function one_shot_data () {
  i2cDevice.writeReg(0x2c, 0x0d) // singleshot measurement clock stretching
  // i2cDevice.writeReg(0x24,0x0d); // singleshot measurement  no clk stretch
  return i2cDevice.read(6)
}

function RH_measure () {
  var data = fetch_data()

  scheduler.hasRun()

  if (!data) {
    logger.log('no data')
    periodic_data_acquisition('stop')
    utils.sleep(500)
    periodic_data_acquisition('start')
    return false
  }

  // convert to Temperature/Humidity
  var val
  val = (data[0] << 8) + data[1]
  if (temp_unit_celcius) mTemperature = TcShift + TcMult * (val / SENSOR_div)
  else mTemperature = TfShift + TfMult * (val / SENSOR_div)
  mTemperature = mTemperature.toFixed(1)

  val = (data[3] << 8) + data[4]
  mHumidity = RH_mult * (val / SENSOR_div)
  mHumidity = mHumidity.toFixed(1)

  // logger.log("temp:"+mTemperature);
  logger.log('rh:' + mHumidity)
  _send_data(mHumidity)

  return true
}

var sendDataEnabled = false

var local_service = scheduler.init({
  module_name: 'air_humidity',
  onInit: () => { sendDataEnabled = true },
  onConnect: () => { periodic_data_acquisition('start') },
  onDisconnect: () => { periodic_data_acquisition('stop'); sendDataEnabled = false },
  onSchedule: RH_measure
})

/*
// DEBUG ONLY
var mcu_loop = setInterval(function () {
  RH_measure();
}, 5000);
*/
