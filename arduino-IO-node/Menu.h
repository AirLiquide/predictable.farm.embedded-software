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
#ifndef __MENU_H__
#define __MENU_H__

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "Sensors.h"
#include "YunBridge.h"
/*=========================================================================
    VALUE OF THE BUTTON
    -----------------------------------------------------------------------*/
#define BTN_UP            (840)
#define BTN_DOWN          (900)
#define BTN_BACK          (700)
#define BTN_ENTER         (980)

#define UP            (1)
#define DOWN          (2)
#define BACK          (3)
#define ENTER         (4)
//

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
    Sensors * p_mySensors;
    Relay *p_myrelay;

    void relayInfo(uint8_t num);
    void reset_curs();
    int prev;
    void Cursor(uint8_t maxi, uint8_t mini, uint8_t val);
    void Space();

};

#endif
