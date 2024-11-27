#!/bin/sh

abort()
{
	umount $dir 2>/dev/null
	rm -rf $dir
	echo >&2 "fail to install"
    exit 0
}

trap 'abort' 0
set -eu

dir=$(mktemp -d)
nsec=$((($(blockdev --getsz $1) - 64)/2))

if [ ${nsec} -ge $((16*1024)) ]
	then
		nsec=$((16*1024-64/2))
fi

mkfs.fat -F16 -R 1 -r 512 -a -I --offset 64 $1 $nsec

mount -o offset=$((512*64)) $1 $dir

mkdir -p $dir/root

cp -f build/boot  $dir/root/boot

trap - 0
umount $dir >&2
rm -rf $dir
sync
exit 1
