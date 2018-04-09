
const VERSION = 1;
const local_service_PORT = 64000;

const client = require('/usr/lib/node_modules/socket.io-client');

const adc = require(__dirname + '/lib/ADS1015.js');
var DEVICE_ID = null;
var sendDataEnabled = false;

const fs = require("fs");
var util = require('util');
var log_file = fs.createWriteStream(__dirname + '/par.log', {flags : 'a'});
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

function _send_data(value) {
    if (sendDataEnabled && DEVICE_ID) {
        //convert
        light_par = Number(value) * 0.0078125 * 99.7;

        var msg2send = "{}";
        var sensorValue = Number(light_par).toFixed(0);
        var msg2send = "{}";
        var sensorValue = value;

        if (isNaN(sensorValue) || Number(sensorValue) < 0) {
            datelog(sensorValue + " is not a valid light_par , exit");
            return;
        }

        msg2send = JSON.stringify({
            device_id: DEVICE_ID.toString(),
            sensor_type: "light_par",
            sensor_value: sensorValue.toString()
        });

        datelog(msg2send);
        local_service.emit("data", msg2send);
    }
}
function _send_scheduled() {
    local_service.emit("scheduled", "light_par");    
}

var local_service = client("http://127.0.0.1:" + local_service_PORT);

local_service.on('connect', function () {

    datelog(" * PAR is connected");
    // The lib will create a client connection to the local_service, namely
    // this url : localhost:local_service_PORT - it does not need any info

});

local_service.on('disconnect', (reason) => {
    datelog(" * Barometer is disconnected :" + reason);
    sendDataEnabled = false;
});

local_service.on('device_id', function (data) {
    datelog(data);
    DEVICE_ID = data;
    local_service.emit('io', "light_par");
    sendDataEnabled = true;

});

function _dataReadCallback(val)
{
    _send_scheduled();
    _send_data(val);
}

local_service.on('scheduler', function (data) {
    //datelog(data);
    if(data == 'light_par')
    {
      //retrieve raw value from ADC 
        adc.readChannel(3, _dataReadCallback);
    }
});

  /*
var mcu_loop = setInterval(function () {
    //retrieve raw value from ADC 
    adc.readChannel(3, _send_data);
}, 3000);*/

