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
#include "YunBridge.h"
//#define DEBUG_YUN

/**
   \fn void YunBridge::init(void)
   \brief initialize the YunBridge

   \param n/a
   \return n/a
*/
void YunBridge::init()
{
  Serial1.begin(115200);
  Serial1.println(STATUS_REQUEST);
}


/**
   \fn void YunBridge::prepareJSON(char * sensorName)
   \brief prepare to send sensor status to linux

   \param sensorName
   \return n/a
*/
void YunBridge::sendJSON(char * sensorName, char * data)
{
  char message[40];
  sprintf(message, "{\"t\":\"%s\",\"v\":\"%s\"}", sensorName, data);
  Serial1.println(message);
}


/**
   \fn void YunBridge::sendInteger(char * sensorName, unsigned long  data)
   \brief sends the status of sensors to linux

   \param sensorName, data
   \return n/a
*/
void YunBridge::sendInteger(char * sensorName, unsigned int data)
{
  if (linuxReady && networkReady) {
    char message[10];
    sprintf(message,"%d",data);
    sendJSON(sensorName, message);
  }
}


/**
   \fn void YunBridge::sendFloat( char * sensorName, float  data)
   \brief sends the status of sensors to linux

   \param sensorName, data
   \return n/a
*/
void YunBridge::sendFloat(char * sensorName, float data)
{
  if (linuxReady && networkReady) {
    char message[10];
    sprintf(message,"%d.%02d", (int)data, (int)(data*100)%100);
    sendJSON(sensorName, message);
  }
}

/**
   \fn int YunBridge::getCommand(unsigned char * p_o_command, unsigned char * p_o_value)
   \brief gets commands and values from the linux

   \param pointer to command and value
   \return nbread Int
*/
int YunBridge::getCommand(unsigned char * p_o_command, int * p_o_value)
{
  char incomingBytes[9]; // 1 + 8
  int nbRead = 0;
#ifdef DEBUG_YUN
  Serial.println(F("Looking for command..."));
#endif
  if (Serial1.available() > 0)
  {
#ifdef DEBUG_YUN
    Serial.println(Serial1.available());
    Serial.println(F("data available"));
#endif
    nbRead = Serial1.readBytesUntil('\n', incomingBytes, 9);
    if (nbRead  > 1)
    {
      *p_o_command = incomingBytes[0];
      incomingBytes[nbRead] = '\0'; // truncate to actual number of chars read
      *p_o_value = atoi(incomingBytes + 1);
    }
  }

  return nbRead;
#ifdef DEBUG_YUN
  else Serial.print("fail");
#endif

}
