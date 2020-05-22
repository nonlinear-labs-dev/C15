FROM ubuntu:19.10

RUN apt-get update -y && apt-get remove -y gstreamer1.0 libwayland-server0 x11-common && apt-get install -y --fix-missing mtools qemu-system-x86 sshpass bash fuseiso squashfs-tools sudo genisoimage xorriso syslinux-utils fuse-overlayfs fuse3 squashfuse wget
