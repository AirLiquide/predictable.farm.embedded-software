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
#ifndef __YUNBRIDGE_H__
#define __YUNBRIDGE_H__

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "SensorTypes.h"

#define BRIDGE_RELAY_1 'a'
#define BRIDGE_RELAY_2 'b'
#define BRIDGE_RELAY_3 'c'
#define BRIDGE_RELAY_4 'd'

#define BRIDGE_STATE_WINDOW         'f'
#define BRIDGE_STATE_DOWN           'g'
#define BRIDGE_STATE_UP             'h'
#define BRIDGE_DEVICE      'i'

#define BRIDGE_START        '#'

#define BRIDGE_WEATHER_FORECAST     'k'
#define BRIDGE_STATE_LIGHT           'l'
#define BRIDGE_STATE_CLIMATE          'm'
#define BRIDGE_NETWORK     'n'
#define BRIDGE_OS          'o'
#define BRIDGE_SYSTEM_REMOTE_INFO   'p'
#define BRIDGE_STATE_SCENARIO       'r'
#define BRIDGE_STATUS_REQUEST       's'
#define BRIDGE_SYSTEM_TIME          't'
#define BRIDGE_SYSTEM_CONFIG        'u'
#define BRIDGE_SYSTEM_REBOOT        'v'
#define BRIDGE_SYSTEM_UPDATE        'w'
#define BRIDGE_SYSTEM_NOTIF         'x'
#define BRIDGE_SYSTEM_DATE          'y'

#define BRIDGE_ACK      "ACK"
#define BRIDGE_OS_DOWN      0
#define BRIDGE_OS_UP        1
#define BRIDGE_NETWORK_DOWN    0
#define BRIDGE_NETWORK_UP      1
#define BRIDGE_NETWORK_SOCKET  2

class YunBridge
{
  public:

    unsigned int deviceID = 0;
    bool linuxReady = false;
    bool networkReady = false;
    bool socketReady = false;

    void init();
    void sendFloat(char * sensorName, float data);
    void sendInteger(char * sensorName, unsigned int data);

    int getCommand(unsigned char * p_o_command, int * p_o_value, unsigned char * p_o_buff);

  private:

    void sendJSON(char * sensorName, char * data);
};

#endif
