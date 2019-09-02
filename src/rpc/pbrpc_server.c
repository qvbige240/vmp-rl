/**
 * History:
 * ================================================================
 * 2019-08-07 qing.zou created
 *
 */

#include "pbc/pbrpc-svc.h"
//#include "calc.pb-c.h"
//#include "workload.pb-c.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define DBUG(fp, x) do {                        \
    fprintf(fp, "%s was called\n", # x);    \
    fflush(fp);                             \
} while (0)

#if 0
static int calculate (ProtobufCBinaryData *req, ProtobufCBinaryData *reply)
{
    Calc__CalcReq *creq;
    Calc__CalcRsp crsp = CALC__CALC_RSP__INIT;
    size_t rsplen;
    int ret = 0;

    creq = calc__calc_req__unpack (NULL, req->len, req->data);

    /* method-specific logic */
    switch(creq->op) {
    default:
        crsp.ret = creq->a + creq->b + 1;
    }

    rsplen = calc__calc_rsp__get_packed_size(&crsp);
    reply->data = calloc (1, rsplen);
    if (!reply->data) {
        ret = -1;
        goto out;
    }

    reply->len = rsplen;
    calc__calc_rsp__pack(&crsp, reply->data);
out:
    calc__calc_req__free_unpacked (creq, NULL);

    return 0;
}
#endif

#if 0
static int workload_exec (ProtobufCBinaryData *req, ProtobufCBinaryData *reply)
{
    int ret = 0;
    size_t rsp_len;
    PbcWorkloadReq *pbc_req;
    PbcWorkloadRsp pbc_rsp = PBC_WORKLOAD_RSP__INIT;
    
    pbc_req = pbc_workload_req__unpack(NULL, req->len, req->data);
    pbc_rsp.id = pbc_req->id;
    pbc_rsp.video_cnt = 1;
    pbc_rsp.info_mem = (2lu << 30);  //2147483648;  //((uint64_t)2 << 30);

    rsp_len = pbc_workload_rsp__get_packed_size(&pbc_rsp);
    reply->data = calloc(1, rsp_len);
    if (!reply->data) {
        ret = -1;
        goto out;
    }

    reply->len = rsp_len;
    pbc_workload_rsp__pack(&pbc_rsp, reply->data);

out:
    pbc_workload_req__free_unpacked(pbc_req, NULL);

    return ret;
}
#endif
extern int workload_exec (void *handler, ProtobufCBinaryData *req, ProtobufCBinaryData *reply);

#include "pbrpc_server.h"

int server(int argc, char **argv)
{
    //struct event_base *base;
    //struct evconnlistener *listener;
    //struct sockaddr_in sin;
    pbrpc_svc_callout tbl[] = 
    {
        //{ calculate, "Calculator.Calculate" },
        { NULL, 200000, 1, "Loader.workload", NULL, workload_exec },
    };

    int ret = -1;
    int port = 9876;

    if (argc > 1) {
        port = atoi(argv[1]);
    }

    if (port <= 0 || port > 65535) {
        puts("Invalid port");
        return 1;
    }

    pbrpc_svc *svc = pbrpc_svc_new("localhost", 9876);
    if (!svc) {
        fprintf(stderr, "Failed to create a new pbrpc_svc object");
        return 1;
    }

    ret = pbrpc_svc_register_methods(svc, tbl);
    if (ret) {
        fprintf(stderr, "Failed to register methods to pbrpc_svc.");
        return 1;
    }

    ret = pbrpc_svc_run(svc);
    if (ret) {
        fprintf(stderr, "Failed to start libevent base loop for "
                        "pbrpc_svc listener.");
        return 1;
    }

    return 0;
}
