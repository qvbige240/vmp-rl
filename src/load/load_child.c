/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "load_child.h"

typedef struct _PrivInfo
{
    void                *parent;

    LoadChildReq        req;
    LoadChildRsp        rsp;

    int                 cond;

    bool                initialized;

    pthread_t           thread;
    pthread_mutex_t     mutex;
} PrivInfo;


static void *load_child_thread(void *arg)
{
    pid_t pid = 0;
    PrivInfo *thiz = (PrivInfo *)arg;

    char** argv = global_default_argv();

    VMP_LOGI("start vmp-sa");
    pid = vmp_run_process(APP_VMP_SA, argv);
    VMP_LOGI("started vmp-sa, pid = %d", pid);

    while (thiz->cond)
    {
        //VMP_LOGD("pid = %d", pid);
        sleep(2);
    }

    VMP_LOGD("load_child_thread end");

    return NULL;
}

int load_child_start(void *p)
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

    ret = pthread_create(&thiz->thread, NULL, load_child_thread, (void *)thiz);
    if (ret != 0)
        VMP_LOGE("create thread 'load_child_thread' failed");

    return 0;

fail:
    load_child_done(thiz);
    return -1;
}

int load_child_stop(PrivInfo *thiz)
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

void *load_child_create(void *parent, LoadChildReq *req)
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

void load_child_destroy(PrivInfo *thiz)
{
    if (thiz)
    {
        pthread_mutex_destroy(&thiz->mutex);

        free(thiz);
    }
}

void load_child_done(void *p)
{
    PrivInfo *thiz = p;
    if (thiz != NULL)
    {
        load_child_stop(thiz);

        load_child_destroy(thiz);
    }
}
