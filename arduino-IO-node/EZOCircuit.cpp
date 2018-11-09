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
#include "EZOCircuit.h"

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
  Wire.requestFrom((uint8_t)addr, (uint8_t)16, (uint8_t)1);
  addr = Wire.read();

  if (addr == 1) {
    while (Wire.available()) {
      addr = Wire.read();
      EZO_data[i] = addr;
      i += 1;
      if (addr == 0) {
        Wire.endTransmission();
        break;
      }
    }
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
