/* Version : 5.0*/

// /!\ We do not 'use strict' mode here since Node 0.10 does not enable const / let by default.

const VERSION = 5;

const DEVICE_ID = 5;

const SERIAL_PORT = "/dev/ttyATH0"; /*This is the default serial port used internally by the arduino Yun*/
const SERIAL_BAUDRATE = 115200;

const SERVER_URL = "http://lafactory.predictable.zone";
const UPDATE_ENDPOINT = "http://update.predictablefarm.net/";

/* ---------------------------------------------------------------------- */

function datelog(message) {
    console.log(Date.now() + ": " + message);
}

const STATUS_REQUEST = "s";
const INFO_MESSAGE   = "m";

const OS_READY       = "o1"; //"z0";
const OS_DOWN        = "o0"; //"z1";
const DEV_PREFIX     = "i";
const NETWORK_DOWN   = "n0"; //"y0";
const NETWORK_UP     = "n1"; //"y1";
const SOCKET_UP      = "n2";
const ACK            = "ACK";

const SENSOR_TYPES = [
    "relay1",
    "relay2",
    "relay3",
    "relay4",
    "air_pressure",
    "air_co2",
    "air_co",
    "air_temperature",
    "air_humidity",
    "light_par",
    "light_lux",
    "light_uv",
    "water_temperature",
    "water_ph",
    "water_ec",
    "water_orp",
    "water_do",
    "moisture",
    "soil_temperature",
    "water_level"
];

//const MutexPromise = require('mutex-promise');
const fs = require("fs");
const exec = require('child_process').exec;
const serialport = require('/usr/lib/node_modules/serialport');
const SerialPort = serialport.SerialPort;

const client = require('/usr/lib/node_modules/socket.io-client');

var networkStatus = {
    socket: false,
    network: false
};

var receivedData = false;
var mcu_ready = false;
var updateLock = false;

var dataTimer = null; // timer in between two sets of data sent to the server
var rebootTimer = null; // timer before rebooting after a network down event

var relay1_state = "0";
var relay2_state = "0";
var relay3_state = "0";
var relay4_state = "0";
var relay1_mode = "0";
var relay2_mode = "0";
var relay3_mode = "0";
var relay4_mode = "0";
var last_server_command = "";
var air_pressure = -1.0;
var air_co2 = -1.0;
var air_co = -1.0;
var air_temperature = -1.0;
var air_humidity = -1.0;
var light_par = -1.0;
var light_lux = -1.0;
var light_uv = -1.0;
var water_temperature = -1.0;
var water_ph = -1.0;
var water_ec = -1.0;
var water_orp = -1.0;
var water_do = -1.0;
var moisture = -1.0;
var soil_temperature = -1.0;
var water_level = -1.0;
var air_pressure_cpt = 0;
var air_co2_cpt = 0;
var air_co_cpt = 0;
var air_temperature_cpt = 0;
var air_humidity_cpt = 0;
var light_par_cpt = 0;
var light_lux_cpt = 0;
var light_uv_cpt = 0;
var water_temperature_cpt = 0;
var water_ph_cpt = 0;
var water_ec_cpt = 0;
var water_orp_cpt = 0;
var water_do_cpt = 0;
var moisture_cpt = 0;
var soil_temperature_cpt = 0;
var water_level_cpt = 0;

const ENDL = '\n';

datelog("Starting app v" + VERSION + "...");
//var mutex = new MutexPromise('sync_write');

function MCU_write(data)
{
   /* mutex.lock();
    
    // Get a promise that resolves when mutex is unlocked or expired
    mutex.promise().then(function(mutex){ return arduino.write(data); });
    
    // Unlock the mutex
    mutex.unlock();
*/
    arduino.write(data);
} 


datelog("Starting app ...");

datelog(" 1. Opening serial port " + SERIAL_PORT);
var arduino = new SerialPort(SERIAL_PORT, {
    baudrate: SERIAL_BAUDRATE,
    parser: serialport.parsers.readline("\r\n"),
});

datelog(" 2. Opening socket to " + SERVER_URL);
var socket = client(
        SERVER_URL,
        {
            path: '/socket/socket.io',
            query: 'role=sensor&sensorId=' + DEVICE_ID,
            pingInterval: 5000,
            pingTimeout: 8000,
        }
    );

datelog(" 3. Starting network check");
var networkCheck = setInterval(function() {
    exec('ping -c 1 -w 2 8.8.8.8 | grep "ttl=" > /dev/null && echo 1 || echo 0', function(error, stdout, stderr) {
        var pingSuccess = (parseInt(stdout) == 1);
        networkStatus = {
            network: pingSuccess,
            socket: pingSuccess && socket.connected
        };
        sendOsReady();
        sendNetworkStatus();

        if (!pingSuccess) {
            datelog("Cannot ping 8.8.8.8, scheduling a reboot in 10 minutes ...");
            rebootTimer = setTimeout(function(){
                reboot();
            }, 10 * 60 * 1000);
        } else {
            datelog("Aborting reboot");
            clearTimeout(rebootTimer);
        }

    })
}, 60 * 1000);

datelog(" 4. Starting reboot check");
var rebootCheck = setInterval(function() {
    if (fs.existsSync('/root/reboot.lock')) {
        sendOsDown();
        networkStatus.socket = false;
        networkStatus.network = false;
        sendNetworkStatus();
        clearInterval(rebootCheck);
        sleep(90); // Wait for reboot now ...
    }
}, 20 * 1000);

/*
        UTILITY FUNCTIONS
*/

function reset_mcu()
{
    exec('reset-mcu', function() {
        mcu_ready = true;
        datelog("MCU ready");

    })
}

function sendOsReady()
{
//    datelog('Sending OS_READY command');
    MCU_write(OS_READY + ENDL);
}

function sendOsDown()
{
    datelog('Sending OS_DOWN command');
    MCU_write(OS_DOWN + ENDL);
}

function sendDeviceId()
{
    datelog('Sending DEVICE_ID ' + DEVICE_ID);
    MCU_write(DEV_PREFIX + DEVICE_ID + ENDL);
}

function sendNetworkStatus()
{
//    datelog('Sending NETWORK/SOCKET status');
    if (networkStatus.socket) {
        datelog('Sending SOCKET_UP');
        MCU_write(SOCKET_UP + ENDL);
    } else if (networkStatus.network) {
        datelog('Sending NETWORK_UP');
        MCU_write(NETWORK_UP + ENDL);
    } else {
        datelog('Sending NETWORK_DOWN');
        MCU_write(NETWORK_DOWN + ENDL);
    }
}

function sendACK(data)
{
    //datelog('Sending ACK ' + data);
    //MCU_write(ACK + ENDL);
}

function exitProcess()
{
    datelog('** Exit process now');
    networkStatus.network = false;
    networkStatus.socket = false;
    sendNetworkStatus();
    process.exit();
}

function reboot()
{
    datelog('WARNING : reboot');
    sendOsDown();
    exec('/sbin/reboot', function() {
        //no op
    })
}

function launchUpdate()
{
    datelog("Launch update");

    // Update bridge file
    datelog("Replacing bridge");
    exec('mkdir -p /root/backup', function() {
        exec('mv /root/bridge.js /root/backup/bridge.backup.' + (Date.now()) + '.js', function() {
            exec('mv /root/update/bridge.js /root/bridge.js', function() {
                // Update INO file
                datelog("Merging sketch with bootloader");
                exec('merge-sketch-with-bootloader.lua /root/update/update.ino.hex', function() {
                    datelog("Flashing sketch");
                    exec('run-avrdude /root/update/update.ino.hex', function(){
                        reset_mcu();
                        setTimeout(function(){
                            // We give 30 secs before it can be updated again, just to avoid race conditions if
                            // socket messages are received twice.
                            updateLock = false;
                        }, 30 * 1000);
                        reboot();
                    })
                })
            })
        })
    })
}

function retrieveUpdateFiles()
{
    datelog("Retrieve update files");
    exec('rm -fr /root/update && mkdir -p /root/update', function () {
        exec('cd /root/update && wget ' + UPDATE_ENDPOINT + "/" + DEVICE_ID + "/" + "update.ino.hex", function() {
            exec('cd /root/update && wget ' + UPDATE_ENDPOINT + "/" + DEVICE_ID + "/" + "bridge.js", function() {
                exec('cd /root/update && wget ' + UPDATE_ENDPOINT + "/" + DEVICE_ID + "/" + "bridge.service", function() {
                    launchUpdate();
                });
            });
        });
    });
}

/*
        SOCKET EVENTS
*/

socket.on('connect', function() {
    datelog('** Resetting MCU');
    reset_mcu();

    datelog('** Connected to server:' + SERVER_URL);

    socket.emit('hello');

    networkStatus.network = true;
    networkStatus.socket = true;
    sendNetworkStatus();

});

socket.on('update', function(newVersion) {

    // If the new version is not really new
    if (parseInt(newVersion) <= VERSION) {
        datelog("Aborted update because v" + newVersion + " is not new enough");
        return;
    }

    if (updateLock) {
        datelog("Aborted update because an update is already in progress");
        return;
    }

    updateLock = true;

    // Retrieve new files : bridge.js, bridge.service and .hex file
    retrieveUpdateFiles();

});

socket.on('sensor-receive', function(d) {
    datelog("COMMAND : " + d);

    if (mcu_ready == false)
    {
        datelog("Server COMMAND discarded until MCU ready to receive command ");
        return;
    }

    mcu_ready = false; // make sure we only send one command at a time

    d = d.replace(/\\/g, "");

    // remove first " and last " from a dirty string if they exist
    if ((d.charAt(0) == "\"") && (d.slice(-1) == "\"")) {
        d = d.slice(1, -1);
    }

    //datelog("   Cleaned message : " + d);

    var command = JSON.parse(d);

    if (command.device_id == DEVICE_ID) {
        if(last_server_command != command)
        {
            var value = Number(command.sensor_value);
            var state = value;
            if (Number(value) == Number(0)) {
                value = Number(1);
            } else {
                value = Number(0);
            }

            if (command.hasOwnProperty('sensor_mode')) {
                var mode = Number(command.sensor_mode);
            } else {
                var mode = Number(0); //by default only manual mode is triggerred
            }

            value = (value & 0x1) | ((mode & 0x1) << 1);

            if (command.sensor_type == "relay1") {
                if((state.toString() == relay1_state) )
                {
                    datelog("Server COMMAND discarded relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "a" + value.toString();
                MCU_write(acmd + ENDL);
            } else if (command.sensor_type == "relay2") {
                if((state.toString() == relay2_state) )
                {
                    datelog("Server COMMAND discarded relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "b" + value.toString();
                MCU_write(acmd + ENDL);
            } else if (command.sensor_type == "relay3") {
                if((state.toString() == relay3_state) )
                {
                    datelog("Server COMMAND discarded relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "c" + value.toString();
                MCU_write(acmd + ENDL);
            } else if (command.sensor_type == "relay4") {
                if((state.toString() == relay4_state) )
                {
                    datelog("Server COMMAND discarded relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "d" + value.toString();
                MCU_write(acmd + ENDL);
            }

            last_server_command = command;
        }
        else
        {
            datelog(" COMMAND discarded : similar to previous command ");
        }
    } else {
        datelog("   Bad DEVICE_ID (" + command.device_id + "), ignoring");
    }

    mcu_ready = true;
});

socket.on('disconnect', function() {
    datelog('** Disconnected from server: ' + SERVER_URL);
    networkStatus.socket = false;
    sendNetworkStatus();
});

socket.on('reconnect', function() {
    datelog('** Reconnected to server: ' + SERVER_URL);
    networkStatus.socket = true;
    networkStatus.network = true;
    sendNetworkStatus();
});

socket.on('connect_error', function(error) {
    datelog('** Connection error from server: ' + SERVER_URL);
    datelog(error);
    datelog('** Exit process in 10 sec');
    setTimeout(function(){
        exitProcess();
    }, 10 * 1000);
});

socket.on('connect_timeout', function() {
    datelog('** Connection timeout from server: ' + SERVER_URL);
    datelog('** Exit process in 10 sec');
    setTimeout(function(){
        exitProcess();
    }, 10 * 1000);
});

socket.on('reconnect_failed', function() {
    datelog('** Reconnection failed to server: ' + SERVER_URL);
    datelog('** Exit process in 10 sec');
    setTimeout(function(){
        exitProcess();
    }, 10 * 1000);
});

socket.on('reconnect_error', function(error) {
    datelog('** Reconnection error from server: ' + SERVER_URL);
    datelog(error);
    datelog('** Exit process in 10 sec');
    setTimeout(function(){
        exitProcess();
    }, 10 * 1000);
});

/*
        ARDUINO EVENTS
*/

arduino.on("error", function(error) {
    datelog("ERROR " + error);
})

arduino.on('open', function() {
    datelog('* Port is now open, starting');
    arduino.flush(function() {
        sendOsReady();
        sendDeviceId();
        sendNetworkStatus();
        datelog("-----------------\n");
    });
});

arduino.on('data', function(data) {

    //datelog("[DATA] " + data);

    // First check if this is a request from ARDUINO
    if (data == STATUS_REQUEST) {
        datelog("   Resending status ...");
        sendOsReady();
        sendDeviceId();
        sendNetworkStatus();
        return;

    // Or an info message maybe ?
    } else if (data.indexOf(INFO_MESSAGE + " ") === 0) {
        datelog("INFO " + data.slice(2));
        return;

    } else if (networkStatus.socket) {
        // It's a command, apparently, process it
        try {
            var emit = JSON.parse(data);
        } catch (e) {
            datelog("   Bad JSON, ignoring");
            datelog(data);
            return; // console.error(e);
        }
        sendACK(data); // send an ACK to arduino
        sensorType = emit.t;
        sensorValue = Number(emit.v);

        if (SENSOR_TYPES.indexOf(sensorType) > -1)
        {
            var sensorMType = sensorType.slice(0, -1);
            var msg2send = "{}";

            if (sensorMType == "relay") {
                sensorMode = "0";
                if (sensorValue >> 1 == 1) {
                    sensorMode = "1";
                }
                sensorValue = sensorValue & 0x1;
                // internal logic is inverted, fix that
                if (Number(sensorValue) == Number(0)) {
                    sensorValue = Number(1);
                } else {
                    sensorValue = Number(0);
                }

                //save state localy
                if(sensorType == "relay1")
                {
                    relay1_state = sensorValue.toString();
                    relay1_mode = sensorMode;
                }
                else if(sensorType == "relay2")
                {
                    relay2_state = sensorValue.toString();
                    relay2_mode = sensorMode;
                }
                else if(sensorType == "relay3")
                {
                    relay3_state = sensorValue.toString();
                    relay3_mode = sensorMode;
                }
                else  if(sensorType == "relay4")
                {
                    relay4_state = sensorValue.toString();
                    relay4_mode = sensorMode;
                }
                msg2send = JSON.stringify({
                    device_id: DEVICE_ID.toString(),
                    sensor_type: sensorType,
                    sensor_value: sensorValue.toString(),
                    sensor_mode: sensorMode.toString(),
                });
            } else {
                msg2send = JSON.stringify({
                    device_id: DEVICE_ID.toString(),
                    sensor_type: sensorType,
                    sensor_value: sensorValue.toString(),
                });
            }

            //datelog(" --> Sending to server : " + msg2send);

            socket.emit("sensor-emit", msg2send);

            // Verify that once we emitted some data,
            // we continue to emit at least every 30 sec.
            // Overwise it means the connection has some problem and we should exit the script to relaunch it
            clearTimeout(dataTimer);
            dataTimer = setTimeout(exitProcess, 30 * 1000);

        } else {
            datelog("   Unknown type " + sensorType + ", dropped" );
        }
    } else {
        datelog("   Socket not connected, discarding data.")
    }

});
