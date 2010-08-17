/* Copyright (c) 2002-2010 Sieve Extdata plugin authors, see the included COPYING file 
 */

#ifndef __EXT_EXTDATA_COMMON_H
#define __EXT_EXTDATA_COMMON_H

#include "sieve-common.h"

/*
 * Extension
 */

struct ext_extdata_context {
	const struct sieve_extension *var_ext;
	char *dict_uri;
};

extern const struct sieve_extension_def extdata_extension;

bool ext_extdata_load(const struct sieve_extension *ext, void **context);
void ext_extdata_unload(const struct sieve_extension *ext);
bool ext_extdata_interpreter_load
	(const struct sieve_extension *ext, const struct sieve_runtime_env *renv,
    	sieve_size_t *address);

/* 
 * Commands 
 */

extern const struct sieve_command_def tst_extdata;

/*
 * Operations
 */

extern const struct sieve_operation_def tst_extdata_operation;

/*
 * Operands
 */

extern const struct sieve_operand_def extdata_namespace_operand;

/*
 * Dict lookup
 */

const char *ext_extdata_get_value
	(const struct sieve_runtime_env *renv, const struct sieve_extension *this_ext,
    	const char *identifier);

#endif /* __EXT_EXTDATA_COMMON_H */
