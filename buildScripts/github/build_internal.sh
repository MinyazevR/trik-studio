#!/bin/bash

set -xueo pipefail
export LANG=C
ccache -V && ccache -p
which g++
g++ --version
which qmake
qmake -query
ccache -sz
{ which python3 && python3 -V || true ; }
{ which python && python -V || true ; }
export PKG_CONFIG_PATH=$(python3.${TRIK_PYTHON3_VERSION_MINOR}-config --prefix)/lib/pkgconfig
rm -f .qmake.cache
qmake -Wall PYTHON3_VERSION_MINOR=$TRIK_PYTHON3_VERSION_MINOR CONFIG+=$CONFIG $QMAKE_EXTRA $PROJECT.pro
make -j $(nproc) qmake_all 2>&1 | tee -a build.log
ccache -s
make -j $(nproc) all 2>&1 | tee -a build.log
ccache -s
ls bin/$CONFIG
export QT_QPA_PLATFORM=minimal
export ASAN_OPTIONS=$(if [[ $RUNNER_OS == Linux ]]; then echo 'detect_leaks=1:'; else echo -n ''; fi)detect_stack_use_after_return=1:fast_unwind_on_malloc=0
export LSAN_OPTIONS=suppressions=$PWD/bin/$CONFIG/lsan.supp:print_suppressions=0
export DISPLAY=:0
export TRIK_PYTHONPATH=${TRIK_PYTHONPATH:-$(python3.${TRIK_PYTHON3_VERSION_MINOR} -c 'import sys; import os; print(os.pathsep.join(sys.path))')}
export PYTHONMALLOC=malloc
env
make check -k -s && ( set +eux ; cd "bin/$CONFIG" && eval "$TESTS" )
