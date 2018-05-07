#!/bin/sh
WORKDIR=/home/root/predictable-farm

echo ""
echo "----------"
echo "POSTINSTALL : chmod +x"
chmod +x $WORKDIR/*.sh
chmod +x $WORKDIR/scripts/*.sh

echo "POSTINSTALL : Restarting forever"
/usr/lib/node_modules/forever/bin/forever stopall
cd $WORKDIR && /usr/lib/node_modules/forever/bin/forever start forever.json
