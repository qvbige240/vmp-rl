/**
 * History:
 * ================================================================
 * 2019-08-08 qing.zou created
 *
 */

#ifndef SERVICE_GATHER_H
#define SERVICE_GATHER_H

#include "workload.pb-c.h"

#ifdef __cplusplus
extern "C" {
#endif

int workload_exec(ProtobufCBinaryData *req, ProtobufCBinaryData *reply);


#ifdef __cplusplus
}
#endif

#endif // SERVICE_GATHER_H
