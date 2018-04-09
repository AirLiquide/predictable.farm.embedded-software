const MIRROR_SERVER ='http://mirror.predictable.run:3000' ;

var shell = require('/usr/lib/node_modules/socket.io-client')(MIRROR_SERVER);

var os = require("os");

var exec = require('child_process').exec;

shell.on('connect', function() {
    shell.emit("get_port", os.hostname());
});

shell.on('disconnect', function() {});

shell.on('open_tunnel', function (port) {
    //exec('ssh -R 0.0.0.0:'+port+'+:0.0.0.0:22 ubuntu@mirror.predictable.run -i /root/id_rsa -y -N');
    console.log(os.arch());
    if(os.arch() == 'ia32')  
    {  
        console.log("openssh on remote server port "+port);
        //exec('kill -9 $(pgrep -x ssh); sleep 1');
        exec('if pgrep -x ssh; then echo "already running"; else ssh -i /home/root/predictablefarm.shellmirror.pem -Ng -R '+port+':127.0.0.1:22 ubuntu@mirror.predictable.run  ; fi '); //openssh on yocto
    } 
    else 
    {   
        console.log("dropbear on remote server port "+port);
        exec('kill -9 $(pgrep -l ssh)');
        exec('ssh -R 0.0.0.0:'+port+'+:0.0.0.0:22 ubuntu@mirror.predictable.run -i /root/id_rsa -y -N'); //dropbear on linino   
    }
    shell.emit("tunnel_ok", os.hostname());
});
/*
shell.on('reset', function () {
    exec('reset-mcu');
    shell.emit("reset-mcu_ok", os.hostname());
});

shell.on('reboot', function () {
    shell.emit("reboot_ok", os.hostname());
    exec('reboot');
});

shell.on('update', function () {
    shell.emit("update_ok", os.hostname());
    exec('/etc/init.d/bridge update');
});
*/