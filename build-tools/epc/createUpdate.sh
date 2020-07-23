#!/bin/sh

NONLINEAR_PACKAGE_CACHE="https://nonlinearlabs.s3.eu-central-1.amazonaws.com/"
PACKAGES_SERVER="https://archive.archlinux.org/packages/"

# https://archive.archlinux.org/packages/l/libdrm/


PACKAGE_ENDINGS="\
-x86_64.pkg.tar.xz \
-x86_64.pkg.tar.zst \
-any.pkg.tar.xz \
-any.pkg.tar.zst \
"

PACKAGES_TO_INSTALL="\
adwaita-icon-theme,3.32.0-1 \
aom,1.0.0.errata1-1 \
atk,2.32-1 \
at-spi2-atk,2.32.0-1 \
at-spi2-core,2.32.1-1 \
avahi,0.7+18+g1b5f401-2 \
cairo,1.16.0-2 \
cdparanoia,10.2-7 \
chromium,76.0.3809.87-1 \
colord,1.4.4-2 \
desktop-file-utils,0.24-1 \
ffmpeg,1:4.1.4-1 \
fontconfig,2:2.13.1+12+g5f5ec56-1 \
fribidi,1.0.5-1 \
fuse-common,3.9.0-1 \
fuse3,3.9.0-1 \
gdk-pixbuf2,2.38.1-1 \
gnu-free-fonts,20120503-6 \
graphene,1.8.6-1 \
gst-plugins-base,1.16.0-3 \
gst-plugins-base-libs,1.16.0-3 \
gstreamer,1.16.0-2 \
gtk3,1:3.24.10-1 \
gtk-update-icon-cache,1:3.24.10-1 \
iso-codes,4.3-1 \
jack,0.125.0-7 \
jasper,2.0.16-1 \
json-glib,1.4.4-1 \
libass,0.14.0-1 \
libasyncns,0.8+3+g68cd5af-2 \
libavc1394,0.5.4-3 \
libbluray,1.1.2-1 \
libcanberra,0.30+2+gc0620e4-2 \
libcups,2.2.11-2 \
libdatrie,0.2.12-1 \
libdrm,2.4.99-1 \
libdrm,2.4.99-1 \
libepoxy,1.5.3-1 \
libfontenc,1.1.4-1 \
libglvnd,1.1.1-1 \
libglvnd,1.1.1-1 \
libgusb,0.3.0-1 \
libibus,1.5.20-2 \
libice,1.0.10-1 \
libiec61883,1.2.0-5 \
libmodplug,0.8.9.0-2 \
libomxil-bellagio,0.9.3-2 \
libomxil-bellagio,0.9.3-2 \
libpciaccess,0.16-1 \
libpulse,12.2-2 \
libraw1394,2.1.2-2 \
librsvg,2:2.44.14+1+g17bb3cd6-1 \
libsm,1.2.3-1 \
libsm,1.2.3-1 \
libsoxr,0.1.3-1 \
libssh,0.9.0-1 \
libthai,0.1.28-1 \
libtheora,1.1.1-4 \
libunwind,1.3.1-1 \
libunwind,1.3.1-1 \
libva,2.5.0-1 \
libvdpau,1.2-1 \
libvisual,0.4.0-7 \
libvpx,1.8.1-1 \
libwebp,1.0.3-1 \
libxcomposite,0.4.5-1 \
libxcursor,1.2.0-1 \
libxdamage,1.1.5-1 \
libxdamage,1.1.5-1 \
libxfixes,5.0.3-2 \
libxfixes,5.0.3-2 \
libxfont2,2.0.3-1 \
libxft,2.3.3-1 \
libxi,1.7.10-1 \
libxinerama,1.1.4-1 \
libxkbcommon,0.8.4-1 \
libxkbfile,1.1.0-1 \
libxmu,1.1.3-1 \
libxrandr,1.5.2-1 \
libxrender,0.9.10-2 \
libxshmfence,1.3-1 \
libxslt,1.1.33-1 \
libxss,1.2.3-1 \
libxt,1.2.0-1 \
libxtst,1.2.3-2 \
libxv,1.0.11-2 \
libxxf86vm,1.1.4-2 \
llvm-libs,8.0.1-1 \
l-smash,2.14.5-1 \
mc,4.8.24-2 \
mesa,19.1.3-1 \
minizip,1:1.2.11-3 \
openjpeg2,2.3.1-1 \
opus,1.3.1-1 \
orc,0.4.29-1 \
pango,1:1.44.1-1 \
pipewire,0.2.6+1+g37613b67-1 \
pixman,0.38.4-1 \
pixman,0.38.4-1 \
re2,20190801-1 \
rest,0.8.1-1 \
rtkit,0.12-1 \
sbc,1.4-1 \
sdl2,2.0.10-1 \
snappy,1.1.7-1 \
sound-theme-freedesktop,0.8-3 \
speex,1.2.0-2 \
speexdsp,1.2.0-1 \
sshfs,3.7.0-1 \
tdb,1.3.18-2 \
v4l-utils,1.16.6-1 \
vid.stab,1.1-2 \
wayland,1.17.0-1 \
wayland,1.17.0-1 \
wayland-protocols,1.18-1 \
x264,2:157.r72db4377-1 \
x265,3.1.1-1 \
xdg-utils,1.1.3-3 \
xkeyboard-config,2.27-1 \
xorg-server-common,1.20.5-2 \
xorg-server,xvfb-1.20.5-2 \
xorg-setxkbmap,1.3.2-1 \
xorg-xauth,1.1-1 \
xorg-xkbcomp,1.4.2-1 \
xorg-xset,1.2.4-1 \
xvidcore,1.3.5-1 \
zita-alsa-pcmi,0.3.2-1 \
zita-resampler,1.6.2-1 \
"

copy_running_os() {
    echo "Copying running os..."
    if tar -C /internal/os --exclude=./build/CMakeFiles -czf /update-scratch/update/NonLinuxOverlay.tar.gz .; then
        echo "Copying running os done."
        return 0
    fi
    echo "Copying running os failed."
    return 1
}

calc_checksum() {
    echo "Calc checksum..."
    if (cd /update-scratch/update/ && touch $(sha256sum ./NonLinuxOverlay.tar.gz | grep -o "^[^ ]*").sign); then
        echo "Calc checksum done."
        return 0
    fi
    echo "Calc checksum failed."
    return 1
}

create_update_tar() {
    echo "Create update.tar..."
    if (cd /update-scratch/ && tar -cf /update.tar ./update); then
        echo "Create update.tar done."
        return 0
    fi
    echo "Create update.tar failed."
    return 1
}

create_update() {
    echo "Creating update..."
    
    mkdir -p /update-scratch/update

    if copy_running_os && calc_checksum && create_update_tar; then
        echo "Created update done."
        return 0
    fi

    echo "Creating update failed."
    return 1
}

deploy_update() {
    mv /update.tar /bindir/update.tar
}

setup_build_overlay() {
    fuse-overlayfs -o lowerdir=/workdir/squashfs-root -o upperdir=/workdir/overlay-scratch -o workdir=/workdir/overlay-workdir /workdir/overlay-fs
    mkdir -p /workdir/overlay-fs/sources
    mkdir -p /workdir/overlay-fs/build
    mkdir -p /workdir/epc-nl-build
    mount --bind /sources /workdir/overlay-fs/sources
    mount --bind /workdir/epc-nl-build /workdir/overlay-fs/build
}

download_package() {
    PACKAGE=$1
    VERSION=$2
    for ENDING in $PACKAGE_ENDINGS; do
        echo "Ending is: ${ENDING}"
        PACKAGE_FILE="${PACKAGE}-${VERSION}${ENDING}"
        if find /workdir/update-packages/pkg/${PACKAGE_FILE}; then
            return 0
        fi
    done

    for ENDING in ${PACKAGE_ENDINGS}; do
        PACKAGE_URL="${NONLINEAR_PACKAGE_CACHE}/$PACKAGE-${VERSION}${ENDING}"
        if wget -T3 -t2 ${PACKAGE_URL}; then
            return 0
        fi
    done

    FIRST_LETTER=${PACKAGE:0:1}
    for ENDING in $PACKAGE_ENDINGS; do
        URL="${PACKAGES_SERVER}${FIRST_LETTER}/${PACKAGE}/${PACKAGE}-${VERSION}${ENDING}"
        if wget -T3 -t2 $URL; then
            return 0
        fi
    done

    return 1
}

download_packages() {
    mkdir -p /workdir/update-packages/pkg
    cd /workdir/update-packages/pkg
    for package in $PACKAGES_TO_INSTALL; do
        IFS=","
        set -- $package
        IFS=" "
        download_package $1 $2 || return 1
    done

    mkdir -p /workdir/overlay-fs/update-packages/pkg
    cp /workdir/update-packages/pkg/* /workdir/overlay-fs/update-packages/pkg
    return 0
}

install_package() {
    PACKAGE=$1
    VERSION=$2

    for ENDING in ${PACKAGE_ENDINGS}; do
        PACKAGE_URL="/update-packages/pkg/$PACKAGE-${VERSION}${ENDING}"
        if /internal/epc-update-partition/bin/arch-chroot /internal/epc-update-partition /bin/bash -c "pacman --noconfirm -U $PACKAGE_URL"; then
            return 0
        fi
    done

    return 1
}

install_packages() {
    for package in $PACKAGES_TO_INSTALL; do
        IFS=","
        set -- $package
        IFS=" "
        install_package $1 $2 || return 1
    done

    return 0
}

build_update() {
    download_packages || return 1

    /workdir/overlay-fs/bin/arch-chroot /workdir/overlay-fs /bin/bash -c "\
        cd /build
        cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_AUDIOENGINE=On -DBUILD_BBB_SCRIPTS=Off -DBUILD_EPC_SCRIPTS=On -DBUILD_BBBB=Off -DBUILD_PLAYGROUND=On -DBUILD_ONLINEHELP=On -DBUILD_TEXT2SOLED=Off -DBUILD_TESTING=Off -DBUILD_LPC=Off /sources
        make -j8"
}

setup_install_overlay() {
    mkdir -p /internal
    cp -ra /workdir/overlay-fs /internal/built
    mkdir -p /internal/os /internal/ow /internal/epc-update-partition
    fuse-overlayfs -o lowerdir=/internal/built -o upperdir=/internal/os -o workdir=/internal/ow /internal/epc-update-partition
}

install_update() {
    install_packages || return 1
    /internal/epc-update-partition/bin/arch-chroot /internal/epc-update-partition /bin/bash -c "\
        cd /build
        make install"
}

update_fstab() {
  /internal/epc-update-partition/bin/arch-chroot /internal/epc-update-partition /bin/bash -c "\
    echo 'root@192.168.10.11:/mnt/usb-stick  /mnt/usb-stick  fuse.sshfs  sshfs_sync,direct_io,cache=no,reconnect,defaults,_netdev,ServerAliveInterval=2,ServerAliveCountMax=3,StrictHostKeyChecking=off  0  0' >> /etc/fstab"
}

set -x
mkdir -p /workdir
mount -o loop /bindir/fs.ext4 /workdir

setup_build_overlay

if ! build_update; then
    exit 1
fi

setup_install_overlay
install_update || return 1
update_fstab
create_update
deploy_update
