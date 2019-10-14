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
