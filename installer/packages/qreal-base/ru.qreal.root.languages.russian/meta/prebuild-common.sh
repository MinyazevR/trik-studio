#!/bin/bash
set -o nounset
set -o errexit

cd "$(dirname "$0")"

mkdir -p "$PWD"/../data/translations
rsync -a "$BIN_DIR"/translations/ru "$PWD"/../data/translations
ls "$QT_TRANSLATIONS"
rsync -a "$QT_TRANSLATIONS"/*ru.qm "$PWD"/../data/translations
