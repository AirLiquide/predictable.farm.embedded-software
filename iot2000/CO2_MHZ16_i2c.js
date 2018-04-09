const VERSION = 1;
const local_service_PORT = 64000;

const client = require('/usr/lib/node_modules/socket.io-client');
const mraa = require('/usr/lib/node_modules/mraa'); //require mraa

console.log('MRAA Version: ' + mraa.getVersion()); //write the mraa version to the console

const fs = require("fs");
var util = require('util');
var log_file = fs.createWriteStream(__dirname + '/co2.log', {flags : 'a'});
var log_stdout = process.stdout;


console.logCopy = console.log.bind(console);
console.log = function () {
    // Timestamp to prepend
    var timestamp = new Date().toJSON();
    if (arguments.length) {
        // True array copy so we can call .splice()
        var args = Array.prototype.slice.call(arguments, 0);
        // If there is a format string then... it must
        // be a string
        if (typeof arguments[0] === "string") {
            // Prepend timestamp to the (possibly format) string
            args[0] = arguments[0];
            // Insert the timestamp where it has to be
            args.splice(1, 0, timestamp);
            // Log the whole array
            this.logCopy.apply(this, args);
        }
        else {
            // "Normal" log
            this.logCopy(timestamp, args);
        }
        log_file.write(timestamp + " - "+util.format.apply(null, arguments) + '\n');
    }
};

function datelog(message) {
  console.log(message);
}


//const SC16IS750_ADDRESS = 0x48; //0X90
const SC16IS750_ADDRESS = 0x4d; //0X9A
//const SC16IS750_ADDRESS = 0x4b; //0X96

//General Registers
const SC16IS750_REG_RHR = 0x00;
const SC16IS750_REG_THR = 0X00;
const SC16IS750_REG_IER = 0X01;
const SC16IS750_REG_FCR = 0X02;
const SC16IS750_REG_IIR = 0X02;
const SC16IS750_REG_LCR = 0X03;
const SC16IS750_REG_MCR = 0X04;
const SC16IS750_REG_LSR = 0X05;
const SC16IS750_REG_MSR = 0X06;
const SC16IS750_REG_SPR = 0X07;
const SC16IS750_REG_TCR = 0X06;
const SC16IS750_REG_TLR = 0X07;
const SC16IS750_REG_TXLVL = 0X08;
const SC16IS750_REG_RXLVL = 0X09;
const SC16IS750_REG_IODIR = 0X0A;
const SC16IS750_REG_IOSTATE = 0X0B;
const SC16IS750_REG_IOINTENA = 0X0C;
const SC16IS750_REG_IOCONTROL = 0X0E;
const SC16IS750_REG_EFCR = 0X0F;

//Special Registers
const SC16IS750_REG_DLL = 0x00;
const SC16IS750_REG_DLH = 0X01;

//Enhanced Registers
const SC16IS750_REG_EFR = 0X02;
const SC16IS750_REG_XON1 = 0X04;
const SC16IS750_REG_XON2 = 0X05;
const SC16IS750_REG_XOFF1 = 0X06;
const SC16IS750_REG_XOFF2 = 0X07;

//
const SC16IS750_INT_CTS = 0X80;
const SC16IS750_INT_RTS = 0X40;
const SC16IS750_INT_XOFF = 0X20;
const SC16IS750_INT_SLEEP = 0X10;
const SC16IS750_INT_MODEM = 0X08;
const SC16IS750_INT_LINE = 0X04;
const SC16IS750_INT_THR = 0X02;
const SC16IS750_INT_RHR = 0X01;

//Application Related

const SC16IS750_CRYSTCAL_FREQ = 14745600;
const SC16IS750_PROTOCOL_I2C = 0;
const SC16IS750_PROTOCOL_SPI = 1;


var i2cDevice = new mraa.I2c(0);
i2cDevice.address(SC16IS750_ADDRESS);

var ppm = 0;

var peek_flag = 0;


function SC16IS750_flush() {
  var tmp_lsr;

  do {
    tmp_lsr = SC16IS750_ReadRegister(SC16IS750_REG_LSR);
  } while ((tmp_lsr & 0x20) == 0);

}

function SC16IS750_peek() {
  if (peek_flag == 0) {
    peek_buf = SC16IS750_ReadByte();
    if (peek_buf >= 0) {
      peek_flag = 1;
    }
  }
  return peek_buf;
}

function SC16IS750_ReadByte() {
  if (SC16IS750_FIFOAvailableData() == 0) {
    return -1;
  } else {
    return SC16IS750_ReadRegister(SC16IS750_REG_RHR);
  }
}

function SC16IS750_WriteByte(val) {
  var tmp_lsr;
  /* do {
     datelog("SC16IS750_WriteByte waiting for LSR ready");
     tmp_lsr = SC16IS750_ReadRegister(SC16IS750_REG_LSR);
   } while ((tmp_lsr & 0x20) == 0);
 
   datelog("SC16IS750_WriteByte "+val);*/
  SC16IS750_WriteRegister(SC16IS750_REG_THR, val);
}

function SC16IS750_FIFOAvailableData() {
  return SC16IS750_ReadRegister(SC16IS750_REG_RXLVL);
}

function SC16IS750_FIFOEnable(fifo_enable) {
  var temp_fcr;

  temp_fcr = SC16IS750_ReadRegister(SC16IS750_REG_FCR);

  if (fifo_enable == 0) {
    temp_fcr &= 0xFE;
  } else {
    temp_fcr |= 0x01;
  }
  SC16IS750_WriteRegister(SC16IS750_REG_FCR, temp_fcr);

  return;
}

function SC16IS750_ResetDevice() {
  var reg;

  reg = SC16IS750_ReadRegister(SC16IS750_REG_IOCONTROL);
  reg |= 0x08;
  SC16IS750_WriteRegister(SC16IS750_REG_IOCONTROL, reg);

  return;
}

function SC16IS750_GPIOGetPinState(pin_number) {
  var temp_iostate;

  temp_iostate = SC16IS750_ReadRegister(SC16IS750_REG_IOSTATE);
  if (temp_iostate & (0x01 << pin_number) == 0) {
    return 0;
  }
  return 1;
}

function SC16IS750_GPIOSetPinState(pin_number, pin_state) {
  var temp_iostate;

  temp_iostate = SC16IS750_ReadRegister(SC16IS750_REG_IOSTATE);
  if (pin_state == 1) {
    temp_iostate |= (0x01 << pin_number);
  } else {
    temp_iostate &= ~(0x01 << pin_number);
  }

  SC16IS750_WriteRegister(SC16IS750_REG_IOSTATE, temp_iostate);
  return;
}

function SC16IS750_GPIOSetPinMode(pin_number, i_o) {
  var temp_iodir;

  temp_iodir = SC16IS750_ReadRegister(SC16IS750_REG_IODIR);
  if (i_o == OUTPUT) {
    temp_iodir |= (0x01 << pin_number);
  } else {
    temp_iodir &= ~(0x01 << pin_number);
  }

  SC16IS750_WriteRegister(SC16IS750_REG_IODIR, temp_iodir);
  return;
}

function SC16IS750_available() {
  return SC16IS750_FIFOAvailableData();
}

function SC16IS750_read() {
  if (peek_flag == 0) {
    return SC16IS750_ReadByte();
  } else {
    peek_flag = 0;
    return peek_buf;
  }
}

function SC16IS750_write(val) {
  SC16IS750_WriteByte(val);
}

function SC16IS750_pinMode(pin, i_o) {
  SC16IS750_GPIOSetPinMode(pin, i_o);
}

function SC16IS750_digitalWrite(pin, value) {
  SC16IS750_GPIOSetPinState(pin, value);
}

function SC16IS750_digitalRead(pin) {
  return SC16IS750_GPIOGetPinState(pin);
}


function SC16IS750_ReadRegister(reg_addr) {
  var result;

  var ret = 0;
  result = i2cDevice.readReg((reg_addr << 3));
  return result;
}

function SC16IS750_WriteRegister(reg_addr, val) {
  i2cDevice.writeReg((reg_addr << 3), val);
  return;
}

function SC16IS750_SetBaudrate(baudrate) //return error of baudrate parts per thousand
{
  var divisor;
  var temp_lcr;

  divisor = (SC16IS750_CRYSTCAL_FREQ) / (baudrate * 16);

  temp_lcr = SC16IS750_ReadRegister(SC16IS750_REG_LCR);
  temp_lcr |= 0x80;
  SC16IS750_WriteRegister(SC16IS750_REG_LCR, temp_lcr);
  //write to DLL
  SC16IS750_WriteRegister(SC16IS750_REG_DLL, divisor);
  //write to DLH
  SC16IS750_WriteRegister(SC16IS750_REG_DLH, (divisor >> 8));
  temp_lcr &= 0x7F;
  SC16IS750_WriteRegister(SC16IS750_REG_LCR, temp_lcr);

}

function SC16IS750_SetLine() {
  var temp_lcr;
  temp_lcr = SC16IS750_ReadRegister(SC16IS750_REG_LCR);
  temp_lcr &= 0xC0; //Clear the lower six bit of LCR (LCR[0] to LCR[5]
  temp_lcr |= 0x03;
  SC16IS750_WriteRegister(SC16IS750_REG_LCR, temp_lcr);
}


SC16IS750_ResetDevice();
SC16IS750_FIFOEnable(1);
SC16IS750_SetBaudrate(9600);
SC16IS750_SetLine();

function CO2_measure() {

  //datelog("CO2_measure");

  //ask co2 sensor to prepare a measurment
  SC16IS750_write(0xFF);
  SC16IS750_write(0x01);
  SC16IS750_write(0x86);
  SC16IS750_write(0x00);
  SC16IS750_write(0x00);
  SC16IS750_write(0x00);
  SC16IS750_write(0x00);
  SC16IS750_write(0x00);
  SC16IS750_write(0x79);

  //datelog("CO2_measure cmd sent");

  var i = 0;
  var buf = new Buffer(9);
  var av;
  var timeoutTO = 0;
  //datelog("CO2_measure update");
  while (i < 9) {
    timeoutTO++;
    if (av = SC16IS750_available()) {
      buf[i] = SC16IS750_read();
      //datelog(i+" = "+buf[i]+" = ");
      if (i == 0 && buf[i] != 255) {
        // datelog("/");
        continue;
      } else {
        i++;
        //datelog(".");
      }
    }
    if (timeoutTO == 1000) 
    {
      _send_scheduled();
      datelog("timeout");
      _co2_supply_off(); //for security reason
      return;
    }
  }


  //  datelog("CO2_measure - "+buf[1]);
  if (buf[1] == 134) {
    ppm = buf[2] << 8 | buf[3];
    _send_scheduled();
    _send_data(ppm);
    datelog("CO2_measure --> " + ppm);
    if(ppm > 3990) _co2_supply_off(); //for security reason

    //temperature = buf[4] - 40;
    return;

    return true;
  } else {
    _send_scheduled();
    datelog("CO2_measure bad buf");
    return false;
  }
}

var DEVICE_ID = null;

function _send_data(value) {
  if (sendDataEnabled && DEVICE_ID) {
    var msg2send = "{}";
    var sensorValue = value;

    if (isNaN(sensorValue) || Number(sensorValue) == 0) {
      datelog(sensorValue + "is not a valid CO2 number , exit");
      return;
    }

    msg2send = JSON.stringify({
      device_id: DEVICE_ID.toString(),
      sensor_type: "air_co2",
      sensor_value: sensorValue.toString()
    });

    datelog(msg2send);
    local_service.emit("data", msg2send);
  }
}

function _send_scheduled() {
  local_service.emit("scheduled", "air_co2");    
}

function _co2_supply_off() {
  if (sendDataEnabled && DEVICE_ID) {
    var msg2send = "{}";
    var sensorValue = 0;

    msg2send = JSON.stringify({
      device_id: DEVICE_ID.toString(),
      sensor_type: "relay1",
      sensor_value: sensorValue.toString()
    });

    datelog(msg2send);
    local_service.emit("data", msg2send);
  }
}

var local_service = client("http://127.0.0.1:" + local_service_PORT);
var sendDataEnabled = false;
local_service.on('connect', function () {

  datelog(" * CO2 is connected");
  // The lib will create a client connection to the local_service, namely
  // this url : localhost:local_service_PORT - it does not need any info

});


local_service.on('disconnect', (reason) => {
  datelog(" * CO2 is disconnected :" + reason);
  sendDataEnabled = false;
});


local_service.on('device_id', function (data) {
  datelog(data);
  DEVICE_ID = data;
  local_service.emit('io', "air_co2");

  sendDataEnabled = true;

});

local_service.on('scheduler', function (data) {
//  datelog(data);
  if(data == 'air_co2')
  {
    datelog("scheduled");
    CO2_measure();
  }
  //else datelog("not scheduled :"+data);
});
/*
var mcu_loop = setInterval(function () {

  CO2_measure();

}, 2000);*/
