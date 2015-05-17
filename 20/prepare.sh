#!/bin/bash
set -e

# Mounts the root image and copies test code and scripts to it.

ROOTFS_PATH=${ROOTFS_PATH:-$HOME/kerndev/rootfs.img}

function unmount()
{
	umount /mnt &>/dev/null || true
}

function elevate()
{
	if [ $EUID != 0 ]; then
		exec sudo -E $0 $@
		exit $?
	fi
}

# $@: message to display.
function fatal()
{
	echo prepare.sh: $@ >&2
	exit 1
}

elevate

# We want access to the underlying user. We keep the dev env image username in
# sync with the host.
[ -z "$SUDO_USER" ] && fatal "Please run this using sudo."

home_image=/home/$SUDO_USER
mnt_home_image=/mnt$home_image

# Copy test files over to dev env rootfs.
mount -o loop $ROOTFS_PATH /mnt
trap unmount EXIT

[ ! -d $mnt_home_image ] && \
	fatal "Can't find mounted home dir: $mnt_home_image"

cp fatnametest.c $mnt_home_image
cp test_sh $mnt_home_image/test.sh
cp do_test_sh $mnt_home_image/do_test.sh
chmod +x $mnt_home_image/test.sh $mnt_home_image/do_test.sh

echo prepare.sh: rootfs prepared: test script is $home_image/test.sh
