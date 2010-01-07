/* Copyright (c) 2002-2010 Dovecot Sieve authors, see the included COPYING file 
 */

#include "sieve-common.h"
#include "sieve-extensions.h"

#include "ext-extdata-common.h"
#include "sieve-extdata-plugin.h"

/*
 * Sieve plugin interface
 */

void sieve_extdata_plugin_load(struct sieve_instance *svinst)
{
	(void)sieve_extension_register(svinst, &extdata_extension, TRUE);
}

void sieve_extdata_plugin_unload(struct sieve_instance *svinst ATTR_UNUSED)
{
	/* Nothing */
}

/*
 * Module interface
 */

void sieve_extdata_plugin_init(void)
{
	/* Nothing */
}

void sieve_extdata_plugin_deinit(void)
{
	/* Nothing */
}
