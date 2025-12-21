#!/bin/bash

if [ "$EUID" -ne 0 ]
then
    echo "You must be root"
    exit 1
fi

PACKAGE_NAME="work_module"
PACKAGE_VERSION="1.0"

function install_module()
{
    local dkms_src="/usr/src/$PACKAGE_NAME-$PACKAGE_VERSION"
    mkdir $dkms_src

    cp Makefile $dkms_src
    cp *.c $dkms_src
    cp *.h $dkms_src
    cp dkms.conf $dkms_src

    sed -i -e "s/P_NAME/$PACKAGE_NAME/g" \
        -i -e "s/P_VERSION/$PACKAGE_VERSION/g" \
        "$dkms_src/dkms.conf"

    /usr/sbin/dkms install "$PACKAGE_NAME/$PACKAGE_VERSION"
}

function uninstall_module
{
    local dkms_src="/usr/src/$PACKAGE_NAME-$PACKAGE_VERSION"
    /usr/sbin/dkms remove "$PACKAGE_NAME/$PACKAGE_VERSION"
    rm -rf $dkms_src
}

case "$1" in
    install)
        install_module
        ;;
    uninstall)
        uninstall_module
        ;;
    *)
        echo "Usage: $0 [action]"
        echo "  [action]   = { install | remove }"
        echo ""
        exit -1
esac