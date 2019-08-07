
if NMON

    bin_PROGRAMS = nmon

    AM_CPPFLAGS=-I$(top_srcdir)

    nmon_CFLAGS=$(CFLAGS) $(NMON_CFLAGS)
    nmon_LDFLAGS=$(EXTRA_LDFLAGS)
    nmon_SOURCES = lmon16f.c
    nmon_LDADD=../src/librl.la $(NMON_LIBS)
    LIBS=$(NMON_LIBS)

endif


bin_PROGRAMS = load

#load_CFLAGS=$(CFLAGS)
#load_LDFLAGS=$(EXTRA_LDFLAGS)
load_SOURCES = main.c
load_LDADD=\
../src/librl.la \
$(STATIC_LIBS) $(DYNAMIC_LIBS)
#LIBS= -lc
#LIBS=$(DYNAMIC_LIBS)
