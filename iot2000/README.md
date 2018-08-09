Predictable Farm — iot2000 flavour
---

### Relevant documentation

  - [Intel IOT MRAA lib](http://iotdk.intel.com/docs/master/mraa/node/classes/i2c.html#method_readWordReg)
  - [MRAA Galileo Gen 2 limitations](https://github.com/intel-iot-devkit/mraa/blob/master/docs/galileorevh.md)
  - [Galileo Gen 2 Pin mapping](https://link.springer.com/chapter/10.1007/978-1-4302-6838-3_14)
  - [Quark X1000 i2C documentation](https://github.com/slavaim/Linux-Intel-Galileo-Gen2-QuarkX1000/tree/master/BSPv1.1.0/linux_v3.8.7/work/Documentation/i2c)

### To enable / disable the i2c bus power

    mraa-gpio set 04 00
    mraa-gpio set 04 01

### To force-stop the i2c bus

    # Force deactivate PIN gpio6 (IO4) - Bus I2C
    echo 0 > /sys/class/gpio/gpio6/value

### To stop all services 

    forever stopall

### To start everything

    cd predictable-farm
    forever start forever.json

### To operate on a single script

    forever restart <script>

### To create a new release

We use SWupdate (https://github.com/sbabic/swupdate) with its Yocto layer : https://github.com/sbabic/meta-swupdate/tree/morty.

    ./create_release.sh

This will create a `predictable-farm_<version>.swu` file in the root folder, that can be used with swupdate on the target device :

    swupdate_unstripped --hwrevision iot2040:rev1 -i predictable-farm_<version>.swu

> release_install.sh and release_restart.sh are pre and post install scripts used by swupdate during the update process.

### Various information gathered around the i2C problems

#### >
To load the I2C driver in isolation from GPIO, use :

    modprobe intel_qrk_gip gpio=0
    modprobe intel_qrk_gip gpio=0 enable_msi=0

#### >
When a pull-up/pull-down resistor is enabled on A4/A5, an I2C bus is blocked with messages related to i2c designware problems.