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
#ifndef __WATER_LEVEL_H__
#define __WATER_LEVEL_H__

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <Wire.h>
#include "SensorTypes.h"
#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR

typedef union float2bytes_t   // union consists of one variable represented in a number of different ways
{
  float f;
  char b[sizeof(float)];
};

class Waterlevel
{
  public:
    Waterlevel();
    float getDistance();
    void Calibration();
};
#endif 
#endif
