SET(ARCH_VERSION 2021-03-11) # a timestamp indicating the version of the packages needed to build an update

SET(SHARED_PACKAGES sudo glibmm libsoup freetype2 libpng png++ boost alsa) 
SET(TARGET_PACKAGES cpupower base linux-rt mc networkmanager nano mkinitcpio mkinitcpio-nfs-utils rsync grub openssh sshfs efibootmgr alsa-tools alsa-utils linux-firmware shared-mime-info)
SET(ALL_PACKAGES ${TARGET_PACKAGES} ${SHARED_PACKAGES})

SET(UPDATE_PACKAGES lm_sensors flac dnsmasq)

string(REPLACE ";" " " SHARED_PACKAGES "${SHARED_PACKAGES}")
string(REPLACE ";" " " TARGET_PACKAGES "${TARGET_PACKAGES}")
string(REPLACE ";" " " ALL_PACKAGES "${ALL_PACKAGES}")
string(REPLACE ";" " " UPDATE_PACKAGES "${UPDATE_PACKAGES}")

SET(PACCACHEDIR pacman-cache-${ARCH_VERSION})
SET(PACCONFDIR pacman-conf-${ARCH_VERSION})

add_subdirectory(host-os)
add_subdirectory(target-os)
add_subdirectory(install-medium)
add_subdirectory(create-update)
