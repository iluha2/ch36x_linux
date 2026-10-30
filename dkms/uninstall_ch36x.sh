#!/bin/sh

if [ $EUID -ne 0 ]; then
  echo -n "Enter su password. "
  exec su - -c "sh $(realpath "$0") $@"
  exit
fi

source $(dirname "$(realpath "$0")")/dkms.conf

pid=$(lsof -t /dev/${PACKAGE_NAME}* 2>/dev/null) && kill $pid && sleep 1
rmmod $PACKAGE_NAME 2>/dev/null

for ver in $(dkms status | awk -v name=$PACKAGE_NAME -F'[/, :]' '$1 == name {print $2}'); do
  dkms remove --all -m $PACKAGE_NAME -v $ver
  rm -rf "/usr/src/${PACKAGE_NAME}-${ver}"
done
