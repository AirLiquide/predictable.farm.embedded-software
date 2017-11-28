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

#include "EZOCircuit.h"
//#define DEBUG

/**
 * \fn void EZO::readData(char addr)
 * \brief sends a request to the water sensors
 *
 * \param addr of the sensor
 * \return n/a
 */
bool EZO::readData(char addr)
{
  char i = 0;

  // send configuration command in form of text command (Atlas Scientific EZO protocol)
  Wire.requestFrom(addr, 16, 1);
  addr = Wire.read();

  if (addr == 1) {
    while (Wire.available()) {
      addr = Wire.read();
      EZO_data[i] = addr;
      i += 1;
      if (addr == 0) {
        Wire.endTransmission();
      }
    }
    // Serial.println("True");
    return true;
#ifdef DEBUG
    if (strlen(EZO_data) > 0)
    {
      Serial.print("EZO Data:");
      Serial.println(EZO_data);

    }
#endif
  } else {
    return false;
  }
}

/**
 * \fn void EZO::sendCommand(char addr, char * i_p_cmd, unsigned short wait_delay)
 * \brief waits for the linux to boot
 *
 * \param addr: address of the sensor, i_p_cmd: the command for the sensor, wait_delay: the delay before the response
 * \return n/a
 */
void EZO::sendCommand(char addr, char * i_p_cmd, unsigned short wait_delay)
{
  // send configuration command in form of text command (Atlas Scientific EZO protocol)
  Wire.beginTransmission(addr);
  Wire.write(i_p_cmd);
  Wire.endTransmission();

  // if we wait for the response
  if (wait_delay > 0) {
    delay(wait_delay);
    readData(addr);
  }

}

/**
 * \fn char * EZO::getData()
 * \brief sends the data
 *
 * \param n/a
 * \return the data
 */
char * EZO::getData()
{
  return EZO_data;
}
