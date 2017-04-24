AUTOMAKE_OPTIONS = foreign
SUBDIRS = src

release-zip: install-strip
	zip $(distdir)-$(shell uname -m).zip /bin/cygregext.exe
