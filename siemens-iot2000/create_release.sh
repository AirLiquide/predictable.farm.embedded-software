#!/bin/sh

# Copyright (C) Air Liquide S.A,  2017
# Author: Sébastien Lalaurette and Cyril Chapellier, La Factory, Creative Foundry
# This file is part of Predictable Farm project.
# 
# The MIT License (MIT)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# 
# See the LICENSE.txt file in this repository for more information.

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
       local-engine.js \
       forever.json \
      "

for i in $FILES;do
        echo $i;done | cpio -ov -H $FORMAT > ${PRODUCT_NAME}_${CONTAINER_VER}_${PACKAGING_DATE}.swu

echo ""
echo "SWU release file '${PRODUCT_NAME}_${CONTAINER_VER}_${PACKAGING_DATE}.swu' created"
