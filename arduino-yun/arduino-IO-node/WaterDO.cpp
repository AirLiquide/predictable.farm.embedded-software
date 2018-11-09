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
#include "WaterDO.h"

#ifdef SENSOR_TYPE_WATER_DO_SENSOR

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
#endif
