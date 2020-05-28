#!/bin/sh

areKernelParametersSet() {
    if cat /etc/default/grub | grep "isolcpus" > /dev/null; then
        if cat /etc/default/grub | grep "mitigations" > /dev/null; then
            return 0
        fi
    fi
    return 1
}

setupGrubKernelParameters() {
    mkdir -p /mnt/sda2
    mount /dev/sda2 /mnt/sda2
    mount /dev/sda1 /mnt/sda2/boot
    arch-chroot /mnt/sda2 /bin/bash -c "sed -i 's/GRUB_CMDLINE_LINUX_DEFAULT.*$/GRUB_CMDLINE_LINUX_DEFAULT=\\\"quiet ip=192.168.10.10:::::eth0:none mitigations=off isolcpus=0,2\\\"/g' /etc/default/grub"
    arch-chroot /mnt/sda2 /bin/bash -c "grub-install --target=x86_64-efi --efi-directory=/boot --bootloader-id=arch_grub --recheck"
    arch-chroot /mnt/sda2 /bin/bash -c "grub-mkconfig -o /boot/grub/grub.cfg"
    arch-chroot /mnt/sda2 /bin/bash -c "mkdir -p /boot/EFI/BOOT"
    arch-chroot /mnt/sda2 /bin/bash -c "cp /boot/EFI/arch_grub/grubx64.efi /boot/EFI/BOOT/BOOTX64.EFI"
    umount /mnt/sda2/boot
    umount /mnt/sda2
    reboot
}

bindSoundCardIRQ() {
    ALLOW_CPU_2="4"
    for irq in $(cat /proc/interrupts | grep snd_hda_intel | awk '{print $1}' | grep -o "^[0-9]*"); do
        echo "$ALLOW_CPU_2" > /proc/irq/$irq/smp_affinity
    done
}

restrictOtherIRQs() {
    ALLOW_CPU_1_AND_3="a"
    for irq in $(cat /proc/interrupts | grep -v snd_hda_intel | awk '{print $1}' | grep -o "^[0-9]*"); do
        echo "$ALLOW_CPU_1_AND_3" > /proc/irq/$irq/smp_affinity
    done
}

main() {
    if areKernelParametersSet; then
        echo "CPU bindings are already set up."
    else
        echo "CPU bindings are not set up, will do ..."
        setupGrubKernelParameters
    fi

    bindSoundCardIRQ
    restrictOtherIRQs
}

main