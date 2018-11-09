#!/bin/bash
echo ""

echo "Enter the device id (ex: 101), followed by [ENTER]: "

read device_id

echo "Enter the device zone (ex: myfood-eu), followed by [ENTER]: "

read device_zone

echo "Creating /etc/profile.d/env_vars.sh file"

echo "Setting root password"

# TODO

mkdir -p /etc/profile.d/
touch /etc/profile.d/env_vars.sh
echo "export DEVICE_ID=$device_id" > /etc/profile.d/env_vars.sh
echo "export DEVICE_ZONE=$device_zone" >> /etc/profile.d/env_vars.sh
chmod +x /etc/profile.d/env_vars.sh

echo "Modifying /etc/hosts and /etc/hostname"
echo "predictable$device_id" > /etc/hostname
echo -e "127.0.0.1  predictable$device_id\n$(cat /etc/hosts)" > /etc/hosts

mkdir -p /home/root/predictable-farm

echo "All done. Delete this script ? [Y/n], followed by [ENTER]:"

read cleanup

if [ "$cleanup" != "${cleanup#[Yy]}" ] ;then
    rm -f /home/root/bootstrap.sh
fi

echo "Done."

