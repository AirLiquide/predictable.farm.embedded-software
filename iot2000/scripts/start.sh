#!/bin/sh
WORKDIR=/home/root/predictable-farm

cd $WORKDIR

$WORKDIR/userled.py 3 #set led to orange

forever start -a -l /var/log/service.log -p /var/log/ $WORKDIR/service.js 100 myfood  &
sleep 2;
forever start -a -l /var/log/lcd.log -p /var/log/ $WORKDIR/peripherals/lcd.js &
sleep 2;
forever start -a -l /var/log/relay_native.log -p /var/log/ $WORKDIR/peripherals/relay_native.js  &
sleep 2;
forever start -a -l /var/log/CO2_MHZ16_i2c.log -p /var/log/ $WORKDIR/peripherals/CO2_MHZ16_i2c.js  &
sleep 2;
forever start -a -l /var/log/Barometer_MPL115A2_i2c.log -p /var/log/ $WORKDIR/peripherals/Barometer_MPL115A2_i2c.js &
sleep 2;
forever start -a -l /var/log/PAR_ADS1015_i2c.log -p /var/log/ $WORKDIR/peripherals/PAR_ADS1015_i2c.js &
sleep 2;
forever start -a -l /var/log/RelativeHumidity_SHT_i2c.log -p /var/log/ $WORKDIR/peripherals/RelativeHumidity_SHT_i2c.js &
sleep 2;
forever start -a -l /var/log/shell.log -p /var/log/ $WORKDIR/shell.js &

#node $WORKDIR/service.js 7 myfood  &> $WORKDIR/service.log
#node $WORKDIR/lcd.js &> $WORKDIR/lcd.log
#node $WORKDIR/Barometer_MPL115A2_i2c.js &> $WORKDIR/Barometer_MPL115A2_i2c.log
#node $WORKDIR/CO2_MHZ16_i2c.js &> $WORKDIR/CO2_MHZ16_i2c.log
#node $WORKDIR/relay_native.js &> $WORKDIR/relay_native.log
#node $WORKDIR/PAR_ADS1015_i2c.js &> $WORKDIR/PAR_ADS1015_i2c.log

forever restart -a -l /var/log/local.log -p /var/log/ local.js 100 myfood  &
forever restart -a -l /var/log/lcd.log -p /var/log/ peripherals/lcd.js &
tail -f /var/log/*.log