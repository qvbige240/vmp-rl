
/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#ifndef MODULE_CONFIG_H
#define MODULE_CONFIG_H

#include "vpk/vpk_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DATA_DEFAULT_PATH       "/tmp/vmp"
#define BIN_DEFAULT_PATH        "./"

typedef struct vmp_config_t
{
    int         server; //test

    void*       network;

    char        data_dir[MAX_PATH_SIZE+1];
} vmp_config_t;


vmp_config_t* vmp_config_create();
int vmp_config_init(vmp_config_t* thiz, int argc, char* argv[]);
int vmp_config_load_file(vmp_config_t* thiz, void *conf);
int vmp_config_load(vmp_config_t* thiz, void *conf);

#ifdef __cplusplus
}
#endif

#endif //MODULE_CONFIG_H
