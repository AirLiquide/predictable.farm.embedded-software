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
#include "WaterPh.h"

/**
 * \fn void WaterPH::init()
 * \brief initialize the water pH sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterPH::init()
{
  myEZO = EZO();
  myEZO.sendCommand(WATER_PH_MEASUREMENT_ADDR, "L,1", 400);
}

/**
 * \fn void WaterPH::getPh()
 * \brief sends the request to get the value of the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterPH::getPh()
{
  myEZO.sendCommand(WATER_PH_MEASUREMENT_ADDR, "R", 0);
}

/**
 * \fn void WaterPH::calibrate_mid()
 * \brief calibrates the sensor with solution at 7
 *
 * \param n/a
 * \return n/a
 */
void WaterPH::calibrate_mid()
{
  myEZO.sendCommand(WATER_PH_MEASUREMENT_ADDR, "cal,mid,7.00", 300);
}

/**
 * \fn void WaterPH::calibrate_low()
 * \brief calibrates the sensor with solution at 4
 *
 * \param n/a
 * \return n/a
 */
void WaterPH::calibrate_low()
{
  myEZO.sendCommand(WATER_PH_MEASUREMENT_ADDR, "Cal,low,4.00", 400);
}
/**
 * \fn void WaterPH::calibrate_high()
 * \brief calibrates the sensor with solution at 10
 *
 * \param n/a
 * \return n/a
 */
void WaterPH::calibrate_high()
{
  myEZO.sendCommand(WATER_PH_MEASUREMENT_ADDR, "Cal,high,10.00", 400);
}

/**
 * \fn void WaterPH::clearCalibration()
 * \brief clear the calibration of the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterPH::clearCalibration()
{
  myEZO.sendCommand(WATER_PH_MEASUREMENT_ADDR, "Cal,clear", 300);
}
