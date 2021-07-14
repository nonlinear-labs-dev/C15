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
    pacstrap -c /overlay-fs $UPDATE_PACKAGES
    
    echo "en_US.UTF-8 UTF-8" > /overlay-fs/etc/locale.gen
    /bin/arch-chroot /overlay-fs locale-gen
}

setup_wifi() {

    cat <<- ENDOFHERE > /overlay-fs/etc/NetworkManager/system-connections/C15.nmconnection
    [connection]
    id=C15
    uuid=61679179-6804-4197-b476-eacad1d492e4
    type=wifi
    interface-name=wlp0s20f3
    permissions=

    [wifi]
    band=bg
    channel=7
    mac-address-blacklist=
    mode=ap
    ssid=NL-C15-Unit-00000

    [wifi-security]
    key-mgmt=wpa-psk
    pairwise=ccmp;
    proto=rsn;
    psk=88888888

    [ipv4]
    address1=192.168.8.2/24,192.168.8.1
    dns-search=
    method=shared

    [ipv6]
    addr-gen-mode=stable-privacy
    dns-search=
    method=auto

    [proxy]
ENDOFHERE

    /bin/arch-chroot /overlay-fs chmod 600 /etc/NetworkManager/system-connections/C15.nmconnection
    /bin/arch-chroot /overlay-fs systemctl enable NetworkManager
}

build_binaries() {
    DESTDIR=/overlay-fs cmake -DTARGET_PLATFORM=epc2 -DCMAKE_BUILD_TYPE=Release ${BUILD_SWITCHES} -S /source -B /build
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
setup_wifi
build_binaries
create_update
