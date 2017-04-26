AUTOMAKE_OPTIONS = foreign
SUBDIRS = src man

install-data-hook:
	gzip -f "$(DESTDIR)$(mandir)/man1/cygregext.1"

release-zip: install-strip
	zip "$(distdir)-$(shell uname -m).zip" \
		"$(DESTDIR)$(bindir)/cygregext.exe" \
		"$(DESTDIR)$(mandir)/man1/cygregext.1.gz"
