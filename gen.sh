#!/bin/sh

# dd if=/dev/zero of=./boot bs=1k count=1440 ==> make disk
# fdisk ./boot ==> make  partion

dd if=./boot.bin of=./boot ibs=446 obs=446 count=1 conv=notrunc
dd if=./kernel.bin of=./boot obs=512 seek=1 conv=notrunc

# qemu-system-i386 boot
