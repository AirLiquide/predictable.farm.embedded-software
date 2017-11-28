/******************************************************************************
  LCD.cpp - LCD library Version 1.11
  Copyright (C), 2015, Sparking Work Space. All right reserved.

 ******************************************************************************

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is build for LCD12864. Please do not use this library on
  any other devices, that could cause unpredictable damage to the unknow device.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*******************************************************************************/


#include "LCD.h"

//#define NOLCD

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"


/*const Font_Info_t fontInfo[LCD_NUM_OF_FONT] =
  {
    Font_6x8, 6, 8,
    Font_6x12, 6, 12,
    Font_8x16_1, 8, 16,
    Font_8x16_2, 8, 16,
    Font_10x20, 10, 20,
    Font_12x24, 12, 24,
    Font_16x32, 16, 32,
  };*/

/********************* The start of LCD basic driver APIs *******************/

LCD::LCD(void) {};

/**************************************************************
      I2C init.
***************************************************************/
void LCD::Init(void)
{
#ifndef NOLCD
  I2C_BUS_Init();
  
#endif
}

/**************************************************************
      Read one byte from device register.
***************************************************************/
uint8_t LCD::ReadByteFromReg(enum LCD_RegAddress regAddr)
{
#ifndef NOLCD
  int8_t waitTime;
  I2C_BUS_BeginTransmission(LCD_ADDRESS);
  I2C_BUS_WriteByte(regAddr);
  I2C_BUS_EndTransmission();

  I2C_BUS_RequestFrom((int16_t)LCD_ADDRESS, 1);
  for (waitTime = 10; !I2C_BUS_Available() && waitTime; waitTime--)
    __asm__("nop\n\t");
  if (I2C_BUS_Available())
  {
    I2C_BUS_EndTransmission();
    return I2C_BUS_ReadByte();
  }
  else
#endif
    return 0;
}

/**************************************************************
      Write one byte to device register.
***************************************************************/
void LCD::WriteByteToReg(enum LCD_RegAddress regAddr, uint8_t buf)
{
#ifndef NOLCD
  I2C_BUS_BeginTransmission(LCD_ADDRESS);
  I2C_BUS_WriteByte(regAddr);
  I2C_BUS_WriteByte(buf);
  I2C_BUS_EndTransmission();
#endif
}

/**************************************************************
      Read multiple bytes from device register.
***************************************************************/
void LCD::ReadSeriesFromReg(enum LCD_RegAddress regAddr, uint8_t *buf, int8_t length)
{
#ifndef NOLCD
  int8_t i, waitTime;
  I2C_BUS_BeginTransmission(LCD_ADDRESS);
  I2C_BUS_WriteByte(regAddr);
  I2C_BUS_EndTransmission();
  if (length > 32)
    length = 32;
  I2C_BUS_RequestFrom((int16_t)LCD_ADDRESS, length);
  for (waitTime = 10; !I2C_BUS_Available() && waitTime; waitTime--)
    __asm__("nop\n\t");
  for (i = 0; i < length; i++)
  {
    buf[i] = I2C_BUS_ReadByte();
    //Serial.print(buf[i], HEX);
    //Serial.print(" ");
    for (waitTime = 10; !I2C_BUS_Available() && waitTime; waitTime--)
      __asm__("nop\n\t");
  }
  I2C_BUS_EndTransmission();
#endif
}

/**************************************************************
      Write multiple bytes to device register.
***************************************************************/
void LCD::WriteSeriesToReg(enum LCD_RegAddress regAddr, const uint8_t *buf, uint8_t length)
{
#ifndef NOLCD
  uint8_t i;
  I2C_BUS_BeginTransmission(LCD_ADDRESS);
  I2C_BUS_WriteByte(regAddr);
  for (i = 0; i < length; i++)
  {
    I2C_BUS_WriteByte(buf[i]);
  }
  I2C_BUS_EndTransmission();    // stop transmitting
#endif
}

/********************* The end of LCD basic driver APIs *******************/

#ifdef  SUPPORT_FULL_API_LIB
#if  SUPPORT_FULL_API_LIB == TRUE

void LCD::CharGotoXY(uint8_t x, uint8_t y)
{
#ifndef NOLCD
  uint8_t buf[2];
  buf[0] = x;
  buf[1] = y;
  WriteSeriesToReg(CharXPosRegAddr, buf, 2);
#endif
}

void LCD::ReadRAMGotoXY(uint8_t x, uint8_t y)
{
#ifndef NOLCD
  uint8_t buf[2];
  buf[0] = x;
  buf[1] = y;
  WriteSeriesToReg(ReadRAM_XPosRegAddr, buf, 2);
#endif
}



void LCD::WriteRAMGotoXY(uint8_t x, uint8_t y)
{
#ifndef NOLCD
  uint8_t buf[2];
  buf[0] = x;
  buf[1] = y;
  WriteSeriesToReg(WriteRAM_XPosRegAddr, buf, 2);
#endif
}



/*void LCD::SendBitmapData(const uint8_t *buf, uint8_t length)
  {
    uint8_t i;
    I2C_BUS_BeginTransmission(LCD_ADDRESS); // transmit device adress
    I2C_BUS_WriteByte(DisRAMAddr);        //  transmit register adress to the device
    for(i=0; i<length; i++)
    {
        I2C_BUS_WriteByte(pgm_read_byte_near(buf++));
    }
    I2C_BUS_EndTransmission();    // stop transmitting
  }*/

void LCD::SendBitmapData(const uint8_t *buf, uint8_t length)
{
#ifndef NOLCD
  uint8_t i;
  uint16_t circleTimes, circleCounter, transBytesNum;

  circleTimes = length / LCD_TRANS_ONCE_BYTE_MAX + 1;

  for (circleCounter = 0; circleCounter < circleTimes; circleCounter ++)
  {
    if (circleCounter + 1 >= circleTimes)
      transBytesNum = length % LCD_TRANS_ONCE_BYTE_MAX;
    else
      transBytesNum = LCD_TRANS_ONCE_BYTE_MAX;

    I2C_BUS_BeginTransmission(LCD_ADDRESS); // transmit device adress
    I2C_BUS_WriteByte(DisRAMAddr);        //  transmit register adress to the device
    for (i = 0; i < transBytesNum; i ++)
      I2C_BUS_WriteByte(pgm_read_byte_near(buf++));
    I2C_BUS_EndTransmission();    // stop transmitting
  }
#endif
}


void LCD::FontModeConf(enum LCD_FontSort font, enum LCD_FontMode mode, enum LCD_CharMode cMode)
{
#ifndef NOLCD
  WriteByteToReg(FontModeRegAddr, cMode | mode | font);
#endif
}

void LCD::DispCharAt(char buf, uint8_t x, uint8_t y)
{
#ifndef NOLCD
  CharGotoXY(x, y);
  WriteByteToReg(DisRAMAddr, buf);
#endif
}

void LCD::DispStringAt(char *buf, uint8_t x, uint8_t y)
{
#ifndef NOLCD
  CharGotoXY(x, y);
  for (; *buf; buf++)
    WriteByteToReg(DisRAMAddr, *buf);
#endif
}

const uint8_t fontYsizeTab[LCD_NUM_OF_FONT] = {8, 12, 16, 16, 20, 24, 32};

inline size_t LCD::write(uint8_t value)
{
#ifndef NOLCD
  uint8_t Y_Present, Y_New, Fontsize_Y, fontIndex;
  switch (value)
  {
    case 0x0d: break;
    case 0x0a:
      Y_Present = ReadByteFromReg(CharYPosRegAddr);
      fontIndex = ReadByteFromReg(FontModeRegAddr) & 0x0f;

      if (Y_Present + 2 * fontYsizeTab[fontIndex] <= LCD_Y_SIZE_MAX)
      {
        Y_New = fontYsizeTab[fontIndex] + Y_Present;
        CharGotoXY(0, Y_New);
      }
      else
        CharGotoXY(0, 0);
      break;
    case 0x09:
      WriteSeriesToReg(DisRAMAddr, (uint8_t *)"  ", 2);
      break;
    default: WriteByteToReg(DisRAMAddr, value);
  }
#endif
  return 1; // assume sucess
}


void LCD::CursorConf(enum LCD_SwitchState swi, uint8_t freq)
{
#ifndef NOLCD
  WriteByteToReg(CursorConfigRegAddr, (char)(swi << 7) | freq);
#endif
}

void LCD::CursorGotoXY(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
#ifndef NOLCD
  uint8_t buf[4];
  buf[0] = x;
  buf[1] = y;
  buf[2] = width;
  buf[3] = height;
  WriteSeriesToReg(CursorXPosRegAddr, buf, 4);
#endif
}

#ifdef  SUPPORT_2D_GRAPHIC_LIB
#if  SUPPORT_2D_GRAPHIC_LIB == TRUE

void LCD::DrawDotAt(uint8_t x, uint8_t y, enum LCD_ColorSort color)
{
#ifndef NOLCD
  uint8_t buf[2];
  if (x < 128 && y < 64)
  {
    buf[0] = x;
    buf[1] = (uint8_t)(color << 7) | y;
    WriteSeriesToReg(DrawDotXPosRegAddr, buf, 2);
  }
#endif
}

void LCD::DrawHLineAt(uint8_t startX, uint8_t endX, uint8_t y, enum LCD_ColorSort color)
{
#ifndef NOLCD
  DrawLineAt(startX, endX, y, y, color);
#endif
}

void LCD::DrawVLineAt(uint8_t startY, uint8_t endY, uint8_t x, enum LCD_ColorSort color)
{
#ifndef NOLCD
  DrawLineAt(x, x, startY, endY, color);
#endif
}

void LCD::DrawLineAt(uint8_t startX, uint8_t endX, uint8_t startY, uint8_t endY, enum LCD_ColorSort color)
{
#ifndef NOLCD
  uint8_t buf[4];
  if (endY < 64)
  {
    buf[0] = startX;
    buf[1] = endX;
    buf[2] = startY;
    buf[3] = (uint8_t)(color << 7) | endY;
    WriteSeriesToReg(DrawLineStartXRegAddr, buf, 4);
  }
#endif
}

void LCD::DrawRectangleAt(uint8_t x, uint8_t y, uint8_t width, uint8_t height, enum LCD_DrawMode mode)
{
#ifndef NOLCD
  uint8_t buf[5];
  buf[0] = x;
  buf[1] = y;
  buf[2] = width;
  buf[3] = height;
  buf[4] = mode;
  WriteSeriesToReg(DrawRectangleXPosRegAddr, buf, 5);
#endif
}

void LCD::DrawCircleAt(int8_t x, int8_t y, uint8_t r, enum LCD_DrawMode mode)
{
#ifndef NOLCD
  uint8_t buf[4];
  if (x < 128 && y < 64 && r < 64)
  {
    buf[0] = x;
    buf[1] = y;
    buf[2] = r;
    buf[3] = mode;
    WriteSeriesToReg(DrawCircleXPosRegAddr, buf, 4);
  }
#endif
}

void LCD::DrawScreenAreaAt(GUI_Bitmap_t *bitmap, uint8_t x, uint8_t y)
{
#ifndef NOLCD
  uint8_t regBuf[4];
  int16_t byteMax;
  int8_t i, counter;
  const uint8_t *buf = bitmap->pData;
  if (y < 64 && x < 128)
  {
    regBuf[0] = x;
    regBuf[1] = y;
    regBuf[2] = bitmap->XSize;
    regBuf[3] = bitmap->YSize;
    WriteSeriesToReg(DrawBitmapXPosRegAddr, regBuf, 4);
    byteMax = regBuf[3] * bitmap->BytesPerLine;
    counter = byteMax / 31;
    if (counter)
      for (i = 0; i < counter; i++, buf += 31)
        SendBitmapData(buf, 31);
    counter = byteMax % 31;
    if (counter)
      SendBitmapData(buf, counter);
  }
#endif
}


void LCD::DrawFullScreen(const uint8_t *buf)
{
#ifndef NOLCD
  uint8_t i;
  WriteRAMGotoXY(0, 0);
  for (i = 0; i < 1024; i++)
    WriteByteToReg(DisRAMAddr, buf[i]);
#endif
}

#endif
#endif



uint8_t LCD::ReadByteDispRAM(uint8_t x, uint8_t y)
{
#ifndef NOLCD
  ReadRAMGotoXY(x, y);
  return ReadByteFromReg(DisRAMAddr);
#else
  return 0;
#endif
}



void LCD::ReadSeriesDispRAM(uint8_t *buf, int8_t length, uint8_t x, uint8_t y)
{
#ifndef NOLCD
  ReadRAMGotoXY(x, y);
  ReadSeriesFromReg(DisRAMAddr, buf, length);
#endif
}

void LCD::WriteByteDispRAM(uint8_t buf, uint8_t x, uint8_t y)
{
#ifndef NOLCD
  WriteRAMGotoXY(x, y);
  WriteByteToReg(DisRAMAddr, buf);
#endif
}



void LCD::WriteSeriesDispRAM(uint8_t *buf, int8_t length, uint8_t x, uint8_t y)
{
#ifndef NOLCD
  WriteRAMGotoXY(x, y);
  WriteSeriesToReg(DisRAMAddr, buf, length);
#endif
}


void LCD::DisplayConf(enum LCD_DisplayMode mode)
{
#ifndef NOLCD
  WriteByteToReg(DisplayConfigRegAddr, mode);
#endif
}

void LCD::WorkingModeConf(enum LCD_SwitchState logoSwi, enum LCD_SwitchState backLightSwi, enum LCD_WorkingMode mode)
{
#ifndef NOLCD
  WriteByteToReg(WorkingModeRegAddr, 0x50 | (uint8_t)(logoSwi << 3) | (uint8_t)(backLightSwi << 2) | mode);
#endif
}

void LCD::BacklightConf(enum LCD_SettingMode mode, uint8_t buf)
{
#ifndef NOLCD
  if (buf > 0x7f)
    buf = 0x7f;
  WriteByteToReg(BackLightConfigRegAddr, mode | buf);
#endif
}

void LCD::ContrastConf(enum LCD_SettingMode mode, uint8_t buf)
{
#ifndef NOLCD
  if (buf > 0x3f)
    buf = 0x3f;
  WriteByteToReg(ContrastConfigRegAddr, mode | buf);
#endif
}

void LCD::DeviceAddrEdit(uint8_t newAddr)
{
#ifndef NOLCD
  uint8_t buf[2];
  buf[0] = 0x80;
  buf[1] = newAddr;
  WriteSeriesToReg(DeviceAddressRegAddr, buf, 2);
#endif
}



void LCD::CleanAll(enum LCD_ColorSort color)
{
#ifndef NOLCD
  uint8_t buf;
  buf = ReadByteFromReg(DisplayConfigRegAddr);
  if (color == WHITE)
    WriteByteToReg(DisplayConfigRegAddr, (buf | 0x40) & 0xdf);
  else
    WriteByteToReg(DisplayConfigRegAddr, buf | 0x60);
#endif
}


#endif
#endif





