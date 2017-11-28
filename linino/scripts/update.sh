#!/bin/bash
logger "test_js"
if [ -f /mnt/sdb1/Upgrade.js ]
then
  #logger "test _js: in"
  if [ "$(awk '/Version/' /mnt/sdb1/Upgrade.js | cut -d ':' -f2)" != "$(awk '/Version/' /root/bridge.js | cut -d ':' -f2)" ];
  then
    #logger "backup"
    cp /root/bridge.js /root/backup/bridge.backup.$(date +%F_%R).js
    #logger "copy"
    cp /mnt/sdb1/Upgrade.js /root/bridge.js
    #rm bridge2.js
    #logger "start node"
    killall node
    node /root/bridge.js &
    logger "done"
  fi

fi
      #  echo "No file"
if [ -f /mnt/sdb1/Upgrade.ino.hex ]
then
  mkdir on
  merge-sketch-with-bootloader.lua /mnt/sdb1/Upgrade.ino.hex
  run-avrdude /mnt/sdb1/Upgrade.ino.hex
  logger "Upgrade: arduino"
  rmdir on
fi

exit
