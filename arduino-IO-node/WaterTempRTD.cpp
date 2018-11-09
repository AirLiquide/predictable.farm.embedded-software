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
