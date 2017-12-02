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
#ifndef __WATERPH_H__
#define __WATERPH_H__

#include "SensorTypes.h"
#ifdef SENSOR_TYPE_WATER_PH_SENSOR
#include "EZOCircuit.h"

class WaterPH
{
  public:
    void init();
    void getPh();
    void calibrate_mid();
    void calibrate_low();
    void calibrate_high();
    void clearCalibration();
  private:
    EZO myEZO ;
    //float ph;
};
#endif
#endif
