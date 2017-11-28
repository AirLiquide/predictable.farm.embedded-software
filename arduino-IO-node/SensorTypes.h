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
#ifndef __SENSORTYPES_H__
#define __SENSORTYPES_H__

#define CO2_SENSOR
//#define CO_SENSOR
//#define AMBIANT_HDC1000_SENSOR //temperature sensor is not accurate, humidity is ok
//#define AMBIANT_TH02_SENSOR  // pose probleme quand utilisé avec d'autres capteurs I2C
//#define DUST_SENSOR
//#define MOISTURE_SENSOR
//#define UV_SENSOR
#define PAR_SENSOR
//#define SOIL_TEMPERATURE_SENSOR
//#define WATER_LEVEL_SENSOR
//define LOW_COST_SUNLIGHT_SENSOR


/* internal bitfield definitions
  #define SENSOR_LIGHT_PAR                        (0x1<<0)
  #define SENSOR_LIGHT_UV                         (0x1<<1)
  #define sensor_air_CO                            (0x1<<2)
  #define sensor_air_CO 2                          (0x1<<3)
  #define SENSOR_AIR_TEMPERATURE                  (0x1<<4)
  #define SENSOR_AIR_RELATIVE_HUMIDITY            (0x1<<5)
  #define SENSOR_AIR_PRESSURE                     (0x1<<6)
  #define SENSOR_SOIL_MOISTURE                    (0x1<<7)
  #define SENSOR_WATER_TEMPERATURE                (0x1<<8)
  #define SENSOR_WATER_DO                         (0x1<<9)
  #define SENSOR_WATER_EC                         (0x1<<10)
  #define SENSOR_WATER_PH                         (0x1<<11)
  #define SENSOR_WATER_ORP                        (0x1<<12)
*/

/* sensor i2c address */
//info:  LCD 0x51
#define  WATER_LEVEL_ADDR                        0X02
#define  AIR_CO_MEASUREMENT_ADDR                0x04
#define  LIGHT_LUX_MEASUREMENT_ADDR             0x29
#define  LIGHT_UV_MEASUREMENT_ADDR              0x41
#define  AIR_HUMIDITY_LEVEL_MEASUREMENT_ADDR    0x44
#define  ADC_MEASUREMENT_ADDR                   0x48
#define  AIR_CO2_MEASUREMENT_ADDR               0x4D
#define  AIR_PRESSURE_MEASUREMENT_ADDR          0x60
#define  WATER_DO_MEASUREMENT_ADDR              0x61 // up to 4 identical water sensor ( Temp, DO , Ph and EC)  per node
#define  WATER_ORP_MEASUREMENT_ADDR             0x62
#define  WATER_PH_MEASUREMENT_ADDR              0x63
#define  WATER_EC_MEASUREMENT_ADDR              0x64
#define  WATER_TEMPERATURE_MEASUREMENT_ADDR     0x66
#define  WATER_DO_MEASUREMENT_ADDR2             0x71
#define  WATER_PH_MEASUREMENT_ADDR2             0x73
#define  WATER_EC_MEASUREMENT_ADDR2             0x74
#define  WATER_TEMPERATURE_MEASUREMENT_ADDR2    0x76
#define  WATER_DO_MEASUREMENT_ADDR3             0x81
#define  WATER_PH_MEASUREMENT_ADDR3             0x83
#define  WATER_EC_MEASUREMENT_ADDR3             0x84
#define  WATER_TEMPERATURE_MEASUREMENT_ADDR3    0x86
#define  WATER_DO_MEASUREMENT_ADDR4             0x91
#define  WATER_PH_MEASUREMENT_ADDR4             0x93
#define  WATER_EC_MEASUREMENT_ADDR4             0x94
#define  WATER_TEMPERATURE_MEASUREMENT_ADDR4    0x96

/*#define  AIR_TEMPERATURE_MEASUREMENT_ADDR     0x40
  #define  AIR_QUALITY_INDEX_ADDR                 17
  #define  LIGHT_PAR_MEASUREMENT_ADDR             19
  #define  WATER_FLOW_MEASUREMENT_ADDR            27
  #define  SOIL_MOISTURE_LEVEL_ADDR               32*/


/* sensor network name */
#define  RELAY_NNAME                            "relay"
#define  AIR_PRESSURE_MEASUREMENT_NNAME         "air_pressure"
#define  AIR_CO2_MEASUREMENT_NNAME              "air_co2"
#define  AIR_CO_MEASUREMENT_NNAME               "air_co"
#define  AIR_TEMPERATURE_MEASUREMENT_NNAME      "air_temperature"
#define  AIR_HUMIDITY_LEVEL_MEASUREMENT_NNAME   "air_humidity"
#ifdef LOW_COST_SUNLIGHT_SENSOR
  #define  LIGHT_LUX_MEASUREMENT_NNAME            "light_lux"
#endif
#define  LIGHT_PAR_MEASUREMENT_NNAME            "light_par"
#define  LIGHT_UV_MEASUREMENT_NNAME             "light_uv"
#define  WATER_TEMPERATURE_MEASUREMENT_NNAME    "water_temperature"
#define  WATER_PH_MEASUREMENT_NNAME             "water_ph"
#define  WATER_EC_MEASUREMENT_NNAME             "water_ec"
#define  WATER_ORP_MEASUREMENT_NNAME            "water_orp"
#define  WATER_DO_MEASUREMENT_NNAME             "water_do"
#define  SOIL_MOISTURE_MEASUREMENT_NNAME        "moisture"
#define  SOIL_TEMPERATURE_MEASUREMENT_NNAME     "soil_temperature"
#define  WATER_LEVEL_NNAME                       "water_level"

/*#define  WATER_LEVEL_ID                       26
  #define  WATER_FLOW_MEASUREMENT_ID              27*/

/* sensor displayed name */

#define  RELAY_DNAME          "RELAY "
#define  AIR_PRESSURE_MEASUREMENT_DNAME         "ATM :"
#define  AIR_CO2_MEASUREMENT_DNAME              "CO2 :"
#define  AIR_CO_MEASUREMENT_DNAME                "CO :"
#define  AIR_TEMPERATURE_MEASUREMENT_DNAME      "Air T. :"
#define  AIR_HUMIDITY_LEVEL_MEASUREMENT_DNAME   "Air Hmdt :"
#define  LIGHT_PAR_MEASUREMENT_DNAME            "PAR :"
#define  LIGHT_UV_MEASUREMENT_DNAME             "UV :"
#define  WATER_TEMPERATURE_MEASUREMENT_DNAME    "Water T. :"
#define  WATER_PH_MEASUREMENT_DNAME             "pH :"
#define  WATER_EC_MEASUREMENT_DNAME             "EC :"
#define  WATER_ORP_MEASUREMENT_DNAME            "ORP :"
#define  WATER_DO_MEASUREMENT_DNAME             "DO :"
#define  SOIL_MOISTURE_MEASUREMENT_DNAME        "Moisture :"
#define  SOIL_TEMPERATURE_MEASUREMENT_DNAME     "Soil T. :"
#define  WATER_LEVEL_DNAME                      "Water lvl :"
#ifdef LOW_COST_SUNLIGHT_SENSOR
  #define  LIGHT_LUX_DNAME                  "LUX :"
#endif
/*
  #define  WATER_LEVEL_ID                        26
  #define  WATER_FLOW_MEASUREMENT_ID             27*/

/* sensor displayed unit */
#define  AIR_PRESSURE_MEASUREMENT_UNIT        " kPa"
#define  AIR_CO2_MEASUREMENT_UNIT             " ppm"
#define  AIR_HUMIDITY_LEVEL_MEASUREMENT_UNIT  " %"
#ifdef LOW_COST_SUNLIGHT_SENSOR
  #define  LIGHT_LUX_MEASUREMENT_UNIT           ""
#endif
#define  LIGHT_PAR_MEASUREMENT_UNIT           " ppf" //mol.m-2.s-1"
#define  LIGHT_UV_MEASUREMENT_UNIT            " index"
#define  TEMPERATURE_MEASUREMENT_UNIT   " C"
#define  WATER_PH_MEASUREMENT_UNIT            ""
#define  WATER_EC_MEASUREMENT_UNIT            " uS/cm"
#define  WATER_ORP_MEASUREMENT_UNIT           " mV"
#define  WATER_DO_MEASUREMENT_UNIT            " mg/L"
#define  SOIL_MOISTURE_MEASUREMENT_UNIT       " kPa" //WHAT  <-------
#define  WATER_LEVEL_UNIT                      " cm"

/*
  #define  WATER_LEVEL_ID                     26
  #define  WATER_FLOW_MEASUREMENT_ID          27 */
#endif

