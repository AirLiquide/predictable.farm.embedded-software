/*
  Copyright (C) Air Liquide S.A,  2017
  Author: Sébastien Lalaurette, La Factory, Creative Foundry
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

const mraa = require('/usr/lib/node_modules/mraa') // require mraa

/* =========================================================================
    I2C ADDRESS/BITS
    ----------------------------------------------------------------------- */
const ADS1015_ADDRESS = 0x48 // 1001 000 (ADDR = GND)
/* ========================================================================= */

/* =========================================================================
    CONVERSION DELAY (in mS)
    ----------------------------------------------------------------------- */
const ADS1015_CONVERSIONDELAY = 1
const ADS1115_CONVERSIONDELAY = 8
/* ========================================================================= */

/* =========================================================================
    POINTER REGISTER
    ----------------------------------------------------------------------- */
const ADS1015_REG_POINTER_MASK = 0x03
const ADS1015_REG_POINTER_CONVERT = 0x00
const ADS1015_REG_POINTER_CONFIG = 0x01
const ADS1015_REG_POINTER_LOWTHRESH = 0x02
const ADS1015_REG_POINTER_HITHRESH = 0x03
/* ========================================================================= */

/* =========================================================================
    CONFIG REGISTER
    ----------------------------------------------------------------------- */
const ADS1015_REG_CONFIG_OS_MASK = 0x8000
const ADS1015_REG_CONFIG_OS_SINGLE = 0x8000 // Write: Set to start a single-conversion
const ADS1015_REG_CONFIG_OS_BUSY = 0x0000 // Read: Bit = 0 when conversion is in progress
const ADS1015_REG_CONFIG_OS_NOTBUSY = 0x8000 // Read: Bit = 1 when device is not performing a conversion

const ADS1015_REG_CONFIG_MUX_MASK = 0x7000
const ADS1015_REG_CONFIG_MUX_DIFF_0_1 = 0x0000 // Differential P = AIN0, N = AIN1 = default;
const ADS1015_REG_CONFIG_MUX_DIFF_0_3 = 0x1000 // Differential P = AIN0, N = AIN3
const ADS1015_REG_CONFIG_MUX_DIFF_1_3 = 0x2000 // Differential P = AIN1, N = AIN3
const ADS1015_REG_CONFIG_MUX_DIFF_2_3 = 0x3000 // Differential P = AIN2, N = AIN3
const ADS1015_REG_CONFIG_MUX_SINGLE_0 = 0x4000 // Single-ended AIN0
const ADS1015_REG_CONFIG_MUX_SINGLE_1 = 0x5000 // Single-ended AIN1
const ADS1015_REG_CONFIG_MUX_SINGLE_2 = 0x6000 // Single-ended AIN2
const ADS1015_REG_CONFIG_MUX_SINGLE_3 = 0x7000 // Single-ended AIN3

const ADS1015_REG_CONFIG_PGA_MASK = 0x0E00
const ADS1015_REG_CONFIG_PGA_6_144V = 0x0000 // +/-6.144V range = Gain 2/3
const ADS1015_REG_CONFIG_PGA_4_096V = 0x0200 // +/-4.096V range = Gain 1
const ADS1015_REG_CONFIG_PGA_2_048V = 0x0400 // +/-2.048V range = Gain 2 = default;
const ADS1015_REG_CONFIG_PGA_1_024V = 0x0600 // +/-1.024V range = Gain 4
const ADS1015_REG_CONFIG_PGA_0_512V = 0x0800 // +/-0.512V range = Gain 8
const ADS1015_REG_CONFIG_PGA_0_256V = 0x0A00 // +/-0.256V range = Gain 16

const ADS1015_REG_CONFIG_MODE_MASK = 0x0100
const ADS1015_REG_CONFIG_MODE_CONTIN = 0x0000 // Continuous conversion mode
const ADS1015_REG_CONFIG_MODE_SINGLE = 0x0100 // Power-down single-shot mode = default;

const ADS1015_REG_CONFIG_DR_MASK = 0x00E0
const ADS1015_REG_CONFIG_DR_128SPS = 0x0000 // 128 samples per second
const ADS1015_REG_CONFIG_DR_250SPS = 0x0020 // 250 samples per second
const ADS1015_REG_CONFIG_DR_490SPS = 0x0040 // 490 samples per second
const ADS1015_REG_CONFIG_DR_920SPS = 0x0060 // 920 samples per second
const ADS1015_REG_CONFIG_DR_1600SPS = 0x0080 // 1600 samples per second = default;
const ADS1015_REG_CONFIG_DR_2400SPS = 0x00A0 // 2400 samples per second
const ADS1015_REG_CONFIG_DR_3300SPS = 0x00C0 // 3300 samples per second

const ADS1015_REG_CONFIG_CMODE_MASK = 0x0010
const ADS1015_REG_CONFIG_CMODE_TRAD = 0x0000 // Traditional comparator with hysteresis = default;
const ADS1015_REG_CONFIG_CMODE_WINDOW = 0x0010 // Window comparator

const ADS1015_REG_CONFIG_CPOL_MASK = 0x0008
const ADS1015_REG_CONFIG_CPOL_ACTVLOW = 0x0000 // ALERT/RDY pin is low when active = default;
const ADS1015_REG_CONFIG_CPOL_ACTVHI = 0x0008 // ALERT/RDY pin is high when active

const ADS1015_REG_CONFIG_CLAT_MASK = 0x0004 // Determines if ALERT/RDY pin latches once asserted
const ADS1015_REG_CONFIG_CLAT_NONLAT = 0x0000 // Non-latching comparator = default;
const ADS1015_REG_CONFIG_CLAT_LATCH = 0x0004 // Latching comparator

const ADS1015_REG_CONFIG_CQUE_MASK = 0x0003
const ADS1015_REG_CONFIG_CQUE_1CONV = 0x0000 // Assert ALERT/RDY after one conversions
const ADS1015_REG_CONFIG_CQUE_2CONV = 0x0001 // Assert ALERT/RDY after two conversions
const ADS1015_REG_CONFIG_CQUE_4CONV = 0x0002 // Assert ALERT/RDY after four conversions
const ADS1015_REG_CONFIG_CQUE_NONE = 0x0003 // Disable the comparator and put ALERT/RDY in high state (default)
const GAIN_TWOTHIRDS = ADS1015_REG_CONFIG_PGA_6_144V
const GAIN_ONE = ADS1015_REG_CONFIG_PGA_4_096V
const GAIN_TWO = ADS1015_REG_CONFIG_PGA_2_048V
const GAIN_FOUR = ADS1015_REG_CONFIG_PGA_1_024V
const GAIN_EIGHT = ADS1015_REG_CONFIG_PGA_0_512V
const GAIN_SIXTEEN = ADS1015_REG_CONFIG_PGA_0_256V

var i2cDevice = new mraa.I2c(0)
i2cDevice.address(ADS1015_ADDRESS)

var busy = false

function _readChannel (callback) {
  callback(i2cDevice.readWordReg(ADS1015_REG_POINTER_CONVERT))// >> m_bitShift;
  busy = false
}

(function () {
  const readADC = function (channel, callback) {
    while (busy);

    busy = true

    // Start with default values
    var config = ADS1015_REG_CONFIG_CQUE_NONE | // Disable the comparator (default val)
      ADS1015_REG_CONFIG_CLAT_NONLAT | // Non-latching (default val)
      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1015_REG_CONFIG_CMODE_TRAD | // Traditional comparator (default val)
      ADS1015_REG_CONFIG_DR_1600SPS | // 1600 samples per second (default)
      ADS1015_REG_CONFIG_MODE_SINGLE // Single-shot mode (default)

    // Set single-ended input channel
    switch (channel) {
      case (0):// UV
        config |= GAIN_SIXTEEN
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_0
        break
      case (1):
        config |= GAIN_ONE
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_1
        break
      case (2):// TEMPERATURE
        config |= GAIN_TWO
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_2
        break
      case (3):// PAR
        config |= GAIN_SIXTEEN
        config |= ADS1015_REG_CONFIG_MUX_SINGLE_3
        break
    }

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE

    // Write config register to the ADC
    i2cDevice.writeWordReg(ADS1015_REG_POINTER_CONFIG, config)

    // Wait for the conversion to complete
    var getValue = setTimeout(() => {
      _readChannel(callback)
    }, ADS1115_CONVERSIONDELAY)
  }
  module.exports.readChannel = function (channel, callback) {
    readADC(channel, callback)
  }
}())
