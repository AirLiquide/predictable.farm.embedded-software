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

const mraa = require('/usr/lib/node_modules/mraa')

logger.init('lcd')
logger.log('MRAA Version: %s ', mraa.getVersion())

var i2cDevice = new mraa.I2c(0)
i2cDevice.address(0x51)

const FontModeRegAddr = 1
const CharXPosRegAddr = 2
const CharYPosRegAddr = 3

const CursorConfigRegAddr = 16
const CursorXPosRegAddr = 17
const CursorYPosRegAddr = 18
const CursorWidthRegAddr = 19
const CursorHeightRegAddr = 20 // 8

const DisRAMAddr = 32 // 9
const ReadRAM_XPosRegAddr = 33
const ReadRAM_YPosRegAddr = 34
const WriteRAM_XPosRegAddr = 35
const WriteRAM_YPosRegAddr = 36

const DrawDotXPosRegAddr = 64 // 14
const DrawDotYPosRegAddr = 65

const DrawLineStartXRegAddr = 66
const DrawLineEndXRegAddr = 67
const DrawLineStartYRegAddr = 68
const DrawLineEndYRegAddr = 69

const DrawRectangleXPosRegAddr = 70 // 20
const DrawRectangleYPosRegAddr = 71
const DrawRectangleWidthRegAddr = 72
const DrawRectangleHeightRegAddr = 73
const DrawRectangleModeRegAddr = 74

const DrawCircleXPosRegAddr = 75
const DrawCircleYPosRegAddr = 76
const DrawCircleRRegAddr = 77
const DrawCircleModeRegAddr = 78

const DrawBitmapXPosRegAddr = 79
const DrawBitmapYPosRegAddr = 80
const DrawBitmapWidthRegAddr = 81
const DrawBitmapHeightRegAddr = 82 // 31

const DisplayConfigRegAddr = 128 // 32
const WorkingModeRegAddr = 129
const BackLightConfigRegAddr = 130
const ContrastConfigRegAddr = 131
const DeviceAddressRegAddr = 132

const ArrowLeft = 0x1c
const ArrowRight = 0x1d
const ArrowUp = 0x1e
const ArrowDown = 0x1f

const WHITE = 0x00
const BLACK = 0xff

const TRUE = 1
const FALSE = 0

const BLACK_BAC = 0x00
const BLACK_NO_BAC = 0x40
const WHITE_BAC = 0x80
const WHITE_NO_BAC = 0xc0

const DisplayMode_AllREV = 0x00
const DisplayMode_AllNOR = 0x80

const OFF = 0x00
const ON = 0x01

const Font_6x8 = 0x00
const Font_6x12 = 0x01
const Font_8x16_1 = 0x02
const Font_8x16_2 = 0x03
const Font_10x20 = 0x04
const Font_12x24 = 0x05
const Font_16x32 = 0x06

const FM_ANL_AAA = 0x00 // FM_AutoNewLine_AutoAddrAdd
const FM_ANL_MAA = 0x10 // FM_AanualNewLine_ManualAddrAdd
const FM_MNL_MAA = 0x30 // FM_ManualNewLine_ManualAddrAdd
const FM_MNL_AAA = 0x20 // FM_ManualNewLine_AutoAddrAdd

const WM_CharMode = 0x00 // Common mode, put char or dot
const WM_BitmapMode = 0x01
const WM_RamMode = 0x02 // 8Bit deal mode, deal with 8-bit RAM directly

const LOAD_TO_RAM = 0x00
const LOAD_TO_EEPROM = 0x80

var DisplayConfigRegVal = DisplayMode_AllREV // DisplayMode_AllNOR;

function ReadByteFromReg (regAddr) {
  return i2cDevice.readReg((regAddr))
}

function WriteByteToReg (regAddr, buf) {
  i2cDevice.writeReg(regAddr, buf)
}

function WriteSeriesToReg (regAddr, buf, length) {
  var data = new Buffer(length + 1)
  data[0] = regAddr
  var i
  for (i = 0; i < length; i++) {
    data[i + 1] = Number(buf[i])
  }
  i2cDevice.write(data)
}

function FontModeConf (font, mode, cMode) {
  WriteByteToReg(FontModeRegAddr, cMode | mode | font)
}

function CharGotoXY (x, y) {
  var buf = [x, y]
  WriteSeriesToReg(CharXPosRegAddr, buf, 2)
}

function DispCharAt (buf, x, y) {
  CharGotoXY(x, y)
  WriteByteToReg(DisRAMAddr, buf.charCodeAt(0))
}

function DispCursorAt (cur, x, y) {
  CharGotoXY(x, y)
  WriteByteToReg(DisRAMAddr, cur)
}

function DispStringAt (buf, x, y) {
  // logger.log(x + " " + y + " " + buf);
  CharGotoXY(x, y)
  for (var i = 0; i < buf.length; i++) {
    WriteByteToReg(DisRAMAddr, buf.charCodeAt(i))
  }
}

function print (string) {
  var Y_Present, Y_New, Fontsize_Y, fontIndex
  for (var i = 0; i < string.length; i++) {
    switch (string.charCodeAt(i)) {
      case 0x0d: break
      case 0x0a:
        Y_Present = ReadByteFromReg(CharYPosRegAddr)
        fontIndex = ReadByteFromReg(FontModeRegAddr) & 0x0f

        if (Y_Present + 2 * fontYsizeTab[fontIndex] <= LCD_Y_SIZE_MAX) {
          Y_New = fontYsizeTab[fontIndex] + Y_Present
          CharGotoXY(0, Y_New)
        } else { CharGotoXY(0, 0) }
        break
      case 0x09:
        WriteSeriesToReg(DisRAMAddr, '  ', 2)
        break
      default: WriteByteToReg(DisRAMAddr, string.charCodeAt(i))
    }
  }
  return 1 // assume sucess
}

function DisplayConf (mode) {
  DisplayConfigRegVal = mode
  WriteByteToReg(DisplayConfigRegAddr, mode)
}

function WorkingModeConf (logoSwi, backLightSwi, mode) {
  WriteByteToReg(WorkingModeRegAddr, 0x50 | (logoSwi << 3) | (backLightSwi << 2) | mode)
}

function BacklightConf (mode, buf) {
  if (buf > 0x7f) { buf = 0x7f }
  WriteByteToReg(BackLightConfigRegAddr, mode | buf)
}

function ContrastConf (mode, buf) {
  if (buf > 0x3f) { buf = 0x3f }
  WriteByteToReg(ContrastConfigRegAddr, mode | buf)
}

function CleanAll (color) {
  var buf = DisplayConfigRegVal
  if (color == WHITE) { WriteByteToReg(DisplayConfigRegAddr, buf | 0x60) } else { WriteByteToReg(DisplayConfigRegAddr, (buf | 0x40) & 0xdf) }
}

function DrawDotAt (x, y, color) {
  var buf = new Buffer(2)
  if (x < 128 && y < 64) {
    buf[0] = x
    buf[1] = (color << 7) | y
    WriteSeriesToReg(DrawDotXPosRegAddr, buf, 2)
  }
}

function DrawHLineAt (startX, endX, y, color) {
  DrawLineAt(startX, endX, y, y, color)
}

function DrawVLineAt (startY, endY, x, color) {
  DrawLineAt(x, x, startY, endY, color)
}

function DrawLineAt (startX, endX, startY, endY, color) {
  var buf = new Buffer(4)
  if (endY < 64) {
    buf[0] = startX
    buf[1] = endX
    buf[2] = startY
    buf[3] = (uint8_t)(color << 7) | endY
    WriteSeriesToReg(DrawLineStartXRegAddr, buf, 4)
  }
}

function DrawRectangleAt (x, y, width, height, mode) {
  var buf = new Buffer(5)
  buf[0] = x
  buf[1] = y
  buf[2] = width
  buf[3] = height
  buf[4] = mode
  WriteSeriesToReg(DrawRectangleXPosRegAddr, buf, 5)
}

function DrawCircleAt (x, y, r, mode) {
  var buf = new Buffer(4)
  if (x < 128 && y < 64 && r < 64) {
    buf[0] = x
    buf[1] = y
    buf[2] = r
    buf[3] = mode
    WriteSeriesToReg(DrawCircleXPosRegAddr, buf, 4)
  }
}

function initialScreen () {
  WorkingModeConf(OFF, ON, WM_CharMode)
  FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC)
  CleanAll(WHITE)
  DispCharAt('X', 60, 2)
  DispStringAt('Air liquide', 20, 30)
}

var x = 0
var y = 0
var busy = false
function lcd_print_now (cmd_process) {
  // logger.log('lcd_print_now ' + cmd_process.length)
  if (busy) {
    logger.log('lcd_print_now busy')
    return
  }
  if (cmd_process.length > 0) {
    busy = true
    /*  var copy = [];
        copy = cmd_process; */
    var cmd = cmd_process.shift()
    var text = null
    var posx = 0
    var posy = 0

    // Reset fontmode in case we are back from a power cycle of the i2c bus
    FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC)

    if (cmd.hasOwnProperty('clean')) {
      CleanAll(WHITE)
    }
    if (cmd.hasOwnProperty('x')) {
      posx = Number(cmd.x)
    }
    if (cmd.hasOwnProperty('y')) {
      posy = Number(cmd.y)
    }
    if (cmd.hasOwnProperty('print')) {
      text = cmd.print
      if (cmd.hasOwnProperty('print_x')) {
        posx = Number(cmd.print_x)
      }
      if (cmd.hasOwnProperty('print_y')) {
        posy = Number(cmd.print_y)
      }
      if (text) DispStringAt(text, posx, posy)
      if (cmd.hasOwnProperty('print_box')) {
        if (text) DrawRectangleAt(posx - Number(cmd.print_box), posy - Number(cmd.print_box), (text.length * 6) + Number(cmd.print_box) * 2, 8 + Number(cmd.print_box) * 2, 0)
      }
      if (cmd.hasOwnProperty('print_cursor')) {
        if (text) DispCursorAt((cmd.print_cursor), posx - 8, posy)
        else DispCursorAt((cmd.print_cursor), posx, posy)
      }
    }
    if (cmd.hasOwnProperty('circle_radius')) {
      DrawCircleAt(posx, posy, Number(cmd.circle_radius), 0)
    }
    busy = false
  }
}

var local_service = scheduler.init({
  mode: scheduler.constants.MODE_QUEUE,
  module_name: 'lcd',
  onInit: () => {},
  onData: () => {},
  onConnect: () => {},
  onDisconnect: initialScreen,
  onSchedule: lcd_print_now
})

initialScreen()

/*
// DEBUG ONLY
var later = setInterval(() => {
  lcd_print_now()
}, 20)
*/
