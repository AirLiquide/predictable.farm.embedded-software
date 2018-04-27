Predictable Farm — iot2000 flavour
---

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

    swupdate_unstripped -i predictable-farm_<version>.swu --hwrevision iot2040:rev1

> release_install.sh and release_restart.sh are pre and post install scripts used by swupdate during the update process.