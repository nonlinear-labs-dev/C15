#!/bin/sh

UPDATE_PACKAGE_SERVERS="https://nonlinearlabs.s3.eu-central-1.amazonaws.com/ https://ind.mirror.pkgbuild.com/community/os/x86_64/ https://sgp.mirror.pkgbuild.com/extra/os/x86_64/"
PACKAGES_TO_INSTALL="fuse-common-3.9.0-1-x86_64.pkg.tar.xz fuse3-3.9.0-1-x86_64.pkg.tar.xz sshfs-3.7.0-1-x86_64.pkg.tar.zst mc-4.8.24-2-x86_64.pkg.tar.zst flac-1.3.3-1-x86_64.pkg.tar.xz"

error() {
    echo "Create ePC update failed: $1"
    exit 1
}

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
    return $?
}

setup_build_overlay() {
    fuse-overlayfs -o lowerdir=/workdir/squashfs-root -o upperdir=/workdir/overlay-scratch -o workdir=/workdir/overlay-workdir /workdir/overlay-fs || error "fuse-overlay failed."
    mkdir -p /workdir/overlay-fs/sources
    mkdir -p /workdir/overlay-fs/build
    mkdir -p /workdir/epc-nl-build
    mount --bind /sources /workdir/overlay-fs/sources || error "Mounting the sources failed."
    mount --bind /workdir/epc-nl-build /workdir/overlay-fs/build || error "Mounting the build folder failed."
}

download_package() {
    if find ./$1 > /dev/null; then
        return 0
    fi

    for server in $UPDATE_PACKAGE_SERVERS; do
        URL="${server}$1"
        if wget $URL; then
            return 0
        fi
    done

    return 1
}

download_packages() {
    mkdir -p /workdir/update-packages
    cd /workdir/update-packages
    for package in $PACKAGES_TO_INSTALL; do
        if ! download_package ${package}; then
            return 1
        fi
    done

    mkdir -p /workdir/overlay-fs/update-packages
    cp /workdir/update-packages/* /workdir/overlay-fs/update-packages
    return 0
}

install_packages() {
    /internal/epc-update-partition/bin/arch-chroot /internal/epc-update-partition /bin/bash -c "\
        cd /update-packages
        for package in $PACKAGES_TO_INSTALL; do
	    
	    if ! pacman --noconfirm -U ./\$package; then
              	return 1
            fi
        done"
    return $?
}

build_update() {
    download_packages || error "Downloading packages failed."

    /workdir/overlay-fs/bin/arch-chroot /workdir/overlay-fs /bin/bash -c "\
        cd /build && cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_EPC_SCRIPTS=On -DBUILD_AUDIOENGINE=On -DBUILD_PLAYGROUND=On -DBUILD_ONLINEHELP=On -DBUILD_WEBUI=On /sources && make -j8"
    return $?
}

setup_install_overlay() {
    mkdir -p /internal
    cp -ra /workdir/overlay-fs /internal/built || error "Copying the build failed."
    mkdir -p /internal/os /internal/ow /internal/epc-update-partition
    fuse-overlayfs -o lowerdir=/internal/built -o upperdir=/internal/os -o workdir=/internal/ow /internal/epc-update-partition
    return $?
}

install_update() {
    install_packages || error "Installing the packages failed."
    /internal/epc-update-partition/bin/arch-chroot /internal/epc-update-partition /bin/bash -c "cd /build && make install"
    return $?
}

update_fstab() {
    /internal/epc-update-partition/bin/arch-chroot /internal/epc-update-partition /bin/bash -c "\
        echo 'root@192.168.10.11:/mnt/usb-stick  /mnt/usb-stick  fuse.sshfs  sshfs_sync,direct_io,cache=no,reconnect,defaults,_netdev,ServerAliveInterval=2,ServerAliveCountMax=3,StrictHostKeyChecking=off  0  0' >> /etc/fstab"
    return $?
}


main() {
    mkdir -p /workdir || error "Creation of workdir failed"
    mount -o loop /bindir/fs.ext4 /workdir || error "Mouning filesystem failed."

    setup_build_overlay || error "Setting up build overlay failed."
    build_update || error "Building the update failed"
    setup_install_overlay || error "Setting up the install overlay failed."
    install_update || error "Installing gthe update failed."
    update_fstab || error "Updateing /etc/fstab failed."
    create_update || error "Creating the update.tar failed."
    deploy_update || error "Deploying the update failed."

    return 0
}

main
