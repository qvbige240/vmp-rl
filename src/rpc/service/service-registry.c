/**
 * History:
 * ================================================================
 * 2019-08-08 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pbc-registry.pb-c.h"
#include "service-gather.h"

int registry_exec(ProtobufCBinaryData *req, ProtobufCBinaryData *reply)
{
    int ret = 0;
    size_t rsp_len;
    PbcRegistryReq *pbc_req;
    PbcRegistryRsp pbc_rsp = PBC_REGISTRY_RSP__INIT;

    pbc_req = pbc_registry_req__unpack(NULL, req->len, req->data);
    pbc_rsp.sid = pbc_req->sid;
    pbc_rsp.name = pbc_req->name;
    pbc_rsp.ip = pbc_req->ip;
    pbc_rsp.has_num = 1;
    pbc_rsp.num = 10;

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
