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
#include "Relay.h"

/**
   \fn void Relay::configure()
   \brief configure I2C expander interna register

   \param N/A
   \return n/a
*/
void Relay::configure()
{
  myrelay = 0x00;
  myrelayMode = 0x00;
  prev = 0x00;

  Wire.beginTransmission(relay_addr);
  Wire.write(0x03);// cfg register
  Wire.write(0xF0); // our cfg
  Wire.endTransmission();

  Wire.beginTransmission(relay_addr);
  Wire.write(0x01);// output register
  Wire.write(0xF0);
  Wire.endTransmission();
}

/**
   \fn void Relay::init(LCD * p_LCD)
   \brief initialize the relay

   \param pointer to LCD.cpp
   \return n/a
*/
void Relay::init(LCD * p_LCD)
{
  p_myLCD = p_LCD;
  relay_addr = RELAY1_ADDR;
  x2on = false;
  on = false;

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
}

void Relay::setupState(uint8_t state, uint8_t swtch) {
  if(state) state = 0x1;
  myrelay = (myrelay & ~(1 << swtch)) ;
  myrelay |= ( (state << swtch));
}

/**
   \fn void Relay::setState(uint8_t state, uint8_t swtch)
   \brief Changes the status of the relay

   \param state: wanted state of sensor /!\ 1 = OFF & 0 = ON /!\, swtch: the relay
   \return n/a
*/
void Relay::setState(uint8_t state, uint8_t swtch) {
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
      delay(100);
    }
  }
  setupState(state,swtch );
  LedState();
}

/**
   \fn void Relay::setMode(int mode, int swtch)
   \brief Changes the mode of the relay

   \param state: wanted state of sensor /!\ 1 = Manual & 0 = Auto /!\, swtch: the relay
   \return n/a
*/
void Relay::setMode(uint8_t mode, uint8_t swtch) {
  myrelayMode = (myrelayMode & ~(1 << swtch)) ;
  myrelayMode |= ( (mode << swtch));
}

/**
   \fn void Relay::LedState()
   \brief Changes the status of the relay

   \param n/a
   \return n/a
*/
void Relay::LedState()
{
  Wire.beginTransmission(relay_addr);
  Wire.write(0x01);// output register
  Wire.write(myrelay); // all on
  Wire.endTransmission();
}

/**
   \fn void Relay::updateState()
   \brief Pulls the status of the buttons

   \param n/a
   \return n/a
*/
void Relay::updateState()
{
  uint8_t raw;
  uint8_t i = 4;

  Wire.beginTransmission(relay_addr);
  Wire.write(0x00);// output register
  Wire.endTransmission();
  Wire.requestFrom(relay_addr, 1);

  raw = Wire.read();

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
        popup(i-3);
      }
      i++;
    }

  }
}

/**
   \fn void Relay::popup(int num)
   \brief  Makes a popup appear when a relay is toggled

   \param num of the relay
   \return n/a
*/
void Relay::popup(uint8_t num) {

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
  p_myLCD->print(num);
  p_myLCD->print(": ");

  if ((myrelay >> (num - 1)) & 1) {
    p_myLCD->print("On ");
  } else {
    p_myLCD->print("Off");
  }
  popupAtMillis = millis();

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
  return !((myrelay >> num) & 1);
}

/**************************************************************
      Sends the mode of the relay
***************************************************************/
int Relay::getMode(uint8_t num)
{
  return ((myrelayMode >> num) & 1);
}

