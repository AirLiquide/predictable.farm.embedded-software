/* Version : 10.0*/

// /!\ We do not 'use strict' mode here since Node 0.10 does not enable const / let by default.
const VERSION = 10;

var DEVICE_ID = 0;
var SERVER_URL = "http://lafactory.predictable.zone";
const UPDATE_ENDPOINT = "http://update.predictablefarm.net/";

process.argv.forEach(function (val, index, array) {
    if (index >= 2) {
        if (index == 2) {
            console.log("Device id: " + val);
            DEVICE_ID = val;
        }
        else if (index == 3) {
            console.log("Server url: " + val);
            SERVER_URL = "http://" + val + ".predictable.zone";
        }
    }
});

/* ---------------------------------------------------------------------- */
const fs = require("fs");
var util = require('util');
var log_file = fs.createWriteStream(__dirname + '/service.log', {flags : 'a'});
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
    console.log(Date.now() + ": " + message);
}

//const MutexPromise = require('mutex-promise');
const exec = require('child_process').exec;
const https = require("https");

const client = require('/usr/lib/node_modules/socket.io-client');

const LOCAL_SERVER_PORT = 64000;
const server = require('/usr/lib/node_modules/socket.io');

const MODE_LOCAL = 0;
const MODE_REMOTE = 1;

var networkStatus = {
    remote_socket: false,
    network: false
};

var receivedData = false;
var mcu_ready = false;
var updateLock = false;

var dataTimer = null; // timer in between two sets of data sent to the server
var rebootTimer = null; // timer before rebooting after a network down event

var relay1_mode = "0";
var relay2_mode = "0";
var relay3_mode = "0";
var relay4_mode = "0";
var last_server_command = "";

const RED = 2;
const ORANGE = 3;
const GREEN = 1;
var userled = RED ; //red


datelog("Starting app v" + VERSION + "...");
//var mutex = new MutexPromise('sync_write');


function all_relay_off() {
    datelog("safe switch OFF all relay  ");
    //todo
}
datelog("Starting app ...");


datelog(" 1. Opening local io socket ");
var my_io = server(LOCAL_SERVER_PORT);

datelog(" 2. Opening remote socket to " + SERVER_URL);

exec('/home/root/userled.py '+RED+''); // set led to red
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

datelog(" 3. Starting network check");
var networkCheck = setInterval(function () {
   // var test_file = 'http://update.predictablefarm.net/' + DEVICE_ID + '/bridge.js'
   var test_file = 'http://update.predictablefarm.net/7/bridge.js' ; //debug
   var test_network_cmdline = '(/usr/bin/curl --head --silent ' + test_file + ' | head -n 1) | grep -q 200 && echo 1 || echo 0';
    exec(test_network_cmdline, function (error, stdout, stderr) {
        var pingSuccess = (parseInt(stdout) == 1);
        networkStatus = {
            network: pingSuccess,
            remote_socket: pingSuccess && remote_socket.connected
        };

        if (!pingSuccess) {
            if(userled != RED) exec('/home/root/userled.py '+RED+''); // set led to red
            userled = RED;
           /* datelog("Cannot get " + test_file + ", scheduling a reboot in 10 minutes ...");
            rebootTimer = setTimeout(function () {
                all_relay_off();
                reboot();
            }, 10 * 60 * 1000);*/
        } else {
            if(userled != ORANGE) exec('/home/root/userled.py '+ORANGE+''); // set led to orange
            userled = ORANGE;
           /* datelog("Aborting reboot");
            clearTimeout(rebootTimer);*/
        }

    })
}, 60 * 1000);

datelog(" 4. Starting reboot check");
var rebootCheck = setInterval(function () {
    if (fs.existsSync('/root/reboot.lock')) {
        networkStatus.remote_socket = false;
        networkStatus.network = false;
        clearInterval(rebootCheck);
        sleep(90); // Wait for reboot now ...
    }
}, 20 * 1000);



datelog(" 5. run");
/*
        UTILITY FUNCTIONS
*/
var ip = "127.0.0.1";

function getIpAddr()
{
    var cmd = 'ifconfig eth1 | grep "inet addr" | cut -d ":" -f 2 | cut -d " " -f 1';
    exec(cmd, function(error, stdout, stderr) {
        ip = "127.0.0.1";
        if(stdout.length > 10) ip=stdout;
        console.log(ip);
        var msg2send = "{}";
        msg2send = JSON.stringify({
                device_id: DEVICE_ID.toString(),
                sensor_type: "ip",
                sensor_value: ip
        });
        print_screen(msg2send);
    });    
}

function exitProcess() {
    if(userled != RED) exec('/home/root/userled.py '+RED+''); // set led to red
    userled = RED;
    datelog('** Exit process now');
    networkStatus.network = false;
    networkStatus.remote_socket = false;
    all_relay_off();
    process.exit();
}

function reboot() {
    if(userled != RED) exec('/home/root/userled.py '+RED+'');  // set led to red
    userled = RED;
    datelog('WARNING : reboot');
    sendOsDown();
    exec('/sbin/reboot', function () {
        //no op
    })
}

function launchUpdate() {
    datelog("Launch update");
    // Update bridge file
    datelog("Replacing bridge");
    exec('mkdir -p /root/backup', function () {
        exec('mv /root/bridge.js /root/backup/bridge.backup.' + (Date.now()) + '.js', function () {
            exec('mv /root/update/bridge.js /root/bridge.js', function () {
                // Update INO file
                datelog("Merging sketch with bootloader");
                exec('merge-sketch-with-bootloader.lua /root/update/update.ino.hex', function () {
                    datelog("Flashing sketch");
                    exec('run-avrdude /root/update/update.ino.hex', function () {
                        reset_mcu();
                        setTimeout(function () {
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

function retrieveUpdateFiles() {
    datelog("Retrieve update files");
    exec('rm -fr /root/update && mkdir -p /root/update', function () {
        exec('cd /root/update && wget ' + UPDATE_ENDPOINT + "/" + DEVICE_ID + "/" + "update.ino.hex", function () {
            exec('cd /root/update && wget ' + UPDATE_ENDPOINT + "/" + DEVICE_ID + "/" + "bridge.js", function () {
                exec('cd /root/update && wget ' + UPDATE_ENDPOINT + "/" + DEVICE_ID + "/" + "bridge.service", function () {
                    launchUpdate();
                });
            });
        });
    });
}

function isRelay(command) {
    try {
        var cmd = JSON.parse(command);
    } catch (e) {
        datelog("isRelay   Bad JSON, ignoring");
        datelog(cmd);
        return false; // console.error(e);
    }
    if ((cmd.sensor_type.slice(0, -1) == "relay")) {
        return true;
    } else {
        datelog(cmd.sensor_type.slice(0, -1) + "---" + Number(cmd.sensor_type.slice(5, 1)));
        return false;
    }
}

function processIncomingCommand(data, mode) {
    var prefix = (mode == MODE_LOCAL) ? "LOCAL Command" : "REMOTE Command";

    datelog(prefix + " : " + data);


    d = data.replace(/\\/g, "");

    // remove first " and last " from a dirty string if they exist
    if ((d.charAt(0) == "\"") && (d.slice(-1) == "\"")) {
        d = d.slice(1, -1);
    }

    var command = JSON.parse(d);

    if (command.device_id == DEVICE_ID) {
        if (last_server_command != command) {
            if (isRelay(d) == true) my_io.emit("relay", d);

            last_server_command = command;
        }
        else {
            datelog(prefix + " discarded: similar to previous command ");
        }
    } else {
        datelog("   Bad DEVICE_ID (" + command.device_id + "), ignoring");
    }
}

/*
        SOCKET EVENTS
*/

remote_socket.on('connect', function () {

    datelog('** Connected to server: ' + SERVER_URL);

    if(userled != GREEN) exec('/home/root/userled.py '+GREEN+'');  // set led to green
    userled = GREEN;
    remote_socket.emit('hello');

    networkStatus.network = true;
    networkStatus.remote_socket = true;

});

remote_socket.on('update', function (newVersion) {

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

remote_socket.on('sensor-receive', function (data) {
    processIncomingCommand(data, MODE_REMOTE);
});

remote_socket.on('disconnect', function () {
    datelog('** Disconnected from server: ' + SERVER_URL);
    networkStatus.remote_socket = false;

});

remote_socket.on('reconnect', function () {
    datelog('** Reconnected to server: ' + SERVER_URL);
    networkStatus.remote_socket = true;
    networkStatus.network = true;
});

remote_socket.on('connect_error', function (error) {
    datelog('** Connection error from server: ' + SERVER_URL);
    datelog(error);
    /*datelog('** Exit process in 10 sec');
    setTimeout(function () {
        exitProcess();
    }, 10 * 1000);*/
});

remote_socket.on('connect_timeout', function () {
    datelog('** Connection timeout from server: ' + SERVER_URL);
    datelog('** Exit process in 10 sec');
    /*setTimeout(function () {
        exitProcess();
    }, 10 * 1000);*/
});

remote_socket.on('reconnect_failed', function () {
    datelog('** Reconnection failed to server: ' + SERVER_URL);
    datelog('** Exit process in 10 sec');
    /*setTimeout(function () {
        exitProcess();
    }, 10 * 1000);*/
});

remote_socket.on('reconnect_error', function (error) {
    datelog('** Reconnection error from server: ' + SERVER_URL);
    datelog(error);
   /* datelog('** Exit process in 10 sec');
    setTimeout(function () {
        exitProcess();
    }, 10 * 1000);*/
});

/*
    IO peripheral events
*/

const ArrowLeft = 0x1c;
const ArrowRight = 0x1d;
const ArrowUp = 0x1e;
const ArrowDown = 0x1f;
var schedulerToken = 0;

var co2_sensor_enabled = false;
var relay_enabled = false;
var barometer_sensor_enabled = false;
var par_sensor_enabled = false;
var relative_humidity_sensor_enabled = false;
var lcd_enabled = false;
my_io.on('connection', function (my_io_socket) {
    datelog('** IO Connection ');
    my_io.emit("device_id", DEVICE_ID);

    my_io_socket.on('io', function (data) {
        datelog(data + " registered");
        if (data === "lcd") {
            var msg2send = "{}";

            msg2send = JSON.stringify({
                clean: "WHITE",
                print_x: "2",
                print_y: "2",
                print: "Predictable",
                print_box: "2",

            });
            my_io.emit("lcd", msg2send);
            msg2send = JSON.stringify({
                print_x: "90",
                print_y: "2",
                print: "id:" + DEVICE_ID.toString(),
                print_cursor: ArrowRight

            });
            my_io.emit("lcd", msg2send);

            lcd_enabled = true;
        }
        if (data === "relay") {
            relay_enabled = true;
        }
        if (data === "air_co2") {
            co2_sensor_enabled = true;
        }
        if (data === "air_pressure") {
            barometer_sensor_enabled = true;
        }
        if (data === "air_humidity") {
            relative_humidity_sensor_enabled = true;
        }
        if (data === "ligth_par") {
            par_sensor_enabled = true;
        }
    });
    my_io_socket.on('data', function (data) {
        //datelog("[DATA] " + data);
        datelog(" --> Sending to server: " + data);
        if(userled != GREEN) exec('/home/root/userled.py '+GREEN+''); 
        userled = GREEN;
        remote_socket.emit("sensor-emit", data);
        print_screen(data);
    });
    my_io_socket.on('scheduled', function (data) {
        //datelog(" scheduler token freed from " + data);
        schedulerToken = 0;
    });
});

function print_screen(data) {
    var cmd = JSON.parse(data);
    var msg2send = "{}";
    var text = "";
    var pos_x = 2;
    var pos_y = 16;
    const line_offset = 10;
    var index = 0;
    var x_offset = 0;
    switch (cmd.sensor_type) {
        case "air_temperature":
            text = "T:" + cmd.sensor_value + "C  ";
            index = 1;
            x_offset = 0;
            break;
        case "air_pressure":
            text = "P:" + cmd.sensor_value + "kPa ";
            index = 1;
            x_offset = 54;
            break;        
        case "air_humidity":
            text = "RH:" + cmd.sensor_value + "%  ";
            index = 2;
            x_offset = 0;
            break;
        case "air_co2":
            text = "co2:" + cmd.sensor_value + "ppm  ";
            index = 3;
            x_offset = 0;
            break;
        case "light_par":
            text = "PAR:" + cmd.sensor_value + "mol/s/m  ";
            index = 4;
            x_offset = 0;
            break;        
        case "relay1":
            if (cmd.sensor_value == "1") text = "1:ON ";
            else text = "1:OFF";
            index = 4;
            x_offset = 0;
            break;
        case "relay2":
            if (cmd.sensor_value == "1") text = "2:ON ";
            else text = "2:OFF";
            index = 4;
            x_offset = 30;
            break;
        case "relay3":
            if (cmd.sensor_value == "1") text = "3:ON ";
            else text = "3:OFF";
            index = 4;
            x_offset = 60;
            break;
        case "relay4":
            if (cmd.sensor_value == "1") text = "4:ON ";
            else text = "4:OFF";
            index = 4;
            x_offset = 90;
            break;
        case "relay0":
            if (cmd.sensor_value == "1") text = "Actuator:ON ";
            else text = "Actuator:OFF";
            index = 4;
            x_offset = 0;
            break;
        case "ip":
            index = 0;
            x_offset = 0;
            text = cmd.sensor_value;
            break;
        default:
            return;
    }
    msg2send = JSON.stringify({
        print_x: (pos_x + x_offset).toString(),
        print_y: (pos_y + line_offset * index).toString(),
        print: text
    });
    my_io.emit("lcd", msg2send);

}

var loop = 0;
var ctrl_loop = 0;
var tokenBusyTimeout = 0;
var dest = null;
// used to schedule read write on i2C bus and prevent driver issue with concurrent access
// not used for relay_native 
var i2c_scheduler = setInterval(function () {
    if (schedulerToken) 
    {
        //console.log("scheduler busy");
        tokenBusyTimeout++;
        if(tokenBusyTimeout < 500)
            return;
        else
        {
            tokenBusyTimeout = 0;
            datelog("scheduler released after 5sec timeout from " + dest);
            datelog("sdisabling sensor until next restart " );
            if(dest == "air_co2") co2_sensor_enabled = false;
            else if(dest == "air_pressure") barometer_sensor_enabled = false;
            else if(dest == "air_humidity") relative_humidity_sensor_enabled = false;
            else if(dest == "light_par") par_sensor_enabled = false;
        }
    }
    dest = null;
    tokenBusyTimeout = 0;
    if (loop == 100) {
        loop = 0;
        dest = ''
    }
    else {
        loop++;
        if(loop % 2 ) {
            if(ctrl_loop == 0)
            {
              if(relay_enabled)  dest = 'relay';
                ctrl_loop =1;
            }
            else {
               if(lcd_enabled) dest = 'lcd';
                ctrl_loop = 0;
            }
        }
        else
        {
            if(loop == 4 ) getIpAddr();
            if(loop == 30)
            {
               // datelog("schedule air_co2");
                if (co2_sensor_enabled) dest = 'air_co2';
            }
            else if(loop == 50)
            {
                //datelog("schedule air_pressure");
                if(barometer_sensor_enabled) dest = 'air_pressure'; // will also send air temperature
            }
            else if(loop == 70 )
            {
                if(relative_humidity_sensor_enabled) dest = 'air_humidity';
            }
            else if(loop == 84)
            {
                if( par_sensor_enabled ) dest = 'light_par';
            }


           // datelog("schedule sensor /"+loop);
        }
    }

   if(dest) 
   {
        schedulerToken = 1;
        my_io.emit("scheduler", dest);
   }
    
}, 10);
