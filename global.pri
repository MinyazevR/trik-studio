# Copyright 2018 Iakov Kirilenko, 2007-2017 QReal Research Group
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#	  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

!isEmpty(_PRO_FILE_):!isEmpty(CONFIG):isEmpty(GLOBAL_PRI_INCLUDED){
#GLOBAL_PRI_INCLUDED = $$PWD

CONFIG *= qt thread exceptions

!win32:CONFIG *= use_gold_linker
#CONFIG *= fat-lto

#deal with mixed configurations
CONFIG -= debug_and_release debug_and_release_target
CONFIG(debug, debug | release): CONFIG -= release
else:!debug:CONFIG *= release
release:CONFIG -= debug
no-sanitizers: CONFIG *= nosanitizers
CONFIG = $$unique(CONFIG)

macx:QT_CONFIG -= no-pkg-config
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += -Og

unix:debug {
		QMAKE_CXXFLAGS += -coverage
		QMAKE_LFLAGS += -coverage
}

!gcc4:!gcc5:!clang:!win32:gcc:*-g++*:system($$QMAKE_CXX --version | grep -qEe '"\\<5\\.[0-9]+\\."' ){ CONFIG += gcc5 }
!gcc4:!gcc5:!clang:!win32:gcc:*-g++*:system($$QMAKE_CXX --version | grep -qEe '"\\<4\\.[0-9]+\\."' ){ CONFIG += gcc4 }

GLOBAL_PWD = $$absolute_path($$PWD)
GLOBAL_OUTPWD = $$absolute_path($$OUT_PWD)


isEmpty(GLOBAL_DESTDIR) {
	GLOBAL_DESTDIR = $$GLOBAL_OUTPWD/bin
}

isEmpty(DESTDIR) {
	DESTDIR = $$GLOBAL_DESTDIR
}

PROJECT_BASENAME = $$basename(_PRO_FILE_)
PROJECT_NAME = $$section(PROJECT_BASENAME, ".", 0, 0)

isEmpty(TARGET) {
	TARGET = $$PROJECT_NAME$$CONFIGURATION_SUFFIX
} else {
	R=$$find( TARGET, "$$CONFIGURATION_SUFFIX$" )
	isEmpty(R):TARGET = $$TARGET$$CONFIGURATION_SUFFIX
}

equals(TEMPLATE, app) {
	VERSION = $${PROJECT_GIT_VERSION_MAJOR}$${PROJECT_GIT_VERSION_MINOR}.$${PROJECT_GIT_VERSION_PATCH}.$${PROJECT_GIT_VERSION_BUILD}
	!no_rpath {
		#reset default rpath before setting new one
		#but this clears path to Qt libraries
		#good for release/installer build, but ...
		#QMAKE_LFLAGS_RPATH =
		#QMAKE_RPATH =

		linux {
			QMAKE_LFLAGS += -Wl,-rpath-link,$$GLOBAL_DESTDIR
			QMAKE_LFLAGS += -Wl,-O1,-rpath,\'\$$ORIGIN\'
			QMAKE_LFLAGS += -Wl,-O1,-rpath,\'\$$ORIGIN/../lib\'
		}
		macx {
			QMAKE_LFLAGS += -rpath @executable_path
			QMAKE_LFLAGS += -rpath @executable_path/../Lib
			QMAKE_LFLAGS += -rpath @executable_path/../Frameworks
			QMAKE_LFLAGS += -rpath @executable_path/../../../
		}
	}
}

#Workaround for a known gcc/ld (before 7.3/bionic) issue
use_gold_linker:!clang: QMAKE_LFLAGS += -Wl,--disable-new-dtags

macx:QMAKE_TARGET_BUNDLE_PREFIX = com.cybertech

macx-clang {
	QMAKE_MACOSX_DEPLOYMENT_TARGET=10.12
	QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
}

equals(TEMPLATE, lib) {
	win32 {
		QMAKE_LFLAGS *= -Wl,--export-all-symbols
	} else:linux {
		#for GNU ld ELF target
		QMAKE_LFLAGS *= -Wl,--export-dynamic
	}else:macx-clang {
		QMAKE_LFLAGS *= -Wl,-export_dynamic
	}
}

!nosanitizers:!clang:gcc:*-g++*:gcc4{
	warning("Disabled sanitizers, failed to detect compiler version or too old compiler: $$QMAKE_CXX")
	CONFIG += nosanitizers
}

!nosanitizers {
	CONFIG += sanitizer
}

unix:!nosanitizers {

	# seems like we want USan always, but are afraid of ....
	!CONFIG(sanitize_address):!CONFIG(sanitize_thread):!CONFIG(sanitize_memory):!CONFIG(sanitize_kernel_address) {
		# Ubsan is turned on by default
		CONFIG += sanitizer sanitize_undefined
	}

	#LSan can be used without performance degrade even in release build
	#But at the moment we can not, because of Qt and MinGW problems
        !win32:!macx-clang:CONFIG(debug):!CONFIG(sanitize_address):!CONFIG(sanitize_memory):!CONFIG(sanitize_thread) { CONFIG += sanitize_leak }

	sanitize_leak {
		QMAKE_CFLAGS *= -fsanitize=leak
		QMAKE_CXXFLAGS *= -fsanitize=leak
		QMAKE_LFLAGS *= -fsanitize=leak
	}

        !win32:sanitize_undefined {
	        #TRIK_SANITIZE_UNDEFINED_FLAGS += \
                #-fsanitize=undefined,float-divide-by-zero,unsigned-integer-overflow,implicit-conversion,local-bounds

		# This hack allows to avoid runtime dependency.
	        # win32:isEmpty(TRIK_SANITIZE_UNDEFINED_FLAGS):TRIK_SANITIZE_UNDEFINED_FLAGS = -fsanitize-undefined-trap-on-error
                
                QMAKE_SANITIZE_UNDEFINED_CFLAGS *= $$TRIK_SANITIZE_UNDEFINED_FLAGS
		QMAKE_SANITIZE_UNDEFINED_CXXFLAGS *= $$TRIK_SANITIZE_UNDEFINED_FLAGS
		QMAKE_SANITIZE_UNDEFINED_LFLAGS *= $$TRIK_SANITIZE_UNDEFINED_FLAGS
	}

	sanitize_memory {
		QMAKE_CFLAGS *= -fsanitize-memory-use-after-dtor -fsanitize-memory-track-origins
		QMAKE_CXXFLAGS *= -fsanitize-memory-use-after-dtor -fsanitize-memory-track-origins

	}

        unix {
                QMAKE_CFLAGS_RELEASE += -fsanitize-recover=all
                QMAKE_CXXFLAGS_RELEASE += -fsanitize-recover=all

                QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO += -fno-sanitize-recover=all
                QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += -fno-sanitize-recover=all

                QMAKE_CFLAGS_DEBUG  += -fno-sanitize-recover=all
                QMAKE_CXXFLAGS_DEBUG += -fno-sanitize-recover=all
        }
}

!noPch:CONFIG *= precompile_header

precompile_header:isEmpty(PRECOMPILED_HEADER):PRECOMPILED_HEADER = $$PWD/pch.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
	QMAKE_CXXFLAGS *= -include $$PRECOMPILED_HEADER -fpch-preprocess
}

!warn_off:QMAKE_CXXFLAGS *= -Wno-error=invalid-pch

QMAKE_CXXFLAGS_DEBUG += -Og -ggdb

small_debug_info:QMAKE_CXXFLAGS += -g1


INCLUDEPATH += $$absolute_path($$_PRO_FILE_PWD_) \
	$$absolute_path($$_PRO_FILE_PWD_/include) \
	$$absolute_path($$PWD) \


CONFIG *= c++14

DEFINES *= QT_FORCE_ASSERTS

DEFINES *= QT_NO_ACCESSIBILITY

!warn_off:QMAKE_CXXFLAGS += -pedantic-errors -Wextra

!clang: QMAKE_CXXFLAGS += -ansi

!warn_off:QMAKE_CXXFLAGS +=-Werror=pedantic -Werror=delete-incomplete

gcc:versionAtLeast(QT_VERSION, 5.15.0):QMAKE_CXXFLAGS *= -Wno-deprecated-declarations

clang {
	#treat git submodules as system path
	SYSTEM_INCLUDE_PREFIX_OPTION += $$system(git submodule status 2>/dev/null | sed $$shell_quote('s/^.[0-9a-fA-F]* \\([^ ]*\\).*$/--system-header-prefix=\\1/g'))

	#treat Qt includes as system headers
	SYSTEM_INCLUDE_PREFIX_OPTION *= --system-header-prefix=$$[QT_INSTALL_HEADERS]
	QMAKE_CXXFLAG += -Wno-nullability-completeness -Wno-error=nullability-extension
}

#gcc {
	#treat Qt includes as system headers
	SYSTEM_INCLUDE_PREFIX_OPTION *= -isystem $$[QT_INSTALL_HEADERS]
#}

QMAKE_CXXFLAGS *= $$SYSTEM_INCLUDE_PREFIX_OPTION

false:clang {
# Problem from Qt system headers
	!warn_off:QMAKE_CXXFLAGS += -Wno-error=expansion-to-defined
}


!warn_off:QMAKE_CXXFLAGS += -Werror=cast-qual -Werror=write-strings -Werror=redundant-decls -Werror=unreachable-code \
			-Werror=non-virtual-dtor -Wno-error=overloaded-virtual \
			-Werror=uninitialized -Werror=init-self

# Hack to log build time.
# ------------------------
PHONY_DEPS = $${PROJECT_NAME}.time.txt
PreBuildTimerEvent.input = PHONY_DEPS
PreBuildTimerEvent.output = $${PROJECT_NAME}.time.txt
PreBuildTimerEvent.commands = \\\"\\033[34;1m$$PROJECT_NAME build started\\033[0m
PreBuildTimerEvent.commands += \\033[34;1mat \$\$(touch $${PreBuildTimerEvent.output} && date -r $${PreBuildTimerEvent.output} +%s) \\033[0m \\\"
PreBuildTimerEvent.commands = bash -c \"echo -e $$PreBuildTimerEvent.commands\"
PreBuildTimerEvent.name = Timer for $${PROJECT_NAME}
PreBuildTimerEvent.CONFIG += no_link no_clean target_predeps
QMAKE_EXTRA_COMPILERS += PreBuildTimerEvent

QMAKE_POST_LINK += bash -c \"echo -e \\\"\\033[34;1m$$PROJECT_NAME build finished\\033[0m
QMAKE_POST_LINK += \\033[34;1min \$\$(( `date +%s` - `date -r $${PreBuildTimerEvent.output} +%s`))\\033[0m\\\"\" $$escape_expand(\\n\\t) $$escape_expand(\\n\\t)


#--------------------------

# Simple function that checks if given argument is a file or directory.
# Returns false if argument 1 is a file or does not exist.
defineTest(isDir) {
	exists($$system_path($$1/*)):return(true)
	return(false)
}

# Useful function to copy additional files to destination,
# from http://stackoverflow.com/questions/3984104/qmake-how-to-copy-a-file-to-the-output
defineTest(copyToDestdir) {
	FILES = $$1
	NOW = $$2

	for(FILE, FILES) {
		DESTDIR_SUFFIX =
		AFTER_SLASH = $$section(FILE, "/", -1, -1)
		isDir($$FILE) {
			ABSOLUTE_PATH = $$absolute_path($$FILE, $$GLOBAL_PWD)
			BASE_NAME = $$section(ABSOLUTE_PATH, "/", -1, -1)
			DESTDIR_SUFFIX = $$BASE_NAME/
			FILE = $$FILE/*
		}

		win32:if(!isEmpty(NOW)|isEmpty(QMAKE_SH)) {
			# probably, xcopy needs /s and /e for directories
			COPY_DIR = "cmd.exe /C xcopy /y /i /s /e "
			!silent: COPY_DIR += /f
		} else {
			COPY_DIR = rsync -a
			!silent: COPY_DIR += -v
		}

		!isEmpty(NOW):if(!build_pass|isEmpty(BUILDS)) {
		    DDIR = $$quote($$system_path($$DESTDIR/$$3$$DESTDIR_SUFFIX))
		    FILE = $$quote($$system_path($$FILE))
		    mkpath($$DDIR)
		    system($$COPY_DIR $$FILE $$DDIR)
		} else {
		    DDIR = $$quote($$shell_path($$DESTDIR/$$3$$DESTDIR_SUFFIX))
		    FILE = $$quote($$shell_path($$FILE))
		    QMAKE_POST_LINK += $(MKDIR) $$DDIR $$escape_expand(\\n\\t)
		    QMAKE_POST_LINK += $$COPY_DIR $$FILE $$DDIR $$escape_expand(\\n\\t)
		}
	}

	export(QMAKE_POST_LINK)
}

defineTest(includes) {
	PROJECTS = $$1

	for(PROJECT, PROJECTS) {
		INCLUDEPATH += $$GLOBAL_PWD/$$PROJECT $$GLOBAL_PWD/$$PROJECT/include
	}

	export(INCLUDEPATH)
}

defineTest(links) {
	LIBS *= -L$$GLOBAL_DESTDIR
	LIBS *= -L$$DESTDIR
	PROJECTS = $$1

	for(PROJECT, PROJECTS) {
		LIBS += -l$$PROJECT$$CONFIGURATION_SUFFIX
	}

	export(LIBS)
}

defineTest(noPch) {
	CONFIG -= precompile_header
	PRECOMPILED_HEADER =
	export(CONFIG)
	export(PRECOMPILED_HEADER)
}

defineTest(enableFlagIfCan) {
  system(/bin/echo $$shell_quote(int main(){return 0;}) | $$QMAKE_CXX $$QMAKE_CXXFLAGS $$1 -x c++ -c - -o $$system(mktemp) 2>/dev/null ) {
    QMAKE_CXXFLAGS += $$1
  export(QMAKE_CXXFLAGS)
  } else {
    message(Cannot enable $$1)
  }
}

defineReplace(fullSystemPath) {
	return($$system_path($$clean_path($$absolute_path($$1))))
}

CONFIG(noPch) {
	noPch()
}


includes(thirdparty/qslog/qslog)
links(trikQsLog)

} # GLOBAL_PRI_INCLUDED
