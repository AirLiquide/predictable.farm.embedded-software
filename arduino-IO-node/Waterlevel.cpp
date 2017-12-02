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
 // Serial.print("cal");
  Wire.beginTransmission(WATER_LEVEL_ADDR); // Envoyer vers device #4
  Wire.write('C'); // Envoi un 1
  Wire.endTransmission(); // Arreter la transmission
  Wire.requestFrom(WATER_LEVEL_ADDR, 0);
}

#endif
