/**
 * History:
 * ================================================================
 * 2019-08-08 qing.zou created
 *
 */

#include "workload.pb-c.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int workload_exec(ProtobufCBinaryData *req, ProtobufCBinaryData *reply)
{
    int ret = 0;
    size_t rsp_len;
    PbcWorkloadReq *pbc_req;
    PbcWorkloadRsp pbc_rsp = PBC_WORKLOAD_RSP__INIT;

    pbc_req = pbc_workload_req__unpack(NULL, req->len, req->data);
    pbc_rsp.id = pbc_req->id;
    pbc_rsp.video_cnt = 1;
    pbc_rsp.info_mem = (2lu << 30); //2147483648;  //((uint64_t)2 << 30);

    rsp_len = pbc_workload_rsp__get_packed_size(&pbc_rsp);
    reply->data = calloc(1, rsp_len);
    if (!reply->data)
    {
        ret = -1;
        goto out;
    }

    reply->len = rsp_len;
    pbc_workload_rsp__pack(&pbc_rsp, reply->data);

out:
    pbc_workload_req__free_unpacked(pbc_req, NULL);

    return ret;
}
