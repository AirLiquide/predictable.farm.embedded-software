
const VERSION = 1;
const local_service_PORT = 64000;

const client = require('/usr/lib/node_modules/socket.io-client');
const mraa = require('/usr/lib/node_modules/mraa'); //require mraa

console.log('MRAA Version: ' + mraa.getVersion()); //write the mraa version to the console

const fs = require("fs");
var util = require('util');
var log_file = fs.createWriteStream(__dirname + '/relay.log', {flags : 'a'});
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
    console.log(Date.now() + ": " + message);
}


const RELAY_MODE_AUTO = "0";
const RELAY_MODE_MANUAL = "1";

const MODE_LOCAL = 0;
const MODE_REMOTE = 1;

const NB_RELAY = 4;
var relay_state = ["0", "0", "0", "0"];
var relay_mode = ["0", "0", "0", "0"];
var r1 = new mraa.Gpio(8);
var r2 = new mraa.Gpio(9);
var r3 = new mraa.Gpio(10);
var r4 = new mraa.Gpio(11);
var relay = [r1, r2, r3, r4]; //setup digital read on pin 8
var DEVICE_ID = 0;

datelog(" * init relay :");
relay.forEach((r) => {
    r.dir(mraa.DIR_OUT);
    r.write(0);
});

var local_service = client("http://127.0.0.1:" + local_service_PORT);

local_service.on('connect', function () {

    datelog(" * RELAY is connected");
    // The lib will create a client connection to the local_service, namely
    // this url : localhost:local_service_PORT - it does not need any info

});
local_service.on('device_id', function (data) {
    datelog(data);
    DEVICE_ID = data;
    local_service.emit('io', "relay");
    var mcu_loop = setInterval(function () {
        for (i = 0; i < NB_RELAY; i++) {

            var msg2send = "{}";
            var sensorValue = relay_state[i];
            var sensorMode = relay_mode[i];

            msg2send = JSON.stringify({
                device_id: DEVICE_ID.toString(),
                sensor_type: "relay" + (i + 1),
                sensor_value: sensorValue.toString(),
                sensor_mode: sensorMode.toString(),
            });

            datelog(msg2send);
            local_service.emit("data", msg2send);

        }
    }, 3000);
});

local_service.on('relay', function (data) {
    datelog(data);
    for (i = 0; i < NB_RELAY; i++) {
        if (isRelay((i + 1), data) == true) {
            var mode = getRelayMode(data);
            if(mode < 0) // command is comming from automation engine
            {
                if(relay_mode[i] == RELAY_MODE_MANUAL)
                {
                    datelog("relay" + (i + 1) + ": automation engine command discarded as relay is in manual mode" );
                    return;
                }
            }
            relay_state[i] = getRelayState(data);
            relay_mode[i] = mode;
            var gpio = 8 + (i);
            //datelog("i found :" + i);
            datelog("relay" + (i + 1) + ": set to " + relay_state[i] + " mode = "+ relay_mode[i] );
            relay[i].write(relay_state[i]); 
        } /*else {
            datelog("i:" + (i) + ":  " + relay_state[i]);
        }*/
    }
});

function isRelay(relayNumber, command) {
    try {
        var cmd = JSON.parse(command);
    } catch (e) {
        datelog("isRelay   Bad JSON, ignoring");
        datelog(cmd);
        return false; // console.error(e);
    }
    if ((cmd.sensor_type.slice(0, -1) == "relay") && (Number(cmd.sensor_type.split("relay").pop()) == Number(relayNumber))) {
        return true;
    } else {
        datelog(cmd.sensor_type.slice(0, -1) + "---" + Number(cmd.sensor_type.split("relay").pop()));
        return false;
    }

}

function getRelayState(command) {
    try {
        var cmd = JSON.parse(command);
    } catch (e) {
        datelog("getRelayState   Bad JSON, ignoring");
        datelog(cmd);
        return -1; // console.error(e);
    }
    //datelog(command+" --- state = "+Number(cmd.sensor_value)+ " raw ="+cmd.sensor_value);
        
    return Number(cmd.sensor_value);

}

function getRelayMode(command) {
    try {
        var cmd = JSON.parse(command);
    } catch (e) {
        datelog("getRelayMode   Bad JSON, ignoring");
        datelog(cmd);
        return -1; // console.error(e);
    }
    if (cmd.hasOwnProperty('sensor_mode')) {
        var mode = Number(cmd.sensor_mode);
        //datelog(command+" --- mode = "+Number(cmd.sensor_mode)+ " raw ="+cmd.sensor_mode);
        return mode;
    } else return -1;
}