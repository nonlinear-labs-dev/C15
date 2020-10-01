FROM ubuntu:20.04

ENV LANG en_US.UTF-8
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y && apt-get remove -y gstreamer1.0 libwayland-server0 x11-common && apt-get install -y --fix-missing mtools qemu-system-x86 sshpass bash fuseiso squashfs-tools sudo genisoimage xorriso syslinux-utils fuse-overlayfs fuse3 squashfuse wget
