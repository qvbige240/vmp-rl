
pkginclude_HEADERS = \
	pbrpc_client.h \
	pbrpc_server.h

lib_LTLIBRARIES=librl.la

librl_la_SOURCES=\
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
	$(pkg_include_HEADERS)

librl_la_LDFLAGS = -version-info $(LIBRL_VERSION_INFO) $(LDFLAGS_NOUNDEFINED)
