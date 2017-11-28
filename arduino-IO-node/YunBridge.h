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
#define BRIDGE_SYSTEM_CONFIG        'u'
#define BRIDGE_SYSTEM_REBOOT        'v'
#define BRIDGE_SYSTEM_UPDATE        'w'

#define STATUS_REQUEST     's'
#define BRIDGE_NETWORK     'n'
#define BRIDGE_OS          'o'
#define BRIDGE_DEVICE      'i'
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

    int getCommand(unsigned char * p_o_command, int * p_o_value);

  private:

    void sendJSON(char * sensorName, char * data);
};

#endif
