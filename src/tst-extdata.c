/* Copyright (c) 2002-2010 Dovecot Sieve authors, see the included COPYING file
 */

#include "sieve-common.h"
#include "sieve-commands.h"
#include "sieve-code.h"
#include "sieve-comparators.h"
#include "sieve-match-types.h"
#include "sieve-validator.h"
#include "sieve-generator.h"
#include "sieve-interpreter.h"
#include "sieve-dump.h"
#include "sieve-match.h"

#include "ext-extdata-common.h"

/* 
 * Extdata test 
 *
 * Syntax:
 *   extdata [COMPARATOR] [MATCH-TYPE]
 *           <name: string> <key-list: string-list>
 */

static bool tst_extdata_registered
	(struct sieve_validator *valdtr, const struct sieve_extension *ext,
		struct sieve_command_registration *cmd_reg);
static bool tst_extdata_validate
	(struct sieve_validator *valdtr, struct sieve_command *tst);
static bool tst_extdata_generate
	(const struct sieve_codegen_env *cgenv, struct sieve_command *cmd);

const struct sieve_command_def tst_extdata = { 
	"extdata", 
	SCT_TEST, 
	2, 0, FALSE, FALSE,
	tst_extdata_registered, 
	NULL,
	tst_extdata_validate, 
	tst_extdata_generate, 
	NULL 
};

/* 
 * Extdata operation
 */

static bool tst_extdata_operation_dump
	(const struct sieve_dumptime_env *denv, sieve_size_t *address);
static int tst_extdata_operation_execute
	(const struct sieve_runtime_env *renv, sieve_size_t *address);

const struct sieve_operation_def tst_extdata_operation = { 
	"EXTDATA",
	&extdata_extension, 
	0, 
	tst_extdata_operation_dump, 
	tst_extdata_operation_execute 
};

/* 
 * Test registration 
 */

static bool tst_extdata_registered
(struct sieve_validator *valdtr, const struct sieve_extension *ext ATTR_UNUSED,
	struct sieve_command_registration *cmd_reg) 
{
	/* The order of these is not significant */
	sieve_comparators_link_tag(valdtr, cmd_reg, SIEVE_MATCH_OPT_COMPARATOR);
	sieve_match_types_link_tags(valdtr, cmd_reg, SIEVE_MATCH_OPT_MATCH_TYPE);

	return TRUE;
}

/* 
 * Test validation 
 */

static bool tst_extdata_validate
(struct sieve_validator *valdtr, struct sieve_command *tst) 
{ 		
	struct sieve_ast_argument *arg = tst->first_positional;
	const struct sieve_match_type mcht_default = 
		SIEVE_MATCH_TYPE_DEFAULT(is_match_type);
	const struct sieve_comparator cmp_default = 
		SIEVE_COMPARATOR_DEFAULT(i_ascii_casemap_comparator);
	
	if ( !sieve_validate_positional_argument
		(valdtr, tst, arg, "name", 1, SAAT_STRING) ) {
		return FALSE;
	}
	
	if ( !sieve_validator_argument_activate(valdtr, tst, arg, FALSE) )
		return FALSE;
	
	arg = sieve_ast_argument_next(arg);

	if ( !sieve_validate_positional_argument
		(valdtr, tst, arg, "key list", 2, SAAT_STRING_LIST) ) {
		return FALSE;
	}
	
	if ( !sieve_validator_argument_activate(valdtr, tst, arg, FALSE) )
		return FALSE;

	/* Validate the key argument to a specified match type */
	return sieve_match_type_validate
		(valdtr, tst, arg, &mcht_default, &cmp_default);
}

/* 
 * Test generation 
 */

static bool tst_extdata_generate
(const struct sieve_codegen_env *cgenv, struct sieve_command *cmd) 
{
	sieve_operation_emit(cgenv->sblock, cmd->ext, &tst_extdata_operation);

 	/* Generate arguments */
	if ( !sieve_generate_arguments(cgenv, cmd, NULL) )
		return FALSE;
	
	return TRUE;
}

/* 
 * Code dump 
 */

static bool tst_extdata_operation_dump
(const struct sieve_dumptime_env *denv, sieve_size_t *address)
{
	int opt_code = 0;

	sieve_code_dumpf(denv, "EXTDATA");
	sieve_code_descend(denv);

	/* Handle any optional arguments */
	if ( !sieve_match_dump_optional_operands(denv, address, &opt_code) )
		return FALSE;

	if ( opt_code != SIEVE_MATCH_OPT_END )
		return FALSE;
		
	return
		sieve_opr_string_dump(denv, address, "name") &&
		sieve_opr_stringlist_dump(denv, address, "key list");
}

/* 
 * Code execution 
 */

static int tst_extdata_operation_execute
(const struct sieve_runtime_env *renv, sieve_size_t *address)
{
	const struct sieve_extension *this_ext = renv->oprtn.ext;
	int ret, mret;
	bool result = TRUE;
	int opt_code = 0;
	struct sieve_match_type mcht = 
		SIEVE_MATCH_TYPE_DEFAULT(is_match_type);
	struct sieve_comparator cmp = 
		SIEVE_COMPARATOR_DEFAULT(i_ascii_casemap_comparator);
	struct sieve_match_context *mctx;
	string_t *name;
	struct sieve_coded_stringlist *key_list;
	const char *ext_value = NULL;
	bool matched = FALSE;

	/*
	 * Read operands 
	 */
	
	/* Handle match-type and comparator operands */
	if ( (ret=sieve_match_read_optional_operands
		(renv, address, &opt_code, &cmp, &mcht)) <= 0 )
		return ret;
	
	/* Check whether we neatly finished the list of optional operands*/
	if ( opt_code != SIEVE_MATCH_OPT_END) {
		sieve_runtime_trace_error(renv, "invalid optional operand");
		return SIEVE_EXEC_BIN_CORRUPT;
	}

	/* Read source */
	if ( !sieve_opr_string_read(renv, address, &name) ) {
		sieve_runtime_trace_error(renv, "invalid name operand");
		return SIEVE_EXEC_BIN_CORRUPT;
	}
	
	/* Read key-list */
	if ( (key_list=sieve_opr_stringlist_read(renv, address)) == NULL ) {
		sieve_runtime_trace_error(renv, "invalid key-list operand");
		return SIEVE_EXEC_BIN_CORRUPT;
	}

	/*
	 * Perform operation
	 */

	sieve_runtime_trace(renv, "DICT test");

	ext_value = ext_extdata_get_value(renv, this_ext, str_c(name));

	if ( ext_value != NULL ) {
		mctx = sieve_match_begin(renv->interp, &mcht, &cmp, NULL, key_list); 	

		if ( (mret=sieve_match_value(mctx, strlen(ext_value) == 0 ? NULL : ext_value, 
			strlen(ext_value))) < 0 ) {
			result = FALSE;
		} else {
			matched = ( mret > 0 );				
		}

		if ( (mret=sieve_match_end(&mctx)) < 0 )
			result = FALSE;
		else
			matched = ( mret > 0 || matched );
	}
	
	if ( result ) {
		sieve_interpreter_set_test_result(renv->interp, matched);
		return SIEVE_EXEC_OK;
	}
	
	sieve_runtime_trace_error(renv, "invalid key list item");
	return SIEVE_EXEC_BIN_CORRUPT;
}
