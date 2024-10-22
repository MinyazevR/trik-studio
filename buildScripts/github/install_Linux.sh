#!/bin/bash
set -euxo pipefail

ID=$(grep '^ID=' /etc/os-release | cut -d'=' -f2)
TRIK_PYTHON=python3.${TRIK_PYTHON3_VERSION_MINOR}

if [ "$ID" = "altlinux" ]; then
    apt-get update && apt-get install -y gcc-c++ curl xz p7zip-standalone rsync libusb-devel \
    libudev-devel libGL-devel libGLX-mesa python3-dev zlib-devel make ccache python3-module-pip time \
    qt5-multimedia-devel qt5-svg-devel qt5-script-devel qt5-tools qt5-serialport-devel
elif [ "$ID" = "ubuntu" ]; then
    sudo apt-get update && sudo apt-get install -y --no-install-recommends vera++ \
    ccache curl libusb-1.0-0-dev make qtscript5-dev qttools5-dev-tools qtmultimedia5-dev libqt5serialport5-dev libqt5svg5-dev \
    libudev-dev ${TRIK_PYTHON}-dev
fi

