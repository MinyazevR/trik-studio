#!/bin/bash
set -euxo pipefail

ID=$(grep '^ID=' /etc/os-release | cut -d'=' -f2)

if [ "$ID" == "altlinux" ]; then
    # sudo bash -c 'echo "rpm [alt] http://ftp.altlinux.org/pub/distributions/ALTLinux/Sisyphus x86_64 classic" > /etc/apt/sources.list.d/sisyphus.list'
    apt-get update && apt-get install -y curl xz p7zip-standalone rsync libusb-devel python3-dev zlib-devel make ccache python3-module-pip
    python3 -m pip install -U pip
    python3 -m pip install aqtinstall
    python3 -m aqt install-qt -O "$HOME/Qt" linux desktop "${TRIK_QT_VERSION}"
    python3 -m aqt install-tool -O "$HOME/Qt" linux desktop tools_ifw
else
    docker pull trikset/linux-builder
    docker run --cap-add SYS_PTRACE -d -v $HOME:$HOME:rw -w `pwd` --name builder trikset/linux-builder Xvfb :0
    docker exec builder git config --global --add safe.directory '*'
fi
