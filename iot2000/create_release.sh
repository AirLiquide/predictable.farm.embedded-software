CONTAINER_VER="2.0-siemens"
PRODUCT_NAME="predictable-farm"
PACKAGING_DATE=$(date '+%d%m%Y_%H%M%S');
OS=$(uname -s)

if [[ $OS == 'Linux' ]]; then
      FORMAT="crc"
else
      FORMAT="newc"
fi

echo "Packaging release on $PACKAGING_DATE"
echo "Using format : $FORMAT"
echo ""

FILES="sw-description \
       release_install.sh \
       release_restart.sh \
       config/* \
       lib/* \
       peripherals/* \
       scripts/* \
       services/* \
       service.js \
       shell.js \
       local-engine.js \
       forever.json \
      "

for i in $FILES;do
        echo $i;done | cpio -ov -H $FORMAT > ${PRODUCT_NAME}_${CONTAINER_VER}_${PACKAGING_DATE}.swu

echo ""
echo "SWU release file '${PRODUCT_NAME}_${CONTAINER_VER}_${PACKAGING_DATE}.swu' created"