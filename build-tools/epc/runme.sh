#!/bin/sh

echo "Starting transormation of AP Linux into Nonlinux:"

SSD_NAME=`lsblk -o RM,NAME | grep "^ 0" | grep -o "sd." | uniq`
SSD=/dev/${SSD_NAME}

while fdisk -l ${SSD} | grep "${SSD}[0-9]"; do
 echo "${SSD} is already partitioned"
 echo "If you are sure to know what you're doing, please type: cfdisk ${SSD}"
 echo "Delete all partitions there manually, write the partition table and retry this skript."
 echo "Here you have a shell for fixing the issue. Once you close the shell (Ctrl+D), a new attempt will be made."
 /bin/bash
done

echo "Partitioning ${SSD}:"
cat /Audiophile2NonLinux/sda.sfdisk | sfdisk ${SSD}
echo ";" | sfdisk -a ${SSD}

echo "Creating filesystems:"
mkfs.fat ${SSD}1
mkfs.ext4 ${SSD}2
mkfs.ext4 ${SSD}3
mkfs.ext4 ${SSD}4

echo "Mounting root and boot partitions:"
mount ${SSD}2 /mnt
mkdir -p /mnt/boot
mount ${SSD}1 /mnt/boot

echo "Tweak AP Linux:"
cp /Audiophile2NonLinux/install/nlhook /lib/initcpio/install/nlhook
cp /Audiophile2NonLinux/install/oroot /lib/initcpio/install/oroot
cp /Audiophile2NonLinux/hook/nlhook /lib/initcpio/hooks/nlhook
cp /Audiophile2NonLinux/hook/oroot /lib/initcpio/hooks/oroot

sed -i 's/read.*username/username=sscl/' /etc/apl-files/runme.sh
sed -i 's/read.*password/password=sscl/' /etc/apl-files/runme.sh
sed -i 's/pacman -U/pacman --noconfirm -U/' /etc/apl-files/runme.sh
sed -i 's/Required DatabaseOptional/Never/' /etc/pacman.conf
sed -i 's/Server.*mettke/#/' /etc/pacman.d/mirrorlist
sed -i 's/GRUB_TIMEOUT=5/GRUB_TIMEOUT=1/' /etc/default/grub
sed -i 's/GRUB_CMDLINE_LINUX_DEFAULT.*$/GRUB_CMDLINE_LINUX_DEFAULT="quiet ip=192.168.10.10:::::eth0:none"/' /etc/default/grub
sed -i 's/^HOOKS=.*$/HOOKS=\"base udev oroot block filesystems autodetect modconf keyboard net nlhook\"/' /etc/mkinitcpio.conf
sed -i 's/^BINARIES=.*$/BINARIES=\"tar rsync gzip lsblk udevadm\"/' /etc/mkinitcpio.conf
sed -i 's/^MODULES=.*$/MODULES=\"e1000e\"/' /etc/mkinitcpio.conf

echo "Copy initial system:"
cp -ax / /mnt

# do not link /mnt/etc/resolv.conf to /etc/resolv.conf, as the former will be updated
sed -i 's/^chroot_add_resolv_conf /#/' /bin/arch-chroot

echo "Do APLinux stuff:"
arch-chroot /mnt /bin/bash -c "cd /etc/apl-files && ./runme.sh"

echo "Install grub:"
arch-chroot /mnt /bin/bash -c "grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=arch_grub --recheck"
arch-chroot /mnt /bin/bash -c "grub-mkconfig -o /boot/grub/grub.cfg"
arch-chroot /mnt /bin/bash -c "mkdir /boot/EFI/BOOT && cp /boot/EFI/arch_grub/grubx64.efi /boot/EFI/BOOT/BOOTX64.EFI"

echo "Configure autologin:"
arch-chroot /mnt /bin/bash -c "cd /etc/apl-files && ./autologin.sh"

echo "Downloading NonLinux/Arch packages:"

arch-chroot /mnt /bin/bash -c "mkdir -p /update-packages"
cp /Audiophile2NonLinux/NonLinux.pkg.tar.gz /mnt/update-packages/NonLinux.pkg.tar.gz

arch-chroot /mnt /bin/bash -c "tar -C /update-packages -xzf /update-packages/NonLinux.pkg.tar.gz"
arch-chroot /mnt /bin/bash -c "echo 'Server = file:///update-packages/pkg/' > /etc/pacman.d/mirrorlist"

echo "Maintain packages:"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -Sy"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -Scc"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -Rcs xorg gnome mesa freetype2 ffmpeg ffmpeg2.8 man-db man-pages"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -Rcs b43-fwcutter bluez-libs geoip ipw2100-fw ipw2200-fw libjpeg-turbo"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -Rcs tango-icon-theme xorg-xmessage xf86-input-evdev xf86-input-synaptics zd1211-firmware"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -S cpupower git networkmanager"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -Rcs xorgproto xfsprogs cifs-utils emacs-nox lvm2 fuse2"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -S cmake make gcc glibmm pkgconf jdk11-openjdk libsoup freetype2"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -S avahi boost libpng png++"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -Su"
arch-chroot /mnt /bin/bash -c "pacman --noconfirm -Qdt"

arch-chroot /mnt /bin/bash -c "mv /update-packages/pkg/gwt-2.8.2 /"
arch-chroot /mnt /bin/bash -c "rm -rf /update-packages"

echo "Generate fstab:"
genfstab -U /mnt >> /mnt/etc/fstab

echo "Configure cpupower:"
sed -i "s/#governor=.*$/governor='performance'/" /mnt/etc/default/cpupower
arch-chroot /mnt /bin/bash -c "systemctl enable cpupower"

echo "Remove some artifacts:"
truncate -s 0 /mnt/home/sscl/.zprofile
arch-chroot /mnt /bin/bash -c "rm -rf /usr/lib/modules/5.1.7-arch1-1-ARCH"
arch-chroot /mnt /bin/bash -c "rm -rf /usr/lib/modules/extramodules-ARCH"
arch-chroot /mnt /bin/bash -c "rm -rf /usr/lib/firmware/netronome"
arch-chroot /mnt /bin/bash -c "rm -rf /usr/lib/firmware/liquidio"
arch-chroot /mnt /bin/bash -c "rm -rf /usr/lib/firmware/amdgpu"
arch-chroot /mnt /bin/bash -c "rm -rf /usr/lib/firmware/qed"
arch-chroot /mnt /bin/bash -c "cd /usr/share/locale && ls -1 | grep -v 'en_US' | xargs rm -rf {}"
arch-chroot /mnt /bin/bash -c "rm -rf /usr/share/doc"
arch-chroot /mnt /bin/bash -c "rm -rf /usr/share/info"
arch-chroot /mnt /bin/bash -c "rm -rf /usr/share/man"

arch-chroot /mnt /bin/bash -c "systemctl mask systemd-backlight@"
arch-chroot /mnt /bin/bash -c "systemctl mask systemd-random-seed"
arch-chroot /mnt /bin/bash -c "systemctl mask systemd-tmpfiles-setup"
arch-chroot /mnt /bin/bash -c "systemctl mask systemd-tmpfiles-clean"
arch-chroot /mnt /bin/bash -c "systemctl mask systemd-tmpfiles-setup-dev"

arch-chroot /mnt /bin/bash -c "rm /etc/profile.d/runme.sh"
arch-chroot /mnt /bin/bash -c "rm -rf /Audiophile2NonLinux"

shutdown -P now
