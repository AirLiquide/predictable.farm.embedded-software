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
#include "Waterlevel.h"

#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR
Waterlevel::Waterlevel() {
  Wire.begin();
}

/**
   \fn void Waterlevel::init(void)
   \brief initialize the ultrasound sensor

   \param n/a
   \return n/a
*/
float Waterlevel::getDistance() {
  uint8_t i2CData[4];
  uint8_t i = 0;
  float2bytes_t b2f;

  Wire.beginTransmission(WATER_LEVEL_ADDR); // Envoyer vers device #4
  Wire.write('V'); // Envoi un 1
  Wire.endTransmission(); // Arreter la transmission

  Wire.requestFrom(WATER_LEVEL_ADDR, 4); // receive a byte of data

  while (Wire.available())   // Wire.available() will return the number of bytes available to read
  {
    i2CData[i++] = Wire.read();
  }

  b2f.b[0] = i2CData[0];
  b2f.b[1] = i2CData[1];
  b2f.b[2] = i2CData[2];
  b2f.b[3] = i2CData[3];
  return b2f.f;
}

void Waterlevel::Calibration(){
  Wire.beginTransmission(WATER_LEVEL_ADDR); // Envoyer vers device #4
  Wire.write('C'); // Envoi un 1
  Wire.endTransmission(); // Arreter la transmission
  Wire.requestFrom(WATER_LEVEL_ADDR, 0);
}

#endif
