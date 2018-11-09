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
#include "Relay.h"

/**
   \fn void Relay::configure()
   \brief configure I2C expander interna register

   \param N/A
   \return n/a
*/
void Relay::configure()
{
#ifdef ACTUATOR_TYPE_RELAY
  myrelay = 0x00;
  myrelayMode = 0x00;
  prev = 0x00;
#ifndef I2CLIB
  Wire.beginTransmission(relay_addr);
  Wire.write(0x03);// cfg register
  Wire.write(0xF0); // our cfg
  Wire.endTransmission();

  Wire.beginTransmission(relay_addr);
  Wire.write(0x01);// output register
  Wire.write(0xF0);
  Wire.endTransmission();
 #else 
  relay_addr = RELAY1_ADDR;
  if(I2c.write(relay_addr,0x03,0xF0) > 0)
  {
   relay_addr = RELAY1x2_ADDR;
   if(I2c.write(relay_addr,0x03,0xF0) == 0)
   {
      x2on = true;
      on = true;
   }
  } 
  else
  {
      on = true;
  }
  I2c.write(relay_addr,0x01,0xF0);
 #endif
 #endif
}

/**
   \fn void Relay::init(LCD * p_LCD)
   \brief initialize the relay

   \param pointer to LCD.cpp
   \return n/a
*/
void Relay::init(LCD * p_LCD,YunBridge *p_bridge)
{
  
#ifdef ACTUATOR_TYPE_RELAY
  p_myLCD = p_LCD;
  p_myBridge = p_bridge;
  relay_addr = RELAY1_ADDR;
  x2on = false;
  on = false;

#ifndef I2CLIB
  Wire.beginTransmission(relay_addr);
  if (Wire.endTransmission() == 0) {
    configure();
    on = true;
  }
  else
  {
    relay_addr = RELAY1x2_ADDR;
    Wire.beginTransmission(relay_addr);
    if (Wire.endTransmission() == 0) {
      configure();
      x2on = true;
      on = true;
    }
  }
 #else
  configure();
 #endif
 #endif
}

void Relay::setupState(uint8_t state, uint8_t swtch) {
  if(state) state = 0x1;
  myrelay = (myrelay & ~(1 << swtch)) ;
  myrelay |= ( (state << swtch));
  //send directly state to Linux side
  switch(swtch)
  {
    case 0:
    p_myBridge->sendInteger("relay1", state | (getMode(swtch) << 1));
    break;
    case 1:
    p_myBridge->sendInteger("relay2", state | (getMode(swtch) << 1));
    break;
    case 2:
    p_myBridge->sendInteger("relay3", state | (getMode(swtch) << 1));
    break;
    case 3:
    p_myBridge->sendInteger("relay4", state | (getMode(swtch) << 1));
    break;
  }
}

/**
   \fn void Relay::setState(uint8_t state, uint8_t swtch)
   \brief Changes the status of the relay

   \param state: wanted state of sensor /!\ 1 = OFF & 0 = ON /!\, swtch: the relay
   \return n/a
*/
void Relay::setState(uint8_t state, uint8_t swtch) {
  
#ifdef ACTUATOR_TYPE_RELAY

#ifdef USE_MENU
  if (x2on) // case of UP/down on 2 relay linked, for ex windows opener
  {
    {
      if (swtch == 0 )
      {
        setupState(0,1 );
      }
      if (swtch == 1 )
      {
        setupState(0,0 );
      }
      if (swtch == 2 )
      {
        setupState(0,3 );
      }
      if (swtch == 3 )
      {
        setupState(0,2 );
      }
      LedState();
      delay(200);
    }
  }
#endif  
#ifdef USE_DASHBOARD_VIEW
// in this special case relay box has mixed usage  : 2 first btns are linked while 2 last are independantif (swtch == 0 )
      if (swtch == 0 )
      {
        setupState(0,1 );
        popup(1);
      }
      if (swtch == 1 )
      {
        setupState(0,0 );
        popup(0);
      }
      LedState();
      delay(200);
#endif
  setupState(state,swtch );
  LedState();
  delay(100);
  popup(swtch);
#endif
}

/**
   \fn void Relay::setMode(int mode, int swtch)
   \brief Changes the mode of the relay

   \param state: wanted state of sensor /!\ 1 = Manual & 0 = Auto /!\, swtch: the relay
   \return n/a
*/
void Relay::setMode(uint8_t mode, uint8_t swtch) {
  myrelayMode = (myrelayMode & ~(1 << (swtch-1))) ;
  myrelayMode |= ( (mode << (swtch-1)));
}

/**
   \fn void Relay::LedState()
   \brief Changes the status of the relay

   \param n/a
   \return n/a
*/
void Relay::LedState()
{

#ifdef ACTUATOR_TYPE_RELAY  
#ifndef I2CLIB
  Wire.beginTransmission(relay_addr);
  Wire.write(0x01);// output register
  Wire.write(myrelay); // all on
  Wire.endTransmission();
  delay(400);
 #else
  if(I2c.write(relay_addr,0x01,myrelay) > 0)
  {
    Serial.print("Timeout I2C Write :");
    Serial.println(relay_addr);
  }
  delay(250);
 #endif
#endif
}

/**
   \fn void Relay::updateState()
   \brief Pulls the status of the buttons

   \param n/a
   \return n/a
*/
void Relay::updateState()
{

#ifdef ACTUATOR_TYPE_RELAY
  uint8_t raw;
  uint8_t i = 4;

#ifndef I2CLIB
  Wire.beginTransmission(relay_addr);
  Wire.write(0x00);// output register
  Wire.endTransmission();
  Wire.requestFrom(relay_addr, (uint8_t)1);

  raw = Wire.read();
  
 #else
   
  if(I2c.read(relay_addr,0x00,1,&raw) > 0)
  {
    Serial.print("Timeout I2C Read :");
    Serial.println(relay_addr);
  }
 #endif

  //we only read the input of the buttons
  raw = raw & 0xF0;

  if ((prev) != (raw)) {
    prev = raw;

    //swap the order of the bits
    raw = ((raw >> 1) & 0x55) | ((raw & 0x55) << 1);
    // swap consecutive pairs
    raw = ((raw >> 2) & 0x33) | ((raw & 0x33) << 2);

    while (i < 8)
    {
      if (!((raw >> i) & 1)) {
        // local button pressed force manual mode. automatic mode can only be reactivated from network side
        setMode(RELAY_MODE_MANUAL, i - 3);
        setState((!((myrelay >> i - 4) & 1)), i - 4);
      }
      i++;
    }

  }
#endif
}

/**
   \fn void Relay::popup(int num)
   \brief  Makes a popup appear when a relay is toggled

   \param num of the relay
   \return n/a
*/
void Relay::popup(uint8_t num) {

#ifdef ACTUATOR_TYPE_RELAY
#ifdef USE_MENU
  p_myLCD->CleanAll(BLACK);
  p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);

  p_myLCD->CharGotoXY(5 * CHAR_OFFSET, 1 * ROW_OFFSET);

  if (getMode(num) == RELAY_MODE_MANUAL) {
    p_myLCD->print("Manual");
  } else {
    p_myLCD->print("Auto");
  }

  p_myLCD->println(" Mode");
  p_myLCD->CharGotoXY(5 * CHAR_OFFSET, 2 * ROW_OFFSET);
  p_myLCD->print("RELAY ");
  p_myLCD->print(num+1);
  p_myLCD->print(": ");

  if ((myrelay >> num ) & 1) {
    p_myLCD->print("On");
  } else {
    p_myLCD->print("Off");
  }
  popupAtMillis = millis();
#endif /*USE_MENU*/

#ifdef USE_DASHBOARD_VIEW
  
  p_myLCD->CharGotoXY((num) * CHAR_OFFSET *2, 5 * ROW_OFFSET);
  uint8_t symbol = 32; //space
  
    if ((myrelay >> num ) & 1) {
      if(num == 0) 
      {
        symbol = ArrowUp;
      } 
      else if(num == 1) 
      {
        symbol = ArrowDown;
      }
      else if(num == 2) 
      {
        symbol = 76;
      } 
      else if(num == 3) 
      {
        symbol = 67;
      }
    }
    p_myLCD->write(symbol);
#endif /*USE_DASHBOARD_VIEW*/
#endif /*ACTUATOR_TYPE_RELAY*/
}

/**
   \fn void Menu::relaysState()
   \brief prints the status of the relays

   \param n/a
   \return n/a
*/
/**************************************************************
      Sends the state of the relay
***************************************************************/
int Relay::getState(uint8_t num)
{
  
#ifdef ACTUATOR_TYPE_RELAY
  return !((myrelay >> num) & 1);
#else
  return 0;
#endif
}

/**************************************************************
      Sends the mode of the relay
***************************************************************/
int Relay::getMode(uint8_t num)
{
  
#ifdef ACTUATOR_TYPE_RELAY
  return ((myrelayMode >> num) & 1);
#else
  return 0;
#endif
}
