#!/bin/bash
set -euxo pipefail

sudo hdiutil attach "$INSTALLER_EXT_NAME"
sudo cp -rf "/Volumes/$INSTALLER_NAME/$INSTALLER_NAME.app" $GITHUB_WORKSPACE
sudo hdiutil detach /Volumes/"$INSTALLER_NAME"
./$INSTALLER_NAME.app/Contents/MacOS/$INSTALLER_NAME --verbose --script trik_studio_installscript.qs

echo "BIN_DIR=/Applications/TRIKStudio/bin" >> $GITHUB_ENV
echo "LIB_DIR=/Applications/TRIKStudio/lib" >> $GITHUB_ENV
echo "APP_DIR=/Applications/TRIKStudio" >> $GITHUB_ENV
