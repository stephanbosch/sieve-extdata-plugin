/* Copyright (c) 2002-2014 Sieve Extdata plugin authors, see the included
   COPYING file
 */

/* Extension vnd.dovecot.extdata
 * -----------------------------
 *
 * Authors: Stephan Bosch
 * Specification: none
 * Implementation: skeleton
 * Status: under development
 *
 */
 
#include "lib.h"

#include "sieve-extensions.h"
#include "sieve-commands.h"
#include "sieve-binary.h" 

#include "sieve-validator.h"
#include "sieve-interpreter.h"

#include "ext-extdata-common.h"
#include "ext-extdata-variables.h"

/* 
 * Extension 
 */

static bool ext_extdata_validator_load
	(const struct sieve_extension *ext, struct sieve_validator *valdtr);
	
const struct sieve_extension_def extdata_extension = { 
	.name = "vnd.dovecot.extdata",
	.load = ext_extdata_load,
	.unload = ext_extdata_unload,
	.validator_load = ext_extdata_validator_load,
	SIEVE_EXT_DEFINE_OPERATION(tst_extdata_operation), 
	SIEVE_EXT_DEFINE_OPERAND(extdata_namespace_operand)
};

static bool ext_extdata_validator_load
(const struct sieve_extension *ext, struct sieve_validator *valdtr)
{
	sieve_validator_register_command(valdtr, ext, &tst_extdata);

	ext_extdata_variables_init(ext, valdtr);

	return TRUE;
}

