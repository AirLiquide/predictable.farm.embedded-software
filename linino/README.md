
Predictable Farm Yùn Bridge
---

Needs NodeJS 0.10.* and BusyBox.

#### ID bootstrapping

To include the correct id, you must run the `scripts/bootstrap.sh` script first, with the correct ID in the **valueID** file.

#### How to install the bridge service

Node modules should be installed in `/usr/lib/node_modules/` beforehand; NPM is not a valid choice to install dependencies on this system.

copy `bridge.service` to `/etc/init.d/bridge` and make it executable :

    cp /root/bridge.service /etc/init.d/bridge
    chmod +x /etc/init.d/bridge

Enable it for auto starting at boot :

    /etc/init.d/bridge enable

#### Crontab

Make the reboot script executable if necessary :

    chmod +x /root/scripts/safeRebootAtMidnight.sh

And create a **crontab** for root :

    5 0 * * * /root/scripts/safeRebootAtMidnight.sh

#### Update code

Use the update script :

    /scripts/update.sh

