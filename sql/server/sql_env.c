/*
 * The contents of this file are subject to the MonetDB Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.monetdb.org/Legal/MonetDBLicense
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is the MonetDB Database System.
 *
 * The Initial Developer of the Original Code is CWI.
 * Portions created by CWI are Copyright (C) 1997-July 2008 CWI.
 * Copyright August 2008-2015 MonetDB B.V.
 * All Rights Reserved.
 */

/*
 * Environment variables
 * =====================
 *
 * The processing setting of the SQL front-end can collect information
 * for postprocessing and debugging by setting a flag
 * using the SQL construct:
 * SET <variable>=<string>
 * SET <variable>=<boolean>
 * SET <variable>=<int>
 *
 * The SQL engine comes with a limited set of environment variables
 * to control its behavior.
 * The 'debug' variable takes an integer and sets the Mserver global
 * debug flag. (See MonetDB documentation.)
 *
 * By default all remaining variables are stored as strings and
 * any type analysis is up to the user. The can be freely used by the
 * SQL programmer for inclusion in his queries.
 *
 * The limited number of built-in variables defined above are
 * strongly typed the hard way.
 * Moreover, they have a counterpart representation in the
 * MVC structure to ease inspection during query processing.
 *
 * The variables can be retrieved using the table producing function var();
 */

#include "monetdb_config.h"
#include "sql_env.h"
#include "sql_semantic.h"
#include "sql_privileges.h"

#define SESSION_RW 0
#define SESSION_RO 1

int
mvc_debug_on(mvc *m, int flg)
{

	if (m->debug & flg)
		return 1;

	return 0;
}

str
sql_update_var(mvc *m, char *name, char *sval, lng sgn)
{
	if (strcmp(name, "debug") == 0) {
		assert((lng) GDK_int_min <= sgn && sgn <= (lng) GDK_int_max);
		m->debug = (int) sgn;
	} else if (strcmp(name, "current_schema") == 0) {
		if (!mvc_set_schema(m, sval)) {
			return sql_message( "Schema (%s) missing\n", sval);
		}
	} else if (strcmp(name, "current_role") == 0) {
		if (!mvc_set_role(m, sval)) {
			return sql_message( "Role (%s) missing\n", sval);
		}
	} else if (strcmp(name, "current_timezone") == 0) {
		assert((lng) GDK_int_min <= sgn && sgn <= (lng) GDK_int_max);
		m->timezone = (int) sgn;
	} else if (strcmp(name, "cache") == 0) {
		assert((lng) GDK_int_min <= sgn && sgn <= (lng) GDK_int_max);
		m->cache = (int) sgn;
	} else if (strcmp(name, "history") == 0) {
		assert((lng) GDK_int_min <= sgn && sgn <= (lng) GDK_int_max);
		m->history = (sgn != 0);
	} 
	return NULL;
}

int
sql_create_env(mvc *m, sql_schema *s)
{
	list *res, *ops;

	res = sa_list(m->sa);
	list_append(res, sql_create_arg(m->sa, "name", sql_bind_subtype(m->sa, "varchar", 1024, 0), ARG_OUT));  
	list_append(res, sql_create_arg(m->sa, "value", sql_bind_subtype(m->sa, "varchar", 2048, 0), ARG_OUT));  

	/* add function */
	ops = sa_list(m->sa);
	mvc_create_func(m, NULL, s, "env", ops, res, F_UNION,  FUNC_LANG_SQL, "sql", "sql_environment", "CREATE FUNCTION env () RETURNS TABLE( name varchar(1024), value varchar(2048)) EXTERNAL NAME sql.sql_environment;", FALSE, FALSE);

	res = sa_list(m->sa);
	list_append(res, sql_create_arg(m->sa, "name", sql_bind_subtype(m->sa, "varchar", 1024, 0), ARG_OUT));  

	/* add function */
	ops = sa_list(m->sa);
	mvc_create_func(m, NULL, s, "var", ops, res, F_UNION, FUNC_LANG_SQL, "sql", "sql_variables", "CREATE FUNCTION var() RETURNS TABLE( name varchar(1024)) EXTERNAL NAME sql.sql_variables;", FALSE, FALSE);
	return 0;
}
