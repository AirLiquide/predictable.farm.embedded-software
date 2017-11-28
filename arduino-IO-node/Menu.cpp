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
#include "Menu.h"

extern GUI_Bitmap_t bmClock;
LCD my_LCD;
uint8_t curs1 = 1;//position of the cursor
int point = 0; //precision of the calibration
int select = 0;

//#define ALLOW_I2C_DEVICE_SCAN_AFTER_POWERON

#ifndef GUI_STORAGE_PTR
#define GUI_STORAGE_PTR PROGMEM
#endif

const unsigned char GUI_STORAGE_PTR acClock[] = {
  0xbc
  , 0x9c
  , 0x8c
  , 0x84
  , 0x8c
  , 0x9c
  , 0xbc
  , 0xfc


};

GUI_Bitmap_t bmClock = {
  6, /* XSize */
  8, /* YSize */
  1, /* bytesPerLine */
  1, /* BitsPerPixel */
  acClock,  /* Pointer to picture data (indices) */
};

typedef uint8_t _item;
struct  {
  _item _item_show = 1;
  _item _item_calibrate = 2;
  _item _item_relays = 3;
#ifdef ALLOW_I2C_DEVICE_SCAN_AFTER_POWERON
  _item _item_scan_again;
#endif
  //_menu_item _item_last=4;
} _menu_item;

uint8_t _nb_menu_item = 3 ;

enum _callibrate_item {
  _item_ph = 1,
  _item_do,
  _item_ec,
  _item_rtd,
  _item_orp,
  _item_co2,
  _item_co,
  _item_uv
};



/**
   \fn void Menu::init(LCD * p_LCD, Sensors* p_Sensors, Relay * p_relay)
   \brief Initialize Affichage.cpp

   \param the pointers to the LCD.cpp, Sensors.cpp, relay.cpp
*/
void Menu::init(LCD * p_LCD, Sensors* p_Sensors, Relay * p_relay) {
  p_myLCD = p_LCD;
  p_mySensors = p_Sensors;
  p_myrelay = p_relay;
  currentMenu = n_menu_main;

  if (p_mySensors->on == false) {

    _menu_item._item_show = 0;
    _menu_item. _item_calibrate = 0;
    _menu_item. _item_relays = 1;
    _nb_menu_item = _nb_menu_item - 2;
    if (p_myrelay->on == true) // in this case show directly relay state
    {
      relaysState();
      printMenu(n_menu_relays_state);
    }
  }
  else
  {
    sensorsValues();// in this case show directly sensors values
    printMenu(n_menu_sensors_values);
  }

  if (p_myrelay->on == false)
  {
    _nb_menu_item = _nb_menu_item - 1;
    _menu_item. _item_relays = 0;
    if (p_mySensors->on == false){ // in this case no sensors and no relays are connected, just display the menu
        reset_curs();
        mainMenu();
    }
  }
}


/**
   \fn uint8_t Menu::printMenu(uint8_t menu)
   \brief print the menu if there is a popup because the relay is toggled

   \param menu uint8_t tells us in which page of the menu we are
   \return menu uint8_t updated it tells us in which page of the menu we are
*/
uint8_t Menu::printMenu(uint8_t menu)
{
    if (p_myrelay->popupAtMillis > 0 &&
        millis() - p_myrelay->popupAtMillis < MAX_POPUP_MILLIS) {
        // Skip menu until MAX_POPUP_MILLIS has passed
        return menu;
    } else if (p_myrelay->popupAtMillis > 0) {
      // Popup will disappear, blank out
      p_myLCD->CleanAll(WHITE);
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
    }

    p_myrelay->popupAtMillis = 0;

    if (menu > n_menu_sensors_values) {

      if (menu == n_menu_main) {
        //prints cursor
        Cursor(0, 0, 0);
        mainMenu();
      }
      else if (menu == n_menu_calibrate) {
        //prints cursor
        Cursor(0, 0, 0);
        calibrationMenu();
      }
      else if (menu == n_menu_accuracy) {
        //prints cursor
        Cursor(0, 0, 0);
        accuracyMenu();
      }
      else if (menu == n_menu_relays_state) {
        relaysState();
      }
    } else if (menu == n_menu_sensors_values) {
      sensorsValues();
    }

  currentMenu = Check(menu);

  return currentMenu;
}


//TO OPTIMISE
/**
   \fn uint8_t Menu::Check(uint8_t menu)
   \brief it checks if a button is pressed and reacts accordingly

   \param menu uint8_t tells us in which page of the menu we are
   \return menu uint8_t updated it tells us in which page of the menu we are
*/
uint8_t Menu::Check(uint8_t menu)
{
  //Get the value from the button
  unsigned short val = analogRead(A5);
  val += analogRead(A5);
  val += analogRead(A5);
  val = val/3;
  //val = val & 0xFFC0;

  //Serial.println(val);

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

  if (val > 0) {

    if (menu == n_menu_sensors_values) //if we are on the READINGS page button 1 or 4 to escape
    {
      if (val == BACK || val == ENTER) {
        menu = n_menu_main;
        reset_curs();
        mainMenu();
      }
      else if (val == DOWN ) {
        if ((p_mySensors->lcdLine - p_mySensors->scrolling) > 6)
        {
          p_myLCD->CleanAll(WHITE);
          p_mySensors->scrolling++;
          //Serial.print("Down: ");
          //Serial.println(p_mySensors->scrolling);
          p_mySensors->update(6);
          delay(150);
        }
      }
      else if (val == UP) {
        if ( p_mySensors->scrolling > 0)
        {
          p_myLCD->CleanAll(WHITE);
          p_mySensors->scrolling--;
          //Serial.print("Up: ");
          //Serial.println(p_mySensors->scrolling);
          p_mySensors->update(6);
          delay(150);
        }
      }
    }

    else if ((val == BACK || val == ENTER)  && menu == n_menu_relays_state) //if we are on the RELAY page button 1 or 4 to escape
    {
      menu = n_menu_main;
      reset_curs();
      mainMenu();
    }
    else if (menu == n_menu_main) //if we are on the MAIN MENU
    {
      if (val == UP || val == DOWN ) { //if we want to move the cursor (button 2/3)
        // if (p_myrelay->on) {
        Cursor(_nb_menu_item, 1, val);
        /* }
          else
          #ifdef ALLOW_I2C_DEVICE_SCAN_AFTER_POWERON
          Cursor(3, 1, val);
          #else
          Cursor(2, 1, val);
          #endif*/
        mainMenu();
      }
      else if (val == ENTER) { //if we click on enter (button 4)
        if (curs1 == _menu_item._item_show) {//Readings
          //p_mySensors->scrolling = 0;
          menu = n_menu_sensors_values;
          p_mySensors->scrolling = 0;
          p_myLCD->CleanAll(WHITE);

          p_mySensors->update(6);


        } else if (curs1 == _menu_item._item_calibrate) {//calibration
          menu = n_menu_calibrate;
          reset_curs();
          calibrationMenu();
        }
        else if (curs1 == _menu_item._item_relays) { //&& p_myrelay->on) {relays
          curs1 = 5;
          menu = n_menu_relays_state;
          p_myLCD->CleanAll(WHITE);
          relaysState();
          Cursor(0, 0, 0);
        }
#ifdef ALLOW_I2C_DEVICE_SCAN_AFTER_POWERON
        else if (curs1 == _item_scan_again )
        { //setup(); //But what about linux?
          p_mySensors->detect();
          reset_curs();
          mainMenu();
        }
#endif
      }

    } else if ( menu == _menu_item._item_calibrate) {//WHAT DO YOU WANT TO CALIBRATE
      if (val == UP || val == DOWN) { //if we want to move the cursor
        Cursor(9, 1, val);
        calibrationMenu();
      }
      else if (val == ENTER) { //if we click on ENTER
        if (curs1 == _item_ph) {//PH
          menu = n_menu_accuracy;
          point = 3;
          select = 1;
          reset_curs();
          accuracyMenu();
        } else if (curs1 == _item_do) {//DO
          menu = n_menu_accuracy;
          point = 2;
          select = 2;
          reset_curs();
          accuracyMenu();
        } else if (curs1 == _item_ec) {//EC
          menu = n_menu_accuracy;
          point = 2;
          select = 3;
          reset_curs();
          accuracyMenu();
        } else if (curs1 == _item_rtd) {//RTD
          p_mySensors->calibrate(_item_rtd, 0);
          reset_curs();
          calibrationMenu();
          //menu = 1;
        } else if (curs1 == _item_orp) {//ORP
          p_mySensors->calibrate(_item_orp, 0);
          reset_curs();
          calibrationMenu();
          //menu = n_menu_main;
        } else if (curs1 == _item_co2) {//CO2
          p_mySensors->calibrate(_item_co2, 0);
          reset_curs();
          calibrationMenu();
          //menu = n_menu_main;
        }
        else if (curs1 == _item_co) {//CO
          p_mySensors->calibrate(_item_co, 0);
          reset_curs();
          calibrationMenu();
        }
        //menu = n_menu_main;
        else if (curs1 == _item_uv) {//DEPTH
          p_mySensors->calibrate(_item_uv, 0);
          reset_curs();
          calibrationMenu();
        }
        else  {//Exit
          menu = n_menu_main;
          reset_curs();
          mainMenu();
        }
      }
      else {//if (val == BACK)
        menu = n_menu_main;
        reset_curs();
        mainMenu();
      }
    } else if ( menu == n_menu_accuracy) {//PRECISION MENU TO CHOSE HOW MANY POINT CALIBRATION TO USE
      if (val == UP || val == DOWN) { //if we want to move the cursor
        Cursor(4, 1, val);
        accuracyMenu();
      }
      else if (val == ENTER) { //if we click on enter
        if (curs1 == 1) {
          p_mySensors->calibrate(select, 1);
        } else if (curs1 == 2) {
          p_mySensors->calibrate(select, 2);
        } else if (curs1 == 3) {
          p_mySensors->calibrate(select, 3);
        } else if (curs1 == 4) {//EXIT
          curs1 = 1;
        }
        reset_curs();
        menu = n_menu_calibrate;
        calibrationMenu();
      }
      else {//if (val == BACK)
        menu = n_menu_calibrate;
        reset_curs();
        calibrationMenu();
      }
    }
    delay(15);
  }
  return menu;
}

/**
   \fn void Menu::reset_curs()
   \brief Reset the cursor to the first line and prints it

   \param n/a
   \return n/a
*/
void Menu::reset_curs()
{
  p_myLCD->CleanAll(WHITE);
  curs1 = 1;
  Cursor(0, 0, 0);
}


/**
   \fn void Menu::mainMenu()
   \brief Prints the main menu

   \param n/a
   \return n/a
*/

void Menu::mainMenu()
{
  uint8_t ligneNb = 1;
  currentMenu = n_menu_main;

  /* manages number of menu item to display */
  //f (p_myrelay->on ) _nb_menu_item = _item_last - 1;
  //else  _nb_menu_item = _item_last - 2;

  p_myLCD->CharGotoXY(0, 0);
  p_myLCD->print(F("MENU"));
  p_myLCD->CharGotoXY(8 * CHAR_OFFSET, 0);
  if(p_mySensors->p_myBridge->linuxReady) p_myLCD->print("L");
  else p_myLCD->print(".");
  p_myLCD->CharGotoXY(9 * CHAR_OFFSET, 0);
  if(p_mySensors->p_myBridge->socketReady) p_myLCD->print("#");
  else if(p_mySensors->p_myBridge->networkReady) p_myLCD->print("@");
  else p_myLCD->print(".");
  p_myLCD->CharGotoXY(14 * CHAR_OFFSET, 0);
  p_myLCD->print("ID:");
  p_myLCD->print(p_mySensors->p_myBridge->deviceID);


  if (_menu_item._item_show != 0) {
    if (curs1 == _menu_item._item_show)
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);

    p_myLCD->CharGotoXY( 5 * CHAR_OFFSET,  ligneNb * ROW_OFFSET);
    p_myLCD->print(" Show");

    ligneNb++;
    if (curs1 == _menu_item._item_calibrate)
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
    else
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);

    p_myLCD->CharGotoXY(5 * CHAR_OFFSET, ligneNb * ROW_OFFSET);
    p_myLCD->print(" Calibrate");

    ligneNb++;
  }
#ifdef ALLOW_I2C_DEVICE_SCAN_AFTER_POWERON
  if (curs1 == _menu_item._item_scan_again)
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
  else
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);

  p_myLCD->CharGotoXY(5 * CHAR_OFFSET, ligneNb * ROW_OFFSET);
  p_myLCD->print(" Scan again");
#endif

  if (_menu_item._item_relays) {

    if (curs1 == _menu_item._item_relays)
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
    else
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);

    p_myLCD->CharGotoXY(5 * CHAR_OFFSET, ligneNb * ROW_OFFSET);
    p_myLCD->print(" Relay");
  }

  p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);

}


/**
   \fn void Menu::Cursor(int maxi, int mini, int val)
   \brief moves the cursor if possible and prints it

   \param maxi Int, mini Int, val Int tells us where to move the cursor
   \return n/a
*/
void Menu::Cursor(uint8_t maxi, uint8_t mini, uint8_t val)
{
  p_myLCD->DispStringAt(" ", 4 * CHAR_OFFSET, curs1 * ROW_OFFSET);//earse the cursor
  if (val == UP && curs1 > mini) { //if we want to go up
    curs1--;
  }
  else if (val == DOWN && curs1 < maxi) { //if we want to go down
    curs1++;
  }
  
  uint8_t i = curs1;

  if (curs1 > 5) {
    i = 5;
  }

  //print the cursor
  p_myLCD->WorkingModeConf(OFF, ON, WM_BitmapMode);
  p_myLCD->DrawScreenAreaAt(&bmClock, 4 * CHAR_OFFSET, ( i) * ROW_OFFSET);
  p_myLCD->WorkingModeConf(OFF, ON, WM_CharMode);

}

/**
   \fn void Menu::calibrationMenu()
   \brief prints the calibration menu

   \param n/a
   \return n/a
*/
void Menu::calibrationMenu() {
  uint8_t i = 0;

  currentMenu = n_menu_calibrate;
  if (curs1 > 5) {
    i = curs1 - 5;
  }

  p_myLCD->CharGotoXY( 0, 0);
  p_myLCD->print("Calibrate");
  if (curs1 < 6) {
    if (curs1 == 1)
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
    p_myLCD->DispStringAt(" PH  ", 5 * CHAR_OFFSET, (1 - i) * ROW_OFFSET);

  }
  if (curs1 < 7) {
    if (curs1 == 2)
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
    else
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
    p_myLCD->DispStringAt(" DO  ", 5 * CHAR_OFFSET, (2 - i) * ROW_OFFSET);
  }
  if (curs1 < 8) {
    if (curs1 == 3)
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
    else
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
    p_myLCD->DispStringAt(" EC  ", 5 * CHAR_OFFSET, (3 - i) * ROW_OFFSET);
  }

  if (curs1 == 4)
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
  else
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
  p_myLCD->DispStringAt(" RTD ", 5 * CHAR_OFFSET, (4 - i) * ROW_OFFSET);
  if (curs1 == 5)
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
  else
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
  p_myLCD->DispStringAt(" ORP ", 5 * CHAR_OFFSET, (5 - i) * ROW_OFFSET);
  if (curs1 == 6)
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
  else
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
  p_myLCD->DispStringAt(" CO2 ", 5 * CHAR_OFFSET, (6 - i) * ROW_OFFSET);

  if (curs1 == 7)
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
  else
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
  p_myLCD->DispStringAt(" CO  ", 5 * CHAR_OFFSET, (7 - i) * ROW_OFFSET);
  if (curs1 == 8)
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
  else
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
  p_myLCD->DispStringAt(" US  ", 5 * CHAR_OFFSET, (8 - i) * ROW_OFFSET);
  if (curs1 == 9)
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
  else
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
  p_myLCD->DispStringAt(" Exit", 5 * CHAR_OFFSET, (9 - i) * ROW_OFFSET);
  p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
  p_myLCD->DispStringAt("    ", 6 * CHAR_OFFSET, (10 - i) * ROW_OFFSET);

}

/**
   \fn void Menu::precisionMenu()
   \brief prints the accuracy menu

   \param n/a
   \return n/a
*/
void Menu::accuracyMenu() {

  currentMenu = n_menu_accuracy;
  p_myLCD->DispStringAt("PRECISION", 0, 0 * ROW_OFFSET);
  if (curs1 == 1)
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);

  p_myLCD->DispStringAt(" One point", 5 * CHAR_OFFSET, 1 * ROW_OFFSET);
  if (point > 1) {
    if (curs1 == 2)
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
    else
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
    p_myLCD->DispStringAt(" Two point", 5 * CHAR_OFFSET, 2 * ROW_OFFSET);
  }
  if (point > 2) {
    if (curs1 == 3)
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
    else
      p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
    p_myLCD->DispStringAt(" Three point", 5 * CHAR_OFFSET, 3 * ROW_OFFSET);
  }
  if (curs1 == 4)
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
  else
    p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);
  p_myLCD->DispStringAt(" Exit", 5 * CHAR_OFFSET, 4 * ROW_OFFSET);
  p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);

}

/**
   \fn void Menu::sensorsValues()
   \brief prints the status of the relays

   \param n/a
   \return n/a
*/
void Menu::sensorsValues() {
  currentMenu = n_menu_sensors_values;
  //p_myLCD->CleanAll(WHITE);
  p_mySensors->update(6);
}

/**
   \fn void Menu::relaysState()
   \brief prints the status of the relays

   \param n/a
   \return n/a
*/
void Menu::relaysState() {
  currentMenu = n_menu_relays_state;

  //prints cursor
  p_myLCD->WorkingModeConf(OFF, ON, WM_BitmapMode);
  p_myLCD->DrawScreenAreaAt(&bmClock, 4 * CHAR_OFFSET, ( 5) * ROW_OFFSET);
  p_myLCD->WorkingModeConf(OFF, ON, WM_CharMode);

  p_myLCD->DispStringAt("Relay", 0, 0 * ROW_OFFSET);

  //prints the status of the relay
  relayInfo(1);

  relayInfo(2);

  relayInfo(3);

  relayInfo(4);

  p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, WHITE_BAC);
  p_myLCD->DispStringAt(" Exit", 5 * CHAR_OFFSET, 5 * ROW_OFFSET);
  p_myLCD->FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);

}
/**
   \fn void Menu::relay(uint8_t num_relay)
   \brief prints the status of one relay

   \param uint8_t num_relay
   \return n/a
*/
void Menu::relayInfo(uint8_t num_relay) {
  p_myLCD->DispStringAt("Relay", 5 * CHAR_OFFSET, (num_relay) * ROW_OFFSET);
  p_myLCD->print(num_relay);
  p_myLCD->print(" : ");
  if ((p_myrelay->myrelay >> (num_relay - 1)) & 1) p_myLCD->print("On ");
  else p_myLCD->print("Off");
}

/**
   \fn uint8_t Menu::getCurrentMenuIndex()
   \brief returns the index of current menu

   \param n/a
   \return index of current menu
*/
uint8_t Menu::getCurrentMenuIndex()
{
  return currentMenu;
}
