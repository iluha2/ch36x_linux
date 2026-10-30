#!/bin/sh

if [ $EUID -ne 0 ]; then
  echo -n "Enter su password. "
  exec su - -c "sh $(realpath "$0") $@"
  exit
fi

source $(dirname "$(realpath "$0")")/dkms.conf
dkms remove --all -m $PACKAGE_NAME -v $PACKAGE_VERSION
