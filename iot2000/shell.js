'use strict'

const MIRROR_SERVER = 'http://mirror.predictable.run:3000'

const shell = require('/usr/lib/node_modules/socket.io-client')(MIRROR_SERVER)

const os = require('os')
const exec = require('child_process').exec

shell.on('connect', () => {
  console.log('[Remote-shell] Connected, emiting "get_port"')
  shell.emit('get_port', os.hostname())
})

shell.on('disconnect', () => {
  console.log('[Remote-shell] Disconnected')
})

shell.on('open_tunnel', (port) => {
  if (os.arch() == 'ia32') {
    // On YOCTO, it's OpenSSH
    console.log('[Remote-shell] OpenSSH on remote server port %d', port)
    // exec('kill -9 $(pgrep -x ssh); sleep 1');
    exec('if pgrep -x ssh; then echo "already running"; else ssh -i /home/root/predictablefarm.shellmirror.pem -N -R ' + port + ':localhost:22 ubuntu@mirror.predictable.run; fi ')
  } else {
    // On Linino, it's DropBear
    console.log('[Remote-shell] Dropbear on remote server port %d', port)
    exec('kill -9 $(pgrep -l ssh)')
    exec('ssh -R 0.0.0.0:' + port + '+:0.0.0.0:22 ubuntu@mirror.predictable.run -i /root/id_rsa -y -N') // dropbear on linino
  }
  shell.emit('tunnel_ok', os.hostname())
})
