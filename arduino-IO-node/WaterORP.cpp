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
#include "WaterORP.h"

/**
 * \fn void WaterORP::init(void)
 * \brief initialize the water ORP sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterORP::init()
{
  myEZO = EZO();
  myEZO.sendCommand(WATER_ORP_MEASUREMENT_ADDR, "D,0", 450);
}

/**
 * \fn void WaterORP::getORP(void)
 * \brief sends the request to get the value of the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterORP::getORP()
{
  myEZO.sendCommand(WATER_ORP_MEASUREMENT_ADDR, "R", 0);
}

/**
 * \fn void WaterORP::calibrate(void)
 * \brief calibrates the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterORP::calibrate(void)
{
  myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, "cal,225", 300);
}

/**
 * \fn void WaterORP::clearCalibration(void)
 * \brief clear the calibration of the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterORP::clearCalibration(void)
{
  myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, "Cal,clear", 300);
}
