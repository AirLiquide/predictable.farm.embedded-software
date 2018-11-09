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
#ifndef __SENSORTYPES_H__
#define __SENSORTYPES_H__


// sensors enabled or diabled are in this file
#include "config.h"


/* sensor i2c address */
//info:  LCD 0x51
#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR
#define  WATER_LEVEL_ADDR                       0X02
#endif
#ifdef SENSOR_TYPE_CO_SENSOR
#define  AIR_CO_MEASUREMENT_ADDR                0x04
#endif
#ifdef SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR
#define  LIGHT_LUX_MEASUREMENT_ADDR             0x29
#define  LIGHT_UV_MEASUREMENT_ADDR              0x41
#endif
#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR
#define  AIR_HUMIDITY_LEVEL_MEASUREMENT_ADDR    0x44
#endif
#define  ADC_MEASUREMENT_ADDR                   0x48
#endif
#ifdef SENSOR_TYPE_CO2_SENSOR
#define  AIR_CO2_MEASUREMENT_ADDR               0x4D
#endif
#ifdef SENSOR_TYPE_PRESSURE_SENSOR
#define  AIR_PRESSURE_MEASUREMENT_ADDR          0x60
#endif
#ifdef SENSOR_TYPE_WATER_DO_SENSOR
#define  WATER_DO_MEASUREMENT_ADDR              0x61 // up to 4 identical water sensor ( Temp, DO , Ph and EC)  per node
#define  WATER_DO_MEASUREMENT_ADDR2             0x71
#define  WATER_DO_MEASUREMENT_ADDR3             0x81
#define  WATER_DO_MEASUREMENT_ADDR4             0x91
#endif
#ifdef SENSOR_TYPE_WATER_ORP_SENSOR
#define  WATER_ORP_MEASUREMENT_ADDR             0x62
#endif
#ifdef SENSOR_TYPE_WATER_PH_SENSOR
#define  WATER_PH_MEASUREMENT_ADDR              0x63
#define  WATER_PH_MEASUREMENT_ADDR2             0x73
#define  WATER_PH_MEASUREMENT_ADDR3             0x83
#define  WATER_PH_MEASUREMENT_ADDR4             0x93
#endif
#ifdef SENSOR_TYPE_WATER_EC_SENSOR
#define  WATER_EC_MEASUREMENT_ADDR              0x64
#define  WATER_EC_MEASUREMENT_ADDR2             0x74
#define  WATER_EC_MEASUREMENT_ADDR3             0x84
#define  WATER_EC_MEASUREMENT_ADDR4             0x94
#endif
#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
#define  WATER_TEMPERATURE_MEASUREMENT_ADDR     0x66
#define  WATER_TEMPERATURE_MEASUREMENT_ADDR2    0x76
#define  WATER_TEMPERATURE_MEASUREMENT_ADDR3    0x86
#define  WATER_TEMPERATURE_MEASUREMENT_ADDR4    0x96
#endif

/*#define  AIR_TEMPERATURE_MEASUREMENT_ADDR     0x40
  #define  AIR_QUALITY_INDEX_ADDR                 17
  #define  LIGHT_PAR_MEASUREMENT_ADDR             19
  #define  WATER_FLOW_MEASUREMENT_ADDR            27
  #define  SOIL_MOISTURE_LEVEL_ADDR               32*/


/* sensor network name  NNAME 
  sensor displayed name DNAME 
  sensor displayed unit UNIT */
#define  RELAY_NNAME                            "relay"
#define  RELAY_DNAME                            "RELAY "

#ifdef SENSOR_TYPE_PRESSURE_SENSOR
#define  AIR_PRESSURE_MEASUREMENT_NNAME         "air_pressure"
#define  AIR_PRESSURE_MEASUREMENT_DNAME         "ATM :"
#define  AIR_PRESSURE_MEASUREMENT_UNIT          " kPa"
#endif
#ifdef SENSOR_TYPE_CO2_SENSOR
#define  AIR_CO2_MEASUREMENT_NNAME              "air_co2"
#define  AIR_CO2_MEASUREMENT_DNAME              "CO2 :"
#define  AIR_CO2_MEASUREMENT_UNIT               " ppm"
#endif
#ifdef SENSOR_TYPE_CO_SENSOR
#define  AIR_CO_MEASUREMENT_NNAME               "air_co"
#define  AIR_CO_MEASUREMENT_DNAME               "CO :"
#endif
#ifdef SENSOR_TYPE_AIR_TEMPERATURE_SENSOR
#define  AIR_TEMPERATURE_MEASUREMENT_NNAME      "air_temperature"
#define  AIR_TEMPERATURE_MEASUREMENT_DNAME      "Air T. :"
#ifndef TEMPERATURE_MEASUREMENT_UNIT
#define  TEMPERATURE_MEASUREMENT_UNIT           " C"
#endif
#endif
#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR
#define  AIR_HUMIDITY_LEVEL_MEASUREMENT_NNAME   "air_humidity"
#define  AIR_HUMIDITY_LEVEL_MEASUREMENT_DNAME   "Air RH :"
#define  AIR_HUMIDITY_LEVEL_MEASUREMENT_UNIT    " %"
#endif
#ifdef SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR
  #define  LIGHT_LUX_MEASUREMENT_NNAME          "light_lux"
  #define  LIGHT_LUX_DNAME                      "LUX :"
  #define  LIGHT_LUX_MEASUREMENT_UNIT           ""
#endif
#ifdef SENSOR_TYPE_PAR_SENSOR
#define  LIGHT_PAR_MEASUREMENT_NNAME            "light_par"
#define  LIGHT_PAR_MEASUREMENT_DNAME            "PAR :"
#define  LIGHT_PAR_MEASUREMENT_UNIT             " ppf" //mol.m-2.s-1"
#endif
#ifdef SENSOR_TYPE_UV_SENSOR
#define  LIGHT_UV_MEASUREMENT_NNAME             "light_uv"
#define  LIGHT_UV_MEASUREMENT_DNAME             "UV :"
#define  LIGHT_UV_MEASUREMENT_UNIT              ""
#endif
#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
#define  WATER_TEMPERATURE_MEASUREMENT_NNAME    "water_temperature"
#define  WATER_TEMPERATURE_MEASUREMENT_DNAME    "Water T. :"
#ifndef TEMPERATURE_MEASUREMENT_UNIT
#define  TEMPERATURE_MEASUREMENT_UNIT           " C"
#endif
#endif
#ifdef SENSOR_TYPE_WATER_PH_SENSOR
#define  WATER_PH_MEASUREMENT_NNAME             "water_ph"
#define  WATER_PH_MEASUREMENT_DNAME             "pH :"
#define  WATER_PH_MEASUREMENT_UNIT              ""
#endif
#ifdef SENSOR_TYPE_WATER_EC_SENSOR
#define  WATER_EC_MEASUREMENT_NNAME             "water_ec"
#define  WATER_EC_MEASUREMENT_DNAME             "EC :"
#define  WATER_EC_MEASUREMENT_UNIT              " uS/cm"
#endif
#ifdef SENSOR_TYPE_WATER_ORP_SENSOR
#define  WATER_ORP_MEASUREMENT_NNAME            "water_orp"
#define  WATER_ORP_MEASUREMENT_DNAME            "ORP :"
#define  WATER_ORP_MEASUREMENT_UNIT             " mV"
#endif
#ifdef SENSOR_TYPE_WATER_DO_SENSOR
#define  WATER_DO_MEASUREMENT_NNAME             "water_do"
#define  WATER_DO_MEASUREMENT_DNAME             "DO :"
#define  WATER_DO_MEASUREMENT_UNIT              " mg/L"
#endif
#ifdef SENSOR_TYPE_SOIL_MOISTURE_SENSOR
#define  SOIL_MOISTURE_MEASUREMENT_NNAME        "moisture"
#define  SOIL_MOISTURE_MEASUREMENT_DNAME        "Moist. :"
#define  SOIL_MOISTURE_MEASUREMENT_UNIT         " kPa"
#endif
#ifdef SENSOR_TYPE_SOIL_TEMPERATURE_SENSOR
#define  SOIL_TEMPERATURE_MEASUREMENT_NNAME     "soil_temperature"
#define  SOIL_TEMPERATURE_MEASUREMENT_DNAME     "Soil T. :"
#ifndef TEMPERATURE_MEASUREMENT_UNIT
#define  TEMPERATURE_MEASUREMENT_UNIT           " C"
#endif
#endif
#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR
#define  WATER_LEVEL_NNAME                      "water_level"
#define  WATER_LEVEL_DNAME                      "Water lvl :"
#define  WATER_LEVEL_UNIT                       " cm"
#endif
