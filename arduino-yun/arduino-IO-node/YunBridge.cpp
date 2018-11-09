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
#include "YunBridge.h"

/**
   \fn void YunBridge::init(void)
   \brief initialize the YunBridge

   \param n/a
   \return n/a
*/
void YunBridge::init()
{
  Serial1.begin(115200);
  Serial1.println(BRIDGE_STATUS_REQUEST);
}


/**
   \fn void YunBridge::sendJSON(char * sensorNamen char * data)
   \brief sends JSON to Linux

   \param sensorName, data
   \return n/a
*/
void YunBridge::sendJSON(char * sensorName, char * data)
{
  char message[30];
  //wait for ack

  sprintf(message, "{\"t\":\"%s\",\"v\":\"%s\"}", sensorName, data);
  Serial1.println(message);

#ifdef USE_ACK
  unsigned char timeout = 0;
  bool msgSent = false;
  do
  {
 //   timeout--;
    if (Serial1.available() > 0)
    {
      char ack[3];
      memset(ack, 0, 3);
      Serial1.readBytes( ack, sizeof(BRIDGE_ACK+1))  ;
      if (strcmp(ack, BRIDGE_ACK) != 0)
      {
        delay(10);
        msgSent = false;
#ifdef DEBUG
        Serial.print("no ack:");
        Serial.println(ack);
#endif
      }
      else {
        msgSent = true;
#ifdef DEBUG
        Serial.println("ack");
#endif
      }
    }
    else
    {
      delay(10);
#ifdef DEBUG
      Serial.println("delayed");
#endif
    }
    if (timeout == 0)
    {
#ifdef DEBUG
      Serial.println("skip");
#endif
      return;
    }
  }
  while (!msgSent);
#endif
}


/**
   \fn void YunBridge::sendInteger(char * sensorName, unsigned int  data)
   \brief sends the status of sensors to linux

   \param sensorName, data
   \return n/a
*/
void YunBridge::sendInteger(char * sensorName, unsigned int data)
{
  if (linuxReady && networkReady) {
    if (isnan(data) != 1) {
      char message[10];
      sprintf(message, "%d", data);
      sendJSON(sensorName, message);
    }
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
    if (isnan(data) != 1) {
      char message[10];
      sprintf(message, "%d.%02d", (int)data, (int)(data * 100) % 100);
      sendJSON(sensorName, message);
    }
  }
}

/**
   \fn int YunBridge::getCommand(unsigned char * p_o_command, unsigned char * p_o_value)
   \brief gets commands and values from the linux

   \param pointer to command and value, buffr (8 bytes after command if exist)
   \return nbread Int
*/
int YunBridge::getCommand(unsigned char * p_o_command, int * p_o_value, unsigned char * p_o_buff)
{
  char incomingBytes[23]; // 1+ 1 + 21 
  uint8_t nbRead = 0;
  uint8_t chksum = 0;
#ifdef DEBUG_YUN
  Serial.println(F("Looking for command..."));
#endif
  if (Serial1.available() > 0)
  {
#ifdef DEBUG_YUN
    Serial.println(Serial1.available());
    Serial.println(F("data available"));
#endif
    nbRead = Serial1.readBytesUntil('\n', incomingBytes, 23);
    if (nbRead  > 1)
    {
     // for(int i=1; i< (nbRead-1); i++) chksum += incomingBytes[i];
      if((incomingBytes[0]==BRIDGE_START)/* && (chksum == incomingBytes[nbRead-1])*/)
      {
        *p_o_command = incomingBytes[1];
        memcpy(p_o_buff,&incomingBytes[2],nbRead -2);
        incomingBytes[nbRead] = '\0'; // truncate to actual number of chars read
        *p_o_value = atoi(p_o_buff);
      }
      else 
      {
        nbRead =0;
#ifdef DEBUG_YUN
   Serial.println("no data header ");
#endif
      }
#ifdef DEBUG_YUN
   Serial.println(incomingBytes);
#endif
    }
  }

  return nbRead;

}
