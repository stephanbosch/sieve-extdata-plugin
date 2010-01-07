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
		sieve_sys_warning("extdata: no dict uri specified, extension is unconfigured "
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

/* FIXME: context not freed fully */

struct ext_extdata_interpreter_context {
	struct dict *dict;
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


	if ( ictx != NULL )
		return ictx;

	if ( ext_data == NULL || senv->username == NULL )
		return NULL;

	dict = dict_init
		(ext_data->dict_uri, DICT_DATA_TYPE_STRING, senv->username, PKG_RUNDIR);
	
	if ( dict == NULL )
		return NULL;

	pool = sieve_interpreter_pool(renv->interp);
	ictx = p_new(pool, struct ext_extdata_interpreter_context, 1);
	ictx->dict = dict;

	sieve_interpreter_extension_set_context(renv->interp, ext, ictx);
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
		sieve_runtime_trace(renv, "extension is not configured");
		return NULL;
	}

	key = t_strconcat("priv/", identifier, NULL);

	if ( dict_lookup(ictx->dict, pool_datastack_create(), key, &value) <= 0 )
		return NULL;

	return value;
}
