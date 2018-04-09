const VERSION = 1;
const local_service_PORT = 64000;

const client = require('/usr/lib/node_modules/socket.io-client');
const mraa = require('/usr/lib/node_modules/mraa'); //require mraa

console.log('MRAA Version: ' + mraa.getVersion()); //write the mraa version to the console


const fs = require("fs");
var util = require('util');
var log_file = fs.createWriteStream(__dirname + '/rh.log', {flags : 'a'});
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

var temp_unit_celcius = true;
const SHT3X_I2C_ADDRESS = 0x44; 
const SENSOR_div = 65535; // 2^16 -1
const RH_mult = 100;
const TcShift = -45;
const TfShift = -49;
const TcMult = 175;
const TfMult = 315;

var mTemperature;
var mHumidity;
var DEVICE_ID = null;

var i2cDevice = new mraa.I2c(0);
i2cDevice.address(SHT3X_I2C_ADDRESS);

var sendDataEnabled = false;

function sleep(delay) {
	delay += new Date().getTime();
	while (new Date() < delay) { }
}

function _send_data(value) {
  if (sendDataEnabled && DEVICE_ID) {
    var msg2send = "{}";
    var sensorValue = value;

    if (isNaN(sensorValue) || Number(sensorValue) == 0) {
      datelog(sensorValue + "is not a valid RH number , exit");
      return;
    }

    msg2send = JSON.stringify({
      device_id: DEVICE_ID.toString(),
      sensor_type: "air_humidity",
      sensor_value: sensorValue.toString()
    });

    datelog(msg2send);
    local_service.emit("data", msg2send);
  }
}

function _send_scheduled() {
  local_service.emit("scheduled", "air_humidity");    
}

function periodic_data_acquisition (startStop)
{
  i2cDevice.writeReg(0x30,0x66); //stop internal heater
  if(startStop == "start") i2cDevice.writeReg(0x20,0x24); //periodic data acquisition every 500ms // medium
  else i2cDevice.writeReg(0x30,0x93); //stop periodic data acquisition
}
periodic_data_acquisition ("start");

function fetch_data()
{
  var data = null;
  i2cDevice.writeReg(0xE0,0x00); //fetch data
  try {
    //var s = get_status();
    //datelog("status :"+s);
    data= i2cDevice.read(6);
  }
  catch(err) {
    //datelog("measurement not ready :"+err);
  }

  return data;
}

function get_status()
{
  i2cDevice.writeReg(0xf3,0x2d);
  return i2cDevice.read(2);
}

function one_shot_data()
{
  i2cDevice.writeReg(0x2c,0x0d); // singleshot measurement clock stretching
  //i2cDevice.writeReg(0x24,0x0d); // singleshot measurement  no clk stretch
  return  i2cDevice.read(6);
}

function RH_measure()
{
  var data = fetch_data();

  _send_scheduled();
  
  if (!data) {
    datelog("no data");
    periodic_data_acquisition("stop");
    sleep(500);
    periodic_data_acquisition("start");
    return false;
  }

  // convert to Temperature/Humidity
  var val;
  val = (data[0] << 8) + data[1];
  if(temp_unit_celcius) mTemperature = TcShift + TcMult * (val / SENSOR_div);
  else mTemperature = TfShift + TfMult * (val / SENSOR_div);
  mTemperature = mTemperature.toFixed(1);

  val = (data[3] << 8) + data[4];
  mHumidity = RH_mult * (val / SENSOR_div);
  mHumidity = mHumidity.toFixed(1);

  //datelog("temp:"+mTemperature);
  datelog("rh:"+mHumidity);
  _send_data(mHumidity);

  return true;
}


var local_service = client("http://127.0.0.1:" + local_service_PORT);

local_service.on('connect', function () {

    datelog(" * RelativeHumidity is connected");
    periodic_data_acquisition("start");
    
});


local_service.on('disconnect', (reason) => {
  datelog(" * RelativeHumidity is disconnected :" + reason);
  sendDataEnabled = false;
  periodic_data_acquisition("stop");
});


local_service.on('device_id', function (data) {
    datelog(data);
    DEVICE_ID = data;
    local_service.emit('io', "air_humidity");
    sendDataEnabled = true;
    
});

local_service.on('scheduler', function (data) {
  //datelog(data);
  if(data == 'air_humidity')
  {
    RH_measure();
  }
});

/*
var mcu_loop = setInterval(function () {

  RH_measure();

}, 5000);
*/