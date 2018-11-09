#!/bin/bash
# Copyright (C) Air Liquide S.A,  2017
# Author: Sébastien Lalaurette and Gwennaëlle Remy, La Factory, Creative Foundry
# This file is part of Predictable Farm project.
# 
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# 
# See the LICENSE.txt file in this repository for more information.

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
