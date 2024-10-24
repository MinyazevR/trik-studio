#!/bin/bash
set -euo pipefail

[ -r tests_qrs.7z ] || curl -Lo tests_qrs.7z https://dl.trikset.com/edu/.solutions20200701/testing_small.7z
which 7z &> /dev/null && 7z -y x tests_qrs.7z || 7za x tests_qrs.7z
export ASAN_OPTIONS=$(if [[ $RUNNER_OS == Linux ]]; then echo 'detect_leaks=0:'; else echo -n ''; fi)detect_stack_use_after_return=1:fast_unwind_on_malloc=0:detect_odr_violation=0 # dirty hack for libtrikQsLog and libtrikRuntimeQsLog
env ASAN_OPTIONS=${ASAN_OPTIONS} \
  LSAN_OPTIONS=${LSAN_OPTIONS:-suppressions=$PWD/bin/lsan.supp:print_suppressions=0} \
  QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-minimal} \
  TRIK_PYTHONPATH=${TRIK_PYTHONPATH:-$(python3.${TRIK_PYTHON3_VERSION_MINOR} -c 'import sys; import os; print(os.pathsep.join(sys.path))')} \
  PYTHONMALLOC=${PYTHONMALLOC:-malloc} \
  PYTHONIOENCODING=utf-8 \
  python3.${TRIK_PYTHON3_VERSION_MINOR} "$(dirname $(realpath ${BASH_SOURCE[0]}))"/../tests/fieldstest.py bin/2D-model testing_small
