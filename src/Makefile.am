AM_CXXFLAGS = -std=c++11 -Wall -Wextra
AM_LDFLAGS =
AM_CPPFLAGS = -DUNICODE -D_UNICODE

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = cygscript
cygscript_SOURCES = main.cpp app.cpp register.cpp exec.cpp \
    strconv.cpp winerror.cpp elevated.cpp registry/key.cpp
