/*
   SunLight.cpp
   A library for Grove - Sunlight Sensor v1.0

   Copyright (c) 2015 seeed technology inc.
   Website    : www.seeed.cc
   Author     : Fuhua.Chen
   Modified Time: June 2015

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
*/

#include "SunLight.h"
#include "I2C.h"
//#include "Wire.h"
#ifdef SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR
/*--------------------------------------------------------//
  default init

*/
void SunLight::DeInit(void)
{
  //ENABLE UV reading
  //these reg must be set to the fixed value
  WriteByte(SUNLIGHT_UCOEFF0, 0x29);
  WriteByte(SUNLIGHT_UCOEFF1, 0x89);
  WriteByte(SUNLIGHT_UCOEFF2, 0x02);
  WriteByte(SUNLIGHT_UCOEFF3, 0x00);
  WriteParamData(SUNLIGHT_CHLIST, SUNLIGHT_CHLIST_ENUV | SUNLIGHT_CHLIST_ENALSIR | SUNLIGHT_CHLIST_ENALSVIS | SUNLIGHT_CHLIST_ENPS1);
  //
  //set LED1 CURRENT(22.4mA)(It is a normal value for many LED)
  //
  WriteParamData(SUNLIGHT_PS1_ADCMUX, SUNLIGHT_ADCMUX_LARGE_IR);
  WriteByte(SUNLIGHT_PS_LED21, SUNLIGHT_LED_CURRENT_22MA);
  WriteParamData(SUNLIGHT_PSLED12_SELECT, SUNLIGHT_PSLED12_SELECT_PS1_LED1); //
  //
  //PS ADC SETTING
  //
  WriteParamData(SUNLIGHT_PS_ADC_GAIN, SUNLIGHT_ADC_GAIN_DIV1);
  WriteParamData(SUNLIGHT_PS_ADC_COUNTER, SUNLIGHT_ADC_COUNTER_511ADCCLK);
  WriteParamData(SUNLIGHT_PS_ADC_MISC, SUNLIGHT_ADC_MISC_HIGHRANGE | SUNLIGHT_ADC_MISC_ADC_RAWADC);
  //
  //VIS ADC SETTING
  //
  WriteParamData(SUNLIGHT_ALS_VIS_ADC_GAIN, SUNLIGHT_ADC_GAIN_DIV1);
  WriteParamData(SUNLIGHT_ALS_VIS_ADC_COUNTER, SUNLIGHT_ADC_COUNTER_511ADCCLK);
  WriteParamData(SUNLIGHT_ALS_VIS_ADC_MISC, SUNLIGHT_ADC_MISC_HIGHRANGE);
  //
  //IR ADC SETTING
  //
  WriteParamData(SUNLIGHT_ALS_IR_ADC_GAIN, SUNLIGHT_ADC_GAIN_DIV1);
  WriteParamData(SUNLIGHT_ALS_IR_ADC_COUNTER, SUNLIGHT_ADC_COUNTER_511ADCCLK);
  WriteParamData(SUNLIGHT_ALS_IR_ADC_MISC, SUNLIGHT_ADC_MISC_HIGHRANGE);
  //
  //interrupt enable
  //
  WriteByte(SUNLIGHT_INT_CFG, SUNLIGHT_INT_CFG_INTOE);
  WriteByte(SUNLIGHT_IRQ_ENABLE, SUNLIGHT_IRQEN_ALS);
  //
  //AUTO RUN
  //
  WriteByte(SUNLIGHT_MEAS_RATE0, 0xFF);
  WriteByte(SUNLIGHT_COMMAND, SUNLIGHT_PSALS_AUTO);
}

/*--------------------------------------------------------//
  Init the si114x and begin to collect data


*/
uint8_t timeout = 100;
bool SunLight::Begin(void)
{
  bool wait = true;
  bool res = false;
  while (wait)
  {
    Wire.begin();
    //
    //Init IIC  and reset si1145
    //
    if (ReadByte(SUNLIGHT_PART_ID) == 0X45)
    {
      Reset();
      //
      //INIT
      //
      DeInit();
      wait = false;
      res = true;
    }
    else
    {
      timeout --;
      delay(20);
      if (timeout == 0) {
        wait = false;
        res = false;
      }
    }
  }
  return res;
}
/*--------------------------------------------------------//
  reset the si114x
  inclue IRQ reg, command regs...

*/
void SunLight::Reset(void)
{
  WriteByte(SUNLIGHT_MEAS_RATE0, 0);
  WriteByte(SUNLIGHT_MEAS_RATE1, 0);
  WriteByte(SUNLIGHT_IRQ_ENABLE, 0);
  WriteByte(SUNLIGHT_IRQ_MODE1, 0);
  WriteByte(SUNLIGHT_IRQ_MODE2, 0);
  WriteByte(SUNLIGHT_INT_CFG, 0);
  WriteByte(SUNLIGHT_IRQ_STATUS, 0xFF);

  WriteByte(SUNLIGHT_COMMAND, SUNLIGHT_RESET);
  delay(10);
  WriteByte(SUNLIGHT_HW_KEY, 0x17);
  delay(10);
}
/*--------------------------------------------------------//
  write one byte into si114x's reg

*/
void SunLight::WriteByte(uint8_t Reg, uint8_t Value)
{
  Wire.beginTransmission(SUNLIGHT_ADDR);
  Wire.write(Reg);
  Wire.write(Value);
  Wire.endTransmission();
}
/*--------------------------------------------------------//
  read one byte data from si114x

*/
uint8_t SunLight::ReadByte(uint8_t Reg)
{
  Wire.beginTransmission(SUNLIGHT_ADDR);
  Wire.write(Reg);
  Wire.endTransmission();
  Wire.requestFrom(SUNLIGHT_ADDR, 1);
  return Wire.read();
}
/*--------------------------------------------------------//
  read half word(2 bytes) data from si114x

*/
uint16_t SunLight::ReadHalfWord(uint8_t Reg)
{
  uint16_t Value;
  Wire.beginTransmission(SUNLIGHT_ADDR);
  Wire.write(Reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)SUNLIGHT_ADDR, (uint8_t)2);
  Value = Wire.read();
  Value |= (uint16_t)Wire.read() << 8;
  return Value;
}
/*--------------------------------------------------------//
  read param data


uint8_t SunLight::ReadParamData(uint8_t Reg)
{
  WriteByte(SUNLIGHT_COMMAND, Reg | SUNLIGHT_QUERY);
  return ReadByte(SUNLIGHT_RD);
}*/
/*--------------------------------------------------------//
  writ param data

*/
uint8_t SunLight::WriteParamData(uint8_t Reg, uint8_t Value)
{
  //write Value into PARAMWR reg first
  WriteByte(SUNLIGHT_WR, Value);
  WriteByte(SUNLIGHT_COMMAND, Reg | SUNLIGHT_SET);
  //SunLight writes value out to PARAM_RD,read and confirm its right
  return ReadByte(SUNLIGHT_RD);
}

/*--------------------------------------------------------//
  Read Visible Value

*/
uint16_t SunLight::ReadVisible(void)
{
  return ReadHalfWord(SUNLIGHT_ALS_VIS_DATA0);
}
/*--------------------------------------------------------//
  Read IR Value


uint16_t SunLight::ReadIR(void)
{
  return ReadHalfWord(SUNLIGHT_ALS_IR_DATA0);
}*/
/*--------------------------------------------------------//
  Read Proximity Value


uint16_t SunLight::ReadProximity(uint8_t PSn)
{
  return ReadHalfWord(PSn);
}*/
/*--------------------------------------------------------//
  Read UV Value
  this function is a int value ,but the real value must be div 100

*/
uint16_t SunLight::ReadUV(void)
{
  return (ReadHalfWord(SUNLIGHT_AUX_DATA0_UVINDEX0));
}
#endif /*SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR*/




