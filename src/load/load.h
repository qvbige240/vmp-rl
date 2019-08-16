/**
 * History:
 * ================================================================
 * 2019-08-15 qing.zou created
 *
 */

#ifndef LOAD_H
#define LOAD_H

//#include "vmp.h"
#include "load_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


void load_init(int argc, char **argv);

void load_start(void);

void load_done(void);


#ifdef __cplusplus
}
#endif

#endif // LOAD_H
