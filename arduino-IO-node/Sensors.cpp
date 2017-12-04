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

#include "Sensors.h"
 
#ifdef SENSOR_TYPE_EZO_SENSOR
EZO myEZO;
#endif

enum _sensor_item {
  _item_rtd = 1,
  _item_ph ,
  _item_ec,
  _item_do,
  _item_orp,
  _item_water_level,
 /* _item_light_PAR,
  _item_light_UV ,
  _item_air_temperature,
  _item_air_pressure,
  _item_air_relative_humidity,
  _item_air_CO,
  _item_air_CO2,*/
  _item_soil_temperature,
  _item_soil_moisture,
  _item_last
};


/**
   \fn void Sensors::init(LCD * p_LCD,  YunBridge * p_bridge)
   \brief initialize Sensors.cpp detect sensors connected, by scaning th i2c bus, then initialize them

   \param pointers to LCD.cpp, YunBridge.cpp
   \return n/a
*/
void Sensors::init(LCD * p_LCD,  YunBridge * p_bridge)
{

  p_myLCD = p_LCD;
  p_myBridge = p_bridge;
  
#ifdef SENSOR_TYPE_EZO_SENSOR
  myEZO = EZO();
#endif
  //initialize values tamp

#ifdef SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR
  sensor_light_lux = 0;

  sensor_light_uv  = 0;
#endif
  delay(2000);
  // detect I2C sensors
  detect();

}

/**
   \fn void  Sensors::detect()
   \brief test which sensors are connected and initialize thel

   \param n/a
   \return n/a
*/
void  Sensors::detect()
{
  on = false;
  
#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
  water_temperature = -1023;
#endif

#ifdef SENSOR_TYPE_PAR_SENSOR
  sensor_light_par = 0;
#endif


#ifdef SENSOR_TYPE_UV_SENSOR
  sensor_light_uv = 0;
#endif

#ifdef SENSOR_TYPE_AIR_TEMPERATURE_SENSOR
  sensor_air_temperature = 0;
#endif

#ifdef SENSOR_TYPE_PRESSURE_SENSOR
  sensor_air_pressure = 0;
#endif

#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR
  sensor_air_relative_humidity = 0;
#endif


#ifdef SENSOR_TYPE_CO_SENSOR
  sensor_air_CO = 0;
#endif

#ifdef SENSOR_TYPE_CO2_SENSOR
  sensor_air_CO2 = 0;
#endif

#ifdef SENSOR_TYPE_SOIL_TEMPERATURE_SENSOR
  sensor_soil_temperature = 0;
#endif

#ifdef SENSOR_TYPE_SOIL_MOISTURE_SENSOR
  sensor_soil_moisture = 0;
#endif

#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
  sensor_water_temperature = 0;
#endif


#ifdef SENSOR_TYPE_WATER_PH_SENSOR
  sensor_water_ph = 0;
#endif

#ifdef SENSOR_TYPE_WATER_EC_SENSOR
  sensor_water_ec = 0;
#endif

#ifdef SENSOR_TYPE_WATER_DO_SENSOR
  sensor_water_do = 0;
#endif

#ifdef SENSOR_TYPE_WATER_ORP_SENSOR
  sensor_water_orp = 0;
#endif  

#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR
  sensor_water_level = 0;
#endif

  sensor_item = _item_rtd;

  uint8_t error, address;
  //uint8_t nDevices = 0;
  float essay;

  p_myLCD->CleanAll(WHITE);
  lcdLine = 0;

#ifdef DEBUG
  Serial.println("Scan I2C");
#endif


  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
#ifdef DEBUG
      Serial.print(F("I2C 0x"));
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.print(" : ");
      Serial.println(address);
#endif

      switch (address) {

#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR
        case WATER_LEVEL_ADDR:
          on = true;
          myWaterlevel = Waterlevel();

          p_myLCD->DispStringAt(WATER_LEVEL_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_water_level = 1;
          break;

#endif
#ifdef SENSOR_TYPE_PRESSURE_SENSOR
        ///BAROMETER + TEMP
        case AIR_PRESSURE_MEASUREMENT_ADDR:
          on = true;
          myBarometer = Barometer();
          myBarometer.begin();

          p_myLCD->DispStringAt(AIR_PRESSURE_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();
          p_myLCD->DispStringAt(AIR_TEMPERATURE_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_air_pressure = 1 ;
          break;
#endif
#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR
        ///HUMIDITY +TEMP
        case AIR_HUMIDITY_LEVEL_MEASUREMENT_ADDR:
          on = true;
          myHum = SHTSensor(SHTSensor::SHT3X);
          //myHum.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM);
          myHum.init();

          p_myLCD->DispStringAt(AIR_HUMIDITY_LEVEL_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();
          p_myLCD->DispStringAt(AIR_TEMPERATURE_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_air_relative_humidity = 1 ;
          break;
#endif
#ifdef SENSOR_TYPE_CO2_SENSOR
        ///CO2
        case AIR_CO2_MEASUREMENT_ADDR:
          on = true;
          myi2cuart = SC16IS750(SC16IS750_PROTOCOL_I2C, SC16IS750_ADDRESS_BB);
          myCO2 = CO2();
          myCO2.init(&myi2cuart);
          myi2cuart.begin(CO2_SENSOR_BAUDRATE);
          myi2cuart.pinMode(0, INPUT);

          p_myLCD->DispStringAt(AIR_CO2_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_air_CO2 = 1 ;
          break;
#endif

#ifdef SENSOR_TYPE_CO_SENSOR
        ///CO
        case AIR_CO_MEASUREMENT_ADDR:

          on = true;
          myCO = MutichannelGasSensor();
          myCO.begin();
          myCO.powerOn();

          p_myLCD->DispStringAt(AIR_CO_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_air_CO = 1 ;
          break;
#endif

        ///ADC
        case ADC_MEASUREMENT_ADDR:
          on = true;
          myAdc = Adafruit_ADS1115();
#ifdef SENSOR_TYPE_SOIL_TEMPERATURE_SENSOR
          essay = myAdc.readADC_SingleEnded(2);
#ifdef DEBUG
          Serial.print("T");
          Serial.println((unsigned long)essay);
#endif
          if (essay != 0 ) {//temp , don't forget to pull down with ground line while not connected
            p_myLCD->DispStringAt(SOIL_TEMPERATURE_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
            printOk();
            soil_temperature = 0;
            sensor_soil_temperature = 1 ;
          }
#endif

#ifdef SENSOR_TYPE_PAR_SENSOR
          essay = myAdc.readADC_SingleEnded(3);
#ifdef DEBUG
          Serial.print("PAR");
          Serial.println(essay);
#endif
          if ((unsigned long)essay != 0) { //PAR, don't forget to pull down with ground line while not connected
            p_myLCD->DispStringAt(LIGHT_PAR_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
            printOk();
            sensor_light_par = 1 ; 
          //Serial.print(sensor_light_par);
          }
#endif
#ifdef SENSOR_TYPE_UV_SENSOR
          essay = myAdc.readADC_SingleEnded(0);
#ifdef DEBUG
          Serial.print("UV");
          Serial.println(essay);
#endif
          if (essay != 0) { //UV, don't forget to pull down with ground line while not connected
            p_myLCD->DispStringAt(LIGHT_UV_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
            printOk();
            sensor_light_uv = 1;
          }
#endif
#ifdef SENSOR_TYPE_MOISTURE_SENSOR
          essay = myAdc.readADC_SingleEnded(1);
          if (essay != 0) { // MOISTURE , don't forget to pull down with ground line while not connected
            sensor_soil_moisture = 1;
            p_myLCD->DispStringAt(SOIL_MOISTURE_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
            printOk();
          }
#endif
          break;

#ifdef SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR
        case LIGHT_LUX_MEASUREMENT_ADDR:
          myLuxSensor = DigitalLight_CalculateLux();
          myLuxSensor.init();
          p_myLCD->print(LIGHT_LUX_MEASUREMENT_DNAME);
          printOk();
          sensor_light_lux = 1;
          break;

        case LIGHT_UV_MEASUREMENT_ADDR:
          mySunLightSensor = SunLight();
          mySunLightSensor.Begin();

          p_myLCD->print(LIGHT_UV_MEASUREMENT_DNAME);
          printOk();
          sensor_light_uv = 1;
          break;
#endif

#ifdef SENSOR_TYPE_WATER_TEMPERATUREL_SENSOR
        case WATER_TEMPERATURE_MEASUREMENT_ADDR:
          on = true;
          myWaterTempRTD  = WaterTempRTD();
          myWaterTempRTD.init();

          p_myLCD->DispStringAt(WATER_TEMPERATURE_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_water_temperature  ++ ;
          break;
#endif

#ifdef SENSOR_TYPE_WATER_PH_SENSOR
        case WATER_PH_MEASUREMENT_ADDR:
          on = true;
          myWaterPH  = WaterPH();
          myWaterPH.init();

          p_myLCD->DispStringAt(WATER_PH_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_water_ph++;
          break;
#endif

#ifdef SENSOR_TYPE_WATER_EC_SENSOR
        case WATER_EC_MEASUREMENT_ADDR:
          on = true;
          myWaterEC  = WaterEC();
          myWaterEC.init();

          p_myLCD->DispStringAt(WATER_EC_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_water_ec++;
          break;
#endif
#ifdef SENSOR_TYPE_WATER_ORP_SENSOR
        case WATER_ORP_MEASUREMENT_ADDR:
          on = true;
          myWaterORP  = WaterORP();
          myWaterORP.init();

          p_myLCD->DispStringAt(WATER_ORP_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          //delay(10);
          printOk();

          sensor_water_orp++;
          break;
#endif
#ifdef SENSOR_TYPE_WATER_DO_SENSOR
        case WATER_DO_MEASUREMENT_ADDR:
          on = true;
          myWaterDO  = WaterDO();
          myWaterDO.init();

          p_myLCD->DispStringAt(WATER_DO_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_water_do++;
          break;
#endif
        default:
          break;
      }

      //nDevices++;
    }
#ifdef DEBUG
    else if (error == 4)
    {
      Serial.print("Uknw err at addr 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
#endif
  }
  //on=true;
  /*
    if (nDevices == 0) {
    Serial.println("No I2C devices");
    }*/
}

void Sensors::printOk() {
  p_myLCD->print(" OK");
  lcdLine++;
}

/**
   \fn void Sensors::printinfo ( char* sensorName, double sensorValue, uint8_t sensorPrecision, char * sensorUnit)
   \brief print info of a sensor on serial monitor and LCD

   \param sensorName, sensorValue, sensorPrecision, sensorUnit
   \return n/a

*/
void Sensors::printinfo (char* sensorName, float sensorValue, uint8_t sensorPrecision, char * sensorUnit)
{

//#ifdef DEBUG
  Serial.print(sensorName);
  Serial.print(sensorValue, sensorPrecision);
  Serial.println(sensorUnit);
//#endif
  if (isnan(sensorValue) != 1){
   if (lcdLine < 50 ) {
     if ((lcdLine - scrolling) >= 0) {
        p_myLCD->DispStringAt(sensorName, 0, (lcdLine - scrolling) * ROW_OFFSET);
        p_myLCD->print(sensorValue, sensorPrecision);
        p_myLCD->print(sensorUnit);
        p_myLCD->print("    ");
     }
     lcdLine++;
    }
  }
}




/**
   \fn void Sensors::update(uint8_t menu)
   \brief Read the sensors to update the values and sends them to linux

   \param menu uint8_t tells us in which page of the menu we are
   \return n/a

*/
void Sensors::update(uint8_t menu)
{
  unsigned short ADCvalue;
  bool bool_water;
  uint8_t check = 0;
  if (menu == 0 || menu == 6) {
    lcdLine = 0;
  } else {
    lcdLine = 50;
  }

  
#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR
  if (sensor_water_level && sensor_item == _item_water_level) { //DEPTH with ultrasound
    //Serial.print("water level");
    water_level = myWaterlevel.getDistance();

#ifdef DEBUG  
    Serial.print("Water lvl:");
    Serial.println(water_level);
#endif

    //Print the info
    printinfo ( WATER_LEVEL_DNAME, water_level, 0, WATER_LEVEL_UNIT);

    //Send the info to linux
    p_myBridge->sendFloat( WATER_LEVEL_NNAME, water_level);
    //Serial.println("us");
    check++;
  }
#endif

#if defined( SENSOR_TYPE_PRESSURE_SENSOR ) && defined (SENSOR_TYPE_AIR_TEMPERATURE_SENSOR)
  if ( sensor_air_pressure /*&& sensor_item == _item_air_pressure*/ ) {//BAROMETER OK - TEMP OK

    myBarometer.getPT(&air_pressure, &air_temperature);
#ifdef DEBUG  
    Serial.print("P:");
    Serial.println(air_pressure);
    Serial.print("Air Temp:");
    Serial.println(air_temperature);
#endif
    //Print the info
    printinfo ( AIR_TEMPERATURE_MEASUREMENT_DNAME, air_temperature, 1, TEMPERATURE_MEASUREMENT_UNIT);
    printinfo ( AIR_PRESSURE_MEASUREMENT_DNAME, air_pressure, 0, AIR_PRESSURE_MEASUREMENT_UNIT);

    //Send the info to linux
    p_myBridge->sendFloat( AIR_TEMPERATURE_MEASUREMENT_NNAME, air_temperature);
    p_myBridge->sendInteger( AIR_PRESSURE_MEASUREMENT_NNAME,  (unsigned long)air_pressure);

    check++;

  }
#endif


#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR
  //p_myLCD->CleanAll(WHITE);
  if ( sensor_air_relative_humidity /*&& sensor_item == _item_air_relative_humidity */) { //HUMIDITY OK - TEMP INPRECISE
    if (myHum.readSample())
    {
      check++;
      //Print humidity
      printinfo ( AIR_HUMIDITY_LEVEL_MEASUREMENT_DNAME, myHum.mHumidity, 1, AIR_HUMIDITY_LEVEL_MEASUREMENT_UNIT);
      p_myBridge->sendFloat( AIR_HUMIDITY_LEVEL_MEASUREMENT_NNAME, myHum.mHumidity);
#ifdef DEBUG  
    Serial.print("RH:");
    Serial.println(myHum.mHumidity);
#endif
#if not defined(SENSOR_TYPE_PRESSURE_SENSOR ) && defined (SENSOR_TYPE_AIR_TEMPERATURE_SENSOR) && defined(SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR)
      //use the temp of this sensor if there is no other
      if (!(sensor_air_pressure))
      {
#ifdef DEBUG  
    Serial.print("Air Temp:");
    Serial.println(myHum.mTemperature);
#endif
        printinfo ( AIR_TEMPERATURE_MEASUREMENT_DNAME, myHum.mTemperature, 1, TEMPERATURE_MEASUREMENT_UNIT);
        p_myBridge->sendFloat( AIR_TEMPERATURE_MEASUREMENT_NNAME, myHum.mTemperature);
      }
#endif
    }
  }
#endif


#ifdef SENSOR_TYPE_CO_SENSOR
  if ( sensor_air_CO /*&& sensor_item == _item_air_CO */) { //CO OK
    check++;
    air_CO = myCO.measure_CO();
#ifdef DEBUG  
    Serial.print("CO:");
    Serial.println(air_CO);
#endif
    //FIX ME : ajouter une deuxieme valeur de verification
    /*if(air_CO==-1){
      sensor_air_CO=0;
      sensor_air_relative_humidity=0;
      sensor_air_pressure=0;
      sensor_soil_temperature=0;
      sensor_light_par=0;
      sensor_light_uv=0;
      sensor_soil_moisture=0;
      sensor_air_CO2=0;
      }*/

    printinfo (AIR_CO_MEASUREMENT_DNAME, air_CO, 1, AIR_CO2_MEASUREMENT_UNIT);
    p_myBridge->sendFloat( AIR_CO_MEASUREMENT_NNAME, air_CO);

  }
#endif

#ifdef SENSOR_TYPE_SOIL_TEMPERATURE_SENSOR
  if ( sensor_soil_temperature /*&& sensor_item == _item_soil_temperature*/) {//ADC - TEMP OK
    ADCvalue = myAdc.readADC_SingleEnded(2) ;
    #if 0
    long tmpval = (long)ADCvalue;
    tmpval = tmpval * 100 / 16 ;
    Serial.println(tmpval);
    tmpval = (tmpval * 4755) / (250000 - tmpval);
    tmpval = (-1 * ((tmpval) / 43103448));
    tmpval = tmpval * 100000 + 1759246;
    tmpval = sqrt(tmpval);
    tmpval = (tmpval - 390800) * -862;
    tmpval = tmpval / 10000;
    soil_temperature = ((float)tmpval)/10.0;
    #else
    soil_temperature = (float)ADCvalue * 0.0000625;
    soil_temperature = soil_temperature * 4755.0 / (2.5 - soil_temperature);
    soil_temperature = -0.00232 * (soil_temperature) + 17.59246;
    soil_temperature = sqrt(soil_temperature);
    soil_temperature = -(soil_temperature - 3.908) / 0.00116;
    #endif
#ifdef DEBUG  
    Serial.print("Soil Temp:");
    Serial.print(ADCvalue);
    Serial.print("/");
    Serial.println(soil_temperature);
#endif
   // Serial.println(soil_temperature);
    check++;
    printinfo (SOIL_TEMPERATURE_MEASUREMENT_DNAME, soil_temperature, 1, TEMPERATURE_MEASUREMENT_UNIT);
    p_myBridge->sendFloat( SOIL_TEMPERATURE_MEASUREMENT_NNAME, soil_temperature);
  }
#endif

#ifdef SENSOR_TYPE_PAR_SENSOR
  if (sensor_light_par /*&& sensor_item == _item_light_PAR*/) {//ADC - JYP OK
    ADCvalue = myAdc.readADC_SingleEnded(3);
    light_par = (float)ADCvalue * 0.77890625 ; /*0.0078125 * 99.7 = 0.77890625 */
    if ((unsigned long)light_par > 51000) light_par = 0.0f;
#ifdef DEBUG  
    Serial.print("PAR:");
    Serial.print(ADCvalue);
    Serial.print("/");
    Serial.println(light_par);
#endif
    check++;
    printinfo (LIGHT_PAR_MEASUREMENT_DNAME, light_par, 1, LIGHT_PAR_MEASUREMENT_UNIT);
    p_myBridge->sendFloat( LIGHT_PAR_MEASUREMENT_NNAME, light_par);
  }
#endif

#ifdef SENSOR_TYPE_UV_SENSOR
  if (sensor_light_uv /* && sensor_item == _item_light_UV*/) {//ADC - UV
    ADCvalue = myAdc.readADC_SingleEnded(0);
    light_uv = (float) ADCvalue* 0.0390625 ; /* 0.0078125 * 5.0 = 0,0390625 */
#ifdef DEBUG  
    Serial.print("UV:");
    Serial.print(ADCvalue);
    Serial.print("/");
    Serial.println(light_uv);
#endif
    if ((int)light_uv > 1224 || (int)light_uv < 6 ) {
      light_uv = 0;
    }
    check++;
    //      if ((int)val_tamp < 100) p_myLCD->BacklightConf(LOAD_TO_RAM, 20);             TO DEAL WITH
    //      else p_myLCD->BacklightConf(LOAD_TO_RAM, 100);
    printinfo (LIGHT_UV_MEASUREMENT_DNAME, light_uv, 1, LIGHT_PAR_MEASUREMENT_UNIT /* this sensor sens PPF value , not a UV index*/);
    p_myBridge->sendFloat( LIGHT_UV_MEASUREMENT_NNAME, light_uv);
    

  }
#endif
  
#ifdef SENSOR_TYPE_SOIL_MOISTURE_SENSOR
  if (sensor_soil_moisture /*&& sensor_item == _item_soil_moisture*/) {//ADC - WMS
    ADCvalue = myAdc.readADC_SingleEnded(1);
    soil_moisture = (float)(ADCvalue * 0.029875 ) / 2.8; /* 0.000125 * 239.0 = 0,029875*/
#ifdef DEBUG  
    Serial.print("MOIST:");
    Serial.print(ADCvalue);
    Serial.print("/");
    Serial.println(soil_moisture);
#endif
    printinfo (SOIL_MOISTURE_MEASUREMENT_DNAME, soil_moisture, 1, SOIL_MOISTURE_MEASUREMENT_UNIT);
    p_myBridge->sendFloat( SOIL_MOISTURE_MEASUREMENT_NNAME, soil_moisture);
  }
#endif
#ifdef SENSOR_TYPE_LOW_COST_SUNLIGHT_SENSOR
  if (sensor_light_uv /*&& sensor_item == _item_light_UV*/) {
    light_lux = mySunLightSensor.ReadVisible();
    printinfo ( LIGHT_LUX_MEASUREMENT_DNAME, light_lux, 0, LIGHT_LUX_MEASUREMENT_UNIT);
    p_myBridge->sendInteger( LIGHT_LUX_MEASUREMENT_NNAME, (unsigned long)light_lux);

    light_uv = mySunLightSensor.ReadUV();
    printinfo ( LIGHT_UV_MEASUREMENT_DNAME, light_uv, 0, LIGHT_UV_MEASUREMENT_UNIT);
    p_myBridge->sendInteger( LIGHT_UV_MEASUREMENT_NNAME, (unsigned long)light_uv);
  }
#endif
#ifdef SENSOR_TYPE_WATER_ORP_SENSOR
  if (sensor_water_orp) {//WATER ORP - OK
    if (menu != 6 && sensor_item == _item_orp) {
      bool_water = myEZO.readData(WATER_ORP_MEASUREMENT_ADDR);
      if (bool_water == false) sensor_water_orp = 0;
      else {
        water_orp = atof(myEZO.getData());
        p_myBridge->sendFloat( WATER_ORP_MEASUREMENT_NNAME, water_orp);
        //sensor_water=_item_rtd;
      }
    }
    check++;
    printinfo ( WATER_ORP_MEASUREMENT_DNAME, water_orp, 1, WATER_ORP_MEASUREMENT_UNIT);
  }
#endif
#ifdef SENSOR_TYPE_WATER_DO_SENSOR
  if (sensor_water_do) { //WATER DO - OK
    if (menu != 6 && sensor_item == _item_do) {
      bool_water = myEZO.readData(WATER_DO_MEASUREMENT_ADDR);
      if (bool_water == false) sensor_water_do = 0;
      else {
        water_do = atof(myEZO.getData());
#ifdef DEBUG  
    Serial.print("Water DO:");
    Serial.println(water_do);
#endif
        p_myBridge->sendFloat( WATER_DO_MEASUREMENT_NNAME, water_do);
        //sensor_water++;
        //sensor_water++;
      }
    }
    check++;
    printinfo ( WATER_DO_MEASUREMENT_DNAME, water_do, 1, WATER_DO_MEASUREMENT_UNIT);
  }
#endif  
#ifdef SENSOR_TYPE_WATER_EC_SENSOR
  if (sensor_water_ec) { // WATER EC - OK
    if (menu != 6 && sensor_item == _item_ec) {
      bool_water = myEZO.readData(WATER_EC_MEASUREMENT_ADDR);
      if (bool_water == false) sensor_water_ec = 0;
      else {
        water_ec = atof(myEZO.getData());
#ifdef DEBUG  
    Serial.print("Water EC:");
    Serial.println(water_ec);
#endif
        p_myBridge->sendFloat( WATER_EC_MEASUREMENT_NNAME, water_ec);
        //sensor_water++;
      }
    }
    check++;
    printinfo ( WATER_EC_MEASUREMENT_DNAME, water_ec, 1, WATER_EC_MEASUREMENT_UNIT);

  }
#endif
#ifdef SENSOR_TYPE_WATER_PH_SENSOR
  if (sensor_water_ph) {//WATER PH - OK
    if (menu != 6 && sensor_item == _item_ph) {
      bool_water = myEZO.readData(WATER_PH_MEASUREMENT_ADDR);
      if (bool_water == false) sensor_water_ph = 0;
      else {
        water_ph = atof(myEZO.getData());
#ifdef DEBUG  
    Serial.print("Water PH:");
    Serial.println(water_ph);
#endif
        p_myBridge->sendFloat( WATER_PH_MEASUREMENT_NNAME, water_ph);
        //sensor_water++;
      }
    }
    // if ((int)water_ph > 0 && (int)water_ph < 14) {
    check++;
    printinfo ( WATER_PH_MEASUREMENT_DNAME, water_ph, 1, WATER_PH_MEASUREMENT_UNIT);

  }
  //}
#endif
#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR 
  if (sensor_water_temperature) { // WATER TEMP -OK
    if (menu != 6 && sensor_item == _item_rtd) {
      bool_water = myEZO.readData(WATER_TEMPERATURE_MEASUREMENT_ADDR);
      if (bool_water == false) sensor_water_temperature = 0;
      else {
        water_temperature = atof(myEZO.getData());
#ifdef DEBUG  
    Serial.print("Water Temp:");
    Serial.println(water_temperature);
#endif
        p_myBridge->sendFloat(WATER_TEMPERATURE_MEASUREMENT_NNAME, water_temperature);
        //sensor_water++;
      }
    }
    check++;
    printinfo ( WATER_TEMPERATURE_MEASUREMENT_DNAME, water_temperature, 1, TEMPERATURE_MEASUREMENT_UNIT);
  }
#endif

#ifdef SENSOR_TYPE_CO2_SENSOR
  if ( sensor_air_CO2 /*&& sensor_item == _item_air_CO2*/) { //CO2
    myCO2.measure();
    check++;
#ifdef DEBUG  
    Serial.print("CO2:");
    Serial.println(myCO2.ppm);
#endif
    printinfo ( AIR_CO2_MEASUREMENT_DNAME, myCO2.ppm, 0, AIR_CO2_MEASUREMENT_UNIT);
    p_myBridge->sendInteger(AIR_CO2_MEASUREMENT_NNAME, myCO2.ppm);
  }
#endif

  if (!check && !menu)
    p_myLCD->DispStringAt("No sensor", 6 * CHAR_OFFSET, 1 * ROW_OFFSET);

#ifdef DEBUG
  Serial.println("");
  Serial.println("");
#endif

  
  if (sensor_item < _item_last)
    sensor_item++;
  else sensor_item = _item_rtd;
}

/**
   \fn void Sensors::compensationEC()
   \brief Sends the compensation order for the EC compensation

   \param n/a
   \return n/a

*/
void Sensors::compensationEC()
{
#ifdef SENSOR_TYPE_WATER_DO_SENSOR
  if (sensor_water_do && (sensor_item + 1) == _item_do) //DO
  {
#ifdef SENSOR_TYPE_WATER_EC_SENSOR
    if ((int)water_ec > 0) myWaterDO.setEcCompensation(water_ec);
#endif
  }
#endif
}

/**
   \fn void Sensors::compensationTemp()
   \brief Sends the compensation order for the Temp compensation

   \param n/a
   \return n/a

*/
void Sensors::compensationTemp()
{
  
#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
  if ((int)water_temperature > -1023) {
    
#ifdef SENSOR_TYPE_WATER_EC_SENSOR
    if (sensor_water_ec && (sensor_item + 1) == _item_ec) //EC
      myWaterEC.setTemperatureCompensation(water_temperature);
#ifdef SENSOR_TYPE_WATER_DO_SENSOR
    if (sensor_water_do && (sensor_item + 1) == _item_do) //DO
      myWaterDO.setTemperatureCompensation(water_temperature);
#endif
#endif
  }
  #endif
}


/**
   \fn void Sensors::waterCall()
   \brief Sends the I2C request to update the water box

   \param n/a
   \return n/a

*/
void Sensors::waterCall() {
  //if(sensors_water_monitoring>0){
  if (sensor_item < _item_orp)
    sensor_item++;
  else sensor_item = _item_rtd;
  if (sensor_item == _item_rtd)
  {
    
#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
    if (sensor_water_temperature)//WATER TEMP
    {
      myWaterTempRTD.getTemperature();
    }
#endif
    //sensor_item++;
  }
  else if (sensor_item == _item_ph)
  {
    
#ifdef SENSOR_TYPE_WATER_PH_SENSOR
    if (sensor_water_ph )//WATER PH
    {
      myWaterPH.getPh();
    }
#endif
    //sensor_item++;
  }
  else if (sensor_item == _item_ec)
  {
    
#ifdef SENSOR_TYPE_WATER_EC_SENSOR
    if (sensor_water_ec )//WATER EC
    {
      myWaterEC.getEc();
    }
#endif
    //sensor_item++;
  }
  else if (sensor_item == _item_do)
  {
    
#ifdef SENSOR_TYPE_WATER_DO_SENSOR
    if (sensor_water_do)//WATER DO
    {
      myWaterDO.getDissolvedOxygen();
    }
#endif
    //sensor_item++;
  }
  else if (sensor_item == _item_orp) {
    
#ifdef SENSOR_TYPE_WATER_ORP_SENSOR
    if (sensor_water_orp) {//WATER ORP

      myWaterORP.getORP();
      //sensor_item = _item_rtd;
    }
#endif
  }
}


/**
   \fn void Sensors::calibrate(uint8_t sensorID, int val)
   \brief Calibrate specific sensors

   \param sensorID: sensor to calibrate, val: num of point for calibration
   \return n/a
*/
#define TXT_PUT_THE_PROBE "Put probe"
#define TXT_WAIT_2MIN "Wait 2 min"
#define TXT_WAIT_1_30_MIN "Wait 1min30"
#define TXT_WAIT_1MIN "Wait 1 min"
#define TXT_WAIT_30S "Wait 30 sec"
#define TXT_PUT_IN_SOLUTION "in solution "
#define TXT_EXPOSED_TO_AIR "exposed to air"

void Sensors::calibrate(uint8_t sensorID, int val)
{
  uint8_t check = 0;
  reset();
  
#ifdef SENSOR_TYPE_WATER_PH_SENSOR
  if ((sensorID == 1) && sensor_water_ph && (water_ph < 14) && (water_ph > 0)) //PH
  {
    check++;
    p_myLCD->println(TXT_PUT_THE_PROBE);
    p_myLCD->print((TXT_PUT_IN_SOLUTION));
    p_myLCD->println(("ph 7"));
    p_myLCD->println(TXT_WAIT_2MIN);
    wait();
    myWaterPH.calibrate_mid();
    if (val > 1) {//Double and triple point calibration
      reset();
      p_myLCD->println(TXT_PUT_THE_PROBE);
      p_myLCD->print((TXT_PUT_IN_SOLUTION));
      p_myLCD->println(("ph 4"));
      p_myLCD->println(TXT_WAIT_2MIN);
      wait();
      myWaterPH.calibrate_low();
    }
    if (val > 2) {//Triple point calibration
      reset();
      p_myLCD->println(TXT_PUT_THE_PROBE);
      p_myLCD->print((TXT_PUT_IN_SOLUTION));
      p_myLCD->println(("ph 10"));
      p_myLCD->println(TXT_WAIT_2MIN);
      wait();
      myWaterPH.calibrate_high();
    }
  }

  else
  #endif
#ifdef SENSOR_TYPE_WATER_DO_SENSOR
  if ((sensorID == 2) && sensor_water_do && (water_do > 0)) //DO
  {
    check++;
    p_myLCD->println((TXT_PUT_THE_PROBE));
    p_myLCD->println(TXT_EXPOSED_TO_AIR);
    p_myLCD->println(TXT_WAIT_30S);
    wait();
    myWaterDO.calibrate();

    if (val == 2) {///Double point calibration

      reset();
      p_myLCD->println(TXT_PUT_THE_PROBE);
      p_myLCD->println(TXT_PUT_IN_SOLUTION);
      p_myLCD->println(TXT_WAIT_1_30_MIN);
      wait();
      myWaterDO.calibrate_zero();
    }
  }
  else 
#endif
#ifdef SENSOR_TYPE_WATER_EC_SENSOR
  if ((sensorID == 3) && sensor_water_ec && (water_ec > 0))    //EC
  {
    check++;
    p_myLCD->println(TXT_PUT_THE_PROBE);
    p_myLCD->println(TXT_EXPOSED_TO_AIR);
    p_myLCD->println(TXT_WAIT_30S);
    wait();
    myWaterEC.calibrate_dry();              //dry calibrate. Only needed once so do we need to put it in the code?
    reset();
    if (val == 1) {///Single point calibration

      p_myLCD->println(TXT_PUT_THE_PROBE);
      p_myLCD->print(TXT_PUT_IN_SOLUTION);
      p_myLCD->println("low");
      p_myLCD->println(TXT_WAIT_2MIN);
      wait();
      myWaterEC.calibrate_high(1413);
    }
    else { //double point calibration

      p_myLCD->println(TXT_PUT_THE_PROBE);
      p_myLCD->print(TXT_PUT_IN_SOLUTION);
      p_myLCD->println("low");
      p_myLCD->println(TXT_WAIT_2MIN);
      wait();
      myWaterEC.calibrate_low();

      reset();
      p_myLCD->println(TXT_PUT_THE_PROBE);
      p_myLCD->print(TXT_PUT_IN_SOLUTION);
      p_myLCD->println("high");
      p_myLCD->println(TXT_WAIT_2MIN);
      wait();
      myWaterEC.calibrate_high(12880);
    }
  }
  else
#endif
#ifdef SENSOR_TYPE_WATER_TEMPERATURE_SENSOR
  if ((sensorID == 4) && sensor_water_temperature && (water_temperature > -900)) //RTC
  {
    check++;
    p_myLCD->println(TXT_PUT_THE_PROBE);
    p_myLCD->println("in boiling water");
    wait();
    myWaterTempRTD.calibrate();
  }
  else
#endif
#ifdef SENSOR_TYPE_WATER_ORP_SENSOR
  if ((sensorID == 5) && sensor_water_orp  && (water_orp < 1000)) //ORP
  {
    check++;
    p_myLCD->println(TXT_PUT_THE_PROBE);
    p_myLCD->println(TXT_PUT_IN_SOLUTION);
    p_myLCD->println(TXT_WAIT_1MIN);
    wait();
    myWaterORP.calibrate();
  }
  else
#endif
#ifdef SENSOR_TYPE_CO2_SENSOR
  if (sensorID == 6 &&  sensor_air_CO2) //CO2
  {
    check++;
    myi2cuart.pinMode(1, OUTPUT);    //set up for the calibration pin.
    myi2cuart.digitalWrite(1, LOW);  //start calibration of MH-Z16 under 400ppm
    delay(10000);                  //5+ sec needed for the calibration process
    myi2cuart.digitalWrite(1, HIGH);
  }
  else
#endif
#ifdef SENSOR_TYPE_CO_SENSOR
  if (sensorID == 7 && sensor_air_CO  ) //CO
  {
    check++;
    myCO.doCalibrate();
  }
  else
#endif
#ifdef SENSOR_TYPE_WATER_LEVEL_SENSOR
  if (sensorID == 8 && sensor_water_level == 1) //Ultrasound
  {
    check++;
    myWaterlevel.Calibration();
  }
#endif
  {
  }

  reset();

  //Check if calibration worked
  if (check) {
    p_myLCD->println("Done");
#ifdef DEBUG
    Serial.print("Calibration finished");
#endif
  }
  else {
    p_myLCD->println("Error");
#ifdef DEBUG
    Serial.print("Problem with calibration");
#endif
  }
  delay(500);
  p_myLCD->CleanAll(WHITE);
}

void Sensors::wait()
{
  p_myLCD->println("Any button to go");
  while (!analogRead(A5))
    ;
  delay(300);
}

/**
   \fn void Sensors::reset()
   \brief resets the LCD screen

   \param n/a
   \return n/a
*/
void Sensors::reset()
{
  p_myLCD->CleanAll(WHITE);
  p_myLCD->CharGotoXY(0, 2 * ROW_OFFSET);
}
