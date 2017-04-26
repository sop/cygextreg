AUTOMAKE_OPTIONS = foreign
SUBDIRS = src man

install-data-hook:
	gzip -f "$(DESTDIR)$(mandir)/man1/cygregext.1"

RELEASEDIR=$(abs_top_builddir)/release
RELEASEZIP=$(abs_top_builddir)/$(distdir)-$(shell uname -m).zip
release-zip: install-strip
	rm -rf "$(RELEASEDIR)" && \
	rm -f "$(RELEASEZIP)" && \
	$(MKDIR_P) "$(RELEASEDIR)" && \
	$(install_sh) "$(DESTDIR)$(bindir)/cygregext.exe" "$(RELEASEDIR)/bin/cygregext.exe" && \
	$(install_sh) "$(DESTDIR)$(mandir)/man1/cygregext.1.gz" "$(RELEASEDIR)$(mandir)/man1/cygregext.1.gz" && \
	cd "$(RELEASEDIR)" && zip -r "$(RELEASEZIP)" *
