set -e

pacman --noconfirm -Syy
pacman --noconfirm -S archiso

cp -r /usr/share/archiso/configs/baseline/ /archlive
echo dosfstools >> /archlive/packages.x86_64
echo arch-install-scripts >> /archlive/packages.x86_64
echo nano >> /archlive/packages.x86_64
cp /in/bootfs.fat /archlive/airootfs/
cp /in/rootfs.ext4 /archlive/airootfs/
cp /src/runme.sh /archlive/airootfs/
cp /src/sda.sfdisk /archlive/airootfs/
chmod +x /archlive/airootfs/runme.sh
gzip /archlive/airootfs/bootfs.fat
gzip /archlive/airootfs/rootfs.ext4

sed -i 's/\[core\]//g' /archlive/pacman.conf
sed -i 's/\[extra\]//g' /archlive/pacman.conf
sed -i 's/\[community\]//g' /archlive/pacman.conf
echo "[nonlinux]" >> /archlive/pacman.conf
echo "SigLevel = Optional TrustAll" >> /archlive/pacman.conf
echo "Server=file:///extracted-packages" >> /archlive/pacman.conf
pacman --noconfirm -Syyuu
pacman --noconfirm -Syy
mkarchiso -v -w /work_dir -o /out /archlive