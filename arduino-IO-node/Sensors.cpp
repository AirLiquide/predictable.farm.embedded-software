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

EZO myEZO;

enum _water_item {
  _item_rtd = 1,
  _item_ph ,
  _item_ec,
  _item_do,
  _item_orp
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
  myEZO = EZO();

  //initialize values tamp

#ifdef LOW_COST_SUNLIGHT_SENSOR
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
  water_temperature = -1023;
  sensor_light_par = 0;
  sensor_light_uv = 0;
  sensor_air_temperature = 0;
  sensor_air_pressure = 0;
  sensor_air_relative_humidity = 0;
  sensor_air_CO = 0;
  sensor_air_CO2 = 0;
  sensor_soil_temperature = 0;
  sensor_soil_moisture = 0;
  sensor_water_temperature = 0;
  sensor_water_ph = 0;
  sensor_water_ec = 0;
  sensor_water_do = 0;
  sensor_water_orp = 0;
  sensor_water_level = 0;

  sensor_water = _item_rtd;

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
        case WATER_LEVEL_ADDR:
#ifdef WATER_LEVEL_SENSOR
          on = true;
          myWaterlevel = Waterlevel();

          p_myLCD->DispStringAt(WATER_LEVEL_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_water_level = 1;
#endif
          break;

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

        ///CO
        case AIR_CO_MEASUREMENT_ADDR:

#ifdef CO_SENSOR
          on = true;
          myCO = MutichannelGasSensor();
          myCO.begin();
          myCO.powerOn();

          p_myLCD->DispStringAt(AIR_CO_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_air_CO = 1 ;
#endif
          break;


        ///ADC
        case ADC_MEASUREMENT_ADDR:
          on = true;
          myAdc = Adafruit_ADS1115();
#ifdef SOIL_TEMPERATURE_SENSOR
          essay = myAdc.readADC_SingleEnded(2);
#ifdef DEBUG
          Serial.print("T");
          Serial.println((unsigned long)essay);
#endif
          if (((unsigned long)essay != 65535) && ((unsigned long)essay > 20)) {//temp , don't forget to pull down with ground line while not connected
            p_myLCD->DispStringAt(SOIL_TEMPERATURE_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
            printOk();
            soil_temperature = 0;
            sensor_soil_temperature = 1 ;
          }
#endif

#ifdef PAR_SENSOR

          essay = myAdc.readADC_SingleEnded(3);
#ifdef DEBUG

          Serial.print("PAR");
          Serial.println((unsigned long)essay);
#endif
          if ( ( (unsigned long)essay < 65517) && ((unsigned long)essay != 0) ) { //PAR, don't forget to pull down with ground line while not connected
            p_myLCD->DispStringAt(LIGHT_PAR_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
            printOk();

            sensor_light_par = 1 ;
          }
#endif
#ifdef UV_SENSOR
          if (myAdc.readADC_SingleEnded(0) < 7950) { //UV, don't forget to pull down with ground line while not connected
            p_myLCD->DispStringAt(LIGHT_UV_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
            printOk();
            sensor_light_uv = 1;
          }
#endif
#ifdef MOISTURE_SENSOR
          if (myAdc.readADC_SingleEnded(1) > 3000) { // MOISTURE , don't forget to pull down with ground line while not connected
            sensor_soil_moisture = 1;
            p_myLCD->DispStringAt(SOIL_MOISTURE_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
            printOk();
          }
#endif
          break;

#ifdef LOW_COST_SUNLIGHT_SENSOR
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

        case WATER_TEMPERATURE_MEASUREMENT_ADDR:
          on = true;
          myWaterTempRTD  = WaterTempRTD();
          myWaterTempRTD.init();

          p_myLCD->DispStringAt(WATER_TEMPERATURE_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_water_temperature  ++ ;
          break;

        case WATER_PH_MEASUREMENT_ADDR:
          on = true;
          myWaterPH  = WaterPH();
          myWaterPH.init();

          p_myLCD->DispStringAt(WATER_PH_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_water_ph++;
          break;

        case WATER_EC_MEASUREMENT_ADDR:
          on = true;
          myWaterEC  = WaterEC();
          myWaterEC.init();

          p_myLCD->DispStringAt(WATER_EC_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_water_ec++;
          break;

        case WATER_ORP_MEASUREMENT_ADDR:
          on = true;
          myWaterORP  = WaterORP();
          myWaterORP.init();

          p_myLCD->DispStringAt(WATER_ORP_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          //delay(10);
          printOk();

          sensor_water_orp++;
          break;

        case WATER_DO_MEASUREMENT_ADDR:
          on = true;
          myWaterDO  = WaterDO();
          myWaterDO.init();

          p_myLCD->DispStringAt(WATER_DO_MEASUREMENT_DNAME, 0, lcdLine * ROW_OFFSET);
          printOk();

          sensor_water_do++;
          break;

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
void Sensors::printinfo (char* sensorName, double sensorValue, uint8_t sensorPrecision, char * sensorUnit)
{

#ifdef DEBUG
  Serial.print(sensorName);
  Serial.print(sensorValue, sensorPrecision);
  Serial.println(sensorUnit);
#endif
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

/**
   \fn void Sensors::update(uint8_t menu)
   \brief Read the sensors to update the values and sends them to linux

   \param menu uint8_t tells us in which page of the menu we are
   \return n/a

*/
void Sensors::update(uint8_t menu)
{

  uint8_t check = 0;
  if (menu == 0 || menu == 6) {
    lcdLine = 0;
  } else {
    lcdLine = 50;
  }

  bool bool_water;

  if (sensor_water_level) { //DEPTH with ultrasound
    //Serial.print("water level");
    water_level = myWaterlevel.getDistance();

    //Print the info
    printinfo ( WATER_LEVEL_DNAME, water_level, 0, WATER_LEVEL_UNIT);

    //Send the info to linux
    p_myBridge->sendFloat( WATER_LEVEL_NNAME, water_level);
    //Serial.println("us");
    check++;

  }

  if ( sensor_air_pressure ) {//BAROMETER OK - TEMP OK

    myBarometer.getPT(&air_pressure, &air_temperature);

    //Print the info
    printinfo ( AIR_TEMPERATURE_MEASUREMENT_DNAME, air_temperature, 1, TEMPERATURE_MEASUREMENT_UNIT);
    printinfo ( AIR_PRESSURE_MEASUREMENT_DNAME, air_pressure, 0, AIR_PRESSURE_MEASUREMENT_UNIT);

    //Send the info to linux
    p_myBridge->sendFloat( AIR_TEMPERATURE_MEASUREMENT_NNAME, air_temperature);
    p_myBridge->sendInteger( AIR_PRESSURE_MEASUREMENT_NNAME,  (unsigned long)air_pressure);

    check++;
  }
  //p_myLCD->CleanAll(WHITE);
  if ( sensor_air_relative_humidity ) { //HUMIDITY OK - TEMP INPRECISE
    if (myHum.readSample())
    {
      check++;
      //Print humidity
      printinfo ( AIR_HUMIDITY_LEVEL_MEASUREMENT_DNAME, myHum.mHumidity, 1, AIR_HUMIDITY_LEVEL_MEASUREMENT_UNIT);
      p_myBridge->sendFloat( AIR_HUMIDITY_LEVEL_MEASUREMENT_NNAME, myHum.mHumidity);

      //use the temp of this sensor if there is no other
      if (!(sensor_air_pressure))
      {
        printinfo ( AIR_TEMPERATURE_MEASUREMENT_DNAME, myHum.mTemperature, 1, TEMPERATURE_MEASUREMENT_UNIT);
        p_myBridge->sendFloat( AIR_TEMPERATURE_MEASUREMENT_NNAME, myHum.mTemperature);
      }
    }
  }
  if ( sensor_air_CO ) { //CO OK
    check++;
    //Serial.print("co");
    air_CO = myCO.measure_CO();

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
  if ( sensor_soil_temperature ) {//ADC - TEMP OK
    check++;

    soil_temperature = myAdc.readADC_SingleEnded(2) ;
    #if 0
    long tmpval = long(soil_temperature);
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
    soil_temperature = myAdc.readADC_SingleEnded(2) ;
    soil_temperature = soil_temperature * 0.0000625;
    soil_temperature = soil_temperature * 4755 / (2.5 - soil_temperature);
    soil_temperature = -0.00232 * (soil_temperature) + 17.59246;
    soil_temperature = sqrt(soil_temperature);
    soil_temperature = -(soil_temperature - 3.908) / 0.00116;
    #endif
    Serial.println(soil_temperature);
    if (isnan(soil_temperature) != 1){
   // Serial.println(soil_temperature);
    printinfo (SOIL_TEMPERATURE_MEASUREMENT_DNAME, soil_temperature, 1, TEMPERATURE_MEASUREMENT_UNIT);
    p_myBridge->sendFloat( SOIL_TEMPERATURE_MEASUREMENT_NNAME, soil_temperature);

    }
  }
  if (sensor_light_par) {//ADC - JYP OK
    light_par = myAdc.readADC_SingleEnded(3) * 0.0078125 * 99.7;
    if ((unsigned long)light_par > 51000) light_par = 0.0f;
    check++;
    printinfo (LIGHT_PAR_MEASUREMENT_DNAME, light_par, 1, LIGHT_PAR_MEASUREMENT_UNIT);
    p_myBridge->sendFloat( LIGHT_PAR_MEASUREMENT_NNAME, light_par);


  }
  if (sensor_light_uv) {//ADC - UV
    light_uv = myAdc.readADC_SingleEnded(0) * 0.0078125 * 5;
    // if ((int)light_uv < 1225 && (int)light_uv > 5 ) { // FIXME move to detec
    check++;
    //      if ((int)val_tamp < 100) p_myLCD->BacklightConf(LOAD_TO_RAM, 20);             TO DEAL WITH
    //      else p_myLCD->BacklightConf(LOAD_TO_RAM, 100);
    printinfo (LIGHT_UV_MEASUREMENT_DNAME, light_uv, 1, LIGHT_PAR_MEASUREMENT_UNIT);
    p_myBridge->sendFloat( LIGHT_UV_MEASUREMENT_NNAME, light_uv);
    // }

  }
  if (sensor_soil_moisture) {//ADC - WMS
    soil_moisture = (myAdc.readADC_SingleEnded(1) * 0.000125 * 239) / 2, 8;
    printinfo (SOIL_MOISTURE_MEASUREMENT_DNAME, soil_moisture, 1, SOIL_MOISTURE_MEASUREMENT_UNIT);
    p_myBridge->sendFloat( SOIL_MOISTURE_MEASUREMENT_NNAME, soil_moisture);

  }
#ifdef LOW_COST_SUNLIGHT_SENSOR
  if (sensor_light_uv) {
    light_lux = mySunLightSensor.ReadVisible();
    printinfo ( LIGHT_LUX_MEASUREMENT_DNAME, light_lux, 0, LIGHT_LUX_MEASUREMENT_UNIT);
    p_myBridge->sendInteger( LIGHT_LUX_MEASUREMENT_NNAME, (unsigned long)light_lux);

    light_uv = mySunLightSensor.ReadUV();
    printinfo ( LIGHT_UV_MEASUREMENT_DNAME, light_uv, 0, LIGHT_UV_MEASUREMENT_UNIT);
    p_myBridge->sendInteger( LIGHT_UV_MEASUREMENT_NNAME, (unsigned long)light_uv);
  }
#endif
  if (sensor_water_orp) {//WATER ORP - OK
    if (menu != 6 && sensor_water == _item_orp) {
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

  if (sensor_water_do) { //WATER DO - OK
    if (menu != 6 && sensor_water == _item_do) {
      bool_water = myEZO.readData(WATER_DO_MEASUREMENT_ADDR);
      if (bool_water == false) sensor_water_do = 0;
      else {
        water_do = atof(myEZO.getData());
        p_myBridge->sendFloat( WATER_DO_MEASUREMENT_NNAME, water_do);
        //sensor_water++;
        //sensor_water++;
      }
    }
    check++;
    printinfo ( WATER_DO_MEASUREMENT_DNAME, water_do, 1, WATER_DO_MEASUREMENT_UNIT);

  }

  if (sensor_water_ec) { // WATER EC - OK
    if (menu != 6 && sensor_water == _item_ec) {
      bool_water = myEZO.readData(WATER_EC_MEASUREMENT_ADDR);
      if (bool_water == false) sensor_water_ec = 0;
      else {
        water_ec = atof(myEZO.getData());
        p_myBridge->sendFloat( WATER_EC_MEASUREMENT_NNAME, water_ec);
        //sensor_water++;
      }
    }
    check++;
    printinfo ( WATER_EC_MEASUREMENT_DNAME, water_ec, 1, WATER_EC_MEASUREMENT_UNIT);

  }

  if (sensor_water_ph) {//WATER PH - OK
    if (menu != 6 && sensor_water == _item_ph) {
      bool_water = myEZO.readData(WATER_PH_MEASUREMENT_ADDR);
      if (bool_water == false) sensor_water_ph = 0;
      else {
        water_ph = atof(myEZO.getData());
        p_myBridge->sendFloat( WATER_PH_MEASUREMENT_NNAME, water_ph);
        //sensor_water++;
      }
    }
    // if ((int)water_ph > 0 && (int)water_ph < 14) {
    check++;
    printinfo ( WATER_PH_MEASUREMENT_DNAME, water_ph, 1, WATER_PH_MEASUREMENT_UNIT);

  }
  //}

  if (sensor_water_temperature) { // WATER TEMP -OK
    if (menu != 6 && sensor_water == _item_rtd) {
      bool_water = myEZO.readData(WATER_TEMPERATURE_MEASUREMENT_ADDR);
      if (bool_water == false) sensor_water_temperature = 0;
      else {
        water_temperature = atof(myEZO.getData());
        p_myBridge->sendFloat(WATER_TEMPERATURE_MEASUREMENT_NNAME, water_temperature);
        //sensor_water++;
      }
    }
    check++;
    printinfo ( WATER_TEMPERATURE_MEASUREMENT_DNAME, water_temperature, 1, TEMPERATURE_MEASUREMENT_UNIT);

  }

  if ( sensor_air_CO2 ) { //CO2
    myCO2.measure();
    check++;
    printinfo ( AIR_CO2_MEASUREMENT_DNAME, myCO2.ppm, 0, AIR_CO2_MEASUREMENT_UNIT);
    p_myBridge->sendInteger(AIR_CO2_MEASUREMENT_NNAME, myCO2.ppm);

  }


  if (!check && !menu)
    p_myLCD->DispStringAt("No sensor", 6 * CHAR_OFFSET, 1 * ROW_OFFSET);

#ifdef DEBUG
  Serial.println("");
  Serial.println("");
#endif
}

/**
   \fn void Sensors::compensationEC()
   \brief Sends the compensation order for the EC compensation

   \param n/a
   \return n/a

*/
void Sensors::compensationEC()
{
  if (sensor_water_do && (sensor_water + 1) == _item_do) //DO
  {
    if ((int)water_ec > 0) myWaterDO.setEcCompensation(water_ec);
  }
}

/**
   \fn void Sensors::compensationTemp()
   \brief Sends the compensation order for the Temp compensation

   \param n/a
   \return n/a

*/
void Sensors::compensationTemp()
{
  if ((int)water_temperature > -1023) {
    if (sensor_water_ec && (sensor_water + 1) == _item_ec) //EC
      myWaterEC.setTemperatureCompensation(water_temperature);
    if (sensor_water_do && (sensor_water + 1) == _item_do) //DO
      myWaterDO.setTemperatureCompensation(water_temperature);
  }
}


/**
   \fn void Sensors::waterCall()
   \brief Sends the I2C request to update the water box

   \param n/a
   \return n/a

*/
void Sensors::waterCall() {
  //if(sensors_water_monitoring>0){
  if (sensor_water < 5)
    sensor_water++;
  else sensor_water = _item_rtd;
  if (sensor_water == _item_rtd)
  {
    if (sensor_water_temperature)//WATER TEMP
    {
      myWaterTempRTD.getTemperature();
    }
    //sensor_water++;
  }
  else if (sensor_water == _item_ph)
  {
    if (sensor_water_ph )//WATER PH
    {
      myWaterPH.getPh();
    }
    //sensor_water++;
  }
  else if (sensor_water == _item_ec)
  {
    if (sensor_water_ec )//WATER EC
    {
      myWaterEC.getEc();
    }
    //sensor_water++;
  }
  else if (sensor_water == _item_do)
  {
    if (sensor_water_do)//WATER DO
    {
      myWaterDO.getDissolvedOxygen();
    }
    //sensor_water++;
  }
  else if (sensor_water == _item_orp) {
    if (sensor_water_orp) {//WATER ORP

      myWaterORP.getORP();
      //sensor_water = _item_rtd;
    }
  }
}


/**
   \fn void Sensors::calibrate(uint8_t sensorID, int val)
   \brief Calibrate specific sensors

   \param sensorID: sensor to calibrate, val: num of point for calibration
   \return n/a
*/
#define TXT_PUT_THE_PROBE "Put the probe"
#define TXT_WAIT_2MIN "Wait 2 min"
#define TXT_WAIT_1_30_MIN "Wait 1:30 min"
#define TXT_WAIT_1MIN "Wait 1 min"
#define TXT_WAIT_30S "Wait 30 sec"
#define TXT_PUT_IN_SOLUTION "in the solution "
#define TXT_EXPOSED_TO_AIR "exoposed to air"

void Sensors::calibrate(uint8_t sensorID, int val)
{
  uint8_t check = 0;
  reset();
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

  else if ((sensorID == 2) && sensor_water_do && (water_do > 0)) //DO
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
  else if ((sensorID == 3) && sensor_water_ec && (water_ec > 0))    //EC
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
  else if ((sensorID == 4) && sensor_water_temperature && (water_temperature > -900)) //RTC
  {
    check++;
    p_myLCD->println(TXT_PUT_THE_PROBE);
    p_myLCD->println("in boiling water");
    wait();
    myWaterTempRTD.calibrate();
  }
  else if ((sensorID == 5) && sensor_water_orp  && (water_orp < 1000)) //ORP
  {
    check++;
    p_myLCD->println(TXT_PUT_THE_PROBE);
    p_myLCD->println(TXT_PUT_IN_SOLUTION);
    p_myLCD->println(TXT_WAIT_1MIN);
    wait();
    myWaterORP.calibrate();
  }
  else if (sensorID == 6 &&  sensor_air_CO2) //CO2
  {
    check++;
    myi2cuart.pinMode(1, OUTPUT);    //set up for the calibration pin.
    myi2cuart.digitalWrite(1, LOW);  //start calibration of MH-Z16 under 400ppm
    delay(10000);                  //5+ sec needed for the calibration process
    myi2cuart.digitalWrite(1, HIGH);
  }
  else if (sensorID == 7 && sensor_air_CO  ) //CO
  {
    check++;
    myCO.doCalibrate();
  }
  else if (sensorID == 8 && sensor_water_level == 1) //Ultrasound
  {
    check++;
    myWaterlevel.Calibration();
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
