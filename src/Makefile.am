
pkginclude_HEADERS = \
	load/load.h \
	pbrpc_client.h \
	pbrpc_server.h

lib_LTLIBRARIES=librl.la

librl_la_CFLAGS = -I$(top_srcdir)/src/vmp -I$(top_srcdir)/src/support
librl_la_SOURCES=\
	load/load_child.c \
	load/load_child.h \
	load/load_core.c \
	load/load_core.h \
	load/load_typedef.h \
	pbc/list.h \
	pbc/pbrpc-clnt.c \
	pbc/pbrpc-clnt.h \
	pbc/pbrpc.pb-c.c \
	pbc/pbrpc.pb-c.h \
	pbc/pbrpc-svc.c \
	pbc/pbrpc-svc.h \
	pbrpc_client.c \
	pbrpc_server.c \
	service/service-workload.c \
	service/service-workload.h \
	service/workload-clnt.c \
	service/workload.pb-c.c \
	service/workload.pb-c.h \
	support/globals.c \
	support/globals.h \
	support/module_config.c \
	support/module_config.h \
	support/module_log.c \
	support/module_log.h \
	support/module_support.h \
	vmp/vmp.h \
	vmp/vmp_log.h \
	vmp/vmp_process.c \
	vmp/vmp_process.h \
	vmp/vmp_thread.c \
	vmp/vmp_thread.h \
	vmp/vmp_typedef.h \
	$(pkg_include_HEADERS)

librl_la_LDFLAGS = -version-info $(LIBRL_VERSION_INFO) $(LDFLAGS_NOUNDEFINED)

