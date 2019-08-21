/**
 * History:
 * ================================================================
 * 2019-08-19 qing.zou created
 *
 */

#ifndef RPC_TYPEDEF_H
#define RPC_TYPEDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HAVE_VPK 1

#ifndef HAVE_VPK

  #define ZERO_LEN_ARRAY	1

  #define DECL_PRIV(thiz, priv) PrivInfo* priv = thiz != NULL ? (PrivInfo*)thiz->priv : NULL

  typedef int (*vmp_callback_func)(void* p, int msg, void* arg);

#else

  #include "vmp.h"

#endif


#if 0
#define RPC_LOGD(format, args...) VMP_LOGD(format, ##args)
#define RPC_LOGI(format, args...) VMP_LOGI(format, ##args)
#define RPC_LOGW(format, args...) VMP_LOGW(format, ##args)
#define RPC_LOGE(format, args...) VMP_LOGE(format, ##args)
#define RPC_LOGF(format, args...) VMP_LOGF(format, ##args)
#else
#define RPC_LOGD(format, args...) printf(format"\n", ##args)
#define RPC_LOGI(format, args...) printf(format"\n", ##args)
#define RPC_LOGW(format, args...) printf(format"\n", ##args)
#define RPC_LOGE(format, args...) printf(format"\n", ##args)
#define RPC_LOGF(format, args...) printf(format"\n", ##args)
#endif


#endif // RPC_TYPEDEF_H
