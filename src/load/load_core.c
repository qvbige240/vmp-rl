/**
 * History:
 * ================================================================
 * 2019-08-15 qing.zou created
 *
 */

#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#include "load.h"
#include "load_core.h"
#include "rpc_svc.h"

typedef struct _PrivInfo
{
    int                     cond;

	pthread_t               core_thread;
    pthread_mutex_t         core_mutex;
	// os_event_t                 *stop_event;

	bool                    initialized;

	// audio_input_callback_t     input_cb;
	// void                       *input_param;
	// pthread_mutex_t            input_mutex;

    vmp_rpcsvc_t            *svc;
    void                    *process;
    void                    *vmon;

} PrivInfo;


#if 0
#include "pbrpc_server.h"
#include "pbrpc_client.h"

static void load_core_test(PrivInfo* thiz)
{
    vmp_config_t* config = global_default_config();
    if (config->client) {
        VMP_LOGD("start client...");
        client();
    } else {
        VMP_LOGD("start server...");
        server(1, NULL);
    }
}
#else

#include "rpc_clnt.h"

#include "clnt_vmon.h"
static int clnt_vmon_callback(void *p, int msg, void *arg)
{
    if (msg != 0) {
        VMP_LOGE("clnt_vmon_callback failed");
        return -1;
    }
    ClntVmonRsp *rsp = arg;
    VMP_LOGD("vmon call response:");
    VMP_LOGD(" id: %d", rsp->id);
    VMP_LOGD(" count: %d", rsp->count);
    return 0;
}
static int clnt_vmon_call(vmp_rpclnt_t *thiz)
{
    if (thiz && thiz->clnt)
    {
        ClntVmonReq req = {0};
        req.id            = 1;

        req.ctx           = thiz;
        req.pfn_callback  = clnt_vmon_callback;
        return rpc_clnt_vmon(thiz, &req);
    }
    return 0;
}

#include "rpc_server_info.h"
static int registry_callback(void *p, int msg, void *arg)
{
    if (msg != 0) {
        VMP_LOGE("registry_callback failed");
        return -1;
    }
    RpcServerInfoRsp *rsp = arg;
    VMP_LOGD("registry call response:");
    VMP_LOGD(" id: %d", rsp->id);
    VMP_LOGD(" name: %s", rsp->name);
    VMP_LOGD(" ip: %s", rsp->ip);
    VMP_LOGD(" num: %d", rsp->num);
    return 0;
}
int load_registry_call(vmp_rpclnt_t *thiz)
{
    if (thiz && thiz->clnt)
    {
        RpcServerInfoReq req = {0};
        req.id            = 1;
        req.name          = "server1";
        req.system        = "ubuntu";
        req.location      = "chengdu";
        req.bandwidth     = 1000 * 1024 * 1024;
        req.port          = 9876;
        strcpy(req.ip, "localhost");

        req.ctx           = thiz;
        req.pfn_callback  = registry_callback;
        return rpc_call_registry(thiz, &req);
    }
    return 0;
}

static void load_core_test(PrivInfo* thiz)
{
    vmp_config_t* config = global_default_config();
    if (config->client) {
        VMP_LOGD("start rpc client...");
        vmp_rpclnt_t* clnt = vmp_rpclnt_create();
        RpclntReq req = {0};
        req.port = 9876;
        strcpy(req.ip, "localhost");
        vmp_rpclnt_set(clnt, &req);
        vmp_rpclnt_start(clnt);
        //vmp_rpclnt_destroy(clnt);

        sleep(1);
        //rpc_workload_call(clnt);

        load_registry_call(clnt);

        clnt_vmon_call(clnt);
    } else {

        VMP_LOGD("start rpc server...");
        vmp_rpcsvc_t* svc = vmp_rpcsvc_create();
        thiz->svc = svc;
        RpcsvcReq req = {0};
        req.port = 9876;
        vmp_rpcsvc_set(svc, &req);
        vmp_rpcsvc_start(svc);
        //vmp_rpcsvc_destroy(svc);
    }
}
#endif

/** task vmon **/
static int vmon_callback(void* p, int msg, void* arg)
{
    if (msg != 0) {
        VMP_LOGE("vmon_callback failed");
        return -1;
    }
    return 0;
}
static void task_vmon_start(PrivInfo* thiz)
{
    LoadVmonReq req = {0};
    req.id              = 1;
    req.svc             = thiz->svc;
    req.process         = thiz->process;
    req.ctx             = thiz;
    req.pfn_callback    = vmon_callback;
    thiz->vmon = load_vmon_create(thiz, &req);
    if (thiz->vmon) {
        load_vmon_start(thiz->vmon);
    }
}

/** task process **/
static int process_callback(void* p, int msg, void* arg)
{
    if (msg != 0) {
        VMP_LOGE("process_callback failed");
        return -1;
    }
    return 0;
}
static void task_process_start(PrivInfo* thiz)
{
    LoadChildReq req = {0};
    req.ctx             = thiz;
    req.pfn_callback    = process_callback;
    thiz->process = load_child_create(thiz, &req);
    if (thiz->process) {
        load_child_start(thiz->process);
    }
}

/** rpc server **/
static void rpc_server_start(PrivInfo* thiz)
{
    VMP_LOGD("start rpc server...");
    thiz->svc = vmp_rpcsvc_create();
    if (thiz->svc)
    {
        RpcsvcReq req = {0};
        req.port = 9876;
        vmp_rpcsvc_set(thiz->svc, &req);
        vmp_rpcsvc_start(thiz->svc);
    }
}

static void* load_core_thread(void* arg)
{
    PrivInfo* thiz = (PrivInfo*)arg;

    vmp_config_t* config = global_default_config();

    if (config->client) {  // test
        load_core_test(thiz);
    } else

{
    rpc_server_start(thiz);

    task_process_start(thiz);

    sleep(1);

    if (thiz->svc)
    {
        task_vmon_start(thiz);
    }
}

    while (thiz->cond)
    {
        sleep(5);
    }
    
    load_vmon_done(thiz->vmon);
    thiz->vmon = NULL;
    load_child_done(thiz->process);
    thiz->process = NULL;

    return NULL;
}

int load_core_start(PrivInfo *thiz)
{
    int ret = 0;
    pthread_mutexattr_t attr;

    if (!thiz)
        return -1;
    
    if (pthread_mutexattr_init(&attr) != 0)
        goto fail;
    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
        goto fail;
    if (pthread_mutex_init(&thiz->core_mutex, &attr) != 0)
        goto fail;

    ret = pthread_create(&thiz->core_thread, NULL, load_core_thread, (void*)thiz);
    if (ret != 0)
        VMP_LOGE("create thread 'load_core_thread' failed");
    
    return 0;

fail:
    load_core_done();
    return -1;
}

int load_core_stop(PrivInfo *thiz)
{
    void *thread_result;

    if (!thiz)
        return -1;

    if (thiz->initialized) {
        thiz->initialized = false;
        pthread_join(thiz->core_thread, &thread_result);
    }

    return 0;
}

void load_core_init(void)
{
    rt_load_t *load = NULL;
    PrivInfo *thiz = calloc(1, sizeof(PrivInfo));

    thiz->cond  = 1;
    thiz->initialized = true;

    load = global_default_load();
    load->core = thiz;

    load_core_start(thiz);
}

void load_core_done(void)
{
    PrivInfo *thiz;
    rt_load_t *load = global_default_load();
    
    thiz = load->core;
    if (thiz != NULL)
    {
        load_core_stop(thiz);

        pthread_mutex_destroy(&thiz->core_mutex);

        free(thiz);
        load->core = NULL;
    }
}

/** business: load **/
void load_init(int argc, char **argv)
{
    rt_load_t *load = NULL;
    void *config = vmp_config_create();
    global_set_config(config);
    vmp_config_init(config, argc, argv);
    
    load = calloc(1, sizeof(rt_load_t));
    global_set_load(load);

    mod_log_init(PROCESS_DAEMON, NULL);

    global_set_argv((void*)argv);
    //load_child_start(argc, argv);

    load_core_init();
}

void load_start(void)
{   
    while (1)
    {
        sleep(10);
    }    
}

void load_done(void)
{
    rt_load_t *load;

    load_core_done();

    load = global_default_load();
    if (load) {
        free(load);
        global_set_load(NULL);
    }
}
