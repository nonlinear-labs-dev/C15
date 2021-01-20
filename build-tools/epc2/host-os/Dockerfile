FROM archlinux:20200908
ARG packages
RUN echo "[dvzrv]" >> /etc/pacman.d/mirrorlist
RUN echo "Server = https://pkgbuild.com/~dvzrv/repo/\$arch" >> /etc/pacman.d/mirrorlist
RUN pacman --noconfirm -Syy
RUN pacman --noconfirm -Syu util-linux-libs util-linux
RUN pacman --noconfirm -S ${packages} arch-install-scripts dosfstools cmake make pkgconf gcc git wget unzip jdk11-openjdk fuse-overlayfs fuse3
