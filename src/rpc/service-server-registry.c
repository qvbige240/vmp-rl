/**
 * History:
 * ================================================================
 * 2019-10-08 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "service-server-registry.h"
#include "service/pbc-registry.pb-c.h"
#include "pbc/pbrpc-svc.h"

static int registry_exec(void *handler, ProtobufCBinaryData *req, ProtobufCBinaryData *reply)
{
    int ret = 0;
    size_t rsp_len;
    PbcRegistryReq *pbc_req;
    PbcRegistryRsp pbc_rsp = PBC_REGISTRY_RSP__INIT;

    pbc_req = pbc_registry_req__unpack(NULL, req->len, req->data);

    {
        //printf("req server id = %d\n", pbc_req->sid);
        printf("req->name = %s\n", pbc_req->name);
        printf("req->system = %s\n", pbc_req->system);
        printf("req->location = %s\n", pbc_req->location);
        printf("req->processor = %s\n", pbc_req->processor);
        printf("req->bandwidth = %lu\n", pbc_req->bandwidth);
        printf("req->memory = %lu\n", pbc_req->memory);
        printf("req->ip = %s\n", pbc_req->ip);
        printf("req->port = %d\n", pbc_req->port);

        RpcServiceRegistryReq svc_req = {0};
        RpcServiceRegistryRsp svc_rsp = {0};
        svc_handler_t *h = (svc_handler_t *)handler;

        svc_req.id = pbc_req->sid;
        h->pfn_callback(h->ctx, &svc_req, &svc_rsp);

        //pbc_rsp.id = pbc_req->id;
        //pbc_rsp.count = 10;
        //pbc_rsp.info_mem = (2lu << 30); //2147483648;  //((uint64_t)2 << 30);
        pbc_rsp.sid = svc_rsp.id;
        //pbc_rsp.count  = svc_rsp.num;

        printf("rsq server id = %d\n", pbc_rsp.sid);
        printf("rsq name = %s\n", pbc_rsp.name);
        printf("rsq index = %d\n", pbc_rsp.index);
        printf("rsq num = %d\n", pbc_rsp.num);
    }

    rsp_len = pbc_registry_rsp__get_packed_size(&pbc_rsp);
    reply->data = calloc(1, rsp_len);
    if (!reply->data)
    {
        ret = -1;
        goto out;
    }

    reply->len = rsp_len;
    pbc_registry_rsp__pack(&pbc_rsp, reply->data);

out:
    pbc_registry_req__free_unpacked(pbc_req, NULL);

    return ret;
}

int service_registry_register(void *svc, void *args)
{
    return pbrpc_svc_register((pbrpc_svc *)svc, REGISTRY_SC_PROG, REGISTRY_SC_VERS, REGISTRY_SC_NAME, registry_exec, args);
}
