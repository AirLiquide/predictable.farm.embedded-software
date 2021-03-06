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
#ifndef __SENSORS_H__
#define __SENSORS_H__

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "config.h"

#include <EEPROM.h>
#ifdef I2CLIB
#include "I2C.h"
#else
#include <Wire.h>
#endif

#include "SensorTypes.h"
#ifdef SENSOR_TYPE_PRESSURE_SENSOR
#include "Barometer.h"
#endif
#ifdef SENSOR_TYPE_CO2_SENSOR
#include "CO2.h"
#endif
#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
#include "WaterTempRTD.h"
#endif
#ifdef SENSOR_TYPE_WATER_PH_SENSOR
#include "WaterPH.h"
#endif
#ifdef SENSOR_TYPE_WATER_EC_SENSOR
#include "WaterEC.h"
#endif
#ifdef SENSOR_TYPE_WATER_DO_SENSOR
#include "WaterDO.h"
#endif
#ifdef SENSOR_TYPE_WATER_ORP_SENSOR
#include "WaterORP.h"
#endif
#include "SC16IS750.h"

#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR
#include "SHTSensor.h"
#endif
#ifdef SENSOR_TYPE_CO_SENSOR
#include "MutichannelGasSensor.h"
#endif
#include "Adafruit_ADS1015.h"
#include "Relay.h"
#include "LCD.h"
#include "YunBridge.h"
#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR
#include "Waterlevel.h"
#endif
#ifdef SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR
    #include "SunLight.h"
#endif

//#define DEBUG

class Sensors
{
  public:
    void update(bool print, bool send,  uint8_t menu);
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



#ifdef SENSOR_TYPE_PAR_SENSOR
    uint8_t sensor_light_par;
    float light_par;
#endif   

#ifdef SENSOR_TYPE_UV_SENSOR
    uint8_t sensor_light_uv;
    float light_uv;
#endif    

#ifdef SENSOR_TYPE_AIR_TEMPERATURE_SENSOR
    uint8_t sensor_air_temperature;
    float air_temperature;
#endif    

#ifdef SENSOR_TYPE_PRESSURE_SENSOR
    uint8_t sensor_air_pressure;
    float air_pressure;
#endif    

#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR
    uint8_t sensor_air_relative_humidity;
    float air_humidity;
#endif

#ifdef SENSOR_TYPE_CO_SENSOR
    uint8_t sensor_air_CO;
    float air_CO;
#endif

#ifdef SENSOR_TYPE_CO2_SENSOR
    uint8_t sensor_air_CO2;
    float air_CO2;
#endif    
#ifdef SENSOR_TYPE_SOIL_TEMPERATURE_SENSOR
    uint8_t sensor_soil_temperature;
    float soil_temperature;
#endif    

#ifdef SENSOR_TYPE_SOIL_MOISTURE_SENSOR
    uint8_t sensor_soil_moisture;
    float soil_moisture;
#endif   

#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
    uint8_t sensor_water_temperature;
    float water_temperature;
#endif
#ifdef SENSOR_TYPE_WATER_PH_SENSOR
    uint8_t sensor_water_ph;
    float water_ph;
#endif   

#ifdef SENSOR_TYPE_WATER_EC_SENSOR
    uint8_t sensor_water_ec;
    float water_ec;
#endif  

#ifdef SENSOR_TYPE_WATER_DO_SENSOR
    uint8_t sensor_water_do;
    float water_do;
#endif
#ifdef SENSOR_TYPE_WATER_ORP_SENSOR
    uint8_t sensor_water_orp;
    float water_orp;
#endif
#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR
    uint8_t sensor_water_level;
    float water_level;
#endif

#ifdef SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR
    uint8_t sensor_light_lux;
    float light_lux;
    uint8_t sensor_light_uv;
    float light_uv;
#endif


  private:

    void reset();
    void printinfo (char* sensorName, float sensorValue, uint8_t sensorPrecision, char * sensorUnit);
    LCD * p_myLCD;

    void printOk();

    //uint8_t actuator_relay;
    uint8_t sensor_item;


#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR
    Waterlevel myWaterlevel;
#endif
#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
    WaterTempRTD myWaterTempRTD;
#endif
#ifdef SENSOR_TYPE_WATER_PH_SENSOR
    WaterPH myWaterPH;
#endif
#ifdef SENSOR_TYPE_WATER_EC_SENSOR
    WaterEC myWaterEC;
#endif
#ifdef SENSOR_TYPE_WATER_ORP_SENSOR
    WaterORP myWaterORP;
#endif
#ifdef SENSOR_TYPE_WATER_DO_SENSOR
    WaterDO myWaterDO;
#endif
#ifdef SENSOR_TYPE_CO2_SENSOR
    CO2 myCO2;
    SC16IS750 myi2cuart;
#endif

#ifdef SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR
    SunLight mySunLightSensor;
#endif

#ifdef SENSOR_TYPE_CO_SENSOR
    MutichannelGasSensor myCO;
#endif

#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR
    //values humidity sensor
    SHTSensor myHum;
#endif
    //Values for the ADC with temp, uv, wms, jyp1000
    Adafruit_ADS1115 myAdc;

#ifdef SENSOR_TYPE_PRESSURE_SENSOR
    Barometer myBarometer;
#endif

};

#endif /*__SENSORS_H__*/
