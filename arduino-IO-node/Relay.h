/*
  Copyright (C) Air Liquide S.A,  2017
  Author: Sébastien Lalaurette and Gwennaëlle Remy, La Factory, Creative Foundry
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
#ifndef __RELAY_H__
#define __RELAY_H__

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "config.h"

#ifndef I2CLIB
#include "Wire.h"
#else
#include "I2C.h"
#endif
#include "LCD.h"
#include "YunBridge.h"

#define RELAY1_ADDR 0X20 //V2
#define RELAY1x2_ADDR 0X24 //V2
/*#define RELAY_ADDR 0X38 //V1*/

#define RELAY_FORCED_ON   3
#define RELAY_FORCED_OFF  2
#define RELAY_ON          1
#define RELAY_OFF         0

#define RELAY_MODE_AUTO         0
#define RELAY_MODE_MANUAL       1

class Relay
{
  public:
    void updateState();
    void setState(uint8_t state, uint8_t swtch);
    int getState(uint8_t num);
    void setMode(uint8_t mode, uint8_t swtch);
    int getMode(uint8_t num);
    void LedState();
    void init(LCD * p_LCD, YunBridge *p_bridge);
    bool on;
    bool x2on;
    uint8_t myrelay; // state
    uint8_t myrelayMode; // auto '0' or manual '1'
    void popup(uint8_t num);
    unsigned long popupAtMillis;
  private:
    LCD * p_myLCD;
    YunBridge *p_myBridge;
    uint8_t prev;
    uint8_t relay_addr;
    void configure();
    void setupState(uint8_t state, uint8_t swtch);

};
#endif
