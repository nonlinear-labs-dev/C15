set -e

BUILD_SWITCHES="-DBUILD_EPC_SCRIPTS=On -DBUILD_AUDIOENGINE=On -DBUILD_PLAYGROUND=On -DBUILD_ONLINEHELP=On -DBUILD_WEBUI=On"

setup_overlay() {
    mkdir -p /run/shm
    mount -o loop /in/rootfs.ext4 /mnt
    mkdir -p /mnt/boot
    mount -o loop /in/bootfs.fat /mnt/boot/
    
    mkdir -p /overlay-scratch
    mkdir -p /overlay-workdir
    mkdir -p /overlay-fs
    
    fuse-overlayfs -o lowerdir=/mnt -o upperdir=/overlay-scratch -o workdir=/overlay-workdir /overlay-fs || error "fuse-overlay failed."
}

install_packages() {
    pacman --noconfirm -S $UPDATE_PACKAGES
}

build_binaries() {
    DESTDIR=/overlay-fs cmake -DCMAKE_BUILD_TYPE=Release ${BUILD_SWITCHES} -S /source -B /build
    make -j8 -C /build
    DESTDIR=/overlay-fs make -C /build install 
}

create_update() {
    mkdir -p /update
    tar -C /overlay-scratch --exclude=.wh..wh..opq -czf /update/NonLinuxOverlay.tar.gz .
    touch /update/$(sha256sum /update/NonLinuxOverlay.tar.gz | grep -o "^[^ ]*").sign
    tar -cf /out/update.tar ./update
}

setup_overlay
install_packages
build_binaries
create_update
