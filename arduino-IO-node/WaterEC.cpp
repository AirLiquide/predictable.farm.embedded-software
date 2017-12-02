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
#include "WaterEC.h"
#ifdef SENSOR_TYPE_WATER_EC_SENSOR
/**
 * \fn void WaterEC::init()
 * \brief initialize the water EC sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterEC::init()
{
  myEZO = EZO();
  myEZO.sendCommand(WATER_EC_MEASUREMENT_ADDR, "O,TDS,0", 300);//disable TDS measure
  myEZO.sendCommand(WATER_EC_MEASUREMENT_ADDR, "O,S,0", 300);//disable Salinity measure
  myEZO.sendCommand(WATER_EC_MEASUREMENT_ADDR, "O,SG,0", 300);//disable SpecificGRavity of sea water measure
}


/**
 * \fn void WaterEC::getEc()
 * \brief sends the request to get the value of the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterEC::getEc()
{
  myEZO.sendCommand(WATER_EC_MEASUREMENT_ADDR, "R", 0);
}

/**
 * \fn void WaterORP::init()
 * \brief sends the request to compensate the sensor because of the temperature
 *
 * \param temperature co compensate
 * \return n/a
 */
void WaterEC::setTemperatureCompensation(float temperature)
{
  char cmd[5];
  memset(cmd, 0, 5);
  sprintf(cmd, "T,%d", (int)temperature);
  myEZO.sendCommand(WATER_EC_MEASUREMENT_ADDR, cmd, 0);
}


/**
 * \fn void WaterEC::calibrate_dry()
 * \brief calibrates the sensor in open air
 *
 * \param n/a
 * \return n/a
 */
void WaterEC::calibrate_dry()
{
  myEZO.sendCommand(WATER_EC_MEASUREMENT_ADDR, "Cal,dry", 300);
}

/**
 * \fn void WaterEC::calibrate_high(int value)
 * \brief calibrates the sensor to a specific value
 *
 * \param value: calibration value
 * \return n/a
 */
void WaterEC::calibrate_high(int value)
{
  char cmd[14];
  sprintf(cmd, "Cal,high,%d", (int)value);
  myEZO.sendCommand(WATER_EC_MEASUREMENT_ADDR, cmd, 300);

}

/**
 * \fn void WaterEC::calibrate_low()
 * \brief calibrates the sensor to a specific value
 *
 * \param n/a
 * \return n/a
 */
void WaterEC::calibrate_low()
{
  myEZO.sendCommand(WATER_EC_MEASUREMENT_ADDR, "Cal,low,1413", 300);
}

/**
 * \fn void WaterEC::clearCalibration()
 * \brief clear the calibration of the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterEC::clearCalibration()
{
  myEZO.sendCommand(WATER_EC_MEASUREMENT_ADDR, "Cal,clear", 300);
}
#endif
