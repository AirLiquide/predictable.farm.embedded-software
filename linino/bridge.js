/* Version : 9.0*/

// /!\ We do not 'use strict' mode here since Node 0.10 does not enable const / let by default.

const VERSION = 8;

var DEVICE_ID = 0;
var SERVER_URL = "http://lafactory.predictable.zone";

const SERIAL_PORT = "/dev/ttyATH0"; /*This is the default serial port used internally by the arduino Yun*/
const SERIAL_BAUDRATE = 115200;
const UPDATE_ENDPOINT = "http://update.predictablefarm.net/";

process.argv.forEach(function (val, index, array) {
    if(index >= 2)
    {
        if(index==2)
        {
            console.log("Device id: " + val);
            DEVICE_ID = val;
        }
        else if (index == 3)
        {
            console.log("Server url: " + val);
            SERVER_URL = "http://" + val + ".predictable.zone";
        }
    }
});

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

const START = '#';

const OS_TIME        = "t";
const OS_STATE_WINDOW        =     'f';
const OS_STATE_DOWN            =   'g';
const OS_STATE_UP              =   'h';
const OS_STATE_LIGHT           =    'l';
const OS_STATE_CLIMATE           =   'm';
const OS_REMOTE_INFO1      ='p';
const OS_REMOTE_INFO2      ='q';
const OS_STATE_SCENARIO          = 'r';
const OS_SYSTEM_NOTIF           =  'x';


const RELAY_MODE_AUTO = "0";
const RELAY_MODE_MANUAL = "1";

const MODE_LOCAL  = 0;
const MODE_REMOTE = 1;

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
    remote_socket: false,
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
    data = START + data + ENDL;
    arduino.write(data);
}

function all_relay_off()
{
    datelog("safe switch OFF all relay  " );
    reset_mcu();
}

datelog("Starting app ...");

datelog(" 1. Opening serial port " + SERIAL_PORT);
var arduino = new SerialPort(SERIAL_PORT, {
    baudrate: SERIAL_BAUDRATE,
    parser: serialport.parsers.readline("\r\n"),
});

datelog(" 2. Opening socket to " + SERVER_URL);
var remote_socket = client(
        SERVER_URL,
        {
            path: '/socket/socket.io',
            query: 'role=sensor&sensorId=' + DEVICE_ID,
            pingInterval: 5000,
            pingTimeout: 8000,
          //  transports: ['websocket'],
          //  rejectUnauthorized: false
        }
    );

datelog(" 3. Starting minute timer for network check and time update");
var networkCheck = setInterval(function() {
    var test_file = 'http://update.predictablefarm.net/'+DEVICE_ID+'/bridge.js'
    var test_network_cmdline = '(/usr/bin/curl --head --silent '+test_file+' | head -n 1) | grep -q 200 && echo 1 || echo 0';
    exec(test_network_cmdline, function(error, stdout, stderr) {
        var pingSuccess = (parseInt(stdout) == 1);
        networkStatus = {
            network: pingSuccess,
            remote_socket: pingSuccess && remote_socket.connected
        };
        sendOsReady();
        sendNetworkStatus();

        if (!pingSuccess) {
            datelog("Cannot get "+test_file+", scheduling a reboot in 10 minutes ...");
            rebootTimer = setTimeout(function(){
                all_relay_off();
                reboot();
            }, 10 * 60 * 1000);
        } else {
            datelog("Aborting reboot");
            clearTimeout(rebootTimer);
        }

    })
    exec('date "+%a %b %d - %H:%M"', function(error, stdout, stderr) {
        sendTime(stdout);
        datelog("Updating time :"+stdout);
    })
}, 60 * 1000);

datelog(" 4. Starting reboot check");
var rebootCheck = setInterval(function() {
    if (fs.existsSync('/root/reboot.lock')) {
        sendOsDown();
        networkStatus.remote_socket = false;
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
    MCU_write(OS_READY );
}

function sendOsDown()
{
    datelog('Sending OS_DOWN command');
    MCU_write(OS_DOWN);
}

function sendDeviceId()
{
    datelog('Sending DEVICE_ID ' + DEVICE_ID);
    MCU_write(DEV_PREFIX + DEVICE_ID );
}

function sendTime(time)
{
//    datelog('Sending OS_TIME command');
    MCU_write( OS_TIME + time );
}

function sendNetworkStatus()
{
//    datelog('Sending NETWORK/SOCKET status');
    if (networkStatus.remote_socket) {
        datelog('Sending remote SOCKET_UP');
        MCU_write(SOCKET_UP);
    } else if (networkStatus.network) {
        datelog('Sending NETWORK_UP');
        MCU_write( NETWORK_UP);
    } else {
        datelog('Sending NETWORK_DOWN');
        MCU_write( NETWORK_DOWN );
    }
}

function sendACK(data)
{
    //datelog('Sending ACK ' + data);
    //MCU_write(ACK);
}

function exitProcess()
{
    datelog('** Exit process now');
    networkStatus.network = false;
    networkStatus.remote_socket = false;
    sendNetworkStatus();
    all_relay_off();
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

function processIncomingCommand(data, mode)
{
    var prefix = (mode == MODE_LOCAL)?"LOCAL Command":"REMOTE Command";

    datelog(prefix + " : " + data);

    if (mcu_ready == false)
    {
        datelog(prefix + " discarded until MCU ready to receive command ");
        return;
    }

    mcu_ready = false; // make sure we only send one command at a time

    d = data.replace(/\\/g, "");

    // remove first " and last " from a dirty string if they exist
    if ((d.charAt(0) == "\"") && (d.slice(-1) == "\"")) {
        d = d.slice(1, -1);
    }

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

            var change_mode = false;
            if (command.hasOwnProperty('sensor_mode')) {
                var mode = Number(command.sensor_mode);
                if ((command.sensor_type == "relay1") && (relay1_mode != mode))
                {
                    relay1_mode = mode;
                    change_mode = true;
                }
                else if ((command.sensor_type == "relay2") && (relay2_mode != mode))
                {
                    relay2_mode = mode;
                    change_mode = true;
                }
                else if ((command.sensor_type == "relay3")&& (relay3_mode != mode))
                {
                    relay3_mode = mode;
                    change_mode = true;
                }
                else if ((command.sensor_type == "relay4") && (relay4_mode != mode))
                {
                    relay4_mode = mode;
                    change_mode = true;
                }
                datelog("   mode set to   : " + mode);
            } else {
                var mode = Number(0); //by default only manual mode is triggerred
                if ((command.sensor_type == "relay1") && (relay1_mode == RELAY_MODE_MANUAL))
                {
                    datelog(prefix + " discarded: relay is in manual mode ");
                    mcu_ready = true;
                    return;
                }
            }

            value = (value & 0x1) | ((mode & 0x1) << 1);

            if (command.sensor_type == "relay1") {
                if((state.toString() == relay1_state) && (change_mode == false) )
                {
                    datelog(prefix + " discarded: relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "a" + value.toString();
                MCU_write(acmd);
            } else if (command.sensor_type == "relay2" ) {
                if((state.toString() == relay2_state)  && (change_mode == false))
                {
                    datelog(prefix + " discarded: relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "b" + value.toString();
                MCU_write(acmd);
            } else if (command.sensor_type == "relay3")  {
                if((state.toString() == relay3_state) && (change_mode == false) )
                {
                    datelog(prefix + " discarded: relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "c" + value.toString();
                MCU_write( acmd);
            } else if (command.sensor_type == "relay4") {
                if((state.toString() == relay4_state) && (change_mode == false) )
                {
                    datelog(prefix + " discarded: relay is already in that state");
                    mcu_ready = true;
                    return;
                }
                var acmd = "d" + value.toString();
                MCU_write( acmd);
            }

            last_server_command = command;
        }
        else
        {
            datelog(prefix + " discarded: similar to previous command ");
        }
    } else {
        datelog("   Bad DEVICE_ID (" + command.device_id + "), ignoring");
    }

    mcu_ready = true;
}

/*
        SOCKET EVENTS
*/

remote_socket.on('connect', function() {
    datelog('** Resetting MCU');
    reset_mcu();

    datelog('** Connected to server: ' + SERVER_URL);

    remote_socket.emit('hello');

    networkStatus.network = true;
    networkStatus.remote_socket = true;
    sendNetworkStatus();

});

remote_socket.on('update', function(newVersion) {

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

remote_socket.on('sensor-receive', function(data) {
    processIncomingCommand(data, MODE_REMOTE);
});

remote_socket.on('disconnect', function() {
    datelog('** Disconnected from server: ' + SERVER_URL);
    networkStatus.remote_socket = false;
    sendNetworkStatus();

});

remote_socket.on('reconnect', function() {
    datelog('** Reconnected to server: ' + SERVER_URL);
    networkStatus.remote_socket = true;
    networkStatus.network = true;
    sendNetworkStatus();
});

remote_socket.on('connect_error', function(error) {
    datelog('** Connection error from server: ' + SERVER_URL);
    datelog(error);
    datelog('** Exit process in 10 sec');
    setTimeout(function(){
        exitProcess();
    }, 10 * 1000);
});

remote_socket.on('connect_timeout', function() {
    datelog('** Connection timeout from server: ' + SERVER_URL);
    datelog('** Exit process in 10 sec');
    setTimeout(function(){
        exitProcess();
    }, 10 * 1000);
});

remote_socket.on('reconnect_failed', function() {
    datelog('** Reconnection failed to server: ' + SERVER_URL);
    datelog('** Exit process in 10 sec');
    setTimeout(function(){
        exitProcess();
    }, 10 * 1000);
});

remote_socket.on('reconnect_error', function(error) {
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

    } else if (networkStatus.remote_socket) {
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

            datelog(" --> Sending to server: " + msg2send);

            remote_socket.emit("sensor-emit", msg2send);

            // Verify that once we emitted some data,
            // we continue to emit at least every 30 sec.
            // Overwise it means the connection has some problem and we should exit the script to relaunch it
            clearTimeout(dataTimer);
            dataTimer = setTimeout(exitProcess, 30 * 1000);

        } else {
            datelog("   Unknown type " + sensorType + ", dropped" );
        }
    } else {
        datelog("   Remote socket not connected, discarding data.")
    }

});
