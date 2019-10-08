
pkginclude_HEADERS = \
    load/load.h \
    rpc/pbrpc_client.h \
    rpc/pbrpc_server.h \
    rpc/rpc_clnt.h \
    rpc/rpc_svc.h

lib_LTLIBRARIES=librl.la

librl_la_CFLAGS = -I$(top_srcdir)/src/vmp -I$(top_srcdir)/src/support -I$(top_srcdir)/src/rpc
librl_la_SOURCES=\
    ipcs/ipc_shared.c \
    ipcs/ipc_shared.h \
    load/load_child.c \
    load/load_child.h \
    load/load_core.c \
    load/load_core.h \
    load/load_demo.c \
    load/load_demo.h \
    load/load_typedef.h \
    load/load_vmon.c \
    load/load_vmon.h \
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
    rpc/clnt_demo.c \
    rpc/clnt_demo.h \
    rpc/clnt_server_registry.c \
    rpc/clnt_server_registry.h \
    rpc/clnt_vmon.c \
    rpc/clnt_vmon.h \
    rpc/rpc_clnt.c \
    rpc/rpc_clnt.h \
    rpc/rpc_svc.c \
    rpc/rpc_svc.h \
    rpc/rpc_typedef.h \
    rpc/service-typedef.h \
    rpc/service-demo.c \
    rpc/service-demo.h \
    rpc/service-vmon.c \
    rpc/service-vmon.h \
    rpc/service-server-registry.c \
    rpc/service-server-registry.h \
    rpc/service/pbc-demo-clnt.c \
    rpc/service/pbc-demo-service.c \
    rpc/service/pbc-demo.pb-c.c \
    rpc/service/pbc-demo.pb-c.h \
    rpc/service/pbc-registry.pb-c.c \
    rpc/service/pbc-registry.pb-c.h \
    rpc/service/pbc-vmon.pb-c.c \
    rpc/service/pbc-vmon.pb-c.h \
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
    vmp/vmp_shared.c \
    vmp/vmp_shared.h \
    vmp/vmp_thread.c \
    vmp/vmp_thread.h \
    vmp/vmp_typedef.h \
    $(pkg_include_HEADERS)

librl_la_LDFLAGS = -version-info $(LIBRL_VERSION_INFO) $(LDFLAGS_NOUNDEFINED)

