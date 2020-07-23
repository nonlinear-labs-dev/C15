#!/bin/sh

mkdir -p /workdir
mount -o loop /bindir/fs.ext4 /workdir
fuse-overlayfs -o lowerdir=/workdir/squashfs-root -o upperdir=/workdir/overlay-scratch -o workdir=/workdir/overlay-workdir /workdir/overlay-fs
mkdir -p /workdir/overlay-fs/sources
mkdir -p /workdir/overlay-fs/build
mkdir -p /workdir/epc-nl-build
mount --bind /sources /workdir/overlay-fs/sources
mount --bind /workdir/epc-nl-build /workdir/overlay-fs/build

/workdir/overlay-fs/bin/arch-chroot /workdir/overlay-fs /bin/bash
