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
 * SQLCopyDesc()
 * CLI Compliance: ISO 92
 *
 * Note: this function is not supported (yet), it returns an error.
 *
 * Author: Martin van Dinther
 * Date  : 30 aug 2002
 *
 **********************************************************************/

#include "ODBCGlobal.h"
#include "ODBCStmt.h"


SQLRETURN SQL_API
SQLCopyDesc(SQLHDESC hSourceDescHandle, SQLHDESC hTargetDescHandle)
{
	ODBCDesc *src = (ODBCDesc *) hSourceDescHandle;
	ODBCDesc *dst = (ODBCDesc *) hTargetDescHandle;

#ifdef ODBCDEBUG
	ODBCLOG("SQLCopyDesc\n");
#endif

	if (!isValidDesc(src))
		return SQL_INVALID_HANDLE;

	if (!isValidDesc(dst))
		return SQL_INVALID_HANDLE;

	if (isIRD(dst)) {
		/* Cannot modify an implementation row descriptor */
		addDescError(src, "HY016", NULL, 0);
		return SQL_ERROR;
	}

	clearDescErrors(src);

	if (isIRD(src) &&
	    src->Stmt->State != PREPARED &&
	    src->Stmt->State != EXECUTED) {
		/* Associated statement is not prepared */
		addDescError(src, "HY007", NULL, 0);
		return SQL_ERROR;
	}

	/* copy sql_desc_count and allocate space for descr. records */
	setODBCDescRecCount(dst, src->sql_desc_count);

	/* don't copy sql_desc_alloc_type */
	dst->sql_desc_array_size = src->sql_desc_array_size;
	dst->sql_desc_array_status_ptr = src->sql_desc_array_status_ptr;
	dst->sql_desc_bind_offset_ptr = src->sql_desc_bind_offset_ptr;
	dst->sql_desc_bind_type = src->sql_desc_bind_type;
	dst->sql_desc_rows_processed_ptr = src->sql_desc_rows_processed_ptr;
	if (src->descRec)
		memcpy(dst->descRec, src->descRec,
		       src->sql_desc_count * sizeof(*src->descRec));

	return SQL_SUCCESS;
}
