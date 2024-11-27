#!/bin/sh

S=$(pwd)/build
img=$1

set -e

make

if [ -z ${1} ]
	then
		img=$S/img
		rm -rf $img 
		dd if=/dev/zero of=$img bs=512 count=262144
fi

sudo ${S}/installmbr $img
sudo ${S}/installboot $img
