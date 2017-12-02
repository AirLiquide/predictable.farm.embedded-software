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
#include "WaterTempRTD.h"
#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
/**
 * \fn void WaterTempRTD::init(void)
 * \brief initialize the water temp sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterTempRTD::init()
{
  myEZO = EZO();
  myEZO.sendCommand(WATER_TEMPERATURE_MEASUREMENT_ADDR, "D,0", 300);
}

/**
 * \fn void WaterTempRTD::getTemperature(void)
 * \brief sends the request to get the value of the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterTempRTD::getTemperature()
{
  myEZO.sendCommand(WATER_TEMPERATURE_MEASUREMENT_ADDR, "R", 0);
}

/**
 * \fn void WaterTempRTD::calibrate(void)
 * \brief calibrates the sensor with water at 100°
 *
 * \param n/a
 * \return n/a
 */
void WaterTempRTD::calibrate()
{
  myEZO.sendCommand(WATER_TEMPERATURE_MEASUREMENT_ADDR, "Cal,99.5", 1000);
}

/**
 * \fn void WaterTempRTD::clearCalibration(void)
 * \brief clear the calibration of the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterTempRTD::clearCalibration()
{
  myEZO.sendCommand(WATER_TEMPERATURE_MEASUREMENT_ADDR, "Cal,clear", 300);
}
#endif
