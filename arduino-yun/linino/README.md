Predictable Farm — Embedded software — Yùn Bridge Flavour
---

Needs NodeJS 0.10.* and BusyBox.

### Installation

Dependencies : **serialport** and **socket.io-client**

> This software is to be installed on the device; hence, having limited resources, we favor a general installation of dependencies, not using `npm`.
> As such, node modules should be installed in `/usr/lib/node_modules/` beforehand; We do not provide a `package.json` file for this repo (but it would be trivial to do so if you need it).

#### ID bootstrapping

To include the correct id, you must run the `scripts/bootstrap.sh` script first, with the correct ID in the **valueID** file.

#### How to install the bridge service

Node modules should be installed in `/usr/lib/node_modules/` beforehand; NPM is not a valid choice to install dependencies on this system.

copy `bridge.service` to `/etc/init.d/bridge` and make it executable :

    cp /root/bridge.service /etc/init.d/bridge
    chmod +x /etc/init.d/bridge

Enable it for auto starting at boot :

    /etc/init.d/bridge enable

#### Swap

It's better to have some swap to manage RAM overflows :

    cd /
    mkdir swap
    dd if=/dev/zero of=/swap/yunswapfile bs=1M count=512
    # wait a bit here, takes ~10min
    mkswap /swap/yunswapfile
    swapon /swap/yunswapfile
    uci add fstab swap
    uci set fstab.@swap[0].device=/swap/yunswapfile
    uci set fstab.@swap[0].enabled=1
    uci set fstab.@swap[0].fstype=swap
    uci set fstab.@swap[0].options=default
    uci set fstab.@swap[0].enabled_fsck=0
    uci commit # might trigger some errors, but that's ok
    reboot

#### Crontab

Make the reboot script executable if necessary :

    chmod +x /root/scripts/safeRebootAtMidnight.sh

And create a **crontab** for root :

    5 0 * * * /root/scripts/safeRebootAtMidnight.sh

#### Update code

Use the update script :

    /scripts/update.sh

