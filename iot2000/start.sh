#!/bin/sh
HOME=/home/root
cd $HOME
$HOME/userled.py 3 #set led to orange
forever start -a -l /var/log/lcd.log -p /var/log/ $HOME/lcd.js &
sleep 2;
forever start -a -l /var/log/service.log -p /var/log/ $HOME/service.js 100 myfood  &
sleep 2;
forever start -a -l /var/log/relay_native.log -p /var/log/ $HOME/relay_native.js  &
sleep 2;
forever start -a -l /var/log/CO2_MHZ16_i2c.log -p /var/log/ $HOME/CO2_MHZ16_i2c.js  &
sleep 2;
forever start -a -l /var/log/Barometer_MPL115A2_i2c.log -p /var/log/ $HOME/Barometer_MPL115A2_i2c.js &
sleep 2;
forever start -a -l /var/log/PAR_ADS1015_i2c.log -p /var/log/ $HOME/PAR_ADS1015_i2c.js &
sleep 2;
forever start -a -l /var/log/RelativeHumidity_SHT_i2c.log -p /var/log/ $HOME/RelativeHumidity_SHT_i2c.js &
sleep 2;
forever start -a -l /var/log/shell.log -p /var/log/ $HOME/shell.js &

#node $HOME/service.js 7 myfood  &> $HOME/service.log
#node $HOME/lcd.js &> $HOME/lcd.log
#node $HOME/Barometer_MPL115A2_i2c.js &> $HOME/Barometer_MPL115A2_i2c.log
#node $HOME/CO2_MHZ16_i2c.js &> $HOME/CO2_MHZ16_i2c.log
#node $HOME/relay_native.js &> $HOME/relay_native.log
#node $HOME/PAR_ADS1015_i2c.js &> $HOME/PAR_ADS1015_i2c.log