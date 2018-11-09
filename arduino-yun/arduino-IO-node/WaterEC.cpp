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
