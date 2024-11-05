#!/bin/bash
set -euxo pipefail

ID=$(grep '^ID=' /etc/os-release | cut -d'=' -f2)

if [ "$ID" = "altlinux" ]; then
  apt-get update && apt-get install -y libxkbcommon-x11 libX11 fontconfig libxcbutil-icccm \
  libxcbutil-image libdbus libxcbutil-keysyms libxcb-render-util
elif [ "$ID" = "arch" ]; then
  pacman -Sy && pacman -S libxkbcommon-x11 xcb-util-wm xcb-util-image xcb-util-keysyms xcb-util-renderutil fontconfig libx11 --noconfirm
elif [[ "$ID" = "rocky" || "$ID" = '"rocky"' ]]; then
  yum update -y && yum install -y libxkbcommon-x11 xcb-util-wm libX11 xcb-util-image \
  dbus-libs xcb-util-keysyms xcb-util-renderutil fontconfig libX11-xcb
fi

INSTALLER_NAME="$INSTALLER_NAME.run"
chmod +x "$INSTALLER_NAME"

if [[ -z "$CONCURRENCY" ]]; then
  ./$INSTALLER_NAME --verbose --script trik_studio_installscript.qs --platform minimal --mco "$CONCURRENCY"
else
  ./$INSTALLER_NAME --verbose --script trik_studio_installscript.qs --platform minimal
fi
BIN_DIR=/opt/TRIKStudio/bin && echo "BIN_DIR=$BIN_DIR" >> $GITHUB_ENV
LIB_DIR=/opt/TRIKStudio/lib && echo "LIB_DIR=$LIB_DIR" >> $GITHUB_ENV
APP_DIR=/opt/TRIKStudio && echo "APP_DIR=$APP_DIR" >> $GITHUB_ENV

"$BIN_DIR"/2D-model --version
"$BIN_DIR"/checkapp --version
"$BIN_DIR"/patcher --version
"$APP_DIR"/maintenance --version
"$APP_DIR"/trik-studio --version

export LD_LIBRARY_PATH="$LIB_DIR"
ls *.so* | xargs ldd | grep "not found" || exit 0
