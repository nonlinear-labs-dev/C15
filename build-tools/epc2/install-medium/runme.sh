#!/bin/sh

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

cat /sda.sfdisk | sfdisk ${SSD} 
echo ";" | sfdisk -a ${SSD}

yes | mkfs.fat ${SSD}1
yes | mkfs.ext4 ${SSD}2
yes | mkfs.ext4 ${SSD}3
yes | mkfs.ext4 ${SSD}4

gzip -cd /bootfs.fat.gz | dd of=/dev/sda1
gzip -cd /rootfs.ext4.gz | dd of=/dev/sda2

mount /dev/sda2 /mnt
mount /dev/sda1 /mnt/boot

arch-chroot /mnt /bin/bash -c "grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=arch_grub --recheck"
arch-chroot /mnt /bin/bash -c "grub-mkconfig -o /boot/grub/grub.cfg"
arch-chroot /mnt /bin/bash -c "mkdir /boot/EFI/BOOT && cp /boot/EFI/arch_grub/grubx64.efi /boot/EFI/BOOT/BOOTX64.EFI"
genfstab -U /mnt >> /mnt/etc/fstab
