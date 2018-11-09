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
#ifndef __MENU_H__
#define __MENU_H__

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Sensors.h"
#include "YunBridge.h"
#include "Button.h"

#define MAX_POPUP_MILLIS 800

enum en_menu_id {
  n_menu_sensors_values = 0,
  n_menu_main,
  n_menu_calibrate,
  n_menu_accuracy,
  n_menu_relays_state,
  n_menu_scan_again
};

class Menu
{
  public:
    uint8_t printMenu(uint8_t menu);
    uint8_t Check(uint8_t menu);
    void init(LCD * p_LCD, Sensors* p_Sensors, Relay * p_relay);
    void calibrationMenu();
    void sensorsValues();
    void accuracyMenu() ;
    void mainMenu();
    void relaysState();
    uint8_t getCurrentMenuIndex();
    uint8_t currentMenu;
    LCD * p_myLCD;
  private:
    Button myButton;
    Sensors * p_mySensors;
    Relay *p_myrelay;

    void relayInfo(uint8_t num);
    void reset_curs();
    void Cursor(uint8_t maxi, uint8_t mini, uint8_t val);
    void Space();

};

#endif
