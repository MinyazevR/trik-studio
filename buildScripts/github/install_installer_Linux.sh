#!/bin/bash
set -euxo pipefail

ID=$(grep '^ID=' /etc/os-release | cut -d'=' -f2)

if [ "$ID" = "altlinux" ]; then
  apt-get update && apt-get install -y libxkbcommon-x11 libX11 fontconfig libxcbutil-icccm \
  libxcbutil-image libdbus libxcbutil-keysyms libxcb-render-util
if [ "$ID" = "arch" ]; then
  pacman -Sy && pacman -S libxkbcommon-x11 xcb-util-wm xcb-util-image xcb-util-keysyms \ 
  xcb-util-renderutil fontconfig libx11 --noconfirm
elif [[ "$ID" = "rocky" || "$ID" = '"rocky"' ]]; then
  yum update -y && yum install -y libxkbcommon-x11 xcb-util-wm libX11 xcb-util-image \
  dbus-libs xcb-util-keysyms xcb-util-renderutil fontconfig libX11-xcb
fi

INSTALLER_NAME="$INSTALLER_NAME.run"
chmod +x "$INSTALLER_NAME"
./$INSTALLER_NAME --verbose --script trik_studio_installscript.qs --platform minimal
echo "BIN_DIR=/opt/TRIKStudio/bin" >> $GITHUB_ENV
echo "LIB_DIR=/opt/TRIKStudio/lib" >> $GITHUB_ENV
echo "APP_DIR=/opt/TRIKStudio" >> $GITHUB_ENV
