#!/bin/sh
WORKDIR=/home/root/predictable-farm

echo ""
echo "----------"
echo "POSTINSTALL : chmod +rx scripts"
chmod -R +rx $WORKDIR/*.js
chmod -R +rx $WORKDIR/scripts/*.sh
chmod +rx $WORKDIR/services/*.py
