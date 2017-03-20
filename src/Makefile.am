AM_CXXFLAGS = -std=c++11 -Wall -Wextra -mwindows
AM_LDFLAGS =
AM_CPPFLAGS = -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0600

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = cygscript
cygscript_SOURCES = main.cpp app.cpp \
	util/strconv.cpp util/message.cpp util/winerror.cpp util/elevated.cpp \
	util/cygpath.cpp util/winpath.cpp \
	cmd/register.cpp cmd/exec.cpp cmd/list.cpp \
	registry/registry.cpp registry/key.cpp env/envvar.cpp \
	cygscript.rc
cygscript_LDADD = -lshlwapi

.rc.o:
	windres -o $@ $<
