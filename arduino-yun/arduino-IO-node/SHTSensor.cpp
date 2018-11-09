/*
    Copyright (c) 2016, Sensirion AG <andreas.brauchli@sensirion.com>
    Copyright (c) 2015-2016, Johannes Winkelmann <jw@smts.ch>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
 *      * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in the
          documentation and/or other materials provided with the distribution.
 *      * Neither the name of the <organization> nor the
          names of its contributors may be used to endorse or promote products
          derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SHTSensor.h"

#ifdef SENSOR_TYPE_RELATIVE_HUMIDITY_SENSOR

//#define PERIODIC //data acquisition

//
// class SHTI2cSensor
//


const uint8_t SHTI2cSensor::CMD_SIZE            = 2;
const uint8_t SHTI2cSensor::EXPECTED_DATA_SIZE  = 6;
//const uint8_t SHTI2cSensor::MAX_I2C_READ_TRIES  = 5;

bool SHTI2cSensor::readFromI2c(uint8_t i2cAddress, const uint8_t *i2cCommand, uint8_t commandLength, uint8_t *data, uint8_t dataLength)
{
#ifndef I2CLIB
  Wire.beginTransmission(i2cAddress);

  
#ifdef PERIODIC
   if (Wire.write(0xe0) != 1) {
      return false;
    } 
    if (Wire.write(0x00) != 1) {
      return false;
    }
#else
  for (int i = 0; i < commandLength; ++i) {
    if (Wire.write(i2cCommand[i]) != 1) {
      return false;
    }
  }
#endif

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  Wire.requestFrom(i2cAddress, dataLength);

  // there should be no reason for this to not be ready, since we're using clock
  // stretching mode, but just in case we'll try a few times
  /*uint8_t tries = 1;
    while (Wire.available() < dataLength) {
    delay(1);
    if (tries++ >= MAX_I2C_READ_TRIES) {
      return false;
    }
    }*/

  for (int i = 0; i < dataLength; ++i) {
    data[i] = Wire.read();
  }

  return true;
#else
  bool returnStatus = I2c.start();
  if (returnStatus) {
    return (returnStatus);
  }
  returnStatus = I2c.sendAddress(SLA_W(i2cAddress));
  if (returnStatus)
  {
    if (returnStatus == 1) {
      return (2);
    }
    return (returnStatus);
  }
  #ifdef PERIODIC
    returnStatus = I2c.sendByte(0xe0);
    if (returnStatus)
    {
      if (returnStatus == 1) {
        return (3);
      }
      return (returnStatus);
    }
    returnStatus = I2c.sendByte(0x00);
    if (returnStatus)
    {
      if (returnStatus == 1) {
        return (3);
      }
      return (returnStatus);
    }
#else
  for (int i = 0; i < commandLength; ++i) {
    returnStatus = I2c.sendByte(i2cCommand[i]);
    if (returnStatus)
    {
      if (returnStatus == 1) {
        return (3);
      }
      return (returnStatus);
    }
  }
#endif

  returnStatus = I2c.read(i2cAddress, dataLength, data);
  if (returnStatus) return false;
  else return true;


#endif
}

uint8_t SHTI2cSensor::crc8(const uint8_t *data, uint8_t len)
{
  // adapted from SHT21 sample code from
  // http://www.sensirion.com/en/products/humidity-temperature/download-center/

  uint8_t crc = 0xff;
  uint8_t byteCtr;
  for (byteCtr = 0; byteCtr < len; ++byteCtr) {
    crc ^= data[byteCtr];
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x31;
      } else {
        crc = (crc << 1);
      }
    }
  }
  return crc;
}


bool SHTI2cSensor::readSample()
{
  //Serial.print(" 3 ");
  uint8_t data[EXPECTED_DATA_SIZE];
  uint8_t cmd[CMD_SIZE];

  cmd[0] = mI2cCommand >> 8;
  cmd[1] = mI2cCommand & 0xff;

  if (!readFromI2c(mI2cAddress, cmd, CMD_SIZE, data, EXPECTED_DATA_SIZE)) {
    return false;
  }

  // -- Important: assuming each 2 byte of data is followed by 1 byte of CRC

  // check CRC for both RH and T
  if (crc8(&data[0], 2) != data[2] || crc8(&data[3], 2) != data[5]) {
    return false;
  }

  // convert to Temperature/Humidity
  uint16_t val;
  val = (data[0] << 8) + data[1];
  mTemperature = mA + mB * (val / mC);

  val = (data[3] << 8) + data[4];
  mHumidity = mX * (val / mY);

  return true;
}





//
// class SHT3xSensor
//

class SHT3xSensor : public SHTI2cSensor
{
  private:
    //static const uint16_t SHT3X_ACCURACY_HIGH    = 0x2c06;
    static const uint16_t SHT3X_ACCURACY_MEDIUM  = 0x2c0d;
    //static const uint16_t SHT3X_ACCURACY_LOW     = 0x2c10;

  public:
    static const uint8_t SHT3X_I2C_ADDRESS_44 = 0x44;
    static const uint8_t SHT3X_I2C_ADDRESS_45 = 0x45;

    SHT3xSensor(uint8_t i2cAddress = SHT3X_I2C_ADDRESS_44)
      : SHTI2cSensor(i2cAddress, SHT3X_ACCURACY_MEDIUM,
                     -45, 175, 65535, 100, 65535)
    {
    }

    /*virtual bool setAccuracy(SHTSensor::SHTAccuracy newAccuracy)
      {
      switch (newAccuracy) {
        case SHTSensor::SHT_ACCURACY_HIGH:
          mI2cCommand = SHT3X_ACCURACY_HIGH;
          break;
        case SHTSensor::SHT_ACCURACY_MEDIUM:
          mI2cCommand = SHT3X_ACCURACY_MEDIUM;
          break;
        case SHTSensor::SHT_ACCURACY_LOW:
          mI2cCommand = SHT3X_ACCURACY_LOW;
          break;
        default:
          return false;
      }
      return true;
      }*/
};





//
// class SHTSensor
//


const float SHTSensor::TEMPERATURE_INVALID = NAN;
const float SHTSensor::HUMIDITY_INVALID = NAN;


//////NECESSARY///////
bool SHTSensor::init()
{
  cleanup();
  mSensor = new SHT3xSensor();
#ifdef PERIODIC
  periodicDataAcquisition(0x44);
#endif
  return (mSensor != NULL);
}


void SHTSensor::periodicDataAcquisition(uint8_t i2cAddress)
{
#ifndef I2CLIB
  Wire.beginTransmission(i2cAddress);
  /* stop internal heater */
  if (Wire.write(0x30) != 1) {
    return false;
  }
  if (Wire.write(0x66) != 1) {
    return false;
  }
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  Wire.beginTransmission(i2cAddress);
  /* periodic data acquisition */
  if (Wire.write(0x20) != 1) {
    return false;
  }
  if (Wire.write(0x24) != 1) {
    return false;
  }
  if (Wire.endTransmission(false) != 0) {
    return false;
  }
  return true;
#else
  bool returnStatus = I2c.start();
  if (returnStatus) {
    return (returnStatus);
  }
  returnStatus = I2c.sendAddress(SLA_W(i2cAddress));
  if (returnStatus)
  {
    if (returnStatus == 1) {
      return (2);
    }
    return (returnStatus);
  }
  returnStatus = I2c.sendByte(0x30);
  if (returnStatus)
  {
    if (returnStatus == 1) {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = I2c.sendByte(0x66);
  if (returnStatus)
  {
    if (returnStatus == 1) {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = I2c.sendByte(0x20);
  if (returnStatus)
  {
    if (returnStatus == 1) {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = I2c.sendByte(0x24);
  if (returnStatus)
  {
    if (returnStatus == 1) {
      return (3);
    }
    return (returnStatus);
  }

  return true;

#endif
}

bool SHTSensor::readSample()
{
  if (!mSensor || !mSensor->readSample())
  {
    return false;
  }
  mTemperature = mSensor->mTemperature;
  mHumidity = mSensor->mHumidity;

  return true;
}
/*
  bool SHTSensor::setAccuracy(SHTAccuracy newAccuracy)
  {
  if (!mSensor)
    return false;
  return mSensor->setAccuracy(newAccuracy);
  }*/

void SHTSensor::cleanup()
{
  if (mSensor) {
    delete mSensor;
    mSensor = NULL;
  }
}

#endif