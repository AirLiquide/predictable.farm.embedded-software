

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
#include "config.h"
#include "Relay.h"
#include "YunBridge.h"
#include "SensorTypes.h"
#include "Sensors.h"
#include "LCD.h"
#include "Menu.h"
#include "Button.h"

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
#ifdef USE_MENU
Menu myMenu;
#endif



#ifdef USE_DASHBOARD_VIEW
Button myButton;
#endif

void setup()
{

  Serial.begin(9600);


#ifdef DEBUG
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
#endif


#ifndef I2CLIB
  Wire.begin();
  //Wire.setClock(10000);
#else
  I2c.reset();
  I2c.begin();
  I2c.timeOut(80);
  I2c.setSpeed(0);
#endif


  // initialize classes
  bridge.init();

  myLCD.WorkingModeConf(OFF, ON, WM_CharMode);
  //8*16 font size, auto new line, black character on white back ground.
  myLCD.FontModeConf(Font_6x8, FM_ANL_AAA, BLACK_BAC);

  //myLCD.Init();

  relay.init(&myLCD, &bridge);
  mySensors.init(&myLCD, &bridge);

#ifdef LCD_DISPLAY
  //Clean the screen with black or white.
  myLCD.CleanAll(WHITE);    //Clean the screen with black or white.
#endif
#ifdef USE_MENU
  myMenu.init(&myLCD, &mySensors, &relay);
  menu = myMenu.getCurrentMenuIndex();
#endif


#ifdef USE_DASHBOARD_VIEW
  // need to init button here as we don't use the menu capability
  myButton = Button();

  //delimiter for date

  myLCD.DrawHLineAt(0, LCD_X_SIZE_MAX - 1, ROW_OFFSET - 2, WHITE);
  //delimiter for time
  //myLCD.DrawVLineAt(0, ROW_OFFSET -2, (CHAR_OFFSET*15)-2, WHITE);
  //delimiter for status bits
  myLCD.DrawHLineAt(0, LCD_X_SIZE_MAX - 1, (5 * ROW_OFFSET) - 3, WHITE);
  for (int i =1; i<7; i++)  myLCD.DrawVLineAt((5 * ROW_OFFSET) - 3, LCD_Y_SIZE_MAX - 1, (i * 2* CHAR_OFFSET) - 4, WHITE);
  // delimiter for device ID bottom right corner
  myLCD.DrawVLineAt((5 * ROW_OFFSET) - 3, LCD_Y_SIZE_MAX - 1, (15 * CHAR_OFFSET) - 2, WHITE);
  //inside symbol
  myLCD.DrawRectangleAt(0, 2 * ROW_OFFSET , 11, 7, WHITE_NO_FILL);
  myLCD.DrawCircleAt(5, 2 * ROW_OFFSET + 3 , 2, WHITE_FILL);

  //outside symbol
  myLCD.DrawCircleAt(5, 3 * ROW_OFFSET + 3 , 2, WHITE_FILL);
  //notification area
  myLCD.DrawHLineAt(0, LCD_X_SIZE_MAX - 1, (2 * ROW_OFFSET) - 3, WHITE);


  /*   myLCD.DispStringAt("12:23",CHAR_OFFSET*15,0);
     //myLCD.DispStringAt("Custom",0,ROW_OFFSET);
     myLCD.DispStringAt("SAM 25 JUL",0,0);
     myLCD.DispStringAt("RDV DENTISTE 16H15",0,(4* ROW_OFFSET));*/
#endif

#ifdef DEBUG
  Serial.println("ready");
#endif

}

void send_relay_state()
{
  if (relay.on) {
   bridge.sendInteger("relay1", relay.getState(0) | (relay.getMode(0) << 1));
   bridge.sendInteger("relay2", relay.getState(1) | (relay.getMode(1) << 1));
   bridge.sendInteger("relay3", relay.getState(2) | (relay.getMode(2) << 1));
   bridge.sendInteger("relay4", relay.getState(3) | (relay.getMode(3) << 1));
  }
}

uint8_t updateCounter = CALL_WATER_SENSOR;
void loop()
{
  char buf[21];
  memset(buf, 0, 21);
#ifdef DEBUG
  Serial.print(".");
#endif
  previousMillis = millis();

#ifdef I2CLIB
  I2c.begin();
#endif

#ifdef USE_DASHBOARD_VIEW

  uint8_t symbol = 32; //space
            
  // get btn state if any
  char btnId[4];
  btnId[0] = 'b';
  btnId[1] = 't';
  btnId[2] = 'n';
  unsigned char btnCurr = myButton.getCurrent();
  unsigned char btnPrev = myButton.getPrev(btnCurr);
  if (btnCurr && btnPrev != btnCurr)
  {
    btnId[3] = (btnCurr + 48);
    bridge.sendInteger(btnId, 1);//send btn down
    // myLCD.DispStringAt(btnId,0, 5 * ROW_OFFSET);
  }
  if (btnPrev && btnPrev != btnCurr) // send state to yun
  {
    btnId[3] = (btnPrev + 48);
    bridge.sendInteger(btnId, 0); //send btn release
    // myLCD.DispStringAt(btnId,40, 5 * ROW_OFFSET);
  }
#endif /*USE_DASHBOARD_VIEW*/

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
#ifdef USE_MENU
    mySensors.update(true, true, menu);
#endif

#ifdef USE_DASHBOARD_VIEW
    // update sensor and store value in variables
    mySensors.update(false, true, 0);


#ifdef LCD_DISPLAY
    // update screen content
    myLCD.DispStringAt("               ", CHAR_OFFSET * 3, 2 * ROW_OFFSET);
    myLCD.CharGotoXY(CHAR_OFFSET * 3, 2 * ROW_OFFSET);
#if 1
    float temp = 19.43;
    if (temp > -10) myLCD.CharGotoXY(CHAR_OFFSET * 4, 2 * ROW_OFFSET);
    else myLCD.CharGotoXY(CHAR_OFFSET * 3, 2 * ROW_OFFSET);
    myLCD.print(temp, 0);
    myLCD.DrawCircleAt((CHAR_OFFSET * 6) + 2, 2 * ROW_OFFSET, 1, WHITE_NO_FILL); //degree symbol
    myLCD.DispStringAt("C ", CHAR_OFFSET * 7, 2 * ROW_OFFSET);
    //Relative humidtity
    myLCD.print(32.87, 0);
    myLCD.print("% ");
    //goutte d'eau : disc + point
    myLCD.DrawCircleAt((CHAR_OFFSET * 12) + 2, 2 * ROW_OFFSET + 4, 2, WHITE_FILL);
    myLCD.DrawDotAt((CHAR_OFFSET * 12) + 2, 2 * ROW_OFFSET + 1, WHITE);
    //co2
    myLCD.CharGotoXY(CHAR_OFFSET * 14, 2 * ROW_OFFSET);
    myLCD.print(1192);
    myLCD.print("ppm");
#endif
#ifdef SENSOR_TYPE_AIR_TEMPERATURE_SENSOR
    myLCD.print(mySensors.air_temperature, 0);
    myLCD.print("\'C ");
#endif
#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR
    myLCD.print(mySensors.air_humidity, 0);
    myLCD.print("% ");
#endif /*SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR*/
#ifdef SENSOR_TYPE_CO2_SENSOR
    myLCD.print(mySensors.air_CO2);
    myLCD.print("ppm");
#endif /*SENSOR_TYPE_CO2_SENSOR*/
#endif /* LCD_DISPLAY*/
#endif /* USE_DASHBOARD_VIEW*/;

    /*****************************************************/
    /* send commands, status or values to the Linux side */
    /*****************************************************/


    while (bridge.getCommand(&bridgeCommandType, &bridgeCommandValue, buf) > 0)
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
              relay.popup(num);
            }
          }
          break;
        case BRIDGE_OS:
          bridge.linuxReady = (bridgeCommandValue == BRIDGE_OS_UP);
#ifdef USE_MENU
          if (myMenu.getCurrentMenuIndex() == n_menu_main) // update id displayed on main menu
            myMenu.mainMenu();
#endif
          break;
        case BRIDGE_DEVICE:
          bridge.deviceID = bridgeCommandValue;
#ifdef USE_MENU
          if (myMenu.getCurrentMenuIndex() == n_menu_main) // update id displayed on main menu
            myMenu.mainMenu();
#endif

#ifdef USE_DASHBOARD_VIEW
          myLCD.DispStringAt("ID:", 15 * CHAR_OFFSET, 5 * ROW_OFFSET);
          myLCD.print(bridge.deviceID);
#endif
          break;

#ifdef USE_DASHBOARD_VIEW
        case BRIDGE_SYSTEM_TIME:
          myLCD.DispStringAt("                     ", 0, 0);
          myLCD.DispStringAt(buf, 0, 0);

          Serial.print("time:");
          Serial.println(buf);
          break;
        /*  case BRIDGE_SYSTEM_DATE:
             myLCD.DispStringAt(buf,0,0);
             break;*/
        case BRIDGE_SYSTEM_NOTIF:
          myLCD.DispStringAt("                     ", 0, 1 * ROW_OFFSET);
          myLCD.DispStringAt(buf, 0, 1 * ROW_OFFSET);
          break;
        case BRIDGE_STATE_SCENARIO:
            myLCD.CharGotoXY(13 * CHAR_OFFSET , 5 * ROW_OFFSET);
            if (bridgeCommandValue) 
            {
              symbol = 83;
            }
          break;
        case BRIDGE_STATE_WINDOW:
            myLCD.CharGotoXY(10 * CHAR_OFFSET , 5 * ROW_OFFSET);
            if (bridgeCommandValue) 
            {
              symbol = 70;
            }
          break;
        case BRIDGE_STATE_UP:
            myLCD.CharGotoXY(0 * CHAR_OFFSET , 5 * ROW_OFFSET);
            if (bridgeCommandValue) 
            {
              symbol = ArrowUp;
            }
            myLCD.write(symbol);
          break;
        case BRIDGE_STATE_DOWN:
            myLCD.CharGotoXY(2 * CHAR_OFFSET , 5 * ROW_OFFSET);
            if (bridgeCommandValue) 
            {
              symbol = ArrowDown;
            }
            myLCD.write(symbol);
          break;
        case BRIDGE_STATE_LIGHT:
            myLCD.CharGotoXY(4 * CHAR_OFFSET , 5 * ROW_OFFSET);
            if (bridgeCommandValue) 
            {
              symbol = 76;
            }
            myLCD.write(symbol);
          break;
        case BRIDGE_STATE_CLIMATE:
            myLCD.CharGotoXY(6 * CHAR_OFFSET , 5 * ROW_OFFSET);
            if (bridgeCommandValue) 
            {
              symbol = 67;
            }
            myLCD.write(symbol);
            break;
        case BRIDGE_SYSTEM_REMOTE_INFO:
          myLCD.DispStringAt("                     ", 0, 4 * ROW_OFFSET);
          myLCD.DispStringAt(buf, 0, 4 * ROW_OFFSET);
          break;
        case BRIDGE_WEATHER_FORECAST:
          myLCD.DispStringAt("                     ", 0, 3 * ROW_OFFSET);
          myLCD.DispStringAt(buf, 0, 3 * ROW_OFFSET);
          break;
#endif  /*USE_DASHBOARD_VIEW*/
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
#ifdef USE_MENU
          if (myMenu.getCurrentMenuIndex() == n_menu_main) // update network status
            myMenu.mainMenu();
#endif
#ifdef USE_DASHBOARD_VIEW
            myLCD.CharGotoXY(8 * CHAR_OFFSET , 5 * ROW_OFFSET);
            if (bridgeCommandValue > BRIDGE_NETWORK_UP) 
            {
              if(bridgeCommandValue == BRIDGE_NETWORK_SOCKET) symbol = 64;
              else symbol = 35;
            }
            myLCD.write(symbol);
#endif  /*USE_DASHBOARD_VIEW*/

          break;
        default:

#ifdef DEBUG
          Serial.print(F("D "));
          Serial.println(bridgeCommandType);
#endif
          break;

      }
      memset(buf, 0, 21);
    }

    //send relay state to Linux
    send_relay_state();
    


    updateCounter = NBRE_CYCLE;
  }
  else
    updateCounter --;



  /********************************************/
  /* Update relay values                      */
  /********************************************/
  relay.updateState();

#ifdef LCD_DISPLAY

#ifdef USE_MENU
  /*****************************************************/
  /*  MENU DISPLAY                                     */
  /*****************************************************/
  menu = myMenu.printMenu(menu);
#endif /*USE MENU*/
#endif /* LCD_DISPLAY */
#ifdef I2CLIB
  I2c.end();
#endif

  elapsed = (millis() - previousMillis);
  if (elapsed < TIME_CYCLE_MS) delay(TIME_CYCLE_MS -  elapsed); //loop every 150 msec exactly

}
