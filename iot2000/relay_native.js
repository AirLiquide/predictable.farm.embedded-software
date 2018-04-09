
const VERSION = 1;
const local_service_PORT = 64000;

const client = require('/usr/lib/node_modules/socket.io-client');
const mraa = require('/usr/lib/node_modules/mraa'); //require mraa

console.log('MRAA Version: ' + mraa.getVersion()); //write the mraa version to the console


const fs = require("fs");
var util = require('util');
var log_file = fs.createWriteStream(__dirname + '/relay_native.log', {flags : 'a'});
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
    console.log( message);
}


const RELAY_MODE_AUTO = "0";
const RELAY_MODE_MANUAL = "1";

const MODE_LOCAL = 0;
const MODE_REMOTE = 1;

var relay_state = "0";
var relay_mode = "0";
var relay = new mraa.Gpio(8);//setup digital read on pin 8
var extBtn = new mraa.Gpio(7);//setup digital read on pin 8

var DEVICE_ID = null;

datelog(" * init relay ");
relay.dir(mraa.DIR_OUT);
relay.write(0);

extBtn.dir(mraa.DIR_IN);

var sendDataEnabled = false;
var local_service = client("http://127.0.0.1:" + local_service_PORT);

local_service.on('connect', function () {

    datelog(" * RELAY NATIVE is connected");
    // The lib will create a client connection to the local_service, namely
    // this url : localhost:local_service_PORT - it does not need any info
});

local_service.on('disconnect', (reason) => {
    datelog(" * RELAY NATIVE  is disconnected :" + reason);
    sendDataEnabled = false;
});



local_service.on('device_id', function (data) {
    datelog(data);
    DEVICE_ID = data;
    local_service.emit('io', "relay");
    sendDataEnabled = true;

});

function _send_data() {
    if (sendDataEnabled && DEVICE_ID) {

            var msg2send = "{}";
            var sensorValue = relay_state;
            var sensorMode = relay_mode;

            msg2send = JSON.stringify({
                device_id: DEVICE_ID.toString(),
                sensor_type: "relay1",
                sensor_value: sensorValue.toString(),
                sensor_mode: sensorMode.toString(),
            });

            datelog("send :" + msg2send);
            local_service.emit("data", msg2send);
    }
}


function _send_scheduled() {
    local_service.emit("scheduled", "relay");   
}

function isUserButtonPressed()
{
    var contents = fs.readFileSync('/sys/class/gpio/gpio63/value', 'utf8');
    var value = parseInt(contents.toString()) ? 0 : 1;
    if(value == 1) return true;
    else return false;
}

function isExtBtnButtonPressed()
{
    var value = parseInt(extBtn.read()) ? 0 : 1;
    if(value == 1) return true;
    else return false;
}

function toggleRelay()
{
    //datelog("relay : set to " + relay_state+ " mode = " + relay_mode);
    relay.write(Number(relay_state));
}

var loop = 0;
var btnState = 0;
//var mcu_loop = setInterval(function () {
local_service.on('scheduler', function (data) {
        //datelog(data);
        if(data == 'relay')
        {
         // datelog("scheduled");
    if (loop == 50) { 
        _send_data(); // send periodically relay state outside of change events
        loop = 0;
    }
    else {
        loop++;
        if(isUserButtonPressed() || isExtBtnButtonPressed())
        {
            //datelog("btn down");
            btnState = 1;           
        }
        else
        {
            //datelog("btn up");
            if (btnState)
            {
                datelog("btn click");
                btnState = 0;
                relay_mode = RELAY_MODE_MANUAL; // force manual mode
                if(relay_state == "0") relay_state = "1";
                else relay_state = "0";
            }
        }
    }
    toggleRelay();

    _send_scheduled();
    }
//else datelog("not scheduled :"+data);
});
//}, 100);
var auto_off = null;
local_service.on('relay', function (data) {
    datelog("relay command from cloud : "+data);
        if (isRelay(1, data) == true) {
            var rstate = getRelayState(data);
            if(rstate != relay_state)  _send_data(); // change broadcast
            var mode = getRelayMode(data);
            if (mode < 0) // command is comming from automation engine
            {
                if (relay_mode == RELAY_MODE_MANUAL) {
                    datelog("relay : automation engine command discarded as relay is in manual mode");
                    return;
                }
                else //auto mode
                {
                    relay_state = rstate;
                    if(relay_state == 1) // if auto mode set relay to on, set it back to off automatically after 5 sec
                    {
                        if(!auto_off)  auto_off = setTimeout(function () {
                            relay_state = 0;   
                            datelog("auto off");   
                            auto_off = null;         
                            _send_data();  //auto off change broadcast      
                        }, 3000);
                    }
                }
            }    
            else // manual command from dashboard
            {
                relay_state = rstate;
            }     
            relay_mode = mode;

            if (relay_mode == RELAY_MODE_MANUAL) {
                if(auto_off) 
                {
                    clearTimeout(auto_off);   
                    datelog("auto off canceled");  
                }
            }
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