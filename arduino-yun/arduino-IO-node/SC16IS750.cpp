/*
  Description:
  This is a example code for Sandbox Electronics' I2C/SPI to UART bridge module.
  You can get one of those products on
  http://sandboxelectronics.com

  Version:
  V0.1

  Release Date:
  2014-02-16

  Author:
  Tiequan Shao          info@sandboxelectronics.com

  Lisence:
  CC BY-NC-SA 3.0

  Please keep the above information when you use this code in your project.
*/


//#define SC16IS750_DEBUG_PRINT
#include "SC16IS750.h"

#ifdef SENSOR_TYPE_CO2_SENSOR
#ifndef I2CLIB
#define WIRE Wire
#endif


SC16IS750::SC16IS750(uint8_t prtcl, uint8_t addr_sspin)
{
  device_address_sspin = (addr_sspin >> 1);
  peek_flag = 0;
}


void SC16IS750::begin(uint32_t baud)
{
#ifndef I2CLIB
  WIRE.begin();
#else
 // I2c.begin();
#endif
  ResetDevice();
  FIFOEnable(1);
  SetBaudrate(baud);
  SetLine();
}

int SC16IS750::available(void)
{
  return FIFOAvailableData();
}

int SC16IS750::read(void)
{
  if ( peek_flag == 0) {
    return ReadByte();
  } else {
    peek_flag = 0;
    return peek_buf;
  }
}

size_t SC16IS750::write(uint8_t val)
{
  WriteByte(val);
}

void SC16IS750::pinMode(uint8_t pin, uint8_t i_o)
{
  GPIOSetPinMode(pin, i_o);
}

void SC16IS750::digitalWrite(uint8_t pin, uint8_t value)
{
  GPIOSetPinState(pin, value);
}

uint8_t SC16IS750::digitalRead(uint8_t pin)
{
  return GPIOGetPinState(pin);
}


uint8_t SC16IS750::ReadRegister(uint8_t reg_addr)
{
  uint8_t result;
#ifndef I2CLIB
  WIRE.beginTransmission(device_address_sspin);
  WIRE.write((reg_addr << 3));
  WIRE.endTransmission(0);
  WIRE.requestFrom(device_address_sspin, (uint8_t)1);
  result = WIRE.read();
#else
 uint8_t ret = 0;
 ret = I2c.read(device_address_sspin, (reg_addr << 3),1, &result);
 if(ret) return 0;
#endif
  return result;
}

void SC16IS750::WriteRegister(uint8_t reg_addr, uint8_t val)
{
  
#ifndef I2CLIB
  WIRE.beginTransmission(device_address_sspin);
  WIRE.write((reg_addr << 3));
  WIRE.write(val);
  WIRE.endTransmission(1);
  #else
  uint8_t result;
  result = I2c.write(device_address_sspin,(reg_addr << 3),val);
#endif
  return ;
}

int16_t SC16IS750::SetBaudrate(uint32_t baudrate) //return error of baudrate parts per thousand
{
  uint16_t divisor;
  //uint8_t prescaler;
  //uint32_t actual_baudrate;
  //int16_t error;
  uint8_t temp_lcr;
  /*if ( (ReadRegister(SC16IS750_REG_MCR) & 0x80) == 0) { //if prescaler==1
    prescaler = 1;
  } else {
    prescaler = 4;
  }*/
  
  divisor = (SC16IS750_CRYSTCAL_FREQ ) / (baudrate * 16);

  temp_lcr = ReadRegister(SC16IS750_REG_LCR);
  temp_lcr |= 0x80;
  WriteRegister(SC16IS750_REG_LCR, temp_lcr);
  //write to DLL
  WriteRegister(SC16IS750_REG_DLL, (uint8_t)divisor);
  //write to DLH
  WriteRegister(SC16IS750_REG_DLH, (uint8_t)(divisor >> 8));
  temp_lcr &= 0x7F;
  WriteRegister(SC16IS750_REG_LCR, temp_lcr);


  //actual_baudrate = (SC16IS750_CRYSTCAL_FREQ / prescaler) / (16 * divisor);
  //error = ((float)actual_baudrate - baudrate) * 1000 / baudrate;
#ifdef  SC16IS750_DEBUG_PRINT
  Serial.print("Desired baudrate: ");
  Serial.println(baudrate, DEC);
  Serial.print("Calculated divisor: ");
  Serial.println(divisor, DEC);
  Serial.print("Actual baudrate: ");
  Serial.println(actual_baudrate, DEC);
  Serial.print("Baudrate error: ");
  Serial.println(error, DEC);
#endif

  //return error;

}

void SC16IS750::SetLine( )
{
  uint8_t temp_lcr;
  temp_lcr = ReadRegister(SC16IS750_REG_LCR);
  temp_lcr &= 0xC0; //Clear the lower six bit of LCR (LCR[0] to LCR[5]
  temp_lcr |= 0x03;
  WriteRegister(SC16IS750_REG_LCR, temp_lcr);
}

void SC16IS750::GPIOSetPinMode(uint8_t pin_number, uint8_t i_o)
{
  uint8_t temp_iodir;

  temp_iodir = ReadRegister(SC16IS750_REG_IODIR);
  if ( i_o == OUTPUT ) {
    temp_iodir |= (0x01 << pin_number);
  } else {
    temp_iodir &= (uint8_t)~(0x01 << pin_number);
  }

  WriteRegister(SC16IS750_REG_IODIR, temp_iodir);
  return;
}

void SC16IS750::GPIOSetPinState(uint8_t pin_number, uint8_t pin_state)
{
  uint8_t temp_iostate;

  temp_iostate = ReadRegister(SC16IS750_REG_IOSTATE);
  if ( pin_state == 1 ) {
    temp_iostate |= (0x01 << pin_number);
  } else {
    temp_iostate &= (uint8_t)~(0x01 << pin_number);
  }

  WriteRegister(SC16IS750_REG_IOSTATE, temp_iostate);
  return;
}


uint8_t SC16IS750::GPIOGetPinState(uint8_t pin_number)
{
  uint8_t temp_iostate;

  temp_iostate = ReadRegister(SC16IS750_REG_IOSTATE);
  if ( temp_iostate & (0x01 << pin_number) == 0 ) {
    return 0;
  }
  return 1;
}

/*uint8_t SC16IS750::GPIOGetPortState(void)
{

  return ReadRegister(SC16IS750_REG_IOSTATE);

}

void SC16IS750::GPIOSetPortMode(uint8_t port_io)
{
  WriteRegister(SC16IS750_REG_IODIR, port_io);
  return;
}

void SC16IS750::GPIOSetPortState(uint8_t port_state)
{
  WriteRegister(SC16IS750_REG_IOSTATE, port_state);
  return;
}

void SC16IS750::SetPinInterrupt(uint8_t io_int_ena)
{
  WriteRegister(SC16IS750_REG_IOINTENA, io_int_ena);
  return;
}*/

void SC16IS750::ResetDevice(void)
{
  uint8_t reg;

  reg = ReadRegister(SC16IS750_REG_IOCONTROL);
  reg |= 0x08;
  WriteRegister(SC16IS750_REG_IOCONTROL, reg);

  return;
}

/*void SC16IS750::ModemPin(uint8_t gpio) //gpio == 0, gpio[7:4] are modem pins, gpio == 1 gpio[7:4] are gpios
{
  uint8_t temp_iocontrol;

  temp_iocontrol = ReadRegister(SC16IS750_REG_IOCONTROL);
  if ( gpio == 0 ) {
    temp_iocontrol |= 0x02;
  } else {
    temp_iocontrol &= 0xFD;
  }
  WriteRegister(SC16IS750_REG_IOCONTROL, temp_iocontrol);

  return;
}

void SC16IS750::GPIOLatch(uint8_t latch)
{
  uint8_t temp_iocontrol;

  temp_iocontrol = ReadRegister(SC16IS750_REG_IOCONTROL);
  if ( latch == 0 ) {
    temp_iocontrol &= 0xFE;
  } else {
    temp_iocontrol |= 0x01;
  }
  WriteRegister(SC16IS750_REG_IOCONTROL, temp_iocontrol);

  return;
}

void SC16IS750::InterruptControl(uint8_t int_ena)
{
  WriteRegister(SC16IS750_REG_IER, int_ena);
}

uint8_t SC16IS750::InterruptPendingTest(void)
{
  return (ReadRegister(SC16IS750_REG_IIR) & 0x01);
}

void SC16IS750::__isr(void)
{
  uint8_t irq_src;

  irq_src = ReadRegister(SC16IS750_REG_IIR);
  irq_src = (irq_src >> 1);
  irq_src &= 0x3F;

  switch (irq_src) {
    case 0x06:                  //Receiver Line Status Error
      break;
    case 0x0c:               //Receiver time-out interrupt
      break;
    case 0x04:               //RHR interrupt
      break;
    case 0x02:               //THR interrupt
      break;
    case 0x00:                  //modem interrupt;
      break;
    case 0x30:                  //input pin change of state
      break;
    case 0x10:                  //XOFF
      break;
    case 0x20:                  //CTS,RTS
      break;
    default:
      break;
  }
  return;
}*/

void SC16IS750::FIFOEnable(uint8_t fifo_enable)
{
  uint8_t temp_fcr;

  temp_fcr = ReadRegister(SC16IS750_REG_FCR);

  if (fifo_enable == 0) {
    temp_fcr &= 0xFE;
  } else {
    temp_fcr |= 0x01;
  }
  WriteRegister(SC16IS750_REG_FCR, temp_fcr);

  return;
}
/*
void SC16IS750::FIFOReset(uint8_t rx_fifo)
{
  uint8_t temp_fcr;

  temp_fcr = ReadRegister(SC16IS750_REG_FCR);

  if (rx_fifo == 0) {
    temp_fcr |= 0x04;
  } else {
    temp_fcr |= 0x02;
  }
  WriteRegister(SC16IS750_REG_FCR, temp_fcr);

  return;

}

void SC16IS750::FIFOSetTriggerLevel(uint8_t rx_fifo, uint8_t length)
{
  uint8_t temp_reg;

  temp_reg = ReadRegister(SC16IS750_REG_MCR);
  temp_reg |= 0x04;
  WriteRegister(SC16IS750_REG_MCR, temp_reg); //SET MCR[2] to '1' to use TLR register or trigger level control in FCR register

  temp_reg = ReadRegister(SC16IS750_REG_EFR);
  WriteRegister(SC16IS750_REG_EFR, temp_reg | 0x10); //set ERF[4] to '1' to use the  enhanced features
  if (rx_fifo == 0) {
    WriteRegister(SC16IS750_REG_TLR, length << 4); //Tx FIFO trigger level setting
  } else {
    WriteRegister(SC16IS750_REG_TLR, length);    //Rx FIFO Trigger level setting
  }
  WriteRegister(SC16IS750_REG_EFR, temp_reg); //restore EFR register

  return;
}*/

uint8_t SC16IS750::FIFOAvailableData(void)
{
#ifdef  SC16IS750_DEBUG_PRINT
  Serial.print("=====Available data:");
  Serial.println(ReadRegister(SC16IS750_REG_RXLVL), DEC);
#endif
  return ReadRegister(SC16IS750_REG_RXLVL);
  //    return ReadRegister(SC16IS750_REG_LSR) & 0x01;
}
/*
uint8_t SC16IS750::FIFOAvailableSpace(void)
{
  return ReadRegister(SC16IS750_REG_TXLVL);

}*/

void SC16IS750::WriteByte(uint8_t val)
{
  uint8_t tmp_lsr;
  do {
    tmp_lsr = ReadRegister(SC16IS750_REG_LSR);
  } while ((tmp_lsr & 0x20) == 0);

  WriteRegister(SC16IS750_REG_THR, val);
}


int SC16IS750::ReadByte(void)
{
  if (FIFOAvailableData() == 0) {
#ifdef  SC16IS750_DEBUG_PRINT
    Serial.println("No data available");
#endif
    return -1;

  } else {

#ifdef  SC16IS750_DEBUG_PRINT
    Serial.println("***********Data available***********");
#endif
    return ReadRegister(SC16IS750_REG_RHR);
  }


}
/*
void SC16IS750::EnableTransmit(uint8_t tx_enable)
{
  uint8_t temp_efcr;
  temp_efcr = ReadRegister(SC16IS750_REG_EFCR);
  if ( tx_enable == 0) {
    temp_efcr |= 0x04;
  } else {
    temp_efcr &= 0xFB;
  }
  WriteRegister(SC16IS750_REG_EFCR, temp_efcr);

  return;
}

uint8_t SC16IS750::ping()
{
  WriteRegister(SC16IS750_REG_SPR, 0x55);
  if (ReadRegister(SC16IS750_REG_SPR) != 0x55) {
    return 0;
  }

  WriteRegister(SC16IS750_REG_SPR, 0xAA);
  if (ReadRegister(SC16IS750_REG_SPR) != 0xAA) {
    return 0;
  }

  return 1;

}*/


void SC16IS750::flush()
{
  uint8_t tmp_lsr;

  do {
    tmp_lsr = ReadRegister(SC16IS750_REG_LSR);
  } while ((tmp_lsr & 0x20) == 0);


}

int SC16IS750:: peek()
{
  if ( peek_flag == 0 ) {
    peek_buf = ReadByte();
    if (  peek_buf >= 0 ) {
      peek_flag = 1;
    }
  }

  return peek_buf;

}

#endif /*SENSOR_TYPE_CO2_SENSOR*/
