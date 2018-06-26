#!/bin/sh
WORKDIR=/home/root/predictable-farm

cd $WORKDIR

echo "Setting orange LED"
./services/userled.py 3 #set led to orange

# See https://link.springer.com/chapter/10.1007/978-1-4302-6838-3_14
# Pin IO4 => BUS I2C
echo "Exporting and setting PIN gpio6 (IO4) - Bus I2C"
mraa-gpio set 04 01 # Activate bus I2C

# Pin IOA1 => 24V/VIN
echo "Exporting and setting PIN gpio50 (IO15 / ADC A1) - 24V/VIN"
# mraa-gpio set 15 01

echo "Starting forever with configuration file"
# Why setting home here ? See https://github.com/foreverjs/forever/issues/415
HOME=/home/root/ forever -p /home/root/.forever start forever.json


# Force deactivate PIN gpio6 (IO4) - Bus I2C
# echo 0 > /sys/class/gpio/gpio6/value