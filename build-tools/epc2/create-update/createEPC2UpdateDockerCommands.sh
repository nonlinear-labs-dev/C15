set -e
set -x

UPDATE_PACKAGE_SERVERS="https://nonlinearlabs.s3.eu-central-1.amazonaws.com https://archive.archlinux.org/packages"
BUILD_SWITCHES="-DBUILD_EPC_SCRIPTS=On -DBUILD_AUDIOENGINE=On -DBUILD_PLAYGROUND=On -DBUILD_ONLINEHELP=On -DBUILD_WEBUI=On"

setup_overlay() {
    mount -o loop /in/rootfs.ext4 /mnt
    mkdir -p /mnt/boot
    mount -o loop /in/bootfs.fat /mnt/boot/
    
    mkdir -p /overlay-scratch
    mkdir -p /overlay-workdir
    mkdir -p /overlay-fs
    
    fuse-overlayfs -o lowerdir=/mnt -o upperdir=/overlay-scratch -o workdir=/overlay-workdir /overlay-fs || error "fuse-overlay failed."
}

download_package() {
    if find ./$1 > /dev/null; then
        return 0
    fi
    
    FIRSTCHAR=$(echo "$1" | cut -b1)
        
    for server in $UPDATE_PACKAGE_SERVERS; do
        URL="${server}/$FIRSTCHAR/$1"
        if wget $URL; then
            return 0
        fi
    done

    return 1
}

download_packages() {
    for package in $UPDATE_PACKAGES; do
        for subpackage in $(pacman -Sp $package); do
            if ! download_package $(basename ${subpackage}); then
                return 1
            fi
        done
    done
    return 0
}


install_packages() {
    for package in $UPDATE_PACKAGES; do
        for subpackage in $(pacman -Sp $package); do
            pacstrap -c -U /overlay-fs $(basename ${subpackage})
        done
    done
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
download_packages
install_packages
build_binaries
create_update
