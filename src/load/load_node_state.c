/**
 * History:
 * ================================================================
 * 2019-10-08 qing.zou created
 *
 */

#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "rpc_svc.h"
#include "service-node-state.h"

#include "load_node_state.h"
#include "load_child.h"
#include "ipcs/ipc_shared.h"
#include "load_system.h"

typedef struct _PrivInfo
{
    void                *parent;

    LoadNodeStateReq    req;
    LoadNodeStateRsp    rsp;

    void                *system;

    int                 cond;

    bool                initialized;

    pthread_t           thread;
    pthread_mutex_t     mutex;
} PrivInfo;

static void load_node_state_test(PrivInfo *thiz)
{
    VMP_LOGD("load_node_state_test run test");
}

static int sc_node_state_func(void *p, void *req, void *rsp)
{
    PrivInfo *thiz = p;
    ServiceNodeStateReq *request = req;
    ServiceNodeStateRsp *response = rsp;
    //VMP_LOGD("req id = %d\n", request->id);

    vmon_connection_t c = load_child_connection_get(thiz->req.process);

    response->index = 0;
    response->name = "server local";
    response->count = c.number;
    response->uplink = ls_uplink_get(thiz->system);
    response->downlink = ls_downlink_get(thiz->system);;
    response->memory = 1024;
    response->cpu = 50.15;

    return 0;
}

static int load_node_state_register(PrivInfo *thiz)
{
    svc_handler_t *handler = calloc(1, sizeof(svc_handler_t));
    handler->ctx            = thiz;
    handler->pfn_callback   = sc_node_state_func;

    vmp_rpcsvc_t *r = thiz->req.svc;
    service_node_state_register(r->svc, handler);

    return 0;
}

static void *load_node_state_thread(void *arg)
{
    PrivInfo *thiz = (PrivInfo *)arg;

    load_node_state_test(thiz);

    load_node_state_register(thiz);

    while (thiz->cond)
    {
        sleep(5);
    }

    VMP_LOGD("load_node_state_thread end");

    return NULL;
}

int load_node_state_start(void *p)
{
    int ret = 0;
    pthread_mutexattr_t attr;
    PrivInfo *thiz = p;

    if (!thiz) return -1;

    if (pthread_mutexattr_init(&attr) != 0)
        goto fail;
    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
        goto fail;
    if (pthread_mutex_init(&thiz->mutex, &attr) != 0)
        goto fail;

    ret = pthread_create(&thiz->thread, NULL, load_node_state_thread, (void *)thiz);
    if (ret != 0)
        VMP_LOGE("create thread 'load_node_state_thread' failed");

    return 0;

fail:
    load_node_state_done(thiz);
    return -1;
}

int load_node_state_stop(PrivInfo *thiz)
{
    void *thread_result;

    if (!thiz) return -1;

    thiz->cond = 0;

    if (thiz->initialized)
    {
        thiz->initialized = false;
        pthread_join(thiz->thread, &thread_result);
    }

    return 0;
}

void *load_node_state_create(void *parent, LoadNodeStateReq *req)
{
    PrivInfo *priv = calloc(1, sizeof(PrivInfo));
    if (priv)
    {
        priv->cond          = 1;
        priv->initialized   = true;

        priv->req           = *req;
        priv->parent        = parent;

        priv->system        = load_system_create(NULL, NULL);
        if (priv->system)
        load_system_start(priv->system);
    }

    return priv;
}

void load_node_state_destroy(PrivInfo *thiz)
{
    if (thiz)
    {
        pthread_mutex_destroy(&thiz->mutex);

        free(thiz);
    }
}

void load_node_state_done(void *p)
{
    PrivInfo *thiz = p;
    if (thiz != NULL)
    {
        load_node_state_stop(thiz);

        load_node_state_destroy(thiz);
    }
}
