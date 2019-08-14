/**
 * History:
 * ================================================================
 * 2019-08-07 qing.zou created
 *
 */
//#include "calc.pb-c.h"
#include "pbc/pbrpc-clnt.h"

#include <inttypes.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


#define DBUG(x) fprintf (stdout, "%s was called\n", # x)

void* mainloop(void *arg)
{
    pbrpc_clnt *clnt = arg;

    pbrpc_clnt_mainloop(clnt);

    return NULL;
}

#if 0
int calc_cbk(pbrpc_clnt *clnt, ProtobufCBinaryData *msg, int ret)
{
        if (ret)
                return ret;

        Calc__CalcRsp *crsp = calc__calc_rsp__unpack (NULL, msg->len,
                                                      msg->data);

        printf("rsp->sum = %d\n", crsp->ret);
        calc__calc_rsp__free_unpacked (crsp, NULL);
}

/** calculate **/
static ProtobufCBinaryData
build_call_args(Calc__CalcReq *calc, int op, int a, int b)
{
        size_t clen;
        char *cbuf;
        ProtobufCBinaryData ret;

        //calc->op = 1; calc->a = 2; calc->b = 3;
        calc->op = 1; calc->a = a; calc->b = b;
        clen = calc__calc_req__get_packed_size(calc);
        cbuf = calloc (1, clen);
        calc__calc_req__pack(calc, cbuf);
        ret.data = cbuf; ret.len = clen;
        return ret;
}

int calculate_call(pbrpc_clnt *clnt)
{
    int ret;

    Calc__CalcReq calc = CALC__CALC_REQ__INIT;
    int i = 1;
    do {
        fprintf(stdout, "call no %d\n", i);
        ProtobufCBinaryData msg = build_call_args(&calc, 1, i, i + 1);
        ret = pbrpc_clnt_call(clnt, "Calculator.Calculate", &msg, calc_cbk);
        if (ret) {
            fprintf(stderr, "RPC call failed\n");
        }
        i++;

    } while (i <= 10);
    //event_base_dispatch (bufferevent_get_base (clnt->bev));

    return 0;
}
#endif

#if 0
/** workload **/
#include "service/workload.pb-c.h"

int workload_cb(pbrpc_clnt *clnt, ProtobufCBinaryData *msg, int ret)
{
    if (ret)
        return ret;

    PbcWorkloadRsp *rsp = pbc_workload_rsp__unpack(NULL, msg->len, msg->data);

    printf("rsp->id = %d\n", rsp->id);
    printf("rsp->video_cnt = %d\n", rsp->video_cnt);
    printf("rsp->info_mem = %lu\n", rsp->info_mem);

    pbc_workload_rsp__free_unpacked(rsp, NULL);
}

static ProtobufCBinaryData build_workload_args(PbcWorkloadReq *req, int id)
{
    size_t clen;
    unsigned char *cbuf;
    ProtobufCBinaryData pbc_req;

    req->id = id;
    clen = pbc_workload_req__get_packed_size(req);
    cbuf = calloc(1, clen);
    pbc_workload_req__pack(req, cbuf);
    pbc_req.data = cbuf;
    pbc_req.len = clen;

    return pbc_req;
}

int workload_call(pbrpc_clnt *clnt)
{
    int ret;
    PbcWorkloadReq req = PBC_WORKLOAD_REQ__INIT;

    ProtobufCBinaryData msg = build_workload_args(&req, 2);
    ret = pbrpc_clnt_call(clnt, "Loader.workload", &msg, workload_cb);
    if (ret) {
        fprintf(stderr, "RPC call failed\n");
    }
}
#endif


#include "pbrpc_client.h"

extern int workload_call(pbrpc_clnt *clnt);

static void* test_clnt_call(void *args)
{
    pbrpc_clnt *clnt = args;

    //sleep(1);

    //calculate_call(clnt);

    //sleep(1);

    workload_call(clnt);

    return NULL;
}

static void test_call_thread(void *args)
{
    int ret = 0;
    pthread_t pth_call;
    ret = pthread_create(&pth_call, NULL, test_clnt_call, args);
    if (ret != 0)
        printf("create thread 'test_clnt_call' failed\n");

    pthread_detach(pth_call);
}

int client(void)
{
    //int ret;
    pbrpc_clnt *clnt = NULL;

    clnt = pbrpc_clnt_new("localhost", 9876);
    if (!clnt) {
        fprintf(stderr, "Failed to create an pbrpc_clnt object\n");
        return 1;
    }
    
    //evthread_use_pthreads();

    test_call_thread(clnt);

    event_base_dispatch(bufferevent_get_base(clnt->bev));

    return 0;
}
