#!/bin/bash
set -euxo pipefail

if [ "$ID" = "altlinux" ]; then
  apt-get update && apt-get install -y libxkbcommon-x11 libX11 fontconfig libxcbutil-icccm \
  libxcbutil-image libdbus libxcbutil-keysyms libxcb-render-util
elif [ "$ID" = "ubuntu" ]; then
  sudo apt-get update && sudo apt-get install -y --no-install-recommends \
  ccache curl libusb-1.0-0-dev make qtscript5-dev qttools5-dev-tools qtmultimedia5-dev libqt5serialport5-dev libqt5svg5-dev \
  libudev-dev python3.${TRIK_PYTHON3_VERSION_MINOR}-dev qtbase5-private-dev qtwayland5
elif [[ "$ID" = "rocky" || "$ID" = '"rocky"' ]]; then
  yum update -y && yum install -y libxkbcommon-x11 xcb-util-wm libX11 xcb-util-image \
  dbus-libs xcb-util-keysyms xcb-util-renderutil fontconfig libX11-xcb
fi

./$INSTALLER_NAME --verbose --script trik_studio_installscript.qs --platform minimal
echo "BIN_DIR=/opt/TRIKStudio/bin" >> $GITHUB_ENV
echo "LIB_DIR=/opt/TRIKStudio/lib" >> $GITHUB_ENV
echo "APP_DIR=/opt/TRIKStudio" >> $GITHUB_ENV
