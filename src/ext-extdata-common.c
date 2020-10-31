/* Copyright (c) 2002-2014 Sieve Extdata plugin authors, see the included
   COPYING file
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

	if (*context != NULL) {
		ext_extdata_unload(ext);
		*context = NULL;
	}

	dict_uri = sieve_setting_get(ext->svinst, "sieve_extdata_dict_uri");
	if  (dict_uri == NULL) {
		sieve_sys_warning(ext->svinst, "extdata: "
			"no dict uri specified, extension is unconfigured "
			"(sieve_extdata_dict_uri is not set).");
	}

	ext_data = i_new(struct ext_extdata_context, 1);
	ext_data->dict_uri = i_strdup(dict_uri);
	ext_data->var_ext = sieve_ext_variables_get_extension(ext->svinst);

	*context = (void *)ext_data;
	return TRUE;
}

void ext_extdata_unload(const struct sieve_extension *ext)
{
	struct ext_extdata_context *ext_data =
		(struct ext_extdata_context *)ext->context;

	if (ext_data == NULL)
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

static void
ext_extdata_interpreter_free(const struct sieve_extension *ext ATTR_UNUSED,
			     struct sieve_interpreter *interp ATTR_UNUSED,
			     void *context)
{
	struct ext_extdata_interpreter_context *ictx =
		(struct ext_extdata_interpreter_context *)context;

	if (ictx->dict != NULL)
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
	struct sieve_instance *svinst = ext->svinst;
	struct ext_extdata_context *ext_data =
		(struct ext_extdata_context *)ext->context;
	struct ext_extdata_interpreter_context *ictx =
		(struct ext_extdata_interpreter_context *)
		sieve_interpreter_extension_get_context(renv->interp, ext);
	const char *error;
	struct dict_settings dict_set;
	struct dict *dict = NULL;
	pool_t pool;
	int ret;

	/* If there is already an interpreter context, return it rightaway */
	if (ictx != NULL)
		return ictx;

	/* We cannot access the dict if no URI is configured or when the
	   username is not known.
	 */
	if (ext_data == NULL || ext_data->dict_uri == NULL ||
	    svinst->username == NULL)
		return NULL;

	/* Initialize the dict */
	i_zero(&dict_set);
	dict_set.username = svinst->username;
	dict_set.base_dir = svinst->base_dir;
	ret = dict_init(ext_data->dict_uri, &dict_set, &dict, &error);
	if (ret < 0) {
		sieve_runtime_critical(renv, NULL,
			"failed to initialize vnd.dovecot.extdata extension",
			"sieve dict backend: "
			"failed to initialize dict with data `%s' "
			"for user `%s': %s",
			ext_data->dict_uri, svinst->username, error);
	}

	/* Create interpreter context */
	pool = sieve_interpreter_pool(renv->interp);
	ictx = p_new(pool, struct ext_extdata_interpreter_context, 1);
	ictx->dict = dict;

	/* Register interpreter extension to deinit the dict when the interpreter
	   is freed.
	 */
	sieve_interpreter_extension_register(renv->interp, ext,
					     &extdata_interpreter_extension,
					     (void *)ictx);
	return ictx;
}

/*
 * Dict lookup
 */

const char *
ext_extdata_get_value(const struct sieve_runtime_env *renv,
		      const struct sieve_extension *this_ext,
		      const char *identifier)
{
	struct ext_extdata_interpreter_context *ictx =
		ext_extdata_interpreter_get_context(this_ext, renv);
	const char *key, *error;
	const char *value = NULL;
	int ret;

	if (ictx == NULL) {
		sieve_runtime_trace_error(renv, "extension is not configured");
		return NULL;
	}

	if (ictx->dict == NULL) {
		sieve_runtime_trace_error(
			renv, "extension is not properly configured");
		return NULL;
	}

	key = t_strconcat("priv/", identifier, NULL);
	ret = dict_lookup(ictx->dict, pool_datastack_create(), key,
			  &value, &error);
	if (ret <= 0) {
		if (ret < 0)
			sieve_runtime_warning(
				renv, NULL,
				"extdata: failed to lookup value `%s': %s",
				identifier, error);
		return NULL;
	}
	return value;
}
