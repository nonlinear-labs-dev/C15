set -e
set -x

pacman --noconfirm --overwrite '*' -S ${INSTALL_PACKAGES}
yes | truncate -s 8G /out/rootfs.ext4
yes | truncate -s 512M /out/bootfs.fat
yes | mkfs.ext4 /out/rootfs.ext4
yes | mkfs.fat /out/bootfs.fat
pacman --noconfirm -Sw ${INSTALL_PACKAGES}
mount -o loop /out/rootfs.ext4 /mnt
mkdir /mnt/boot
mount -o loop /out/bootfs.fat /mnt/boot/
mkdir -p /run/shm
echo Server\ =\ file:///var/lib/pacman/sync > /etc/pacman.d/mirrorlist
echo [dvzrv] >> /etc/pacman.d/mirrorlist
echo Server\ =\ file:///var/lib/pacman/sync >> /etc/pacman.d/mirrorlist

pacstrap -c /mnt base ${INSTALL_PACKAGES}

cp /in/install/nlhook /mnt/lib/initcpio/install/nlhook
cp /in/install/oroot /mnt/lib/initcpio/install/oroot
cp /in/hook/nlhook /mnt/lib/initcpio/hooks/nlhook
cp /in/hook/oroot /mnt/lib/initcpio/hooks/oroot

sed -i 's/GRUB_TIMEOUT=5/GRUB_TIMEOUT=1/' /mnt/etc/default/grub
sed -i 's/GRUB_CMDLINE_LINUX_DEFAULT.*$/GRUB_CMDLINE_LINUX_DEFAULT="quiet ip=192.168.10.10:::::eth0:none mitigations=off isolcpus=0,2"/' /mnt/etc/default/grub
sed -i 's/^HOOKS=.*$/HOOKS=\"base udev oroot block filesystems autodetect modconf keyboard net nlhook\"/' /mnt/etc/mkinitcpio.conf
sed -i 's/^BINARIES=.*$/BINARIES=\"tar rsync gzip lsblk udevadm\"/' /mnt/etc/mkinitcpio.conf
sed -i 's/^MODULES=.*$/MODULES=\"e1000e\"/' /mnt/etc/mkinitcpio.conf
sed -i "s/#governor=.*$/governor='performance'/" /mnt/etc/default/cpupower

echo "root@192.168.10.11:/mnt/usb-stick  /mnt/usb-stick  fuse.sshfs  sshfs_sync,direct_io,cache=no,reconnect,defaults,_netdev,ServerAliveInterval=2,ServerAliveCountMax=3,StrictHostKeyChecking=off  0  0" >> /mnt/etc/fstab

arch-chroot /mnt /bin/bash -c "systemctl enable cpupower"
arch-chroot /mnt /bin/bash -c "systemctl enable sshd"

arch-chroot /mnt bash -c "useradd -m sscl"
echo "sscl:sscl" | chpasswd -R /mnt
echo "sscl   ALL=(ALL) NOPASSWD:ALL" >> /mnt/etc/sudoers

arch-chroot /mnt bash -c "mkinitcpio -p linux-rt"
