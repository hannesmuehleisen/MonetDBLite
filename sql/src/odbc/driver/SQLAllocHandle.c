/*
 * This code was created by Peter Harvey (mostly during Christmas 98/99).
 * This code is LGPL. Please ensure that this message remains in future
 * distributions and uses of this code (thats about all I get out of it).
 * - Peter Harvey pharvey@codebydesign.com
 * 
 * This file has been modified for the MonetDB project.  See the file
 * Copyright in this directory for more information.
 */

/**********************************************************************
 * SQLAllocHandle
 * CLI compliance: ISO 92
 *
 * Note: This function also implements the deprecated ODBC functions
 * SQLAllocEnv(), SQLAllocConnect() and SQLAllocStmt()
 * Those functions are simply mapped to this function.
 * All checks and allocation is done in this function.
 *
 * Author: Martin van Dinther
 * Date  : 30 Aug 2002
 *
 **********************************************************************/

#include "ODBCGlobal.h"
#include "ODBCEnv.h"
#include "ODBCDbc.h"
#include "ODBCStmt.h"
#include "ODBCError.h"

SQLRETURN
SQLAllocEnv_(SQLHANDLE *pnOutputHandle)
{
	if (pnOutputHandle == NULL) {
		return SQL_INVALID_HANDLE;
	}
	*pnOutputHandle = (SQLHANDLE *) newODBCEnv();
	return *pnOutputHandle == NULL ? SQL_ERROR : SQL_SUCCESS;
}

SQLRETURN
SQLAllocDbc_(ODBCEnv *env, SQLHANDLE *pnOutputHandle)
{
	if (env->sql_attr_odbc_version == 0) {
		addEnvError(env, "HY010", NULL, 0);
		return SQL_ERROR;
	}
	if (pnOutputHandle == NULL) {
		addEnvError(env, "HY009", NULL, 0);
		return SQL_ERROR;
	}
	*pnOutputHandle = (SQLHANDLE *) newODBCDbc(env);
	return *pnOutputHandle == NULL ? SQL_ERROR : SQL_SUCCESS;
}

SQLRETURN
SQLAllocStmt_(ODBCDbc *dbc, SQLHANDLE *pnOutputHandle)
{
	if (!dbc->Connected) {
		addDbcError(dbc, "08003", NULL, 0);
		return SQL_ERROR;
	}
	if (pnOutputHandle == NULL) {
		addDbcError(dbc, "HY009", NULL, 0);
		return SQL_ERROR;
	}
	*pnOutputHandle = (SQLHANDLE *) newODBCStmt(dbc);
	return *pnOutputHandle == NULL ? SQL_ERROR : SQL_SUCCESS;
}

SQLRETURN
SQLAllocDesc_(ODBCDbc *dbc, SQLHANDLE *pnOutputHandle)
{
	if (!dbc->Connected) {
		addDbcError(dbc, "08003", NULL, 0);
		return SQL_ERROR;
	}
	if (pnOutputHandle == NULL) {
		addDbcError(dbc, "HY009", NULL, 0);
		return SQL_ERROR;
	}
	*pnOutputHandle = (SQLHANDLE *) newODBCDesc(dbc);
	return *pnOutputHandle == NULL ? SQL_ERROR : SQL_SUCCESS;
}

SQLRETURN
SQLAllocHandle_(SQLSMALLINT nHandleType, SQLHANDLE nInputHandle,
		SQLHANDLE *pnOutputHandle)
{
	switch (nHandleType) {
	case SQL_HANDLE_ENV:
		if (nInputHandle != NULL)
			return SQL_INVALID_HANDLE;
		return SQLAllocEnv_(pnOutputHandle);
	case SQL_HANDLE_DBC:
		if (!isValidEnv((ODBCEnv *) nInputHandle))
			return SQL_INVALID_HANDLE;
		clearEnvErrors((ODBCEnv *) nInputHandle);
		return SQLAllocDbc_((ODBCEnv *) nInputHandle, pnOutputHandle);
	case SQL_HANDLE_STMT:
		if (!isValidDbc((ODBCDbc *) nInputHandle))
			return SQL_INVALID_HANDLE;
		clearDbcErrors((ODBCDbc *) nInputHandle);
		return SQLAllocStmt_((ODBCDbc *) nInputHandle, pnOutputHandle);
	case SQL_HANDLE_DESC:
		if (!isValidDbc((ODBCDbc *) nInputHandle))
			return SQL_INVALID_HANDLE;
		clearDbcErrors((ODBCDbc *) nInputHandle);
		return SQLAllocDesc_((ODBCDbc *) nInputHandle, pnOutputHandle);
	default:
		/* we cannot set an error because we do not know
		   the handle type of the possibly non-null handle */
		return SQL_INVALID_HANDLE;
	}
}

SQLRETURN SQL_API
SQLAllocHandle(SQLSMALLINT nHandleType,	/* type to be allocated */
	       SQLHANDLE nInputHandle,	/* context for new handle */
	       SQLHANDLE *pnOutputHandle) /* ptr for allocated handle struct */
{
#ifdef ODBCDEBUG
	ODBCLOG("SQLAllocHandle %s\n",
		nHandleType == SQL_HANDLE_ENV ? "Env" :
		nHandleType == SQL_HANDLE_DBC ? "Dbc" :
		nHandleType == SQL_HANDLE_STMT ? "Stmt" : "Desc");
#endif

	return SQLAllocHandle_(nHandleType, nInputHandle, pnOutputHandle);
}
