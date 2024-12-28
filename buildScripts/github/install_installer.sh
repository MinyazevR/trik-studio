#!/bin/bash
set -eux

prepare_environment(){
  case "$(uname)" in
    Linux)
      ID=$(grep '^ID=' /etc/os-release | cut -d'=' -f2)
      if [ "$ID" = "altlinux" ]; then
        sudo apt-get update && sudo apt-get install -y fontconfig libdbus libX11 libXrender libXext libxkbcommon-x11
      elif [ "$ID" = "ubuntu" ]; then
        sudo apt-get update && sudo apt-get install -y --no-install-recommends fontconfig libdbus-1-3 libx11-xcb1 libxkbcommon-x11-0
      elif [ "$ID" = "arch" ]; then
        sudo pacman -Sy && sudo pacman -S fontconfig libx11 libxrender libxext libxkbcommon-x11 --noconfirm
      elif [[ "$ID" = "rocky" || "$ID" = '"rocky"' ]]; then
        sudo yum install -y --setopt=install_weak_deps=False libX11-xcb libXext libxkbcommon-x11 fontconfig freetype libXrender
      fi 
      return 0 ;;
    Darwin) return 0 ;;
    MINGW64*) return 0 ;;
    *) exit 1 ;;
  esac
}

install_installer(){
  case "$(uname)" in
    Darwin)
      sudo hdiutil attach "$INSTALLER_EXT_NAME"
      sudo cp -rf "/Volumes/$INSTALLER_NAME/$INSTALLER_NAME.app" $GITHUB_WORKSPACE
      sudo hdiutil detach /Volumes/"$INSTALLER_NAME"
      ./"$INSTALLER_NAME".app/Contents/MacOS/$INSTALLER_NAME --verbose --script trik_studio_installscript.qs
      sudo rm -rf "$INSTALLER_NAME".app
      return 0 ;;
    Linux)
      INSTALLER_NAME="$INSTALLER_NAME.run"
      chmod +x "$INSTALLER_NAME"
      ./"$INSTALLER_NAME" --verbose --script trik_studio_installscript.qs --platform minimal 
      return 0 ;;  
    MINGW64*)
      ./"$INSTALLER_NAME" --verbose --script trik_studio_installscript.qs
      return 0 ;;  
    *) exit 1 ;;
  esac
}

prepare_environment_variable_and_check_tools(){  
  EXT=""   
  case "$(uname)" in
    Darwin)
      PREFIX="/Applications"
      LIB_DIR="$PREFIX/TRIKStudio/TRIK Studio.app/Contents/Lib"
      export DYLD_LIBRARY_PATH="$LIB_DIR" 
      ;; 
    Linux)
      ID=$(grep '^ID=' /etc/os-release | cut -d'=' -f2)
      if [[ $ID = ubuntu ]]; then PREFIX="$HOME"; else PREFIX="/opt"; fi
      LIB_DIR="$PREFIX/TRIKStudio/lib"
      export LD_LIBRARY_PATH="$LIB_DIR"
      export QT_QPA_PLATFORM=minimal
      ;;
    MINGW64*)
      PREFIX="/C"
      LIB_DIR="$PREFIX/TRIKStudio"
      export LD_LIBRARY_PATH="LIB_DIR"
      EXT=".exe"
      ;;
    *) exit 1 ;; 
  esac
  
  APP_DIR="$PREFIX/TRIKStudio"
  
  TWOD_EXEC_NAME=$(find "$APP_DIR" -name "2D-model$EXT" -print -quit)
  PATCHER_NAME=$(find "$APP_DIR" -name "patcher$EXT" -print -quit)
  TRIK_STUDIO_NAME=$(find "$APP_DIR" -name "trik-studio$EXT" -print -quit)
  MAINTENANCE=$(find "$APP_DIR" -name "maintenance$EXT" -print -quit)
  
  "$TWOD_EXEC_NAME" --version
  "$PATCHER_NAME" --version
  "$TRIK_STUDIO_NAME" --version
  "$MAINTENANCE" --version
  
  echo "TWOD_EXEC_NAME=$TWOD_EXEC_NAME" >> $GITHUB_ENV
  echo "PATCHER_NAME=$PATCHER_NAME" >> $GITHUB_ENV
  echo "LIB_DIR=$LIB_DIR" >> $GITHUB_ENV
  echo "APP_DIR=$APP_DIR" >> $GITHUB_ENV
}

dll_search(){
  cd "$LIB_DIR"
  case "$(uname)" in
    Darwin)
      ls -- *.dylib | xargs otool -L | grep "not found" || exit 0
      ;;
    Linux)
      # Find dependencies that have not been packaged, but are still in the system
      ls -- *.so* | xargs ldd | grep -Ev "not found$" | grep so | sed -e '/^[^\t]/ d' | sed -e 's/\t//' \
         | sed -e 's/.*=..//' | sed -e 's/ (0.*)//' | grep -Ev "lib(c|dl|m|pthread|rt)\.so.*" \
         | grep -Ev "$LD_LIBRARY_PATH" | grep -Ev "ld|linux-vdso"
      ls -- *.so* | xargs ldd | grep "not found" || exit 0
      ;;
    MINGW64*)
      # Find dependencies that have not been packaged, but are still in the system
      ls -- *.dll* | xargs ldd | grep -Ev "not found$" | grep dll | sed -e '/^[^\t]/ d' | sed -e 's/\t//' \
	| sed -e 's/.*=..//' | sed -e 's/ (0.*)//' | grep -Ev "lib(System|SYSTEM)32.*dll" \
	| grep -Ev "$LD_LIBRARY_PATH"

      ls -- *.dll* | xargs ldd | grep "not found" || exit 0
      ;;
    *) exit 1 ;;
  esac
  exit 1
}

prepare_environment
install_installer
prepare_environment_variable_and_check_tools
dll_search
