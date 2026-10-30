#!/bin/sh

if [ $EUID -ne 0 ]; then
  echo -n "Enter su password. "
  exec su - -c "sh $(realpath "$0") $@"
  exit
fi

kver=$(uname -r | cut -d "-" -f2)
#apt-get update
apt-get -y install dkms kernel-build-tools kernel-headers-$kver kernel-headers-modules-$kver

pushd $(dirname "$(realpath "$0")")
source ./dkms.conf

src_dir="/usr/src/$PACKAGE_NAME-$PACKAGE_VERSION"
mkdir -p $src_dir

cp -p ./ch36x.c   $src_dir/
cp -p ./dkms.conf $src_dir/
cp -p ./Makefile  $src_dir/

dkms install --modprobe-on-install -m $PACKAGE_NAME -v $PACKAGE_VERSION

popd
