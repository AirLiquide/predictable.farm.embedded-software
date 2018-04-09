const VERSION = 1;
const local_service_PORT = 64000;

const client = require('/usr/lib/node_modules/socket.io-client');
const mraa = require('/usr/lib/node_modules/mraa'); //require mraa

console.log('MRAA Version: ' + mraa.getVersion()); //write the mraa version to the console

const fs = require("fs");
var util = require('util');
var log_file = fs.createWriteStream(__dirname + '/co2.log', { flags: 'a' });
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
		log_file.write(timestamp + " - " + util.format.apply(null, arguments) + '\n');
	}
};

function datelog(message) {
	console.log(message);
}

function sleep(delay) {
	delay += new Date().getTime();
	while (new Date() < delay) { }
}


var uart = new mraa.Uart(0);

uart.setBaudRate(9600);
uart.setMode(8, 0, 1);
uart.setFlowcontrol(false, false);
sleep(200);


var ppm = 0;

const cmd_calibrateZero = [0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78];

function CO2_calibrate() {

	var cmd_calibrateZero = new Buffer("FF0187000000000078", 'hex');
	var i = 0;
	uart.flush();
	uart.write(cmd_calibrateZero);
	sleep(200);

	datelog("CO2_calibrate ");
}

function CO2_measurement_error(error) {
	_send_scheduled();
	datelog(error);
	_co2_supply_off(); //for security reason
}

function CO2_measure() {

	var i = 0;

	var cmd_measure = new Buffer("FF019C000000000063", 'hex');

	//datelog("CO2_measure");

	//ask co2 sensor to prepare a measurment
	uart.flush();
	uart.write(cmd_measure);

	sleep(200);
	/*
	for (i = 0; i < 9; i++) {
		uart.write(cmd_measure[i]);
	}*/
	//datelog("CO2_measure cmd sent");

	var buf = new Buffer(9);

	//datelog("CO2_measure update");
	if (uart.dataAvailable(400)) {
		buf = uart.read(9);

	//	datelog("CO2_measure - " + buf.toString('hex'));
		//if (buf[1] == 134) 
		{
			var checksum = new Buffer("00", 'hex');

			for (i = 0; i < 9; i++) {
				checksum[0] += buf[i];
			}

			if (buf[0] == 0xFF && buf[1] == 0x9C) {
				if (checksum[0] == 0xFF) {
					ppm = buf[2] << 24 | buf[3] << 16 | buf[4] << 8 | buf[5];
					if (ppm > 3990) CO2_measurement_error("too high !");
					else {
						_send_scheduled();
						_send_data(ppm);
						datelog("CO2_measure --> " + ppm);
						//temperature = buf[4] - 40;
					}
					return true;
				} else {
					CO2_measurement_error("bad checksum :" + checksum.toString('hex'));
					return false;
				}
			}
			else {
				CO2_measurement_error("bad buf");
				return false;
			}
		}
		/* else {
			CO2_measurement_error("bad buf");
			return false;
		}*/
	} else {
		CO2_measurement_error("timeout");
		return;
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
	if (data == 'air_co2') {
		//datelog("scheduled");
		CO2_measure();
	}
	//else datelog("not scheduled :"+data);
});
/*
var mcu_loop = setInterval(function () {

	CO2_measure();

}, 2000);*/
