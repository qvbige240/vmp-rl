
pkginclude_HEADERS = \
	load/load.h \
	rpc/pbrpc_client.h \
	rpc/pbrpc_server.h

lib_LTLIBRARIES=librl.la

librl_la_CFLAGS = -I$(top_srcdir)/src/vmp -I$(top_srcdir)/src/support -I$(top_srcdir)/src/rpc
librl_la_SOURCES=\
	load/load_child.c \
	load/load_child.h \
	load/load_core.c \
	load/load_core.h \
	load/load_typedef.h \
	rpc/pbc/list.h \
	rpc/pbc/pbrpc-clnt.c \
	rpc/pbc/pbrpc-clnt.h \
	rpc/pbc/pbrpc.pb-c.c \
	rpc/pbc/pbrpc.pb-c.h \
	rpc/pbc/pbrpc-svc.c \
	rpc/pbc/pbrpc-svc.h \
	rpc/pbrpc_client.c \
	rpc/pbrpc_client.h \
	rpc/pbrpc_server.c \
	rpc/pbrpc_server.h \
	rpc/service/service-gather.h \
	rpc/service/service-workload.c \
	rpc/service/workload-clnt.c \
	rpc/service/workload.pb-c.c \
	rpc/service/workload.pb-c.h \
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

