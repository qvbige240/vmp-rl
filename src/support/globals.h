
/**
 * History:
 * ================================================================
 * 2019-08-14 qing.zou created
 *
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef __cplusplus
extern "C" {
#endif

void* log_default_config(void);
void log_set_config(void *config);

void* global_default_config(void);
void global_set_config(void *config);

void* global_default_load(void);
void global_set_load(void *load);

#ifdef __cplusplus
}
#endif

#endif //GLOBALS_H