#!/bin/sh
WORKDIR=/home/root/predictable-farm

echo "POSTINSTALL : Restarting forever"
forever stopall
forever start $WORKDIR/forever.json
