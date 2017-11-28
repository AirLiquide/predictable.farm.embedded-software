#!/bin/sh

# Warn bridge that we're going for reboot in 90secs
touch /root/reboot.lock

sleep 90

# Now remove the lock file
rm reboot.lock

# Log this
date >> /root/reboot.log
sleep 1

# Finally reboot
/sbin/reboot