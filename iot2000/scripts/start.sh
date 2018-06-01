#!/bin/sh
WORKDIR=/home/root/predictable-farm

cd $WORKDIR

echo "Setting orange LED"
./services/userled.py 3 #set led to orange

# Pin IO4 => 24V
echo "Exporting and setting PIN gpio6 (IO4) - 24V/VIN"
echo 6 > /sys/class/gpio/export || echo "Pin 6 (IO4) already exported"
echo "out" > /sys/class/gpio/gpio6/direction
echo 1 > /sys/class/gpio/gpio6/value

# Pin IO5 = BUS I2C
echo "Exporting and setting PIN gpio0 (IO5) - Bus I2C"
echo 0 > /sys/class/gpio/export || echo "Pin 0 (IO5) already exported"
echo "out" > /sys/class/gpio/gpio0/direction
echo 1 > /sys/class/gpio/gpio0/value

echo "Starting forever with configuration file"
# Why setting home here ? See https://github.com/foreverjs/forever/issues/415
HOME=/home/root/ forever -p /home/root/.forever start forever.json

# forever start -a -l /var/log/service.log -p /var/log/ $WORKDIR/service.js 100 myfood  &
# sleep 2;
# forever start -a -l /var/log/lcd.log -p /var/log/ $WORKDIR/peripherals/lcd.js &
# sleep 2;
# forever start -a -l /var/log/relay_native.log -p /var/log/ $WORKDIR/peripherals/relay_native.js  &
# sleep 2;
# forever start -a -l /var/log/CO2_MHZ16_i2c.log -p /var/log/ $WORKDIR/peripherals/CO2_MHZ16_i2c.js  &
# sleep 2;
# forever start -a -l /var/log/Barometer_MPL115A2_i2c.log -p /var/log/ $WORKDIR/peripherals/Barometer_MPL115A2_i2c.js &
# sleep 2;
# forever start -a -l /var/log/PAR_ADS1015_i2c.log -p /var/log/ $WORKDIR/peripherals/PAR_ADS1015_i2c.js &
# sleep 2;
# forever start -a -l /var/log/RelativeHumidity_SHT_i2c.log -p /var/log/ $WORKDIR/peripherals/RelativeHumidity_SHT_i2c.js &
# sleep 2;
# forever start -a -l /var/log/shell.log -p /var/log/ $WORKDIR/shell.js &

# node $WORKDIR/service.js 7 myfood  &> $WORKDIR/service.log
# node $WORKDIR/lcd.js &> $WORKDIR/lcd.log
# node $WORKDIR/Barometer_MPL115A2_i2c.js &> $WORKDIR/Barometer_MPL115A2_i2c.log
# node $WORKDIR/CO2_MHZ16_i2c.js &> $WORKDIR/CO2_MHZ16_i2c.log
# node $WORKDIR/relay_native.js &> $WORKDIR/relay_native.log
# node $WORKDIR/PAR_ADS1015_i2c.js &> $WORKDIR/PAR_ADS1015_i2c.log
