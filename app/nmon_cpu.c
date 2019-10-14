/**
 * History:
 * ================================================================
 * 2019-08-07 qing.zou created
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MALLOC(argument)        malloc(argument)
#define FREE(argument)          free(argument)
#define REALLOC(argument1,argument2)    realloc(argument1,argument2)

#define CPUMAX (192 * 8)	/* MAGIC COOKIE WARNING */
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

struct data {
    // struct dsk_stat *dk;
    struct cpu_stat cpu_total;
    struct cpu_stat cpuN[CPUMAX];
    // struct mem_stat mem;
    struct timeval tv;
     double time;
} database[2], *p, *q;

#define P_CPUINFO	0
#define P_STAT		1
#define P_VERSION	2
#define P_MEMINFO   	3
#define P_UPTIME   	4
#define P_LOADAVG   	5
#define P_NFS   	6
#define P_NFSD   	7
#define P_VMSTAT	8	/* new in 13h */
#define P_NUMBER	9	/* one more than the max */


#define PROC_MAXLINES (16*1024)	/*MAGIC COOKIE WARNING */

int proc_cpu_done = 0;		/* Flag if we have run function proc_cpu() already in this interval */
/* Counts of resources */
int cpus = 1;			/* number of CPUs in system (lets hope its more than zero!) */
int old_cpus = 1;		/* Number of CPU seen in previuos interval */
int max_cpus = 1;		/* highest number of CPUs in DLPAR */

struct {
    FILE *fp;
    char *filename;
    int size;
    int lines;
    char *line[PROC_MAXLINES];
    char *buf;
    int read_this_interval;	/* track updates for each update to stop  double data collection */
} proc[P_NUMBER];

void proc_init()
{
    proc[P_CPUINFO].filename = "/proc/cpuinfo";
    proc[P_STAT].filename = "/proc/stat";
    proc[P_VERSION].filename = "/proc/version";
    proc[P_MEMINFO].filename = "/proc/meminfo";
    proc[P_UPTIME].filename = "/proc/uptime";
    proc[P_LOADAVG].filename = "/proc/loadavg";
    proc[P_NFS].filename = "/proc/net/rpc/nfs";
    proc[P_NFSD].filename = "/proc/net/rpc/nfsd";
    proc[P_VMSTAT].filename = "/proc/vmstat";
}

void proc_read(int num, int reread)
{
    int i;
    int size;
    int found;
    char buf[1024];

    if (proc[num].read_this_interval == 1)
	return;

    if (proc[num].fp == 0) {
	if ((proc[num].fp = fopen(proc[num].filename, "r")) == NULL) {
	    sprintf(buf, "failed to open file %s", proc[num].filename);
	    error(buf);
	    proc[num].fp = 0;
	    return;
	}
    }
    rewind(proc[num].fp);

    /* We re-read P_STAT, now flag proc_cpu() that it has to re-process that data */
    if (num == P_STAT)
	proc_cpu_done = 0;

    if (proc[num].size == 0) {
	/* first time so allocate  initial now */
	proc[num].buf = MALLOC(512);
	proc[num].size = 512;
    }

    for (i = 0; i < 4096; i++) {	/* MAGIC COOKIE WARNING  POWER8 default install can have 2655 processes */
	size = fread(proc[num].buf, 1, proc[num].size - 1, proc[num].fp);
	if (size < proc[num].size - 1)
	    break;
	proc[num].size += 512;
	proc[num].buf = REALLOC(proc[num].buf, proc[num].size);
	rewind(proc[num].fp);
    }

    proc[num].buf[size] = 0;
    proc[num].lines = 0;
    proc[num].line[0] = &proc[num].buf[0];
    if (num == P_VERSION) {
	found = 0;
	for (i = 0; i < size; i++) {	/* remove some weird stuff found the hard way in various Linux versions and device drivers */
	    /* place ") (" on two lines */
	    if (found == 0 &&
		proc[num].buf[i] == ')' &&
		proc[num].buf[i + 1] == ' ' &&
		proc[num].buf[i + 2] == '(') {
		proc[num].buf[i + 1] = '\n';
		found = 1;
	    } else {
		/* place ") #" on two lines */
		if (proc[num].buf[i] == ')' &&
		    proc[num].buf[i + 1] == ' ' &&
		    proc[num].buf[i + 2] == '#') {
		    proc[num].buf[i + 1] = '\n';
		}
		/* place "#1" on two lines */
		if (proc[num].buf[i] == '#' && proc[num].buf[i + 2] == '1') {
		    proc[num].buf[i] = '\n';
		}
	    }
	}
    }
    for (i = 0; i < size; i++) {
	/* replace Tab characters with space */
	if (proc[num].buf[i] == '\t') {
	    proc[num].buf[i] = ' ';
	} else if (proc[num].buf[i] == '\n') {
	    /* replace newline characters with null */
	    proc[num].lines++;
	    proc[num].buf[i] = '\0';
	    proc[num].line[proc[num].lines] = &proc[num].buf[i + 1];
	}
	if (proc[num].lines == PROC_MAXLINES - 1)
	    break;
    }
    if (reread) {
	fclose(proc[num].fp);
	proc[num].fp = 0;
    }
    /* Set flag so we do not re-read the data even if called multiple times in same interval */
    proc[num].read_this_interval = 1;
}

int stat8 = 0;
int stat10 = 0;
void proc_cpu()
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

    /* Only read data once per interval */
    if (proc_cpu_done == 1)
	return;

    /* If number of CPUs changed, then we need to find the index of intr_line, ... again */
    if (old_cpus != cpus)
	intr_line = 0;

    if (proc_cpu_first_time) {
	stat8 =
	    sscanf(&proc[P_STAT].line[0][5],
		   "%lld %lld %lld %lld %lld %lld %lld %lld", &user, &nice,
		   &sys, &idle, &iowait, &hardirq, &softirq, &steal);
	stat10 =
	    sscanf(&proc[P_STAT].line[0][5],
		   "%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
		   &user, &nice, &sys, &idle, &iowait, &hardirq, &softirq,
		   &steal, &guest, &guest_nice);
	proc_cpu_first_time = 0;
    }
    user = nice = sys = idle = iowait = hardirq = softirq = steal = guest =
	guest_nice = 0;
    if (stat10 == 10) {
	sscanf(&proc[P_STAT].line[0][5],
	       "%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld", &user,
	       &nice, &sys, &idle, &iowait, &hardirq, &softirq, &steal,
	       &guest, &guest_nice);
    } else {
	if (stat8 == 8) {
	    sscanf(&proc[P_STAT].line[0][5],
		   "%lld %lld %lld %lld %lld %lld %lld %lld", &user, &nice,
		   &sys, &idle, &iowait, &hardirq, &softirq, &steal);
	} else {		/* stat 4 variables here as older Linux proc */
	    sscanf(&proc[P_STAT].line[0][5], "%lld %lld %lld %lld",
		   &user, &nice, &sys, &idle);
	}
    }
    p->cpu_total.user = user;
    p->cpu_total.nice = nice;
    p->cpu_total.idle = idle;
    p->cpu_total.sys = sys;
    p->cpu_total.wait = iowait;	/* in the case of 4 variables = 0 */
    /* p->cpu_total.sys  = sys + hardirq + softirq + steal; */

    p->cpu_total.irq = hardirq;
    p->cpu_total.softirq = softirq;
    p->cpu_total.steal = steal;
    p->cpu_total.guest = guest;
    p->cpu_total.guest_nice = guest_nice;

#ifdef DEBUG
    if (debug)
	fprintf(stderr, "XX user=%lld wait=%lld sys=%lld idle=%lld\n",
		p->cpu_total.user,
		p->cpu_total.wait, p->cpu_total.sys, p->cpu_total.idle);
#endif /*DEBUG*/
	for (i = 0; i < cpus; i++) {
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

	if (stat10 == 10) {
	    sscanf(&proc[P_STAT].line[i + 1][row],
		   "%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
		   &user,
		   &nice,
		   &sys,
		   &idle,
		   &iowait,
		   &hardirq, &softirq, &steal, &guest, &guest_nice);

	} else if (stat8 == 8) {
	    sscanf(&proc[P_STAT].line[i + 1][row],
		   "%lld %lld %lld %lld %lld %lld %lld %lld",
		   &user,
		   &nice,
		   &sys, &idle, &iowait, &hardirq, &softirq, &steal);
	} else {
	    sscanf(&proc[P_STAT].line[i + 1][row], "%lld %lld %lld %lld",
		   &user, &nice, &sys, &idle);
	}
	p->cpuN[i].user = user;
	p->cpuN[i].nice = nice;
	p->cpuN[i].sys = sys;
	p->cpuN[i].idle = idle;
	p->cpuN[i].wait = iowait;
	p->cpuN[i].irq = hardirq;
	p->cpuN[i].softirq = softirq;
	p->cpuN[i].steal = steal;
	p->cpuN[i].guest = guest;
	p->cpuN[i].guest_nice = guest_nice;
    }

    if (intr_line == 0) {
	if (proc[P_STAT].line[i + 1][0] == 'p' &&
	    proc[P_STAT].line[i + 1][1] == 'a' &&
	    proc[P_STAT].line[i + 1][2] == 'g' &&
	    proc[P_STAT].line[i + 1][3] == 'e') {
	    /* 2.4 kernel */
	    intr_line = i + 3;
	    ctxt_line = i + 5;
	    btime_line = i + 6;
	    proc_line = i + 7;
	    run_line = i + 8;
	    block_line = i + 9;
	} else {
	    /* 2.6 kernel */
	    intr_line = i + 1;
	    ctxt_line = i + 2;
	    btime_line = i + 3;
	    proc_line = i + 4;
	    run_line = i + 5;
	    block_line = i + 6;
	}
    }
    p->cpu_total.intr = -1;
    p->cpu_total.ctxt = -1;
    p->cpu_total.procs = -1;
    p->cpu_total.running = -1;
    p->cpu_total.blocked = -1;
    if (proc[P_STAT].lines >= intr_line)
	sscanf(&proc[P_STAT].line[intr_line][0], "intr %lld",
	       &p->cpu_total.intr);
    if (proc[P_STAT].lines >= ctxt_line)
	sscanf(&proc[P_STAT].line[ctxt_line][0], "ctxt %lld",
	       &p->cpu_total.ctxt);
    // if(boottime == 0) {
	// struct tm ts;
	// if (proc[P_STAT].lines >= btime_line)
	// sscanf(&proc[P_STAT].line[btime_line][0], "btime %lld", &boottime);
	// ts = *localtime((time_t *)&boottime);
	// strftime (boottime_str, 64, "%I:%M %p %d-%b-%Y", &ts);
    // }
    if (proc[P_STAT].lines >= proc_line)
	sscanf(&proc[P_STAT].line[proc_line][0], "processes %lld",
	       &p->cpu_total.procs);
    if (proc[P_STAT].lines >= run_line)
	sscanf(&proc[P_STAT].line[run_line][0], "procs_running %lld",
	       &p->cpu_total.running);
    if (proc[P_STAT].lines >= block_line)
	sscanf(&proc[P_STAT].line[block_line][0], "procs_blocked %lld",
	       &p->cpu_total.blocked);

    /* If we had a change in the number of CPUs, copy current interval data to the previous, so we
     * get a "0" utilization interval, but better than negative or 100%.
     * Heads-up - This effects POWER SMT changes too.
     */
    if (old_cpus != cpus) {
        printf("old_cpus != cpus\n");
	memcpy((void *) &(q->cpu_total), (void *) &(p->cpu_total),
	       sizeof(struct cpu_stat));
	memcpy((void *) q->cpuN, (void *) p->cpuN,
	       sizeof(struct cpu_stat) * cpus);
    }

    /* Flag that we processed /proc/stat data; re-set in proc_read() when we re-read /proc/stat */
    proc_cpu_done = 1;
}

/* Convert secs + micro secs to a double */
double doubletime(void)
{

    gettimeofday(&p->tv, 0);
    return ((double) p->tv.tv_sec + p->tv.tv_usec * 1.0e-6);
}

#define RAW(member)      (long)((long)(p->cpuN[i].member)   - (long)(q->cpuN[i].member))
#define RAWTOTAL(member) (long)((long)(p->cpu_total.member) - (long)(q->cpu_total.member))
#define RAWTOTAL_PRINT(member) (long)((long)(p->cpu_total.member))

// gcc -g -O0 -o cpu nmon_cpu.c
int main(int argc, char **argv)
{
    int i = 0;
    double elapsed;		/* actual seconds between screen updates */
        
    int cpu_idle;
    int cpu_user;
    int cpu_sys;
    int cpu_wait;
    int cpu_steal;
    double cpu_sum;

    int stat8 = 0;
    int stat10 = 0;

    p = &database[0];
    q = &database[1];

    p->time = doubletime();
    q->time = doubletime();

    proc_init();
	    proc_read(P_STAT, 0);
	    proc_cpu();
        memcpy(&q->cpu_total, &p->cpu_total, sizeof(struct cpu_stat));

    // int networks = proc_net(0);
    // memcpy(q->ifnets, p->ifnets, sizeof(struct net_stat) * networks);
    // for (i = 0; i < networks; i++)
    // {
    //     printf("%8s  %8s   ", (char *)&p->ifnets[i].if_name[0], " ");
    // }
    // printf("\n");

    // for (i = 0; i < networks; i++)
    //     printf(" KB/s in  KB/s out   ");
    // printf("\n");

    while (1)
    {
        p->time = doubletime();
        elapsed = p->time - q->time;

        proc_read(P_STAT, 0);
	    proc_cpu();

        // printf("%ld  %ld  %ld  %ld  %ld\n", 
        //     RAWTOTAL_PRINT(user), RAWTOTAL_PRINT(sys), 
        //     RAWTOTAL_PRINT(wait), RAWTOTAL_PRINT(idle), RAWTOTAL_PRINT(nice));

        // // printf("%7.1f  %7.1f  %7.1f  %7.1f  %7.1f", 
        // //     RAWTOTAL(user) / elapsed, RAWTOTAL(sys) / elapsed, 
        // //     RAWTOTAL(wait) / elapsed, RAWTOTAL(idle) / elapsed, RAWTOTAL(nice) / elapsed);
        // printf("%7.1f  %7.1f  %7.1f  %7.1f  %7.1f", 
        //     RAWTOTAL(user), RAWTOTAL(sys), 
        //     RAWTOTAL(wait), RAWTOTAL(idle), RAWTOTAL(nice));
        for (i = 0; i < cpus; i++)
        {
            //printf("%8s  %8.1f  %8.1f\n", &p->ifnets[i].if_name[0], IFDELTA(if_ibytes) / 1024.0, IFDELTA(if_obytes) / 1024.0);
            //printf("%8.2f  %8.2f   ", IFDELTA(if_ibytes, elapsed) / 1024.0, IFDELTA(if_obytes, elapsed) / 1024.0);
        }
        printf("\n");

        cpu_user = RAWTOTAL(user) + RAWTOTAL(nice);
	    cpu_sys = RAWTOTAL(sys) + RAWTOTAL(irq) + RAWTOTAL(softirq) ;
		/* + RAWTOTAL(guest) + RAWTOTAL(guest_nice); these are in addition to the 100% */
	    cpu_wait = RAWTOTAL(wait);
	    cpu_idle = RAWTOTAL(idle);
	    cpu_steal = RAWTOTAL(steal);
	    /* DEBUG inject steal       cpu_steal = cpu_sys; */
	    cpu_sum = cpu_idle + cpu_user + cpu_sys + cpu_wait + cpu_steal;
        		/* Check if we had a CPU # change and have to set idle to 100 */
		// if (cpu_sum == 0)
		//     cpu_sum = cpu_idle = 100.0;

        printf("cpu_sum %7.1f\n", cpu_sum);

        printf("%7.1f  %7.1f  %7.1f  %7.1f   %7.1f\n",
              (double) cpu_user / (double) cpu_sum * 100.0,
		      (double) cpu_sys / (double) cpu_sum * 100.0,
		      (double) cpu_wait / (double) cpu_sum * 100.0,
		      (double) cpu_idle / (double) cpu_sum * 100.0,
		      (double) cpu_steal / (double) cpu_sum * 100.0 );

        //memcpy(q->ifnets, p->ifnets, sizeof(struct net_stat) * networks);
        memcpy(&q->cpu_total, &p->cpu_total, sizeof(struct cpu_stat));
        q->time = doubletime();

    /* Reset flags so /proc/... is re-read in next interval */
    for (i = 0; i < P_NUMBER; i++) {
	proc[i].read_this_interval = 0;
    }
        sleep(3);
    }

    return (0);
}
