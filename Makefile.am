AUTOMAKE_OPTIONS = foreign
SUBDIRS = src man

install-data-hook:
	gzip -f "$(mandir)/man1/cygregext.1"

release-zip: install-strip
	zip "$(distdir)-$(shell uname -m).zip" \
		"$(bindir)/cygregext.exe" "$(mandir)/man1/cygregext.1.gz"
