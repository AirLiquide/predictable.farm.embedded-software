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
#ifndef __SENSORS_H__
#define __SENSORS_H__

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <EEPROM.h>
#include <Wire.h>

#include "SensorTypes.h"
#include "Barometer.h"
#include "CO2.h"
#include "WaterTempRTD.h"
#include "WaterPH.h"
#include "WaterEC.h"
#include "WaterDO.h"
#include "WaterORP.h"
#include "SC16IS750.h"
#include "SHTSensor.h"
#include "MutichannelGasSensor.h"
#include "Adafruit_ADS1015.h"
#include "Relay.h"
#include "LCD.h"
#include "YunBridge.h"
#include "Waterlevel.h"

#ifdef LOW_COST_SUNLIGHT_SENSOR
    #include "SunLight.h"
#endif

//#define DEBUG

class Sensors
{
  public:
    void update(uint8_t menu);
    void calibrate(uint8_t sensorID, int val );
    void init(LCD * p_LCD,  YunBridge * p_bridge);
    void waterCall();
    void compensationTemp();
    void compensationEC();
    YunBridge * p_myBridge;
    //scrolling
    uint8_t lcdLine;
    uint8_t scrolling;
    void detect();
    bool on;
    void wait();

  private:

    void reset();
    void printinfo (char* sensorName, double sensorValue, uint8_t sensorPrecision, char * sensorUnit);
    LCD * p_myLCD;

    void printOk();

    //uint8_t actuator_relay;
    uint8_t sensor_water;

    uint8_t sensor_light_par;
    uint8_t sensor_light_uv;
    uint8_t sensor_air_temperature;
    uint8_t sensor_air_pressure;
    uint8_t sensor_air_relative_humidity;
    uint8_t sensor_air_CO;
    uint8_t sensor_air_CO2;
    uint8_t sensor_soil_temperature;
    uint8_t sensor_soil_moisture;
    uint8_t sensor_water_temperature;
    uint8_t sensor_water_ph;
    uint8_t sensor_water_ec;
    uint8_t sensor_water_do;
    uint8_t sensor_water_orp;
    uint8_t sensor_water_level;

    float light_par;
    float light_uv;
    float soil_temperature;
    float soil_moisture;
    float water_temperature;
    float air_temperature;
    float air_pressure;
    float air_CO2;
    float air_CO;
    float water_ec;
    float water_ph;
    float water_do;
    float water_orp;
    float water_level;

    //values humidity sensor
    SHTSensor myHum;

    //Values for the ADC with temp, uv, wms, jyp1000
    Adafruit_ADS1115 myAdc;

    Barometer myBarometer;

#ifdef LOW_COST_SUNLIGHT_SENSOR
    uint8_t sensor_light_lux;
    float light_lux;
    uint8_t sensor_light_uv;
    float light_uv;
    SunLight mySunLightSensor;
#endif

    Waterlevel myWaterlevel;

    WaterTempRTD myWaterTempRTD;

    WaterPH myWaterPH;

    WaterEC myWaterEC;

    WaterORP myWaterORP;

    WaterDO myWaterDO;

    CO2 myCO2;
    SC16IS750 myi2cuart;

    MutichannelGasSensor myCO;

};

#endif
