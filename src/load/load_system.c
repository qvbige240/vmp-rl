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

#define IFDELTA(member, elapsed) ((float)((q->ifnets[i].member > p->ifnets[i].member) ? 0 : (p->ifnets[i].member - q->ifnets[i].member) / elapsed))
#define IFDELTA0(ifnets1, ifnets2, member, elapsed) ((float)((ifnets1.member > ifnets2.member) ? 0 : (ifnets2.member - ifnets1.member) / elapsed))

#define RAW(member)      (long)((long)(p->cpuN[i].member)   - (long)(q->cpuN[i].member))
#define RAWTOTAL(member) (long)((long)(p->cpu_total.member) - (long)(q->cpu_total.member))

typedef struct system_data {
    // struct dsk_stat *dk;
    struct cpu_stat cpu_total;
    struct cpu_stat cpuN[VMP_CPUMAX];
    // struct mem_stat mem;
    // struct vm_stat vm;
    // struct nfs_stat nfs;
    struct net_stat ifnets[VMP_NETMAX];
    // struct timeval tv;
    double time;
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

    volatile double         cpu_idle;

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
double ls_cpu_get(void *p)
{
    PrivInfo *thiz = p;
    if (!thiz) return 0.0;

    return 100.0 - thiz->cpu_idle;
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
#define IFSTAT_DEGUG
#endif

#define data_switcher()                     \
    do                                      \
    {                                       \
        static int which = 1;               \
        if (which)                          \
        {                                   \
            p = &sys_data[0];               \
            q = &sys_data[1];               \
            which = 0;                      \
        }                                   \
        else                                \
        {                                   \
            q = &sys_data[0];               \
            p = &sys_data[1];               \
            which = 1;                      \
        }                                   \
        int i;                              \
        for (i = 0; i < P_NUMBER; i++)      \
        {                                   \
            proc[i].read_this_interval = 0; \
        }                                   \
    } while (0)

static void system_net_proc(PrivInfo *thiz)
{
    int i = 0;
    double elapsed; /* actual seconds between screen updates */

    int cpu_idle;
    int cpu_user;
    int cpu_sys;
    int cpu_wait;
    int cpu_steal;
    double cpu_sum;

    system_data_t sys_data[2];
    system_data_t *p = &sys_data[0];
    system_data_t *q = &sys_data[1];

    // double curr_time = current_time();
    // double prev_time = curr_time;
    p->time = current_time();
    q->time = p->time;

    /* network */
    int networks = vmp_proc_net(0, system_interface_stat, p);
    memcpy(q->ifnets, p->ifnets, sizeof(struct net_stat) * networks);

#ifdef IFSTAT_DEGUG
    for (i = 0; i < networks; i++)
    {
        IFSTAT_PRINT("%8s  %8s   ", (char *)&p->ifnets[i].if_name[0], " ");
    }
    IFSTAT_PRINT("\n");

    for (i = 0; i < networks; i++)
        IFSTAT_PRINT(" KB/s in  KB/s out   ");
    IFSTAT_PRINT("\n");
#endif //IFSTAT_DEGUG

    /* cpu utilization rate */
    struct nmon_proc proc[P_NUMBER];
    struct nmon_proc *info_cpu = &proc[P_STAT];
    proc_init(proc);
    int cpus = get_cpu_cnt(info_cpu);
    proc_cpu(info_cpu, cpus, &p->cpu_total, (struct cpu_stat *)&p->cpuN);
    memcpy((void *)&(q->cpu_total), (void *)&(p->cpu_total), sizeof(struct cpu_stat));
    memcpy((void *)q->cpuN, (void *)p->cpuN, sizeof(struct cpu_stat) * cpus);

    thiz->interval = 1;
    sleep(1);

    while (1)
    {
        // curr_time = current_time();
        // elapsed = curr_time - prev_time;
        data_switcher();

        p->time = current_time();
        elapsed = p->time - q->time;

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

        /* cpu */
        cpus = get_cpu_cnt(info_cpu);
        proc_cpu(info_cpu, cpus, &p->cpu_total, (struct cpu_stat *)&p->cpuN);

        cpu_user = RAWTOTAL(user) + RAWTOTAL(nice);
        cpu_sys = RAWTOTAL(sys) + RAWTOTAL(irq) + RAWTOTAL(softirq);
        /* + RAWTOTAL(guest) + RAWTOTAL(guest_nice); these are in addition to the 100% */
        cpu_wait = RAWTOTAL(wait);
        cpu_idle = RAWTOTAL(idle);
        cpu_steal = RAWTOTAL(steal);
        /* DEBUG inject steal       cpu_steal = cpu_sys; */
        cpu_sum = cpu_idle + cpu_user + cpu_sys + cpu_wait + cpu_steal;

        //IFSTAT_PRINT("cpu_sum %7.1f\n", cpu_sum);

        IFSTAT_PRINT("%7.1f  %7.1f  %7.1f  %7.1f   %7.1f\n",
                     (double)cpu_user / (double)cpu_sum * 100.0,
                     (double)cpu_sys / (double)cpu_sum * 100.0,
                     (double)cpu_wait / (double)cpu_sum * 100.0,
                     (double)cpu_idle / (double)cpu_sum * 100.0,
                     (double)cpu_steal / (double)cpu_sum * 100.0);

        thiz->cpu_idle = (double)cpu_idle / (double)cpu_sum * 100.0;

        // memcpy(q->ifnets, p->ifnets, sizeof(struct net_stat) * networks);
        // prev_time = curr_time;

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
