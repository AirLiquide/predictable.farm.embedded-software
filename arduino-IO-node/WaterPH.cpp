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
#include "WaterPh.h"
#ifdef SENSOR_TYPE_WATER_PH_SENSOR
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
#endif
