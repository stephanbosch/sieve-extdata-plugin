#ifndef __SIEVE_EXTDATA_PLUGIN_H
#define __SIEVE_EXTDATA_PLUGIN_H

/*
 * Plugin interface
 */

void sieve_extdata_plugin_load(struct sieve_instance *svinst, void **context);
void sieve_extdata_plugin_unload(struct sieve_instance *svinst, void *context);

/*
 * Module interface
 */

void sieve_extdata_plugin_init(void);
void sieve_extdata_plugin_deinit(void);

#endif
