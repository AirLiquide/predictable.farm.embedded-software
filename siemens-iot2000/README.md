Predictable Farm — Embedded software — Siemens iot2000 flavour
---

### Introduction

This is the on-board software for the predictable farm devices.
It is self-updateable via [SWUpdate](https://github.com/sbabic/swupdate).

**Configuration**

The zone pattern indicate the naming pattern for the farms, e.g. `http://farm1.my.domain`, `http://farm2.my.domain`, if necessary. Else, the default server url is used.

Devices are supposed to be hostnamed as per the following pattern : `predictable{XXX}.local` where `{XXX}` is an integer.

### Installation

Dependencies : **mraa** and **socket.io**

> This software is to be installed on the device; hence, having limited resources, we favor a general installation of dependencies, not using `npm`.
> As such, node modules should be installed in `/usr/lib/node_modules/` beforehand; We do not provide a `package.json` file for this repo (but it would be trivial to do so if you need it).

The `start.sh`  script should be launched at boot time.

The `local-engine.js` file is the compiled bundle from the [Predictable farm offline strategy generator](http://airliquide.github.com/predictable.farm.embedded-software.offline-strategy-generator). If the latter is updated, compile it for `iot2000` and put it in the root folder of your installation.

### To start everything

    cd predictable-farm
    forever start forever.json

### To stop all services 

    forever stopall

### To operate on a single script

    forever restart <script>

### Relevant documentation

  - [Intel IOT MRAA lib](http://iotdk.intel.com/docs/master/mraa/node/classes/i2c.html#method_readWordReg)
  - [MRAA Galileo Gen 2 limitations](https://github.com/intel-iot-devkit/mraa/blob/master/docs/galileorevh.md)
  - [Galileo Gen 2 Pin mapping](https://link.springer.com/chapter/10.1007/978-1-4302-6838-3_14)
  - [Quark X1000 i2C documentation](https://github.com/slavaim/Linux-Intel-Galileo-Gen2-QuarkX1000/tree/master/BSPv1.1.0/linux_v3.8.7/work/Documentation/i2c)

### To enable / disable the i2c bus power

    mraa-gpio set 04 00 # Disable
    mraa-gpio set 04 01 # Enable

### To force-stop the i2c bus

    # Force deactivate PIN gpio6 (IO4) - Bus I2C
    echo 0 > /sys/class/gpio/gpio6/value

### To create a new release / update the code

We use SWupdate (https://github.com/sbabic/swupdate) with its Yocto layer : https://github.com/sbabic/meta-swupdate/tree/morty.

    ./create_release.sh

This will create a `predictable-farm_<version>.swu` file in the root folder, that can be used with swupdate on the target device :

    swupdate_unstripped --hwrevision iot2040:rev1 -i predictable-farm_<version>.swu

> release_install.sh and release_restart.sh are pre and post install scripts used by swupdate during the update process.

### CAD Files

In order to build the necessary interface board, the CAD folder includes : the Fritzing source design + the BOM and relevant datasheets.

### License

MIT. See License.txt file

**This work uses sofware that is licensed under GPL-2.0. The respective files have kept their original license notices.**