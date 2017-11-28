/* Version : 4.0*/

// /!\ We do not 'use strict' mode here since Node 0.10 does not enable const / let by default.

const DEVICE_ID = 9;

const SERIAL_PORT = "/dev/ttyATH0"; /*This is the default serial port used internally by the arduino Yun*/
const SERIAL_BAUDRATE = 115200;

const SERVER_URL = "http://ecf-berlin.predictable.farm";

/* ---------------------------------------------------------------------- */

function datelog(message) {
    console.log(Date.now() + ": " + message);
}

const STATUS_REQUEST = "s";

const OS_READY       = "o1"; //"z0";
const OS_DOWN        = "o0"; //"z1";
const DEV_PREFIX     = "i";
const NETWORK_DOWN   = "n0"; //"y0";
const NETWORK_UP     = "n1"; //"y1";
const SOCKET_UP      = "n2";

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
            datelog("Scheduling a reboot in 10 minutes ...");
            rebootTimer = setTimeout(function(){
                reboot();
            }, 10 * 60 * 1000);
        } else {
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
    arduino.write(OS_READY + ENDL);
}

function sendOsDown()
{
    datelog('Sending OS_DOWN command');
    arduino.write(OS_DOWN + ENDL);
}

function sendDeviceId()
{
    datelog('Sending DEVICE_ID ' + DEVICE_ID);
    arduino.write(DEV_PREFIX + DEVICE_ID + ENDL);
}

function sendNetworkStatus()
{
//    datelog('Sending NETWORK/SOCKET status');
    if (networkStatus.socket) {
        arduino.write(SOCKET_UP + ENDL);
    } else if (networkStatus.network) {
        arduino.write(NETWORK_UP + ENDL);
    } else {
        arduino.write(NETWORK_DOWN + ENDL);
    }
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
                arduino.write(acmd + ENDL);
            } else if (command.sensor_type == "relay2") {
                if((state.toString() == relay2_state) )
                {
                    datelog("Server COMMAND discarded relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "b" + value.toString();
                arduino.write(acmd + ENDL);
            } else if (command.sensor_type == "relay3") {
                if((state.toString() == relay3_state) )
                {
                    datelog("Server COMMAND discarded relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "c" + value.toString();
                arduino.write(acmd + ENDL);
            } else if (command.sensor_type == "relay4") {
                if((state.toString() == relay4_state) )
                {
                    datelog("Server COMMAND discarded relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "d" + value.toString();
                arduino.write(acmd + ENDL);
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

    var networkStatus = /*isNetworkAvailable() && */ socketConnected;

    // First check if this is a request from ARDUINO
    if (data == STATUS_REQUEST) {
        datelog("   Resending status ...");
        sendOsReady();
        sendDeviceId();
        sendNetworkStatus();
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
