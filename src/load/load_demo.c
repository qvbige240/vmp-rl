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
#include "service-demo.h"

#include "load_demo.h"

typedef struct _PrivInfo
{
    void                *parent;
    
    LoadDemoReq         req;
    LoadDemoRsp         rsp;

    int                 cond;

    bool                initialized;

    pthread_t           thread;
    pthread_mutex_t     mutex;
} PrivInfo;

static void load_demo_test(PrivInfo *thiz)
{
    VMP_LOGD("load_demo_test run test");
}

int sc_demo_func(void *p, void *req, void *rsp)
{
    PrivInfo *thiz = p;
    ServiceDemoReq *request = req;
    ServiceDemoRsp *response = rsp;
    VMP_LOGD("req id = %d\n", request->id);

    response->cnt = 2;
    response->mem = (2lu << 30);

    return 0;
}

static int load_demo_register(PrivInfo *thiz)
{
    svc_handler_t *handler = calloc(1, sizeof(svc_handler_t));
    handler->ctx            = thiz;
    handler->pfn_callback   = sc_demo_func;

    vmp_rpcsvc_t *r = thiz->req.svc;
    service_demo_register(r->svc, handler);

    return 0;
}

static void *load_demo_thread(void *arg)
{
    PrivInfo *thiz = (PrivInfo *)arg;

    //load_demo_test(thiz);
    load_demo_register(thiz);

    while (thiz->cond)
    {
        sleep(5);
    }

    VMP_LOGD("load_demo_thread end");
    return NULL;
}

int load_demo_start(void *p)
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

    ret = pthread_create(&thiz->thread, NULL, load_demo_thread, (void *)thiz);
    if (ret != 0)
        VMP_LOGE("create thread 'load_demo_thread' failed");

    return 0;

fail:
    load_demo_done(thiz);
    return -1;
}

int load_demo_stop(PrivInfo *thiz)
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

void *load_demo_create(void *parent, LoadDemoReq *req)
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

void load_demo_destroy(PrivInfo *thiz)
{
    if (thiz)
    {
        pthread_mutex_destroy(&thiz->mutex);

        free(thiz);
    }
}

void load_demo_done(void *p)
{
    PrivInfo *thiz = p;
    if (thiz != NULL)
    {
        load_demo_stop(thiz);

        load_demo_destroy(thiz);
    }
}
