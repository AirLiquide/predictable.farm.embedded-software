'use strict'

var DEVICE_ID = 0

var SERVER_URL = 'http://lafactory.predictable.zone'
const UPDATE_ENDPOINT = 'http://update.predictablefarm.net/'

const config = require('./config/default')
const logger = require('./services/logger')
const utils = require('./services/utils')

logger.init('service')

process.argv.forEach(function (val, index, array) {
  if (index >= 2) {
    if (index == 2) {
      DEVICE_ID = val
      logger.log('Device ID is ' + DEVICE_ID)
    } else if (index == 3) {
      SERVER_URL = 'http://' + val + '.predictable.zone'
      logger.log('Server URL is ' + SERVER_URL)
    }
  }
})

/* ---------------------------------------------------------------------- */

// const MutexPromise = require('mutex-promise');
const exec = require('child_process').exec
const https = require('https')

const client = require('/usr/lib/node_modules/socket.io-client')
const server = require('/usr/lib/node_modules/socket.io')

const MODE_LOCAL = 0
const MODE_REMOTE = 1

var networkStatus = {
  remote_socket: false,
  network: false
}

var receivedData = false
var mcu_ready = false
var updateLock = false

var dataTimer = null // timer in between two sets of data sent to the server
var rebootTimer = null // timer before rebooting after a network down event

var relay1_mode = '0'
var relay2_mode = '0'
var relay3_mode = '0'
var relay4_mode = '0'
var last_server_command = ''

logger.log('Starting app v' + config.version + '...')
// var mutex = new MutexPromise('sync_write');

logger.log('Starting app ...')

logger.log(' 1. Opening local io socket ')
var my_io = server(config.socket.port)

logger.log(' 2. Opening remote socket to ' + SERVER_URL)

utils.led(utils.colors.RED)

var remote_socket = client(
  SERVER_URL,
  {
    path: '/socket/socket.io',
    query: 'role=sensor&sensorId=' + DEVICE_ID,
    pingInterval: 5000,
    pingTimeout: 8000
    //  transports: ['websocket'],
    //  rejectUnauthorized: false
  }
)

logger.log(' 3. Starting network check')
var networkCheck = setInterval(function () {
  // var test_file = 'http://update.predictablefarm.net/' + DEVICE_ID + '/bridge.js'
  var test_file = 'http://update.predictablefarm.net/7/bridge.js' // debug
  var test_network_cmdline = '(/usr/bin/curl --head --silent ' + test_file + ' | head -n 1) | grep -q 200 && echo 1 || echo 0'
  exec(test_network_cmdline, function (error, stdout, stderr) {
    var pingSuccess = (parseInt(stdout) == 1)
    networkStatus = {
      network: pingSuccess,
      remote_socket: pingSuccess && remote_socket.connected
    }

    if (!pingSuccess) {
      utils.led(utils.colors.RED)
      /* logger.log("Cannot get " + test_file + ", scheduling a reboot in 10 minutes ...");
            rebootTimer = setTimeout(function () {
                all_relay_off();
                reboot();
            }, 10 * 60 * 1000); */
    } else {
      utils.led(utils.colors.ORANGE)
      /* logger.log("Aborting reboot");
            clearTimeout(rebootTimer); */
    }
  })
}, 60 * 1000)

logger.log(' 4. Starting reboot check')
var rebootCheck = setInterval(function () {
  if (fs.existsSync('/root/reboot.lock')) {
    networkStatus.remote_socket = false
    networkStatus.network = false
    clearInterval(rebootCheck)
    sleep(90) // Wait for reboot now ...
  }
}, 20 * 1000)

logger.log(' 5. run')

/*
        UTILITY FUNCTIONS
*/
var ip = '127.0.0.1'

function getIpAddr () {
  var cmd = 'ifconfig eth1 | grep "inet addr" | cut -d ":" -f 2 | cut -d " " -f 1'
  exec(cmd, function (error, stdout, stderr) {
    if (stdout.length > 10) {
      ip = stdout
    }
    logger.log('IP : %s', ip)
    var msg2send = JSON.stringify({
      device_id: DEVICE_ID.toString(),
      sensor_type: 'ip',
      sensor_value: ip
    })
    print_screen(msg2send)
  })
}

function all_relay_off () {
  logger.log('safe switch OFF all relay  ')
  // todo
}

function exitProcess () {
  utils.led(utils.colors.RED)
  logger.log('** Exit process now')
  networkStatus.network = false
  networkStatus.remote_socket = false
  all_relay_off()
  process.exit()
}

function reboot () {
  utils.led(utils.colors.RED)
  logger.log('WARNING : reboot')
  sendOsDown()
  exec('/sbin/reboot', function () {
    // no op
  })
}

function launchUpdate () {
  logger.log('Launch update')
  // Update bridge file
  logger.log('Replacing bridge')
  exec('mkdir -p /root/backup', function () {
    exec('mv /root/bridge.js /root/backup/bridge.backup.' + (Date.now()) + '.js', function () {
      exec('mv /root/update/bridge.js /root/bridge.js', function () {
        // Update INO file
        logger.log('Merging sketch with bootloader')
        exec('merge-sketch-with-bootloader.lua /root/update/update.ino.hex', function () {
          logger.log('Flashing sketch')
          exec('run-avrdude /root/update/update.ino.hex', function () {
            reset_mcu()
            setTimeout(function () {
              // We give 30 secs before it can be updated again, just to avoid race conditions if
              // socket messages are received twice.
              updateLock = false
            }, 30 * 1000)
            reboot()
          })
        })
      })
    })
  })
}

function retrieveUpdateFiles () {
  logger.log('Retrieve update files')
  exec('rm -fr /root/update && mkdir -p /root/update', function () {
    exec('cd /root/update && wget ' + UPDATE_ENDPOINT + '/' + DEVICE_ID + '/' + 'update.ino.hex', function () {
      exec('cd /root/update && wget ' + UPDATE_ENDPOINT + '/' + DEVICE_ID + '/' + 'bridge.js', function () {
        exec('cd /root/update && wget ' + UPDATE_ENDPOINT + '/' + DEVICE_ID + '/' + 'bridge.service', function () {
          launchUpdate()
        })
      })
    })
  })
}

function isRelay (command) {
  try {
    var cmd = JSON.parse(command)
  } catch (e) {
    logger.log('isRelay   Bad JSON, ignoring')
    logger.log(cmd)
    return false // console.error(e);
  }
  if ((cmd.sensor_type.slice(0, -1) == 'relay')) {
    return true
  } else {
    logger.log(cmd.sensor_type.slice(0, -1) + '---' + Number(cmd.sensor_type.slice(5, 1)))
    return false
  }
}

function processIncomingCommand (data, mode) {
  var prefix = (mode == MODE_LOCAL) ? 'LOCAL Command' : 'REMOTE Command'

  logger.log(prefix + ' : ' + data)

  d = data.replace(/\\/g, '')

  // remove first " and last " from a dirty string if they exist
  if ((d.charAt(0) == '"') && (d.slice(-1) == '"')) {
    d = d.slice(1, -1)
  }

  var command = JSON.parse(d)

  if (command.device_id == DEVICE_ID) {
    if (last_server_command != command) {
      if (isRelay(d) == true) my_io.emit('relay', d)

      last_server_command = command
    } else {
      logger.log(prefix + ' discarded: similar to previous command ')
    }
  } else {
    logger.log('   Bad DEVICE_ID (' + command.device_id + '), ignoring')
  }
}

/*
        SOCKET EVENTS
*/

remote_socket.on('connect', function () {
  logger.log('** Connected to server: ' + SERVER_URL)

  utils.led(utils.colors.GREEN)
  remote_socket.emit('hello')

  networkStatus.network = true
  networkStatus.remote_socket = true
})

remote_socket.on('update', function (newVersion) {
  // If the new version is not really new
  if (parseInt(newVersion) <= VERSION) {
    logger.log('Aborted update because v' + newVersion + ' is not new enough')
    return
  }

  if (updateLock) {
    logger.log('Aborted update because an update is already in progress')
    return
  }

  updateLock = true

  // Retrieve new files : bridge.js, bridge.service and .hex file
  retrieveUpdateFiles()
})

remote_socket.on('sensor-receive', function (data) {
  processIncomingCommand(data, MODE_REMOTE)
})

remote_socket.on('disconnect', function () {
  logger.log('** Disconnected from server: ' + SERVER_URL)
  networkStatus.remote_socket = false
})

remote_socket.on('reconnect', function () {
  logger.log('** Reconnected to server: ' + SERVER_URL)
  networkStatus.remote_socket = true
  networkStatus.network = true
})

remote_socket.on('connect_error', function (error) {
  logger.log('** Connection error from server: ' + SERVER_URL)
  logger.log(error)
  /* logger.log('** Exit process in 10 sec');
    setTimeout(function () {
        exitProcess();
    }, 10 * 1000); */
})

remote_socket.on('connect_timeout', function () {
  logger.log('** Connection timeout from server: ' + SERVER_URL)
  logger.log('** Exit process in 10 sec')
  /* setTimeout(function () {
        exitProcess();
    }, 10 * 1000); */
})

remote_socket.on('reconnect_failed', function () {
  logger.log('** Reconnection failed to server: ' + SERVER_URL)
  logger.log('** Exit process in 10 sec')
  /* setTimeout(function () {
        exitProcess();
    }, 10 * 1000); */
})

remote_socket.on('reconnect_error', function (error) {
  logger.log('** Reconnection error from server: ' + SERVER_URL)
  logger.log(error)
  /* logger.log('** Exit process in 10 sec');
    setTimeout(function () {
        exitProcess();
    }, 10 * 1000); */
})

/*
    IO peripheral events
*/

var schedulerBusy = false

const ArrowLeft = 0x1c
const ArrowRight = 0x1d
const ArrowUp = 0x1e
const ArrowDown = 0x1f

var co2_sensor_enabled = false
var relay_enabled = false
var barometer_sensor_enabled = false
var par_sensor_enabled = false
var relative_humidity_sensor_enabled = false
var lcd_enabled = false

my_io.on('connection', function (my_io_socket) {
  logger.log('** IO Connection ')

  my_io_socket.on('io', function (data) {
    logger.log(data + ' registered')
    if (data === 'lcd') {
      var msg2send = JSON.stringify({
        clean: 'WHITE',
        print_x: '2',
        print_y: '2',
        print: 'Predictable',
        print_box: '2'

      })
      my_io.emit('lcd', msg2send)
      msg2send = JSON.stringify({
        print_x: '90',
        print_y: '2',
        print: 'id:' + DEVICE_ID.toString(),
        print_cursor: ArrowRight

      })
      my_io.emit('lcd', msg2send)

      lcd_enabled = true
    }
    if (data === 'relay') {
      relay_enabled = true
    }
    if (data === 'air_co2') {
      co2_sensor_enabled = true
    }
    if (data === 'air_pressure') {
      barometer_sensor_enabled = true
    }
    if (data === 'air_humidity') {
      relative_humidity_sensor_enabled = true
    }
    if (data === 'light_par') {
      par_sensor_enabled = true
    }
  })

  my_io_socket.on('data', function (data) {
    // logger.log("[DATA] " + data);
    logger.log(' --> Sending to server: ' + data)

    utils.led(utils.colors.GREEN)
    remote_socket.emit('sensor-emit', data)
    print_screen(data)
  })

  my_io_socket.on('scheduled', function (data) {
    // logger.log(" scheduler token freed from " + data);
    schedulerBusy = false
  })

  my_io.emit('device_id', DEVICE_ID)
})

function print_screen (data) {
  var cmd = JSON.parse(data)
  var text = ''
  var pos_x = 2
  var pos_y = 16
  const line_offset = 10
  var index = 0
  var x_offset = 0
  switch (cmd.sensor_type) {
    case 'air_temperature':
      text = 'T:' + cmd.sensor_value + 'C  '
      index = 1
      x_offset = 0
      break
    case 'air_pressure':
      text = 'P:' + cmd.sensor_value + 'kPa '
      index = 1
      x_offset = 54
      break
    case 'air_humidity':
      text = 'RH:' + cmd.sensor_value + '%  '
      index = 2
      x_offset = 0
      break
    case 'air_co2':
      text = 'co2:' + cmd.sensor_value + 'ppm  '
      index = 3
      x_offset = 0
      break
    case 'light_par':
      text = 'PAR:' + cmd.sensor_value + 'mol/s/m  '
      index = 4
      x_offset = 0
      break
    case 'relay1':
      if (cmd.sensor_value == '1') text = '1:ON '
      else text = '1:OFF'
      index = 4
      x_offset = 0
      break
    case 'relay2':
      if (cmd.sensor_value == '1') text = '2:ON '
      else text = '2:OFF'
      index = 4
      x_offset = 30
      break
    case 'relay3':
      if (cmd.sensor_value == '1') text = '3:ON '
      else text = '3:OFF'
      index = 4
      x_offset = 60
      break
    case 'relay4':
      if (cmd.sensor_value == '1') text = '4:ON '
      else text = '4:OFF'
      index = 4
      x_offset = 90
      break
    case 'relay0':
      if (cmd.sensor_value == '1') text = 'Actuator:ON '
      else text = 'Actuator:OFF'
      index = 4
      x_offset = 0
      break
    case 'ip':
      index = 0
      x_offset = 0
      text = cmd.sensor_value
      break
    default:
      return
  }
  var msg2send = JSON.stringify({
    print_x: (pos_x + x_offset).toString(),
    print_y: (pos_y + line_offset * index).toString(),
    print: text
  })
  my_io.emit('lcd', msg2send)
}

var loop = 0
var ctrl_loop = 0
var tokenBusyTimeout = 0
var dest = null
// used to schedule read write on i2C bus and prevent driver issue with concurrent access
// not used for relay_native
var i2c_scheduler = setInterval(function () {
  if (schedulerBusy) {
    // logger.log("scheduler busy");
    tokenBusyTimeout++
    if (tokenBusyTimeout < 500) {
      return
    } else {
      tokenBusyTimeout = 0
      logger.log('scheduler released after 5sec timeout from ' + dest)
      logger.log('disabling sensor until next restart ')
      if (dest == 'air_co2') co2_sensor_enabled = false
      else if (dest == 'air_pressure') barometer_sensor_enabled = false
      else if (dest == 'air_humidity') relative_humidity_sensor_enabled = false
      else if (dest == 'light_par') par_sensor_enabled = false
      else if (dest == 'lcd') lcd_enabled = false
    }
  }
  dest = null
  tokenBusyTimeout = 0
  if (loop == 100) {
    loop = 0
    dest = ''
  } else {
    loop++
    if (loop % 2) {
      if (ctrl_loop == 0) {
        if (relay_enabled) dest = 'relay'
        ctrl_loop = 1
      } else {
        if (lcd_enabled) dest = 'lcd'
        ctrl_loop = 0
      }
    } else {
      if (loop == 4) getIpAddr()
      if (loop == 30) {
        // logger.log("schedule air_co2");
        if (co2_sensor_enabled) dest = 'air_co2'
      } else if (loop == 50) {
        // logger.log("schedule air_pressure");
        if (barometer_sensor_enabled) dest = 'air_pressure' // will also send air temperature
      } else if (loop == 70) {
        if (relative_humidity_sensor_enabled) dest = 'air_humidity'
      } else if (loop == 84) {
        if (par_sensor_enabled) dest = 'light_par'
      }

      // logger.log("schedule sensor /"+loop);
    }
  }

  if (dest) {
    schedulerBusy = true
    my_io.emit('scheduler', dest)
  }
}, 10)
