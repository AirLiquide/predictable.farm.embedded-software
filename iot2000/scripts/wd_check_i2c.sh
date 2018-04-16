#!/bin/sh
# while true
# do
if mraa-i2c get 0 0x27 0 | grep "Register 00 = 00" > /dev/null
then
  echo "i2c ok"
  # exit 0
else
  echo "i2c ko"
  # Cycling power on pin IO 5 (gpio0)
  # Make sure pin is an output
  echo "out" > gpio0/direction
  # Set to 0, wait 3 seconds, back to 1
  echo 0 > /sys/class/gpio/gpio0/value
  sleep 3 # secs
  echo 1 > /sys/class/gpio/gpio0/value
  # exit 255 #reboot
fi
# done