#!/bin/sh
WORKDIR=/home/root/predictable-farm

echo "PREINSTALL : Creating directory structure"
cd $WORKDIR
mkdir -p lib config graphs logs peripherals scripts services
