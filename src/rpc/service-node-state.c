/**
 * History:
 * ================================================================
 * 2019-10-08 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "service-node-state.h"
#include "service/pbc-nstatus.pb-c.h"
#include "pbc/pbrpc-svc.h"

static int node_state_exec(void *handler, ProtobufCBinaryData *req, ProtobufCBinaryData *reply)
{
    int ret = 0;
    size_t rsp_len;
    PbcNodeStateReq *pbc_req;
    PbcNodeStateRsp pbc_rsp = PBC_NODE_STATE_RSP__INIT;

    pbc_req = pbc_node_state_req__unpack(NULL, req->len, req->data);

    ServiceNodeStateRsp svc_rsp = {0};
    {
        printf("req id = %d\n", pbc_req->id);

        ServiceNodeStateReq svc_req = {0};
        svc_handler_t *h = (svc_handler_t *)handler;

        svc_req.id = pbc_req->id;
        h->pfn_callback(h->ctx, &svc_req, &svc_rsp);

        pbc_rsp.id          = pbc_req->id;
        pbc_rsp.index       = svc_rsp.index;
        pbc_rsp.name        = svc_rsp.name;
        pbc_rsp.count       = svc_rsp.count;
        pbc_rsp.uplink      = svc_rsp.uplink;
        pbc_rsp.downlink    = svc_rsp.downlink;
        pbc_rsp.memory      = svc_rsp.memory;
        pbc_rsp.cpu         = svc_rsp.cpu;

        printf("rsq id = %d\n", pbc_rsp.id);
        printf("rsq name = %s\n", pbc_rsp.name);
        printf("rsq count = %d\n", pbc_rsp.count);
        printf("rsq uplink = %ld\n", pbc_rsp.uplink);
        printf("rsq downlink = %ld\n", pbc_rsp.downlink);
        printf("rsq memory = %ld\n", pbc_rsp.memory);
        printf("rsq cpu = %lf\n", pbc_rsp.cpu);
    }

    rsp_len = pbc_node_state_rsp__get_packed_size(&pbc_rsp);
    reply->data = calloc(1, rsp_len);
    if (!reply->data)
    {
        ret = -1;
        goto out;
    }

    reply->len = rsp_len;
    pbc_node_state_rsp__pack(&pbc_rsp, reply->data);

out:
    pbc_node_state_req__free_unpacked(pbc_req, NULL);

    return ret;
}

int service_node_state_register(void *svc, void *args)
{
    return pbrpc_svc_register((pbrpc_svc *)svc, NODE_STATE_SC_PROG, NODE_STATE_SC_VERS, NODE_STATE_SC_NAME, node_state_exec, args);
}
