/**
 * History:
 * ================================================================
 * 2019-10-10 qing.zou created
 *
 */

#ifndef VMP_NMON_H
#define VMP_NMON_H

#include "vmp_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VMP_NETMAX  32

struct net_stat {
    unsigned long if_name[17];
    unsigned long long if_ibytes;
    unsigned long long if_obytes;
    unsigned long long if_ipackets;
    unsigned long long if_opackets;
    unsigned long if_ierrs;
    unsigned long if_oerrs;
    unsigned long if_idrop;
    unsigned long if_ififo;
    unsigned long if_iframe;
    unsigned long if_odrop;
    unsigned long if_ofifo;
    unsigned long if_ocarrier;
    unsigned long if_ocolls;
};

#define VMP_CPUMAX (192 * 8)	/* MAGIC COOKIE WARNING */

struct cpu_stat {		/* changed the order here to match this years kernel (man 5 /proc/stat) */
    long long user;
    long long nice;
    long long sys;
    long long idle;
    long long wait;		/* for IO */
    long long irq;
    long long softirq;
    long long steal;
    long long guest;
    long long guest_nice;
    /* below are non-cpu based numbers in the same file */
    long long intr;
    long long ctxt;
    long long procs;
    long long running;
    long long blocked;
    float uptime;
    float idletime;
    float mins1;
    float mins5;
    float mins15;
};

#define P_CPUINFO	    0
#define P_STAT		    1
#define P_VERSION	    2
#define P_MEMINFO       3
#define P_UPTIME   	    4
#define P_LOADAVG       5
#define P_NFS   	    6
#define P_NFSD   	    7
#define P_VMSTAT	    8	/* new in 13h */
#define P_NUMBER	    9	/* one more than the max */

#define PROC_MAXLINES   (16*1024)	    /*MAGIC COOKIE WARNING */

struct nmon_proc
{
    FILE *fp;
    char *filename;
    int size;
    int lines;
    char *line[PROC_MAXLINES];
    char *buf;
    int read_this_interval;     /* track updates for each update to stop  double data collection */
};

/* network */
typedef void* (*proc_interfaces_stat)(void *ctx, void *data, int count);
int vmp_proc_net(int reread, proc_interfaces_stat proc, void *ctx);
int vmp_proc_net_stat(struct net_stat *ifstat, int reread);

/* cpu */
void proc_init(struct nmon_proc *proc_info);
void proc_read(struct nmon_proc *proc_info, int num, int reread);
int get_cpu_cnt(struct nmon_proc *proc_info);
void proc_cpu(struct nmon_proc *proc_info, int cpu_num, struct cpu_stat *cpu_total, struct cpu_stat *cpuN);

#ifdef __cplusplus
}
#endif

#endif // VMP_NMON_H
