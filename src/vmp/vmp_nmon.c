/**
 * History:
 * ================================================================
 * 2019-10-10 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vmp_nmon.h"

#define MALLOC(argument)        malloc(argument)
#define FREE(argument)          free(argument)
#define REALLOC(argument1,argument2)    realloc(argument1,argument2)

void strip_spaces(char *s)
{
    char *p;
    int spaced = 1;

    p = s;
    for (p = s; *p != 0; p++)
    {
        if (*p == ':')
            *p = ' ';
        if (*p != ' ')
        {
            *s = *p;
            s++;
            spaced = 0;
        }
        else if (spaced)
        {
            /* do no thing as this is second space */
        }
        else
        {
            *s = *p;
            s++;
            spaced = 1;
        }
    }
    *s = 0;
}

// int vmp_proc_net(nmon_data_t *p, int reread)
// {
//     static FILE *fp = (FILE *)-1;
//     char buf[1024];
//     int i = 0;
//     int ret;
//     unsigned long junk;
//     int networks = 0;

//     if (fp == (FILE *)-1)
//     {
//         if ((fp = fopen("/proc/net/dev", "r")) == NULL)
//         {
//             error("failed to open - /proc/net/dev");
//             networks = 0;
//             return networks;
//         }
//     }
//     if (fgets(buf, 1024, fp) == NULL)
//         goto end; /* throw away the header lines */
//     if (fgets(buf, 1024, fp) == NULL)
//         goto end; /* throw away the header lines */
// /*
// Inter-|   Receive                                                |  Transmit
//  face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
//     lo:    1956      30    0    0    0     0          0         0     1956      30    0    0    0     0       0          0
//   eth0:       0       0    0    0    0     0          0         0   458718       0  781    0    0     0     781          0
//   sit0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
//   eth1:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
// */
//     for (i = 0; i < VMP_NETMAX; i++)
//     {
// 	if (fgets(buf, 1024, fp) == NULL)
// 	    break;
// 	strip_spaces(buf);
// 	/* 1   2   3    4   5   6   7   8   9   10   11   12  13  14  15  16 */
// 	ret =
// 	    sscanf(&buf[0],
// 		   "%s %llu %llu %lu %lu %lu %lu %lu %lu %llu %llu %lu %lu %lu %lu %lu",
// 		   (char *) &p->ifnets[i].if_name, &p->ifnets[i].if_ibytes,
// 		   &p->ifnets[i].if_ipackets, &p->ifnets[i].if_ierrs,
// 		   &p->ifnets[i].if_idrop, &p->ifnets[i].if_ififo,
// 		   &p->ifnets[i].if_iframe, &junk, &junk,
// 		   &p->ifnets[i].if_obytes, &p->ifnets[i].if_opackets,
// 		   &p->ifnets[i].if_oerrs, &p->ifnets[i].if_odrop,
// 		   &p->ifnets[i].if_ofifo, &p->ifnets[i].if_ocolls,
// 		   &p->ifnets[i].if_ocarrier);
// 	if (ret != 16)
// 	    fprintf(stderr, "sscanf wanted 16 returned = %d line=%s\n",
// 		    ret, (char *) buf);
//     }
// end:
//     if (reread)
//     {
//         fclose(fp);
//         fp = (FILE *)-1;
//     }
//     else
//         rewind(fp);
//     networks = i;
//     return networks;
// }

int vmp_proc_net_stat(struct net_stat *ifstat, int reread)
{
    static FILE *fp = (FILE *)-1;
    char buf[1024];
    int i = 0;
    int ret;
    unsigned long junk;
    int networks = 0;

    if (fp == (FILE *)-1)
    {
        if ((fp = fopen("/proc/net/dev", "r")) == NULL)
        {
            perror("failed to open - /proc/net/dev");
            networks = 0;
            return networks;
        }
    }
    if (fgets(buf, 1024, fp) == NULL)
        goto end; /* throw away the header lines */
    if (fgets(buf, 1024, fp) == NULL)
        goto end; /* throw away the header lines */
/*
Inter-|   Receive                                                |  Transmit
 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
    lo:    1956      30    0    0    0     0          0         0     1956      30    0    0    0     0       0          0
  eth0:       0       0    0    0    0     0          0         0   458718       0  781    0    0     0     781          0
  sit0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  eth1:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
*/
    struct net_stat *ifnets = ifstat;
    for (i = 0; i < VMP_NETMAX; i++)
    {
        //memset(&ifnets, 0x0, sizeof(struct net_stat));
        if (fgets(buf, 1024, fp) == NULL)
            break;
        strip_spaces(buf);
        /* 1   2   3    4   5   6   7   8   9   10   11   12  13  14  15  16 */
        ret =
            sscanf(&buf[0],
                   "%s %llu %llu %lu %lu %lu %lu %lu %lu %llu %llu %lu %lu %lu %lu %lu",
                   (char *)&ifnets[i].if_name, &ifnets[i].if_ibytes,
                   &ifnets[i].if_ipackets, &ifnets[i].if_ierrs,
                   &ifnets[i].if_idrop, &ifnets[i].if_ififo,
                   &ifnets[i].if_iframe, &junk, &junk,
                   &ifnets[i].if_obytes, &ifnets[i].if_opackets,
                   &ifnets[i].if_oerrs, &ifnets[i].if_odrop,
                   &ifnets[i].if_ofifo, &ifnets[i].if_ocolls,
                   &ifnets[i].if_ocarrier);
        if (ret != 16)
            fprintf(stderr, "sscanf wanted 16 returned = %d line=%s\n", ret, (char *)buf);
    }

end:
    if (reread)
    {
        fclose(fp);
        fp = (FILE *)-1;
    }
    else
        rewind(fp);
    networks = i;
    return networks;
}

int vmp_proc_net(int reread, proc_interfaces_stat proc, void *ctx)
{
    static FILE *fp = (FILE *)-1;
    char buf[1024];
    int i = 0;
    int ret;
    unsigned long junk;
    int networks = 0;

    if (fp == (FILE *)-1)
    {
        if ((fp = fopen("/proc/net/dev", "r")) == NULL)
        {
            perror("failed to open - /proc/net/dev");
            networks = 0;
            return networks;
        }
    }
    if (fgets(buf, 1024, fp) == NULL)
        goto end; /* throw away the header lines */
    if (fgets(buf, 1024, fp) == NULL)
        goto end; /* throw away the header lines */
/*
Inter-|   Receive                                                |  Transmit
 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
    lo:    1956      30    0    0    0     0          0         0     1956      30    0    0    0     0       0          0
  eth0:       0       0    0    0    0     0          0         0   458718       0  781    0    0     0     781          0
  sit0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
  eth1:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
*/
    struct net_stat ifnets[VMP_NETMAX] = {0};
    for (i = 0; i < VMP_NETMAX; i++)
    {
        //memset(&ifnets, 0x0, sizeof(struct net_stat));
        if (fgets(buf, 1024, fp) == NULL)
            break;
        strip_spaces(buf);
        /* 1   2   3    4   5   6   7   8   9   10   11   12  13  14  15  16 */
        ret =
            sscanf(&buf[0],
                   "%s %llu %llu %lu %lu %lu %lu %lu %lu %llu %llu %lu %lu %lu %lu %lu",
                   (char *)&ifnets[i].if_name, &ifnets[i].if_ibytes,
                   &ifnets[i].if_ipackets, &ifnets[i].if_ierrs,
                   &ifnets[i].if_idrop, &ifnets[i].if_ififo,
                   &ifnets[i].if_iframe, &junk, &junk,
                   &ifnets[i].if_obytes, &ifnets[i].if_opackets,
                   &ifnets[i].if_oerrs, &ifnets[i].if_odrop,
                   &ifnets[i].if_ofifo, &ifnets[i].if_ocolls,
                   &ifnets[i].if_ocarrier);
        if (ret != 16)
            fprintf(stderr, "sscanf wanted 16 returned = %d line=%s\n", ret, (char *)buf);
    }
    proc(ctx, &ifnets, i);
end:
    if (reread)
    {
        fclose(fp);
        fp = (FILE *)-1;
    }
    else
        rewind(fp);
    networks = i;
    return networks;
}

/**  proc  **/
int proc_cpu_done = 0;		/* Flag if we have run function proc_cpu() already in this interval */
/* Counts of resources */
// int cpus = 1;			/* number of CPUs in system (lets hope its more than zero!) */
// int old_cpus = 1;		/* Number of CPU seen in previuos interval */

void proc_init(struct nmon_proc *proc_info)
{
    proc_info[P_CPUINFO].filename = "/proc/cpuinfo";
    proc_info[P_STAT].filename = "/proc/stat";
    proc_info[P_VERSION].filename = "/proc/version";
    proc_info[P_MEMINFO].filename = "/proc/meminfo";
    proc_info[P_UPTIME].filename = "/proc/uptime";
    proc_info[P_LOADAVG].filename = "/proc/loadavg";
    proc_info[P_NFS].filename = "/proc/net/rpc/nfs";
    proc_info[P_NFSD].filename = "/proc/net/rpc/nfsd";
    proc_info[P_VMSTAT].filename = "/proc/vmstat";
}

void proc_read(struct nmon_proc *proc_info, int num, int reread)
{
    int i;
    int size;
    int found;
    char buf[1024];

    if (proc_info->read_this_interval == 1)
        return;

    if (proc_info->fp == 0)
    {
        if ((proc_info->fp = fopen(proc_info->filename, "r")) == NULL)
        {
            sprintf(buf, "failed to open file %s", proc_info->filename);
            perror(buf);
            proc_info->fp = 0;
            return;
        }
    }
    rewind(proc_info->fp);

    /* We re-read P_STAT, now flag proc_cpu() that it has to re-process that data */
    if (num == P_STAT)
        proc_cpu_done = 0;

    if (proc_info->size == 0)
    {
        /* first time so allocate  initial now */
        proc_info->buf = MALLOC(512);
        proc_info->size = 512;
    }

    for (i = 0; i < 4096; i++)
    { /* MAGIC COOKIE WARNING  POWER8 default install can have 2655 processes */
        size = fread(proc_info->buf, 1, proc_info->size - 1, proc_info->fp);
        if (size < proc_info->size - 1)
            break;
        proc_info->size += 512;
        proc_info->buf = REALLOC(proc_info->buf, proc_info->size);
        rewind(proc_info->fp);
    }

    proc_info->buf[size] = 0;
    proc_info->lines = 0;
    proc_info->line[0] = &proc_info->buf[0];
    if (num == P_VERSION)
    {
        found = 0;
        for (i = 0; i < size; i++)
        { /* remove some weird stuff found the hard way in various Linux versions and device drivers */
            /* place ") (" on two lines */
            if (found == 0 &&
                proc_info->buf[i] == ')' &&
                proc_info->buf[i + 1] == ' ' &&
                proc_info->buf[i + 2] == '(')
            {
                proc_info->buf[i + 1] = '\n';
                found = 1;
            }
            else
            {
                /* place ") #" on two lines */
                if (proc_info->buf[i] == ')' &&
                    proc_info->buf[i + 1] == ' ' &&
                    proc_info->buf[i + 2] == '#')
                {
                    proc_info->buf[i + 1] = '\n';
                }
                /* place "#1" on two lines */
                if (proc_info->buf[i] == '#' && proc_info->buf[i + 2] == '1')
                {
                    proc_info->buf[i] = '\n';
                }
            }
        }
    }
    for (i = 0; i < size; i++)
    {
        /* replace Tab characters with space */
        if (proc_info->buf[i] == '\t')
        {
            proc_info->buf[i] = ' ';
        }
        else if (proc_info->buf[i] == '\n')
        {
            /* replace newline characters with null */
            proc_info->lines++;
            proc_info->buf[i] = '\0';
            proc_info->line[proc_info->lines] = &proc_info->buf[i + 1];
        }
        if (proc_info->lines == PROC_MAXLINES - 1)
            break;
    }
    if (reread)
    {
        fclose(proc_info->fp);
        proc_info->fp = 0;
    }
    /* Set flag so we do not re-read the data even if called multiple times in same interval */
    proc_info->read_this_interval = 1;
}

int get_cpu_cnt(struct nmon_proc *proc_info)
{
    int i, cpus = 0;

    /* Get CPU info from /proc/stat and populate proc[P_STAT] */
    proc_read(proc_info, P_STAT, 0);

    /* Start with index [1] as [0] contains overall CPU statistics */
    for (i = 1; i < proc_info->lines; i++)
    {
        if (strncmp("cpu", proc_info->line[i], 3) == 0)
            cpus = i;
        else
            break;
    }
    if (cpus >= VMP_CPUMAX)
    {
        printf("This nmon supports only %d CPU threads (Logical CPUs) and the machine appears to have %d.\nnmon stopping as its unsafe to continue.\n",
               VMP_CPUMAX, cpus);
        exit(44);
    }
    return cpus;
}

void proc_cpu(struct nmon_proc *proc_info, int cpu_num, struct cpu_stat *cpu_total, struct cpu_stat *cpuN)
{
    int i;
    int row;
    static int intr_line = 0;
    static int ctxt_line = 0;
    static int btime_line = 0;
    static int proc_line = 0;
    static int run_line = 0;
    static int block_line = 0;
    static int proc_cpu_first_time = 1;
    long long user;
    long long nice;
    long long sys;
    long long idle;
    long long iowait;
    long long hardirq;
    long long softirq;
    long long steal;
    long long guest;
    long long guest_nice;

    static int stat8 = 0;
    static int stat10 = 0;

    static int old_cpus = 1;
    int cpus = cpu_num;

    /* Only read data once per interval */
    if (proc_cpu_done == 1)
        return;

    /* If number of CPUs changed, then we need to find the index of intr_line, ... again */
    if (old_cpus != cpus)
        intr_line = 0;

    if (proc_cpu_first_time)
    {
        stat8 =
            sscanf(&proc_info->line[0][5],
                   "%lld %lld %lld %lld %lld %lld %lld %lld", &user, &nice,
                   &sys, &idle, &iowait, &hardirq, &softirq, &steal);
        stat10 =
            sscanf(&proc_info->line[0][5],
                   "%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
                   &user, &nice, &sys, &idle, &iowait, &hardirq, &softirq,
                   &steal, &guest, &guest_nice);
        proc_cpu_first_time = 0;
    }
    user = nice = sys = idle = iowait = hardirq = softirq = steal = guest = guest_nice = 0;
    if (stat10 == 10)
    {
        sscanf(&proc_info->line[0][5],
               "%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", &user,
               &nice, &sys, &idle, &iowait, &hardirq, &softirq, &steal,
               &guest, &guest_nice);
    }
    else
    {
        if (stat8 == 8) {
            sscanf(&proc_info->line[0][5],
                   "%lld %lld %lld %lld %lld %lld %lld %lld", &user, &nice,
                   &sys, &idle, &iowait, &hardirq, &softirq, &steal);
        } else { /* stat 4 variables here as older Linux proc */
            sscanf(&proc_info->line[0][5], "%lld %lld %lld %lld",
                   &user, &nice, &sys, &idle);
        }
    }
    cpu_total->user = user;
    cpu_total->nice = nice;
    cpu_total->idle = idle;
    cpu_total->sys = sys;
    cpu_total->wait = iowait; /* in the case of 4 variables = 0 */
    /* cpu_total->sys  = sys + hardirq + softirq + steal; */

    cpu_total->irq = hardirq;
    cpu_total->softirq = softirq;
    cpu_total->steal = steal;
    cpu_total->guest = guest;
    cpu_total->guest_nice = guest_nice;

#ifdef _DEBUG
    //if (debug)
    fprintf(stderr, "XX user=%lld wait=%lld sys=%lld idle=%lld\n",
            cpu_total->user, cpu_total->wait, cpu_total->sys, cpu_total->idle);
    fprintf(stderr, "XX stat8=%d, stat10=%d\n", stat8, stat10);
#endif /*DEBUG*/

    for (i = 0; i < cpus; i++)
    {
        user = nice = sys = idle = iowait = hardirq = softirq = steal = 0;

        /* allow for large CPU numbers */
        if (i + 1 > 1000)
            row = 8;
        else if (i + 1 > 100)
            row = 7;
        else if (i + 1 > 10)
            row = 6;
        else
            row = 5;

        if (stat10 == 10)
        {
            sscanf(&proc_info->line[i + 1][row],
                   "%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
                   &user,
                   &nice,
                   &sys,
                   &idle,
                   &iowait,
                   &hardirq, &softirq, &steal, &guest, &guest_nice);
        }
        else if (stat8 == 8)
        {
            sscanf(&proc_info->line[i + 1][row],
                   "%lld %lld %lld %lld %lld %lld %lld %lld",
                   &user,
                   &nice,
                   &sys, &idle, &iowait, &hardirq, &softirq, &steal);
        }
        else
        {
            sscanf(&proc_info->line[i + 1][row], "%lld %lld %lld %lld",
                   &user, &nice, &sys, &idle);
        }
        cpuN[i].user = user;
        cpuN[i].nice = nice;
        cpuN[i].sys = sys;
        cpuN[i].idle = idle;
        cpuN[i].wait = iowait;
        cpuN[i].irq = hardirq;
        cpuN[i].softirq = softirq;
        cpuN[i].steal = steal;
        cpuN[i].guest = guest;
        cpuN[i].guest_nice = guest_nice;
    }

    if (intr_line == 0)
    {
        if (proc_info->line[i + 1][0] == 'p' &&
            proc_info->line[i + 1][1] == 'a' &&
            proc_info->line[i + 1][2] == 'g' &&
            proc_info->line[i + 1][3] == 'e')
        {
            /* 2.4 kernel */
            intr_line = i + 3;
            ctxt_line = i + 5;
            btime_line = i + 6;
            proc_line = i + 7;
            run_line = i + 8;
            block_line = i + 9;
        }
        else
        {
            /* 2.6 kernel */
            intr_line = i + 1;
            ctxt_line = i + 2;
            btime_line = i + 3;
            proc_line = i + 4;
            run_line = i + 5;
            block_line = i + 6;
        }
    }
    cpu_total->intr = -1;
    cpu_total->ctxt = -1;
    cpu_total->procs = -1;
    cpu_total->running = -1;
    cpu_total->blocked = -1;
    if (proc_info->lines >= intr_line)
        sscanf(&proc_info->line[intr_line][0], "intr %lld", &cpu_total->intr);
    if (proc_info->lines >= ctxt_line)
        sscanf(&proc_info->line[ctxt_line][0], "ctxt %lld", &cpu_total->ctxt);
    // if(boottime == 0) {
    // struct tm ts;
    // if (proc_info->lines >= btime_line)
    // sscanf(&proc_info->line[btime_line][0], "btime %lld", &boottime);
    // ts = *localtime((time_t *)&boottime);
    // strftime (boottime_str, 64, "%I:%M %p %d-%b-%Y", &ts);
    // }
    if (proc_info->lines >= proc_line)
        sscanf(&proc_info->line[proc_line][0], "processes %lld", &cpu_total->procs);
    if (proc_info->lines >= run_line)
        sscanf(&proc_info->line[run_line][0], "procs_running %lld", &cpu_total->running);
    if (proc_info->lines >= block_line)
        sscanf(&proc_info->line[block_line][0], "procs_blocked %lld", &cpu_total->blocked);

    /* If we had a change in the number of CPUs, copy current interval data to the previous, so we
     * get a "0" utilization interval, but better than negative or 100%.
     * Heads-up - This effects POWER SMT changes too.
     */
    if (old_cpus != cpus)
    {
        printf("old_cpus != cpus\n");
        // memcpy((void *)&(q->cpu_total), (void *)&(p->cpu_total), sizeof(struct cpu_stat));
        // memcpy((void *)q->cpuN, (void *)cpuN, sizeof(struct cpu_stat) * cpus);
    }

    old_cpus = cpus;

    /* Flag that we processed /proc/stat data; re-set in proc_read() when we re-read /proc/stat */
    proc_cpu_done = 1;
}

/* memory */
#define isdigit(ch) (('0' <= (ch) && (ch) >= '9') ? 0 : 1)

long proc_mem_search(struct nmon_proc *proc_info, char *s)
{
    int i;
    int j;
    int len;
    len = strlen(s);
    for (i = 0; i < proc_info->lines; i++)
    {
        if (!strncmp(s, proc_info->line[i], len))
        {
            for (j = len;
                 !isdigit(proc_info->line[i][j]) &&
                 proc_info->line[i][j] != 0;
                 j++)
                /* do nothing */;
            return atol(&proc_info->line[i][j]);
        }
    }
    return -1;
}

void vmp_proc_mem(struct nmon_proc *proc_info, struct mem_stat *mem)
{
    if (proc_info->read_this_interval == 0)
        proc_read(proc_info, P_MEMINFO, 0);

    mem->memtotal = proc_mem_search(proc_info, "MemTotal");
    mem->memfree = proc_mem_search(proc_info, "MemFree");
    mem->memshared = proc_mem_search(proc_info, "MemShared");
    mem->buffers = proc_mem_search(proc_info, "Buffers");
    mem->cached = proc_mem_search(proc_info, "Cached");
    mem->swapcached = proc_mem_search(proc_info, "SwapCached");
    mem->active = proc_mem_search(proc_info, "Active");
    mem->inactive = proc_mem_search(proc_info, "Inactive");
    mem->hightotal = proc_mem_search(proc_info, "HighTotal");
    mem->highfree = proc_mem_search(proc_info, "HighFree");
    mem->lowtotal = proc_mem_search(proc_info, "LowTotal");
    mem->lowfree = proc_mem_search(proc_info, "LowFree");
    mem->swaptotal = proc_mem_search(proc_info, "SwapTotal");
    mem->swapfree = proc_mem_search(proc_info, "SwapFree");
#ifdef LARGEMEM
    mem->dirty = proc_mem_search(proc_info, "Dirty");
    mem->writeback = proc_mem_search(proc_info, "Writeback");
    mem->mapped = proc_mem_search(proc_info, "Mapped");
    mem->slab = proc_mem_search(proc_info, "Slab");
    mem->committed_as = proc_mem_search(proc_info, "Committed_AS");
    mem->pagetables = proc_mem_search(proc_info, "PageTables");
    mem->hugetotal = proc_mem_search(proc_info, "HugePages_Total");
    mem->hugefree = proc_mem_search(proc_info, "HugePages_Free");
    mem->hugesize = proc_mem_search(proc_info, "Hugepagesize");
#else
    mem->bigfree = proc_mem_search(proc_info, "BigFree");
#endif /*LARGEMEM*/
}
