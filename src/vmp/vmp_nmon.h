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

typedef void* (*proc_interfaces_stat)(void *ctx, void *data, int count);
int vmp_proc_net(int reread, proc_interfaces_stat proc, void *ctx);
int vmp_proc_net_stat(struct net_stat *ifstat, int reread);

#ifdef __cplusplus
}
#endif

#endif // VMP_NMON_H
