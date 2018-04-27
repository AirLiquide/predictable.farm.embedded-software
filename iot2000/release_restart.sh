#!/bin/sh
WORKDIR=/home/root/predictable-farm

echo "POSTINSTALL : chmod +x"
chmod +x $WORKDIR/*

echo "POSTINSTALL : remove update package from target"
rm $WORKDIR/*.swu

echo "POSTINSTALL : Restarting forever"
forever stopall
cd $WORKDIR && forever start forever.json
