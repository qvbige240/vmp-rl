/**
 * History:
 * ================================================================
 * 2019-10-08 qing.zou created
 *
 */

#ifndef LOAD_NODE_STATE_H
#define LOAD_NODE_STATE_H

#include "load_typedef.h"

VMP_BEGIN_DELS

typedef struct _LoadNodeStateReq
{
    unsigned int            id;

    void                    *svc;
    void                    *process;

    void                    *ctx;
    vmp_callback_func       pfn_callback;
} LoadNodeStateReq;

typedef struct _LoadNodeStateRsp
{
    unsigned int            num;
} LoadNodeStateRsp;

void *load_node_state_create(void *parent, LoadNodeStateReq *req);

int load_node_state_start(void *p);

void load_node_state_done(void *p);

VMP_END_DELS

#endif // LOAD_NODE_STATE_H
