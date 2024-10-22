#!/bin/bash
set -euxo pipefail

ID=$(grep '^ID=' /etc/os-release | cut -d'=' -f2)

if [ "$ID" = "altlinux" ]; then
    apt-get update && apt-get install -y gcc-c++ curl xz p7zip-standalone rsync libusb-devel \
    libudev-devel libGL-devel libGLX-mesa python3-dev zlib-devel make ccache python3-module-pip time \
    qt5-multimedia-devel qt5-svg-devel qt5-script-devel qt5-tools qt5-serialport-devel
elif [ "$ID" = "ubuntu" ]; then
    sudo apt-get update && sudo apt-get install -y --no-install-recommends vera++ qttools5-dev-tools \
    qtbase5-dev ccache curl libusb-dev make qtbase5-dev-tools
fi
