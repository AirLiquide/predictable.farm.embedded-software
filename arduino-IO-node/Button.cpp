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
