#!/bin/sh

UPDATE_PACKAGE_SERVERS="http://hoegelow.com/nl/ https://ind.mirror.pkgbuild.com/community/os/x86_64/ https://sgp.mirror.pkgbuild.com/extra/os/x86_64/"
PACKAGES_TO_INSTALL="fuse-common-3.9.0-1-x86_64.pkg.tar.xz fuse3-3.9.0-1-x86_64.pkg.tar.xz sshfs-3.7.0-1-x86_64.pkg.tar.zst mc-4.8.24-2-x86_64.pkg.tar.zst"

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

download_packages() {
    mkdir -p /workdir/update-packages
    cd /workdir/update-packages
    for package in $PACKAGES_TO_INSTALL; do
        if ! find ./${package} > /dev/null; then
            for server in $UPDATE_PACKAGE_SERVERS; do
                URL="${server}${package}"
                if wget $URL; then
                    break
                fi
            done
        fi
    done

    mkdir -p /workdir/overlay-fs/update-packages
    cp /workdir/update-packages/* /workdir/overlay-fs/update-packages
}

install_packages() {
    /internal/epc-update-partition/bin/arch-chroot /internal/epc-update-partition /bin/bash -c "\
        cd /update-packages
        for package in $PACKAGES_TO_INSTALL; do
            pacman --noconfirm -U ./\$package
        done
        "
}

build_update() {
    download_packages
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
    install_packages
    /internal/epc-update-partition/bin/arch-chroot /internal/epc-update-partition /bin/bash -c "\
        cd /build
        make install"
}

update_fstab() {
  /internal/epc-update-partition/bin/arch-chroot /internal/epc-update-partition /bin/bash -c "\
    echo 'root@192.168.10.11:/mnt/usb-stick  /mnt/usb-stick  fuse.sshfs  reconnect,defaults,_netdev,ServerAliveInterval=2,ServerAliveCountMax=3,StrictHostKeyChecking=off  0  0' >> /etc/fstab"
}

set -x
mkdir -p /workdir
mount -o loop /bindir/fs.ext4 /workdir

setup_build_overlay
build_update
setup_install_overlay
install_update
update_fstab
create_update
deploy_update
