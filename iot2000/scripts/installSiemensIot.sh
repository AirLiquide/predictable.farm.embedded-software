vi /etc/opkg/opkg.conf
# add : 
src all http://downloads.yoctoproject.org/releases/yocto/yocto-2.1/ipk/i586/


opkg update

opkg install watchdog
sed -i "/#test-binary/c\test-binary = /home/root/scripts/wd_check_i2c.sh" /etc/watchdog.conf
/etc/init.d/watchdog.sh restart


npm install -g socket.io-client
npm install -g mraa
npm install -g forever

# at init stage, launch 
socat -d -d pty,raw,echo=0 pty,raw,echo=0 

# it wil create pseudo terminal under /dev/pts/0 and /dev/pts/1
# we will use those to fake arduino communication


