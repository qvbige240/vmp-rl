
pkginclude_HEADERS = \
	client.h

lib_LTLIBRARIES=librl.la

librl_la_SOURCES=\
	client.c \
	$(pkg_include_HEADERS)

librl_la_LDFLAGS = -version-info $(LIBRL_VERSION_INFO) $(LDFLAGS_NOUNDEFINED)
