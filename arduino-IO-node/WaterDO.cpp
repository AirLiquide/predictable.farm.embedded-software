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
#include "WaterDO.h"

/**
 * \fn void WaterDO::init()
 * \brief initialize the water DO sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterDO::init()
{
  myEZO = EZO();
  myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, "O,%,0", 300);//disable percent saturation
  myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, "O,mg,1", 300);//enable mg/L
}

/**
 * \fn void WaterDO::getDissolvedOxygen()
 * \brief sends the request to get the value of the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterDO::getDissolvedOxygen()
{
  myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, "R", 0); // single readings , note that continuous reading is propores by the device
}


/**
 * \fn void WaterEC::init()
 * \brief initialize the water EC sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterDO::setTemperatureCompensation(float temperature)
{
  char cmd[5];
  memset(cmd, 0, 5);
  sprintf(cmd, "T,%d", (int)temperature);
  myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, cmd, 0);
}

/**
 * \fn void WaterDO::setEcCompensation(float ec)
 * \brief sends the request to compensate the sensor because of the EC
 *
 * \param ec to compensate
 * \return n/a
 */
void WaterDO::setEcCompensation(float ec)
{
  if (ec > 2500) {
    char cmd[7];
    memset(cmd, 0, 7);
    sprintf(cmd, "S,%d", (int)ec);
    myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, cmd, 300);
  }
}

/**
 * \fn void WaterDO::calibrate()
 * \brief calibrates the sensor in open air
 *
 * \param n/a
 * \return n/a
 */
void WaterDO::calibrate()
{
  myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, "cal", 1400);
}

/**
 * \fn void WaterDO::calibrate_zero()
 * \brief calibrates the sensor in zero oxygen
 *
 * \param n/a
 * \return n/a
 */
void WaterDO::calibrate_zero()
{
  myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, "cal,0", 1400);
  myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, "cal,?", 500);
}

/**
 * \fn void WaterDO::clearCalibration()
 * \brief clear the calibration of the sensor
 *
 * \param n/a
 * \return n/a
 */
void WaterDO::clearCalibration()
{
  myEZO.sendCommand(WATER_DO_MEASUREMENT_ADDR, "Cal,clear", 300);
}
