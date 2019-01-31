#!/bin/bash
set -o nounset
set -o errexit

cd "$(dirname "$0")"

mkdir -p $PWD/../data/lib/plugins/tools/kitPlugins
cp     $BIN_DIR/plugins/tools/kitPlugins/librobots-trik-v62-f-sharp-generator.so               $PWD/../data/lib/plugins/tools/kitPlugins/

# Cleaning up prebuild-common.sh results...
rm -rf $PWD/../data/plugins/
