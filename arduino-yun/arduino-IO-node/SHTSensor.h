/*
 *  Copyright (c) 2016, Sensirion AG <andreas.brauchli@sensirion.com>
 *  Copyright (c) 2015-2016, Johannes Winkelmann <jw@smts.ch>
 *  All rights reserved.
 * 
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of the <organization> nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SHTSENSOR_H
#define SHTSENSOR_H

#include "SensorTypes.h"

#ifndef I2CLIB
#include <Wire.h>
#else
#include "I2C.h"
#endif
#include <Arduino.h>

#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR

// Forward declaration
class SHTSensorDriver;

/**
 * Official interface for Sensirion SHT Sensors
 */
class SHTSensor
{
public:
  /**
   * Enum of the supported Digital Sensirion SHT Sensors.
   * For analog sensors, see SHT3xAnalogSensor.
   * Using the special AUTO_DETECT sensor causes all i2c sensors to be
   * probed. The first matching sensor will then be used.
   */
  enum SHTSensorType {
    /** Automatically detect the sensor type (only i2c sensors listed above) */
    AUTO_DETECT,
    // i2c Sensors:
    /** SHT3x-DIS with ADDR (sensor pin 2) connected to VSS (default) */
    SHT3X
  };

  /** Value reported by getHumidity() when the sensor is not initialized */
  static const float HUMIDITY_INVALID;
  /** Value reported by getTemperature() when the sensor is not initialized */
  static const float TEMPERATURE_INVALID;

  /**
   * Instantiate a new SHTSensor
   * By default, the i2c bus is queried for known SHT Sensors. To address
   * a specific sensor, set the `sensorType'.
   */
  SHTSensor(SHTSensorType sensorType = AUTO_DETECT)
      : mSensorType(sensorType),
        mSensor(NULL),
        mTemperature(SHTSensor::TEMPERATURE_INVALID),
        mHumidity(SHTSensor::HUMIDITY_INVALID)
  {
  }
  
  /**
   * Initialize the sensor driver
   * To read out the sensor use readSample(), followed by getTemperature() and
   * getHumidity() to retrieve the values from the sample
   */
  bool init();

  /**
   * Read new values from the sensor
   * After the call, use getTemperature() and getHumidity() to retrieve the
   * values
   * Returns true if the sample was read and the values are cached
   */
  bool readSample();
  float mTemperature;
  float mHumidity;
private:
  void periodicDataAcquisition(uint8_t i2cAddress);
  void cleanup();
  SHTSensorType mSensorType;
  SHTSensorDriver *mSensor;

};


/** Abstract class for a digital SHT Sensor driver */
class SHTSensorDriver
{
public:
  /** Returns true if the next sample was read and the values are cached */
  virtual bool readSample();
  float mTemperature;
  float mHumidity;
};

/** Base class for i2c SHT Sensor drivers */
class SHTI2cSensor : public SHTSensorDriver {
public:
  /** Size of i2c commands to send */
  static const uint8_t CMD_SIZE;

  /** Size of i2c replies to expect */
  static const uint8_t EXPECTED_DATA_SIZE;

  /**
   * Constructor for i2c SHT Sensors
   * Takes the `i2cAddress' to read, the `i2cCommand' issues when sampling
   * the sensor and the values `a', `b', `c' to convert the fixed-point
   * temperature value received by the sensor to a floating point value using
   * the formula: temperature = a + b * (rawTemperature / c)
   * and the values `x' and `y' to convert the fixed-point humidity value
   * received by the sensor to a floating point value using the formula:
   * humidity = x * (rawHumidity / y)
   */
  SHTI2cSensor(uint8_t i2cAddress, uint16_t i2cCommand,
               float a, float b, float c,
               float x, float y)
      : mI2cAddress(i2cAddress), mI2cCommand(i2cCommand),
        mA(a), mB(b), mC(c), mX(x), mY(y)
  {
  }

  virtual bool readSample();
  uint8_t mI2cAddress;
  uint16_t mI2cCommand;
  float mA;
  float mB;
  float mC;
  float mX;
  float mY;

private:
  static uint8_t crc8(const uint8_t *data, uint8_t len);
  static bool readFromI2c(uint8_t i2cAddress,
                          const uint8_t *i2cCommand,
                          uint8_t commandLength, uint8_t *data,
                          uint8_t dataLength);
};

#endif
#endif /* SHTSENSOR_H */
