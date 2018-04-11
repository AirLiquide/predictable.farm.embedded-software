'use strict'

const config = require('../config/default')
const logger = require('../services/logger')
const scheduler = require('../services/scheduler-client')

const mraa = require('/usr/lib/node_modules/mraa')

logger.init('co2')
logger.log('MRAA Version: %s ', mraa.getVersion())

// const SC16IS750_ADDRESS = 0x48; //0X90
const SC16IS750_ADDRESS = 0x4d // 0X9A
// const SC16IS750_ADDRESS = 0x4b; //0X96

// General Registers
const SC16IS750_REG_RHR = 0x00
const SC16IS750_REG_THR = 0X00
const SC16IS750_REG_IER = 0X01
const SC16IS750_REG_FCR = 0X02
const SC16IS750_REG_IIR = 0X02
const SC16IS750_REG_LCR = 0X03
const SC16IS750_REG_MCR = 0X04
const SC16IS750_REG_LSR = 0X05
const SC16IS750_REG_MSR = 0X06
const SC16IS750_REG_SPR = 0X07
const SC16IS750_REG_TCR = 0X06
const SC16IS750_REG_TLR = 0X07
const SC16IS750_REG_TXLVL = 0X08
const SC16IS750_REG_RXLVL = 0X09
const SC16IS750_REG_IODIR = 0X0A
const SC16IS750_REG_IOSTATE = 0X0B
const SC16IS750_REG_IOINTENA = 0X0C
const SC16IS750_REG_IOCONTROL = 0X0E
const SC16IS750_REG_EFCR = 0X0F

// Special Registers
const SC16IS750_REG_DLL = 0x00
const SC16IS750_REG_DLH = 0X01

// Enhanced Registers
const SC16IS750_REG_EFR = 0X02
const SC16IS750_REG_XON1 = 0X04
const SC16IS750_REG_XON2 = 0X05
const SC16IS750_REG_XOFF1 = 0X06
const SC16IS750_REG_XOFF2 = 0X07

//
const SC16IS750_INT_CTS = 0X80
const SC16IS750_INT_RTS = 0X40
const SC16IS750_INT_XOFF = 0X20
const SC16IS750_INT_SLEEP = 0X10
const SC16IS750_INT_MODEM = 0X08
const SC16IS750_INT_LINE = 0X04
const SC16IS750_INT_THR = 0X02
const SC16IS750_INT_RHR = 0X01

// Application Related

const SC16IS750_CRYSTCAL_FREQ = 14745600
const SC16IS750_PROTOCOL_I2C = 0
const SC16IS750_PROTOCOL_SPI = 1

var i2cDevice = new mraa.I2c(0)
i2cDevice.address(SC16IS750_ADDRESS)

var ppm = 0

var peek_flag = 0

function SC16IS750_flush () {
  var tmp_lsr

  do {
    tmp_lsr = SC16IS750_ReadRegister(SC16IS750_REG_LSR)
  } while ((tmp_lsr & 0x20) == 0)
}

function SC16IS750_peek () {
  if (peek_flag == 0) {
    peek_buf = SC16IS750_ReadByte()
    if (peek_buf >= 0) {
      peek_flag = 1
    }
  }
  return peek_buf
}

function SC16IS750_ReadByte () {
  if (SC16IS750_FIFOAvailableData() == 0) {
    return -1
  } else {
    return SC16IS750_ReadRegister(SC16IS750_REG_RHR)
  }
}

function SC16IS750_WriteByte (val) {
  var tmp_lsr
  /* do {
     logger.log("SC16IS750_WriteByte waiting for LSR ready");
     tmp_lsr = SC16IS750_ReadRegister(SC16IS750_REG_LSR);
   } while ((tmp_lsr & 0x20) == 0);

   logger.log("SC16IS750_WriteByte "+val); */
  SC16IS750_WriteRegister(SC16IS750_REG_THR, val)
}

function SC16IS750_FIFOAvailableData () {
  return SC16IS750_ReadRegister(SC16IS750_REG_RXLVL)
}

function SC16IS750_FIFOEnable (fifo_enable) {
  var temp_fcr

  temp_fcr = SC16IS750_ReadRegister(SC16IS750_REG_FCR)

  if (fifo_enable == 0) {
    temp_fcr &= 0xFE
  } else {
    temp_fcr |= 0x01
  }
  SC16IS750_WriteRegister(SC16IS750_REG_FCR, temp_fcr)
}

function SC16IS750_ResetDevice () {
  var reg

  reg = SC16IS750_ReadRegister(SC16IS750_REG_IOCONTROL)
  reg |= 0x08
  SC16IS750_WriteRegister(SC16IS750_REG_IOCONTROL, reg)
}

function SC16IS750_GPIOGetPinState (pin_number) {
  var temp_iostate

  temp_iostate = SC16IS750_ReadRegister(SC16IS750_REG_IOSTATE)
  if (temp_iostate & (0x01 << pin_number) == 0) {
    return 0
  }
  return 1
}

function SC16IS750_GPIOSetPinState (pin_number, pin_state) {
  var temp_iostate

  temp_iostate = SC16IS750_ReadRegister(SC16IS750_REG_IOSTATE)
  if (pin_state == 1) {
    temp_iostate |= (0x01 << pin_number)
  } else {
    temp_iostate &= ~(0x01 << pin_number)
  }

  SC16IS750_WriteRegister(SC16IS750_REG_IOSTATE, temp_iostate)
}

function SC16IS750_GPIOSetPinMode (pin_number, i_o) {
  var temp_iodir

  temp_iodir = SC16IS750_ReadRegister(SC16IS750_REG_IODIR)
  if (i_o == OUTPUT) {
    temp_iodir |= (0x01 << pin_number)
  } else {
    temp_iodir &= ~(0x01 << pin_number)
  }

  SC16IS750_WriteRegister(SC16IS750_REG_IODIR, temp_iodir)
}

function SC16IS750_available () {
  return SC16IS750_FIFOAvailableData()
}

function SC16IS750_read () {
  if (peek_flag == 0) {
    return SC16IS750_ReadByte()
  } else {
    peek_flag = 0
    return peek_buf
  }
}

function SC16IS750_write (val) {
  SC16IS750_WriteByte(val)
}

function SC16IS750_pinMode (pin, i_o) {
  SC16IS750_GPIOSetPinMode(pin, i_o)
}

function SC16IS750_digitalWrite (pin, value) {
  SC16IS750_GPIOSetPinState(pin, value)
}

function SC16IS750_digitalRead (pin) {
  return SC16IS750_GPIOGetPinState(pin)
}

function SC16IS750_ReadRegister (reg_addr) {
  var result

  var ret = 0
  result = i2cDevice.readReg((reg_addr << 3))
  return result
}

function SC16IS750_WriteRegister (reg_addr, val) {
  i2cDevice.writeReg((reg_addr << 3), val)
}

function SC16IS750_SetBaudrate (baudrate) // return error of baudrate parts per thousand
{
  var divisor
  var temp_lcr

  divisor = (SC16IS750_CRYSTCAL_FREQ) / (baudrate * 16)

  temp_lcr = SC16IS750_ReadRegister(SC16IS750_REG_LCR)
  temp_lcr |= 0x80
  SC16IS750_WriteRegister(SC16IS750_REG_LCR, temp_lcr)
  // write to DLL
  SC16IS750_WriteRegister(SC16IS750_REG_DLL, divisor)
  // write to DLH
  SC16IS750_WriteRegister(SC16IS750_REG_DLH, (divisor >> 8))
  temp_lcr &= 0x7F
  SC16IS750_WriteRegister(SC16IS750_REG_LCR, temp_lcr)
}

function SC16IS750_SetLine () {
  var temp_lcr
  temp_lcr = SC16IS750_ReadRegister(SC16IS750_REG_LCR)
  temp_lcr &= 0xC0 // Clear the lower six bit of LCR (LCR[0] to LCR[5]
  temp_lcr |= 0x03
  SC16IS750_WriteRegister(SC16IS750_REG_LCR, temp_lcr)
}

SC16IS750_ResetDevice()
SC16IS750_FIFOEnable(1)
SC16IS750_SetBaudrate(9600)
SC16IS750_SetLine()

function CO2_measure () {
  // logger.log("CO2_measure");

  // ask co2 sensor to prepare a measurment
  SC16IS750_write(0xFF)
  SC16IS750_write(0x01)
  SC16IS750_write(0x86)
  SC16IS750_write(0x00)
  SC16IS750_write(0x00)
  SC16IS750_write(0x00)
  SC16IS750_write(0x00)
  SC16IS750_write(0x00)
  SC16IS750_write(0x79)

  // logger.log("CO2_measure cmd sent");

  var i = 0
  var buf = new Buffer(9)
  var av
  var timeoutTO = 0
  // logger.log("CO2_measure update");
  while (i < 9) {
    timeoutTO++
    if (av = SC16IS750_available()) {
      buf[i] = SC16IS750_read()
      // logger.log(i+" = "+buf[i]+" = ");
      if (i == 0 && buf[i] != 255) {
        // logger.log("/");
        continue
      } else {
        i++
        // logger.log(".");
      }
    }
    if (timeoutTO == 1000) {
      scheduler.hasRun()
      logger.log('timeout')
      _co2_supply_off() // for security reason
      return
    }
  }

  //  logger.log("CO2_measure - "+buf[1]);
  if (buf[1] == 134) {
    ppm = buf[2] << 8 | buf[3]
    scheduler.hasRun()
    _send_data(ppm)
    logger.log('CO2_measure --> ' + ppm)
    if (ppm > 3990) _co2_supply_off() // for security reason

    // temperature = buf[4] - 40;
    return

    return true
  } else {
    scheduler.hasRun()
    logger.log('CO2_measure bad buf')
    return false
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

    logger.log(msg2send)
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
  CO2_measure()
}, 2000)
*/
