/* Copyright (c) 2002-2010 Sieve Extdata plugin authors, see the included COPYING file 
 */

#include "lib.h"
#include "dict.h"

#include "sieve-common.h"
#include "sieve-error.h"
#include "sieve-settings.h"
#include "sieve-extensions.h"
#include "sieve-code.h"
#include "sieve-validator.h"
#include "sieve-interpreter.h"
#include "sieve-ext-variables.h"

#include "ext-extdata-common.h"

/*
 * Extension context
 */

bool ext_extdata_load(const struct sieve_extension *ext, void **context)
{
	struct ext_extdata_context *ext_data;
	const char *dict_uri;

	if ( *context != NULL ) {
		ext_extdata_unload(ext);
		*context = NULL;
	}

	dict_uri = sieve_setting_get(ext->svinst, "sieve_extdata_dict_uri");
	if ( dict_uri == NULL ) {
		sieve_sys_warning(ext->svinst, 
			"extdata: no dict uri specified, extension is unconfigured "
			"(sieve_extdata_dict_uri is not set).");
	}

	ext_data = i_new(struct ext_extdata_context, 1);
	ext_data->dict_uri = i_strdup(dict_uri);
	ext_data->var_ext = sieve_ext_variables_get_extension(ext->svinst);

	*context = (void *) ext_data;

	return TRUE;
}

void ext_extdata_unload(const struct sieve_extension *ext)
{
	struct ext_extdata_context *ext_data =
		(struct ext_extdata_context *) ext->context;

	if ( ext_data == NULL )
		return;

	i_free(ext_data->dict_uri);
	i_free(ext_data);
}

/*
 * Interpreter context
 */

struct ext_extdata_interpreter_context {
	struct dict *dict;
};

static void ext_extdata_interpreter_free
(const struct sieve_extension *ext ATTR_UNUSED,
	struct sieve_interpreter *interp ATTR_UNUSED, void *context)
{
	struct ext_extdata_interpreter_context *ictx = 
		(struct ext_extdata_interpreter_context *) context;

	dict_deinit(&ictx->dict);
}

static struct sieve_interpreter_extension extdata_interpreter_extension = {
	&extdata_extension,
	NULL,
	ext_extdata_interpreter_free,
};

static struct ext_extdata_interpreter_context *
ext_extdata_interpreter_get_context
(const struct sieve_extension *ext, const struct sieve_runtime_env *renv)
{
	struct ext_extdata_context *ext_data =
		(struct ext_extdata_context *) ext->context;
	struct ext_extdata_interpreter_context *ictx = 
		(struct ext_extdata_interpreter_context *)
		sieve_interpreter_extension_get_context(renv->interp, ext);
	const struct sieve_script_env *senv = renv->scriptenv;
	struct dict *dict;
	pool_t pool;

	/* If there is already an interpreter context, return it rightaway */
	if ( ictx != NULL )
		return ictx;

	/* We cannot access the dict if no URI is configured or when the username is
	 * not known.
	 */
	if ( ext_data == NULL || senv->username == NULL )
		return NULL;

	/* Initialize the dict */
	dict = dict_init
		(ext_data->dict_uri, DICT_DATA_TYPE_STRING, senv->username, PKG_RUNDIR);

	if ( dict == NULL ) {
		sieve_runtime_critical(renv, NULL,
			"failed to retrieve external data item",
			"sieve extdata: failed to initialize dict %s", ext_data->dict_uri);
	}

	/* Create interpreter context */
	pool = sieve_interpreter_pool(renv->interp);
	ictx = p_new(pool, struct ext_extdata_interpreter_context, 1);
	ictx->dict = dict;

	/* Register interpreter extension to deinit the dict when the interpreter
	 * is freed.
	 */
	sieve_interpreter_extension_register
		(renv->interp, ext, &extdata_interpreter_extension, (void *) ictx);

	return ictx;
}

/*
 * Dict lookup
 */

const char *ext_extdata_get_value
(const struct sieve_runtime_env *renv, const struct sieve_extension *this_ext,
	const char *identifier)
{
	struct ext_extdata_interpreter_context *ictx = 
		ext_extdata_interpreter_get_context(this_ext, renv);
	const char *key;
	const char *value = NULL;

	if ( ictx == NULL ) {
		sieve_runtime_trace_error(renv, "extension is not configured");
		return NULL;
	}

	if ( ictx->dict == NULL )
		return NULL;

	key = t_strconcat("priv/", identifier, NULL);

	if ( dict_lookup(ictx->dict, pool_datastack_create(), key, &value) <= 0 )
		return NULL;

	return value;
}
