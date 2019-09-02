/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "rpc_svc.h"
#include "service-vmon.h"

#include "load_vmon.h"
#include "load_child.h"
#include "ipcs/ipc_shared.h"

typedef struct _PrivInfo
{
    void                *parent;

    LoadVmonReq         req;
    LoadVmonRsp         rsp;

    int                 cond;

    bool                initialized;

    pthread_t           thread;
    pthread_mutex_t     mutex;
} PrivInfo;

static void load_vmon_test(PrivInfo *thiz)
{
    VMP_LOGD("load_vmon_test run test");
}

int sc_vmon_func(void *p, void *req, void *rsp)
{
    PrivInfo *thiz = p;
    ServiceVmonReq *request = req;
    ServiceVmonRsp *response = rsp;
    VMP_LOGD("req id = %d\n", request->id);

    vmon_connection_t c = load_child_connection_get(thiz->req.process);

    response->num = c.number;

    return 0;
}

static int load_vmon_register(PrivInfo *thiz)
{
    svc_handler_t *handler = calloc(1, sizeof(svc_handler_t));
    handler->ctx            = thiz;
    handler->pfn_callback   = sc_vmon_func;

    vmp_rpcsvc_t *r = thiz->req.svc;
    service_vmon_register(r->svc, handler);

    return 0;
}

static void *load_vmon_thread(void *arg)
{
    PrivInfo *thiz = (PrivInfo *)arg;

    load_vmon_test(thiz);

    load_vmon_register(thiz);

    while (thiz->cond)
    {
        sleep(5);
    }

    VMP_LOGD("load_vmon_thread end");

    return NULL;
}

int load_vmon_start(void *p)
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

    ret = pthread_create(&thiz->thread, NULL, load_vmon_thread, (void *)thiz);
    if (ret != 0)
        VMP_LOGE("create thread 'load_vmon_thread' failed");

    return 0;

fail:
    load_vmon_done(thiz);
    return -1;
}

int load_vmon_stop(PrivInfo *thiz)
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

void *load_vmon_create(void *parent, LoadVmonReq *req)
{
    PrivInfo *priv = calloc(1, sizeof(PrivInfo));
    if (priv)
    {
        priv->cond          = 1;
        priv->initialized   = true;

        priv->req           = *req;
        priv->parent        = parent;
    }

    return priv;
}

void load_vmon_destroy(PrivInfo *thiz)
{
    if (thiz)
    {
        pthread_mutex_destroy(&thiz->mutex);

        free(thiz);
    }
}

void load_vmon_done(void *p)
{
    PrivInfo *thiz = p;
    if (thiz != NULL)
    {
        load_vmon_stop(thiz);

        load_vmon_destroy(thiz);
    }
}
