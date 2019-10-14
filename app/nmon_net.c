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

#define NETMAX 32
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

struct data {
    // struct dsk_stat *dk;
    // struct cpu_stat cpu_total;
    // struct cpu_stat cpuN[CPUMAX];
    // struct mem_stat mem;
    // struct vm_stat vm;
    // struct nfs_stat nfs;
    struct net_stat ifnets[NETMAX];
// #ifdef PARTITIONS
//     struct part_stat parts[PARTMAX];
// #endif /*PARTITIONS*/
    struct timeval tv;
     double time;
//     struct procsinfo *procs;

//     int proc_records;
//     int processes;
} database[2], *p, *q;

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
/* Convert secs + micro secs to a double */
double doubletime(void)
{

    gettimeofday(&p->tv, 0);
    return ((double) p->tv.tv_sec + p->tv.tv_usec * 1.0e-6);
}

//int reread = 0;
//int networks = 0;
int proc_net(int reread)
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
            error("failed to open - /proc/net/dev");
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
    for (i = 0; i < NETMAX; i++)
    {
	if (fgets(buf, 1024, fp) == NULL)
	    break;
	strip_spaces(buf);
	/* 1   2   3    4   5   6   7   8   9   10   11   12  13  14  15  16 */
	ret =
	    sscanf(&buf[0],
		   "%s %llu %llu %lu %lu %lu %lu %lu %lu %llu %llu %lu %lu %lu %lu %lu",
		   (char *) &p->ifnets[i].if_name, &p->ifnets[i].if_ibytes,
		   &p->ifnets[i].if_ipackets, &p->ifnets[i].if_ierrs,
		   &p->ifnets[i].if_idrop, &p->ifnets[i].if_ififo,
		   &p->ifnets[i].if_iframe, &junk, &junk,
		   &p->ifnets[i].if_obytes, &p->ifnets[i].if_opackets,
		   &p->ifnets[i].if_oerrs, &p->ifnets[i].if_odrop,
		   &p->ifnets[i].if_ofifo, &p->ifnets[i].if_ocolls,
		   &p->ifnets[i].if_ocarrier);
	if (ret != 16)
	    fprintf(stderr, "sscanf wanted 16 returned = %d line=%s\n",
		    ret, (char *) buf);
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

double net_read_peak[NETMAX];
double net_write_peak[NETMAX];

// gcc -g -O0 -o net nmon_net.c
int main(int argc, char **argv)
{
    int i = 0;
    double elapsed;		/* actual seconds between screen updates */

    p = &database[0];
    q = &database[1];

    p->time = doubletime();
    q->time = doubletime();

    int networks = proc_net(0);
    memcpy(q->ifnets, p->ifnets, sizeof(struct net_stat) * networks);
    for (i = 0; i < networks; i++)
    {
        net_read_peak[i] = 0.0;
        net_write_peak[i] = 0.0;

        printf("%8s  %8s   ", (char *)&p->ifnets[i].if_name[0], " ");
    }
    printf("\n");

    for (i = 0; i < networks; i++)
        printf(" KB/s in  KB/s out   ");
    printf("\n");

    while (1)
    {
        p->time = doubletime();
        elapsed = p->time - q->time;

        networks = proc_net(0);
        for (i = 0; i < networks; i++)
        {

#define IFDELTA(member, elapsed) ((float)((q->ifnets[i].member > p->ifnets[i].member) ? 0 : (p->ifnets[i].member - q->ifnets[i].member) / elapsed))
//#define IFDELTA_ZERO(member1, member2) ((IFDELTA(member1) == 0) || (IFDELTA(member2) == 0) ? 0.0 : IFDELTA(member1) / IFDELTA(member2))

            //printf("%8s  %8.1f  %8.1f\n", &p->ifnets[i].if_name[0], IFDELTA(if_ibytes) / 1024.0, IFDELTA(if_obytes) / 1024.0);
            printf("%8.2f  %8.2f   ", IFDELTA(if_ibytes, elapsed) / 1024.0, IFDELTA(if_obytes, elapsed) / 1024.0);
        }
        printf("\n");

        memcpy(q->ifnets, p->ifnets, sizeof(struct net_stat) * networks);
        q->time = doubletime();

        sleep(1);
    }

    return (0);
}
