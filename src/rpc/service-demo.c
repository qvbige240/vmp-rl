/**
 * History:
 * ================================================================
 * 2019-08-29 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "service-demo.h"
#include "service/pbc-demo.pb-c.h"
#include "pbc/pbrpc-svc.h"

static int demo_exec(void *handler, ProtobufCBinaryData *req, ProtobufCBinaryData *reply)
{
    int ret = 0;
    size_t rsp_len;
    PbcDemoReq *pbc_req;
    PbcDemoRsp pbc_rsp = PBC_DEMO_RSP__INIT;

    pbc_req = pbc_demo_req__unpack(NULL, req->len, req->data);

    {
        printf("req id = %d\n", pbc_req->id);

        ServiceDemoReq svc_req = {0};
        ServiceDemoRsp svc_rsp = {0};
        svc_handler_t *h = (svc_handler_t *)handler;

        svc_req.id = pbc_req->id;
        h->pfn_callback(h->ctx, &svc_req, &svc_rsp);

        //pbc_rsp.id = pbc_req->id;
        //pbc_rsp.count = 10;
        //pbc_rsp.info_mem = (2lu << 30); //2147483648;  //((uint64_t)2 << 30);
        pbc_rsp.id          = pbc_req->id;
        pbc_rsp.video_cnt   = svc_rsp.cnt;
        pbc_rsp.info_mem    = svc_rsp.mem;

        printf("rsq id = %d\n", pbc_rsp.id);
        printf("rsq video_cnt = %d\n", pbc_rsp.video_cnt);
        printf("rsq info_mem = %ld\n", pbc_rsp.info_mem);
    }

    rsp_len = pbc_demo_rsp__get_packed_size(&pbc_rsp);
    reply->data = calloc(1, rsp_len);
    if (!reply->data)
    {
        ret = -1;
        goto out;
    }

    reply->len = rsp_len;
    pbc_demo_rsp__pack(&pbc_rsp, reply->data);

out:
    pbc_demo_req__free_unpacked(pbc_req, NULL);

    return ret;
}

int service_demo_register(void *svc, void *args)
{
    return pbrpc_svc_register((pbrpc_svc*)svc, DEMO_SC_PROG, DEMO_SC_VERS, DEMO_SC_NAME, demo_exec, args);
}
