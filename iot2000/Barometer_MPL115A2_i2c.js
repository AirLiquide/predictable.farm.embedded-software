
const VERSION = 1;
const local_service_PORT = 64000;

const client = require('/usr/lib/node_modules/socket.io-client');
const mraa = require('/usr/lib/node_modules/mraa'); //require mraa

console.log('MRAA Version: ' + mraa.getVersion()); //write the mraa version to the console

const fs = require("fs");
var util = require('util');
var log_file = fs.createWriteStream(__dirname + '/barometer.log', {flags : 'a'});
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
            args[0] = "%o: " + arguments[0];
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

function sleep(delay) {
	delay += new Date().getTime();
	while (new Date() < delay) { }
}


/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
const MPL115A2_ADDRESS = 0x60;    // 1100000
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
const MPL115A2_REGISTER_PRESSURE_MSB = 0x00;
const MPL115A2_REGISTER_PRESSURE_LSB = 0x01;
const MPL115A2_REGISTER_TEMP_MSB = 0x02;
const MPL115A2_REGISTER_TEMP_LSB = 0x03;
const MPL115A2_REGISTER_A0_COEFF_MSB = 0x04;
const MPL115A2_REGISTER_A0_COEFF_LSB = 0x05;
const MPL115A2_REGISTER_B1_COEFF_MSB = 0x06;
const MPL115A2_REGISTER_B1_COEFF_LSB = 0x07;
const MPL115A2_REGISTER_B2_COEFF_MSB = 0x08;
const MPL115A2_REGISTER_B2_COEFF_LSB = 0x09;
const MPL115A2_REGISTER_C12_COEFF_MSB = 0x0A;
const MPL115A2_REGISTER_C12_COEFF_LSB = 0x0B;
const MPL115A2_REGISTER_STARTCONVERSION = 0x12;
/*=========================================================================*/

var i2cDevice = new mraa.I2c(0);
i2cDevice.address(MPL115A2_ADDRESS);



var _mpl115a2_a0 = 0.0;
var _mpl115a2_b1 = 0.0;
var _mpl115a2_b2 = 0.0;
var _mpl115a2_c12 = 0.0;


/**************************************************************************/
/*!
    @brief  Gets the factory-set coefficients for this particular sensor
*/
/**************************************************************************/
function Barometer_readCoefficients() {
    var a0coeff;
    var b1coeff;
    var b2coeff;
    var c12coeff;

    a0coeff = i2cDevice.readWordReg(MPL115A2_REGISTER_A0_COEFF_MSB);
    b1coeff = i2cDevice.readWordReg(MPL115A2_REGISTER_B1_COEFF_MSB);
    b2coeff = i2cDevice.readWordReg(MPL115A2_REGISTER_B2_COEFF_MSB);
    c12coeff = i2cDevice.readWordReg(MPL115A2_REGISTER_C12_COEFF_MSB);


    _mpl115a2_a0 = Number(a0coeff).toFixed(8) / 8;
    _mpl115a2_b1 = Number(b1coeff).toFixed(8) / 8192;
    _mpl115a2_b2 = Number(b2coeff).toFixed(8) / 16384;
    _mpl115a2_c12 = Number(c12coeff).toFixed(8);
    _mpl115a2_c12 /= 4194304.0;
}


var pressurekPa;
var tempCelcius;
var tempRAW = 0;

function getP() {
    var pressureRAW;
    var pressureComp;

    //pressureRAW = (i2cDevice.readWordReg(MPL115A2_REGISTER_PRESSURE_MSB) >> 6);

    pressureRAW = (i2cDevice.readReg(MPL115A2_REGISTER_PRESSURE_MSB) << 8);
    pressureRAW |= i2cDevice.readReg(MPL115A2_REGISTER_PRESSURE_LSB);
    pressureRAW = pressureRAW >> 6;

    // See datasheet p.6 for evaluation sequence
    pressureComp = _mpl115a2_a0 + (_mpl115a2_b1 + _mpl115a2_c12 * tempRAW) * pressureRAW + _mpl115a2_b2 * tempRAW;

    // Return pressure and temperature as floating point values
    pressurekPa = ((65.0 / 1023.0) * pressureComp) + 50.0;        // kPa
    return pressurekPa.toFixed(1);
}
function Barometer_getPressure() {

    i2cDevice.writeReg(MPL115A2_REGISTER_STARTCONVERSION, 0x00);
    sleep(10);
    var p = getP();
    if(p < 600 ) // meaning atmospheric pressure @ 4000m above see we don't send it , sensor may have an issue
        send_pressure(p);    
}

function getT() {
    tempRAW = (i2cDevice.readReg(MPL115A2_REGISTER_TEMP_MSB) << 8);
    tempRAW |= i2cDevice.readReg(MPL115A2_REGISTER_TEMP_LSB);
    tempRAW = tempRAW >> 6;

    //convert
    tempCelcius = (Number(tempRAW).toFixed(2) - 498.0) / -5.35 + 25.0;          // C
    return tempCelcius.toFixed(1);
}

function Barometer_getTemperature() {
    var tempCelcius;
    var tempRAW;
    i2cDevice.writeReg(MPL115A2_REGISTER_STARTCONVERSION, 0x00);
    sleep(10);
    var t = getT();
    send_temperature(t);
}

Barometer_readCoefficients();

var sendDataEnabled = false;
var DEVICE_ID = null;
var local_service = client("http://127.0.0.1:" + local_service_PORT);

local_service.on('connect', function () {

    datelog(" * Barometer is connected");
    // The lib will create a client connection to the local_service, namely
    // this url : localhost:local_service_PORT - it does not need any info

});

local_service.on('disconnect', (reason) => {
    datelog(" * Barometer is disconnected :" + reason);
    sendDataEnabled = false;
});

function _send_data(type, value) {
    if (sendDataEnabled && DEVICE_ID) {
        var msg2send = "{}";
        var sensorValue = value;

        if (isNaN(sensorValue) || Number(sensorValue) == 0) {
            datelog(sensorValue + " is not a valid " + type + " , exit");
            return;
        }

        msg2send = JSON.stringify({
            device_id: DEVICE_ID.toString(),
            sensor_type: type,
            sensor_value: sensorValue.toString()
        });

        datelog(msg2send);
        local_service.emit("data", msg2send);
    }
}
function _send_scheduled() {
  local_service.emit("scheduled", "air_pressure");    
}

function send_pressure(pressure) {
    _send_data("air_pressure", pressure);
}

function send_temperature(temp) {
    _send_data("air_temperature", temp);
}

local_service.on('device_id', function (data) {
    datelog(data);
    DEVICE_ID = data;
    local_service.emit('io', "air_pressure");
    local_service.emit('io', "air_temperature");

    sendDataEnabled = true;

});

local_service.on('scheduler', function (data) {
    //datelog(data);
    if(data == 'air_pressure')
    {
        Barometer_getPressure();
        Barometer_getTemperature();
        _send_scheduled();
    }
  });
/*
var loop = setInterval(function () {

    Barometer_getPressure();

}, 5000);*/