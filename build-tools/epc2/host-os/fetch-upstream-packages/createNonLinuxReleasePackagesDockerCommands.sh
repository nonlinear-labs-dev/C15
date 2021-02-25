pacman --noconfirm -Syy
pacman --noconfirm -Su pacman-contrib

ALL=""

for sub in $PACKAGES; do
  DEPS=$(pactree -u $sub)
  ALL="$ALL $sub $DEPS"
done

for sub in $(pacman -Qeq); do
  DEPS=$(pactree -u $sub)
  ALL="$ALL $sub $DEPS"
done

pacman --noconfirm -Sw ${ALL}

mkdir -p /nonlinux-packages
cp /var/cache/pacman/pkg/* /nonlinux-packages
repo-add /nonlinux-packages/nonlinux.db.tar.gz /nonlinux-packages/*
tar -C /nonlinux-packages -czvf /out/out.tar.gz ./