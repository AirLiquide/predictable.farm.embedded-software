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

#include <SoftwareSerial.h>
#include "Relay.h"
#include "YunBridge.h"
#include "SensorTypes.h"
#include "Sensors.h"
#include "LCD.h"
#include "Menu.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define CALL_COMPENSATION_EC 15
#define CALL_COMPENSATION_RTD 11 //at least 3 less than CALL_COMPENSATION_EC
#define CALL_WATER_SENSOR 7//at least 3 less than CALL_COMPENSATION_RTD
#define VALUE_SENSOR_SEND_LINUX 0
#define NBRE_CYCLE 20 //can't be less than CALL_COMPENSATION_EC

#define TIME_CYCLE_MS 150

//#define DEBUG
#define LCD_DISPLAY


/* Global Variables */
//unsigned char deviceConnected = 0 ;
long previousMillis = 0;
long elapsed = 0;
uint8_t menu = 0;
unsigned char bridgeCommandType = 0;
int bridgeCommandValue = 0;
#ifdef LCD_DISPLAY
LCD myLCD;
#endif

YunBridge bridge;
Relay relay;
Sensors mySensors;
Menu myMenu;


void setup()
{

  Serial.begin(9600);
  Wire.begin();

#ifdef DEBUG_YUN
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
#endif

  // initialize classes
  bridge.init();

  myLCD.WorkingModeConf(OFF, ON, WM_CharMode);
  //8*16 font size, auto new line, black character on white back ground.
  myLCD.FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);

  //myLCD.Init();

  relay.init(&myLCD);
  mySensors.init(&myLCD, &bridge);


#ifdef LCD_DISPLAY
  //Clean the screen with black or white.
  myLCD.CleanAll(WHITE);    //Clean the screen with black or white.
#endif


  myMenu.init(&myLCD, &mySensors, &relay);
  menu = myMenu.getCurrentMenuIndex();


#ifdef DEBUG
  Serial.println("ready");
#endif

}

uint8_t updateCounter = CALL_WATER_SENSOR;
void loop()
{

  previousMillis = millis();

  /********************************************/
  /* update sensors values                    */
  /********************************************/

  if (updateCounter == CALL_COMPENSATION_EC)
  {
    mySensors.compensationEC();//We compensate the sensors if we have a EC probe
  }
  else if (updateCounter == CALL_COMPENSATION_RTD)
  {
    mySensors.compensationTemp();//We compensate the sensors if we have a temperature probe
  }
  else if (updateCounter == CALL_WATER_SENSOR)
  {
    mySensors.waterCall();// We call the water sensors
  }
  if (updateCounter == VALUE_SENSOR_SEND_LINUX) {
    mySensors.update(menu);

    /*****************************************************/
    /* send commands, status or values to the Linux side */
    /*****************************************************/


    while (bridge.getCommand(&bridgeCommandType, &bridgeCommandValue) > 0)
    {
      uint8_t num = bridgeCommandType - BRIDGE_RELAY_1;
      switch (bridgeCommandType)
      {
        case BRIDGE_RELAY_1:
        case BRIDGE_RELAY_2:
        case BRIDGE_RELAY_3:
        case BRIDGE_RELAY_4:
          if (bridge.socketReady) {
#ifdef DEBUG
            Serial.print(F("R"));
            Serial.print(num);
            Serial.print(F(":"));
            Serial.println(bridgeCommandValue);
#endif
            if (bridgeCommandValue >= RELAY_OFF && bridgeCommandValue <= RELAY_FORCED_ON ) { // make sure we dont get garbage from linux during boot or eventual linux side app crash
              if ((bridgeCommandValue & 0x1) == RELAY_ON) relay.setState(0, num);
              else relay.setState(1, num);
              if ((bridgeCommandValue & 0x2)  == RELAY_MODE_AUTO) relay.setMode(RELAY_MODE_AUTO, num);
              else relay.setMode(RELAY_MODE_MANUAL, num);
              relay.popup(num + 1);
            }
          }
          break;
        case BRIDGE_OS:
          bridge.linuxReady = (bridgeCommandValue == BRIDGE_OS_UP);
          if (myMenu.getCurrentMenuIndex() == n_menu_main) // update id displayed on main menu
            myMenu.mainMenu();
          break;
        case BRIDGE_DEVICE:
          bridge.deviceID = bridgeCommandValue;
          if (myMenu.getCurrentMenuIndex() == n_menu_main) // update id displayed on main menu
            myMenu.mainMenu();
          break;
        case BRIDGE_SYSTEM_CONFIG:
        case BRIDGE_SYSTEM_REBOOT:
        case BRIDGE_SYSTEM_UPDATE:
          /*  if (bridgeCommandValue == 67) {
              myLCD.CleanAll(WHITE);
              myLCD.CharGotoXY(2 * CHAR_OFFSET, 2 * ROW_OFFSET);
              myLCD.print(F("Updating"));

              //Serial.print(F("U "));
              delay(10000);
              //myLCD.print("Reset");
            }*/
          break;
        case BRIDGE_NETWORK:
          bridge.networkReady = bridge.linuxReady && (bridgeCommandValue > BRIDGE_NETWORK_UP);
          bridge.socketReady = bridge.linuxReady && (bridgeCommandValue == BRIDGE_NETWORK_SOCKET);
          if (myMenu.getCurrentMenuIndex() == n_menu_main) // update network status
            myMenu.mainMenu();
          break;
        default:

#ifdef DEBUG
          Serial.print(F("D "));
          Serial.println(bridgeCommandType);
#endif
          break;

      }
    }

    //send relay state to Linux
    if (relay.on) {
      bridge.sendInteger("relay1", relay.getState(0) | (relay.getMode(0) << 1));
      bridge.sendInteger("relay2", relay.getState(1) | (relay.getMode(1) << 1));
      bridge.sendInteger("relay3", relay.getState(2) | (relay.getMode(2) << 1));
      bridge.sendInteger("relay4", relay.getState(3) | (relay.getMode(3) << 1));
    }


    updateCounter = NBRE_CYCLE;
  }
  else
    updateCounter --;



  /********************************************/
  /* Update relay values                      */
  /********************************************/
  relay.updateState();

#ifdef LCD_DISPLAY

  /*****************************************************/
  /*  MENU DISPLAY                                     */
  /*****************************************************/
  menu = myMenu.printMenu(menu);

#endif /* LCD_DISPLAY */
  elapsed = (millis() - previousMillis);
  if (elapsed < TIME_CYCLE_MS) delay(TIME_CYCLE_MS -  elapsed); //loop every 150 msec exactly

}
