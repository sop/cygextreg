AM_CXXFLAGS = -std=c++11 -Wall -Wextra -mwindows
AM_LDFLAGS =
AM_CPPFLAGS = -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0600

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = cygscript
cygscript_SOURCES = main.cpp app.cpp register.cpp exec.cpp \
    strconv.cpp message.cpp winerror.cpp elevated.cpp \
    registry/key.cpp env/envvar.cpp
cygscript_LDADD = -lshlwapi
