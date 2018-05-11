  /*
  Copyright (C) Air Liquide S.A,  2017
  Author : Sébastien Lalaurette and Gwennaëlle Remy, La Factory, Creative Foundry
  This file is part of Predictable Farm embedded software project.

  Predictable Farm embedded software is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  Predictable Farm embedded software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
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
    //Relay();
    void updateState();
    void setState(uint8_t state, uint8_t swtch);
    int getState(uint8_t num);
    void setMode(uint8_t mode, uint8_t swtch);
    int getMode(uint8_t num);
    void LedState();
    void init(LCD * p_LCD);
    bool on;
    bool x2on;
    uint8_t myrelay; // state
    uint8_t myrelayMode; // auto '0' or manual '1'
    void popup(uint8_t num);
    unsigned long popupAtMillis;
  private:
    LCD * p_myLCD;
    uint8_t prev;
    uint8_t relay_addr;
    void configure();
    void setupState(uint8_t state, uint8_t swtch);

};
#endif
