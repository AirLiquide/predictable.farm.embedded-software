#!/bin/sh
WORKDIR=/home/root/predictable-farm

echo ""
echo "----------"
echo "PREINSTALL : Creating directory structure if necessary"

cd $WORKDIR
mkdir -p lib config graphs logs peripherals scripts services
