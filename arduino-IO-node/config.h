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
#ifndef _CONFIG_H_
#define _CONFIG_H_

//#define DEBUG

//#define I2CLIB

//#define NOLCD
//#define USE_MENU
#ifndef USE_MENU 
#define USE_DASHBOARD_VIEW
#endif
//#define USE_ACK
/* As the Arduino MCU is limited in size / memory , this is the location where the project can be tweaked to add/remove capabilities */
//#define SENSOR_TYPE_CO2_SENSOR
//#define SENSOR_TYPE_CO_SENSOR
//#define SENSOR_TYPE_SOIL_MOISTURE_SENSOR
//#define SENSOR_TYPE_SOIL_TEMPERATURE_SENSOR
//#define SENSOR_TYPE_PAR_SENSOR
//#define SENSOR_TYPE_UV_SENSOR
//#define SENSOR_TYPE_PRESSURE_SENSOR
//#define SENSOR_TYPE_AIR_TEMPERATURE_SENSOR
//#define AMBIANT_HDC1000_SENSOR //temperature sensor is not accurate, humidity is ok
//#define AMBIANT_TH02_SENSOR  // pose probleme quand utilisé avec d'autres capteurs I2C
//#define SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR

#define ACTUATOR_TYPE_RELAY

//#define SENSOR_TYPE_WATER_LEVEL_SENSOR


//#define SENSOR_TYPE_EZO_SENSOR
//#define SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
//#define SENSOR_TYPE_WATER_PH_SENSOR
//#define SENSOR_TYPE_WATER_EC_SENSOR
//#define SENSOR_TYPE_WATER_DO_SENSOR
//#define SENSOR_TYPE_WATER_ORP_SENSOR


//define SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR

#endif /*_CONFIG_H_*/
