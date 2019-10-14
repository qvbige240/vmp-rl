/**
 * History:
 * ================================================================
 * 2019-10-11 qing.zou created
 *
 */

#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

#include "load_system.h"


typedef struct system_data {
    // struct dsk_stat *dk;
    // struct cpu_stat cpu_total;
    // struct cpu_stat cpuN[CPUMAX];
    // struct mem_stat mem;
    // struct vm_stat vm;
    // struct nfs_stat nfs;
    struct net_stat ifnets[VMP_NETMAX];
    // struct timeval tv;
    // double time;
    // struct procsinfo *procs;

    // int proc_records;
    // int processes;
} system_data_t;

typedef struct _PrivInfo
{
    LoadSystemReq           req;
    LoadSystemRsp           rsp;

    int                     interval;

    volatile double         uplink;
    volatile double         downlink;

    int                     cond;

    bool                    initialized;

    pthread_t               thread;
    pthread_mutex_t         mutex;
} PrivInfo;

int load_system_interval_set(void *p, int interval)
{
    PrivInfo *thiz = p;
    if (!thiz) return -1;

    thiz->interval = interval;
    return 0;
}

double ls_uplink_get(void *p)
{
    PrivInfo *thiz = p;
    if (!thiz) return 0.0;

    return thiz->uplink;
}
double ls_downlink_get(void *p)
{
    PrivInfo *thiz = p;
    if (!thiz) return 0.0;

    return thiz->downlink;
}

static void load_system_test(PrivInfo *thiz)
{
    VMP_LOGD("load_system_test run test");
}

double current_time(void)
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return ((double) tv.tv_sec + tv.tv_usec * 1.0e-6);
}

static void* system_interface_stat(void *ctx, void *data, int count)
{
    system_data_t *p = ctx;
    struct net_stat *ifnets = data;

    memcpy(p->ifnets, ifnets, sizeof(struct net_stat) * count);

    return NULL;
}

#if 1
#define IFSTAT_PRINT    printf
#else
#define IFSTAT_PRINT
#endif

static void system_net_proc(PrivInfo *thiz)
{
    int i = 0;
    double elapsed; /* actual seconds between screen updates */

    system_data_t sys_data[2];
    system_data_t *p = &sys_data[0];
    system_data_t *q = &sys_data[1];

    double curr_time = current_time();
    double prev_time = curr_time;

    int networks = vmp_proc_net(0, system_interface_stat, p);
    memcpy(q->ifnets, p->ifnets, sizeof(struct net_stat) * networks);
    for (i = 0; i < networks; i++)
    {
        IFSTAT_PRINT("%8s  %8s   ", (char *)&p->ifnets[i].if_name[0], " ");
    }
    IFSTAT_PRINT("\n");

    for (i = 0; i < networks; i++)
        IFSTAT_PRINT(" KB/s in  KB/s out   ");
    IFSTAT_PRINT("\n");

#define IFDELTA(member, elapsed) ((float)((q->ifnets[i].member > p->ifnets[i].member) ? 0 : (p->ifnets[i].member - q->ifnets[i].member) / elapsed))
#define IFDELTA0(ifnets1, ifnets2, member, elapsed) ((float)((ifnets1.member > ifnets2.member) ? 0 : (ifnets2.member - ifnets1.member) / elapsed))

    thiz->interval = 1;
    //sleep(1);

    while (1)
    {
        curr_time = current_time();
        elapsed = curr_time - prev_time;

        networks = vmp_proc_net(0, system_interface_stat, p);
        //networks = vmp_proc_net_stat(p->ifnets, 0);
        for (i = 0; i < networks; i++)
        {
            IFSTAT_PRINT("%8.2f  %8.2f   ", IFDELTA(if_ibytes, elapsed) / 1024.0, IFDELTA(if_obytes, elapsed) / 1024.0);
            // printf("elapsed: %.2f  ", elapsed);
            // printf("p->ifnets[%d].if_ibytes: %.2f  ", i, p->ifnets[i].if_ibytes);
            // printf("q->ifnets[%d].if_ibytes: %.2f  ", i, q->ifnets[i].if_ibytes);
            //printf("%8.2f", IFDELTA(if_ibytes, elapsed));
            thiz->uplink = IFDELTA(if_ibytes, elapsed) / 1024.0;
            thiz->downlink = IFDELTA(if_obytes, elapsed) / 1024.0;

        }
        IFSTAT_PRINT("\n");

        memcpy(q->ifnets, p->ifnets, sizeof(struct net_stat) * networks);
        prev_time = curr_time;

        sleep(thiz->interval);
    }
}

static void *load_system_thread(void *arg)
{
    PrivInfo *thiz = (PrivInfo *)arg;

    load_system_test(thiz);

    system_net_proc(thiz);

    while (thiz->cond)
    {
        sleep(5);
    }

    pthread_exit(0);

    return NULL;
}

int load_system_start(void *p)
{
    int ret = -1;
    pthread_mutexattr_t attr;
    PrivInfo *thiz = p;

    if (!thiz) return -1;

    if (pthread_mutexattr_init(&attr) != 0)
        goto fail;
    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
        goto fail;
    if (pthread_mutex_init(&thiz->mutex, &attr) != 0)
        goto fail;

    ret = pthread_create(&thiz->thread, NULL, load_system_thread, (void *)thiz);
    if (ret != 0)
        VMP_LOGE("create thread 'load_system_thread' failed");

    pthread_detach(thiz->thread);

fail:
    //load_system_done(thiz);
    return ret;
}

int load_system_stop(PrivInfo *thiz)
{
    //void *thread_result;

    if (!thiz) return -1;

    thiz->cond = 0;

    if (thiz->initialized)
    {
        thiz->initialized = false;
        //pthread_join(thiz->thread, &thread_result);
    }

    return 0;
}

void *load_system_create(void *parent, LoadSystemReq *req)
{
    PrivInfo *priv = calloc(1, sizeof(PrivInfo));
    if (priv)
    {
        priv->cond          = 1;
        priv->initialized   = true;

        priv->interval      = 1;
        //priv->req           = *req;
        //priv->parent        = parent;
    }

    return priv;
}

void load_system_destroy(void *p)
{
    PrivInfo *thiz = p;
    if (thiz != NULL)
    {
        load_system_stop(thiz);

        pthread_mutex_destroy(&thiz->mutex);

        free(thiz);
    }
}
