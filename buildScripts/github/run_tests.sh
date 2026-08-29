#!/bin/bash
set -uo

export QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-minimal}

export ASAN_OPTIONS=$(if [[ $RUNNER_OS == Linux ]]; then echo 'detect_leaks=1:'; else echo -n ''; fi)detect_stack_use_after_return=1:fast_unwind_on_malloc=0
export LSAN_OPTIONS=suppressions=$PWD/bin/lsan.supp:print_suppressions=0
export DISPLAY=:0
export TRIK_PYTHONPATH=${TRIK_PYTHONPATH:-$(python3.${TRIK_PYTHON3_VERSION_MINOR} -c 'import sys; import os; print(os.pathsep.join(sys.path))')}
export PYTHONMALLOC=${PYTHONMALLOC:-malloc}
export PYTHONIOENCODING=utf-8
env


PYTHONQT_RUN_ONLY_MEMORY_TESTS="true" make check -k -s
pushd "bin"

for i in {1..100}; do
    echo "Run #$i"

    strace -f -o strace.log ./trik-v62-qts-generator-tests
    ret=$?
    cat strace.log
    ((ret == 0)) || break
done

сat strace.log
 
eval "$TESTS" && popd

[ -r tests_qrs.7z ] || curl -Lo tests_qrs.7z https://dl.trikset.com/edu/.solutions20200701/testing_small.7z
which 7z &> /dev/null && 7z -y x tests_qrs.7z || 7za x tests_qrs.7z
python3.${TRIK_PYTHON3_VERSION_MINOR} "$(dirname $(realpath ${BASH_SOURCE[0]}))"/../tests/fieldstest.py bin/2D-model testing_small

XML_PREPOCESSOR_TEST_DIR="$(dirname $(realpath ${BASH_SOURCE[0]}))"/../tests/xml-preprocessor-tests/
XML_PREPOCESSOR_OUTPUT_FILE="$XML_PREPOCESSOR_TEST_DIR/system_library.xml"
XML_PREPOCESSOR_TEST_FILE="$XML_PREPOCESSOR_TEST_DIR/system_library.qrs"
XML_PREPOCESSOR_EXPECTED_FILE="$XML_PREPOCESSOR_TEST_DIR/system_library_expected.xml"
TRIK_PREPROCESSOR_XML_OUTPUT="$XML_PREPOCESSOR_OUTPUT_FILE" bin/2D-model --close "$XML_PREPOCESSOR_TEST_FILE" || true
diff "$XML_PREPOCESSOR_OUTPUT_FILE" "$XML_PREPOCESSOR_EXPECTED_FILE" || exit 1
