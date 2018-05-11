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
