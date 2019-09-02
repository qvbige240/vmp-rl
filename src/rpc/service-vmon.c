/**
 * History:
 * ================================================================
 * 2019-08-29 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "service-vmon.h"
#include "service/pbc-vmon.pb-c.h"
#include "pbc/pbrpc-svc.h"

static int vmon_exec(void *handler, ProtobufCBinaryData *req, ProtobufCBinaryData *reply)
{
    int ret = 0;
    size_t rsp_len;
    PbcVmonReq *pbc_req;
    PbcVmonRsp pbc_rsp = PBC_VMON_RSP__INIT;

    pbc_req = pbc_vmon_req__unpack(NULL, req->len, req->data);

    {
        printf("req id = %d\n", pbc_req->id);

        ServiceVmonReq svc_req = {0};
        ServiceVmonRsp svc_rsp = {0};
        svc_handler_t *h = (svc_handler_t *)handler;

        svc_req.id = pbc_req->id;
        h->pfn_callback(h->ctx, &svc_req, &svc_rsp);

        //pbc_rsp.id = pbc_req->id;
        //pbc_rsp.count = 10;
        //pbc_rsp.info_mem = (2lu << 30); //2147483648;  //((uint64_t)2 << 30);
        pbc_rsp.id     = pbc_req->id;
        pbc_rsp.count  = svc_rsp.num;

        printf("rsq id = %d\n", pbc_rsp.id);
        printf("rsq count = %d\n", pbc_rsp.count);
    }

    rsp_len = pbc_vmon_rsp__get_packed_size(&pbc_rsp);
    reply->data = calloc(1, rsp_len);
    if (!reply->data)
    {
        ret = -1;
        goto out;
    }

    reply->len = rsp_len;
    pbc_vmon_rsp__pack(&pbc_rsp, reply->data);

out:
    pbc_vmon_req__free_unpacked(pbc_req, NULL);

    return ret;
}

int service_vmon_register(void *svc, void *args)
{
    return pbrpc_svc_register((pbrpc_svc*)svc, VMON_SC_PROG, VMON_SC_VERS, VMON_SC_NAME, vmon_exec, args);
}
