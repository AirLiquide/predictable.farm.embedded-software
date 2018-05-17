/*
  Copyright (C) Air Liquide S.A,  2017
  Author : Sébastien Lalaurette, La Factory, Creative Foundry
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
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#include "Button.h"



/**************************************************************************/
/*!
    @brief  Get button state for menu system
*/
/**************************************************************************/
unsigned char Button::get() {
  //Get the value from the button
  unsigned short val = getRawCurrent();

  if (val > BTN_ENTER) val = ENTER;
  else if (val > BTN_DOWN) val = DOWN;
  else if (val > BTN_UP) val = UP;
  else if (val > BTN_BACK) val = BACK;
  
  //if the ENTER button is already pressed
  if (val == ENTER || val == UP || val == DOWN || val == BACK)
  {
    if (val == prev && val == ENTER)
      val = 0;
    else
      prev = val;
  } else {
    val = 0;
    prev = val;
  }
  return (unsigned char)val;
}


/**************************************************************************/
/*!
    @brief  Get button current state
*/
/**************************************************************************/
unsigned char Button::getCurrent() {
  //Get the value from the button
  unsigned short val = getRawCurrent();

  if (val > BTN_ENTER) val = BTN4;
  else if (val > BTN_DOWN) val = BTN3;
  else if (val > BTN_UP) val = BTN2;
  else if (val > BTN_BACK) val = BTN1;
  //Serial.println(val);
  return (unsigned char)val;
}

/**************************************************************************/
/*!
    @brief  Get button current raw value
*/
/**************************************************************************/
unsigned short Button::getRawCurrent() {
  //Get the value from the button
  unsigned short val = analogRead(A5);
  val += analogRead(A5);
  val += analogRead(A5);
  val = val / 3;
  //val = val & 0xFFC0;

  return (unsigned short)val;
}


/**************************************************************************/
/*!
    @brief  Get previous button state and update with current state 
*/
/**************************************************************************/
unsigned char Button::getPrev(unsigned char current) {
  unsigned char val = prev;
  if (current == BTN1 || current == BTN2 || current == BTN3 || current == BTN4)
  {
    prev = current;
  }
  else
  {
    prev = 0;
  }
  return val;
}

/**************************************************************************/
/*!
    @brief  Instantiates a new button class
*/
/**************************************************************************/
Button::Button() {
  prev = 0;
}


