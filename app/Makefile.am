#bin_PROGRAMS = load

#AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/src/

#load_CFLAGS=$(CFLAGS)
#load_LDFLAGS=$(EXTRA_LDFLAGS)
#load_SOURCES = main.c
#load_LDADD=\
#../src/librl.la \
#$(STATIC_LIBS) $(DYNAMIC_LIBS)
#LIBS= -lc
#LIBS=$(DYNAMIC_LIBS)


LDADD = ../src/librl.la $(STATIC_LIBS) $(DYNAMIC_LIBS) 
#AM_LDFLAGS = -L$(LIBRL_LDFLAGS)
AM_LDFLAGS = -L$(top_builddir)/src/.libs
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/include -I$(top_srcdir)/src -I$(top_srcdir)/src/rpc

noinst_PROGRAMS = rpc-client rpc-server time-test

rpc_client_SOURCES = rpc-client.c 
rpc_server_SOURCES = rpc-server.c
time_test_SOURCES = time-test.c

if NMON
    noinst_PROGRAMS += nmon
    nmon_CFLAGS = $(NMON_CFLAGS)
    nmon_SOURCES = lmon16f.c
    nmon_LDADD= $(LDADD) $(NMON_LIBS)
endif

    noinst_PROGRAMS += vmp-daemon
    vmp_daemon_CFLAGS = -I$(top_srcdir)/src/vmp -I$(top_srcdir)/src/support -I$(top_srcdir)/src/load
    vmp_daemon_SOURCES = vmp-daemon.c

verify:

DISTCLEANFILES = *~

