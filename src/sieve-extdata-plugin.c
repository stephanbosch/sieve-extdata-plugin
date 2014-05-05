/* Copyright (c) 2002-2014 Sieve Extdata plugin authors, see the included
   COPYING file 
 */

#include "sieve-common.h"
#include "sieve-error.h"
#include "sieve-extensions.h"

#include "ext-extdata-common.h"
#include "sieve-extdata-plugin.h"

/*
 * Sieve plugin interface
 */

struct _plugin_context {
	const struct sieve_extension *ext_extdata;
};

void sieve_extdata_plugin_load
(struct sieve_instance *svinst, void **context)
{
	struct _plugin_context *pctx = i_new(struct _plugin_context, 1);

	pctx->ext_extdata = sieve_extension_register
		(svinst, &extdata_extension, FALSE);

	if ( svinst->debug ) {
		sieve_sys_debug(svinst, "%s version %s loaded",
			SIEVE_EXTDATA_NAME, SIEVE_EXTDATA_VERSION);
	}

	*context = (void *)pctx;
}

void sieve_extdata_plugin_unload
(struct sieve_instance *svinst ATTR_UNUSED, void *context)
{
	struct _plugin_context *pctx = (struct _plugin_context *)context;

	sieve_extension_unregister(pctx->ext_extdata);

	i_free(pctx);
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
