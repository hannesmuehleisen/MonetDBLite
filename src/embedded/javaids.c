/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0.  If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Copyright 2008-2017 MonetDB B.V.
 */

#include "javaids.h"

#include <jni.h>

/* Embedded database environment Classes */

static jmethodID monetDBEmbeddedDatabaseConstructorID;
static jclass monetDBEmbeddedExceptionClassID;
static jclass monetDBEmbeddedConnectionClassID;
static jmethodID monetDBEmbeddedConnectionConstructorID;
static jclass jDBCEmbeddedConnectionClassID;
static jmethodID jDBCDBEmbeddedConnectionConstructorID;

static jclass queryResultSetID;
static jmethodID queryResultSetConstructorID;
static jclass monetDBTableClassID;
static jmethodID monetDBTableClassConstructorID;

static jfieldID monetDBEmbeddedConnectionPointerID;
static jfieldID connectionResultPointerID;

/* Java MonetDB mappings constructors */

static jclass javaObjectClassID;
static jclass byteArrayClassID;
static jclass bigDecimalClassID;
static jmethodID bigDecimalConstructorID;

static jclass dateClassID;
static jmethodID dateConstructorID;
static jclass timeClassID;
static jmethodID timeConstructorID;
static jclass timestampClassID;
static jmethodID timestampConstructorID;
static jclass gregorianCalendarClassID;
static jmethodID gregorianCalendarConstructorID;
static jmethodID gregorianCalendarSetterID;

static jclass stringClassID;
static jmethodID stringByteArrayConstructorID;

/* JDBC Embedded Connection */

static jfieldID serverResponsesID;
static jfieldID lastErrorID;
static jfieldID lastResultSetPointerID;
static jfieldID serverHeaderResponseID;
static jfieldID lastServerResponseParametersID;
static jfieldID lastServerResponseID;

static jclass autoCommitResponseClassID;
static jmethodID autoCommitResponseConstructorID;
static jclass updateResponseClassID;
static jmethodID updateResponseConstructorID;

/* MonetDB Table */

static jclass monetDBTableColumnClassID;
static jmethodID monetDBTableColumnConstructorID;
static jclass mappingEnumID;
static jmethodID getEnumValueID;

static jfieldID getConnectionID;
static jfieldID getConnectionLongID;
static jfieldID getSchemaID;
static jfieldID getTableID;

static jmethodID bigDecimalToStringID;
static jmethodID setBigDecimalScaleID;
static jmethodID dateToLongID;
static jmethodID timeToLongID;
static jmethodID timestampToLongID;

void initializeIDS(JNIEnv *env) {
    /* Embedded database environment Classes */

    jobject tempLocalRef;
    jclass monetDBEmbeddedDatabaseClass = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedDatabase");

    // private MonetDBEmbeddedDatabase(String dbDirectory, boolean silentFlag, boolean sequentialFlag)
    monetDBEmbeddedDatabaseConstructorID = (*env)->GetMethodID(env, monetDBEmbeddedDatabaseClass, "<init>", "(Ljava/lang/String;ZZ)V");
    (*env)->DeleteLocalRef(env, monetDBEmbeddedDatabaseClass);

    tempLocalRef = (jobject) (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedException");
    monetDBEmbeddedExceptionClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    tempLocalRef = (jobject) (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/MonetDBEmbeddedConnection");
    monetDBEmbeddedConnectionClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    // protected MonetDBEmbeddedConnection(long connectionPointer)
    monetDBEmbeddedConnectionConstructorID = (*env)->GetMethodID(env, monetDBEmbeddedConnectionClassID, "<init>", "(J)V");
    monetDBEmbeddedConnectionPointerID = (*env)->GetFieldID(env, monetDBEmbeddedConnectionClassID, "connectionPointer", "J");

    jclass abstractConnectionResultClass = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/env/AbstractConnectionResult");
    connectionResultPointerID = (*env)->GetFieldID(env, abstractConnectionResultClass, "tablePointer", "J");
    (*env)->DeleteLocalRef(env, abstractConnectionResultClass);

    tempLocalRef = (jobject) (*env)->FindClass(env, "nl/cwi/monetdb/embedded/jdbc/JDBCEmbeddedConnection");
    jDBCEmbeddedConnectionClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    // protected JDBCEmbeddedConnection(long connectionPointer)
    jDBCDBEmbeddedConnectionConstructorID = (*env)->GetMethodID(env, jDBCEmbeddedConnectionClassID, "<init>", "(J)V");

    tempLocalRef = (jobject) (*env)->FindClass(env, "nl/cwi/monetdb/embedded/resultset/QueryResultSet");
    queryResultSetID = (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    //public QueryResultSet(MonetDBEmbeddedConnection connection, long tablePointer, String[] columnNames, String[] columnTypes, int[] columnDigits, int[] columnScales, Object[] data, int numberOfRows
    queryResultSetConstructorID = (*env)->GetMethodID(env, queryResultSetID, "<init>", "(Lnl/cwi/monetdb/embedded/env/MonetDBEmbeddedConnection;J[Ljava/lang/String;[Ljava/lang/String;[I[I[Ljava/lang/Object;I)V");

    tempLocalRef = (jobject) (*env)->FindClass(env, "nl/cwi/monetdb/embedded/tables/MonetDBTable");
    monetDBTableClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    //public MonetDBTable(MonetDBEmbeddedConnection connection, String tableSchema, String tableName)
    monetDBTableClassConstructorID = (*env)->GetMethodID(env, monetDBTableClassID, "<init>", "(Lnl/cwi/monetdb/embedded/env/MonetDBEmbeddedConnection;Ljava/lang/String;Ljava/lang/String;)V");

    /* Java MonetDB mappings constructors */

    tempLocalRef = (jobject) (*env)->FindClass(env, "java/lang/Object");
    javaObjectClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    tempLocalRef = (jobject) (*env)->FindClass(env, "[B");
    byteArrayClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    tempLocalRef = (jobject) (*env)->FindClass(env, "java/math/BigDecimal");
    bigDecimalClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    bigDecimalConstructorID = (*env)->GetMethodID(env, bigDecimalClassID, "<init>", "(Ljava/lang/String;)V");

    tempLocalRef = (jobject) (*env)->FindClass(env, "java/sql/Date");
    dateClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    dateConstructorID = (*env)->GetMethodID(env, dateClassID, "<init>", "(J)V");

    tempLocalRef = (jobject) (*env)->FindClass(env, "java/sql/Time");
    timeClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    timeConstructorID = (*env)->GetMethodID(env, timeClassID, "<init>", "(J)V");

    tempLocalRef = (jobject) (*env)->FindClass(env, "java/sql/Timestamp");
    timestampClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    timestampConstructorID = (*env)->GetMethodID(env, timestampClassID, "<init>", "(J)V");

    tempLocalRef = (jobject) (*env)->FindClass(env, "java/util/GregorianCalendar");
    gregorianCalendarClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    gregorianCalendarConstructorID = (*env)->GetMethodID(env, gregorianCalendarClassID, "<init>", "()V");
    gregorianCalendarSetterID = (*env)->GetMethodID(env, gregorianCalendarClassID, "setTimeInMillis", "(J)V");

    tempLocalRef = (jobject) (*env)->FindClass(env, "java/lang/String");
    stringClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    stringByteArrayConstructorID = (*env)->GetMethodID(env, stringClassID, "<init>", "([B)V");

    /* JDBC Embedded Connection */

    serverResponsesID = (*env)->GetFieldID(env, jDBCEmbeddedConnectionClassID, "lineResponse", "[I");
    lastErrorID = (*env)->GetFieldID(env, jDBCEmbeddedConnectionClassID, "lastError", "Ljava/lang/String;");
    lastResultSetPointerID = (*env)->GetFieldID(env, jDBCEmbeddedConnectionClassID, "lastResultSetPointer", "J");
    serverHeaderResponseID = (*env)->GetFieldID(env, jDBCEmbeddedConnectionClassID, "serverHeaderResponse", "I");
    lastServerResponseParametersID = (*env)->GetFieldID(env, jDBCEmbeddedConnectionClassID, "lastServerResponseParameters", "[I");
    lastServerResponseID = (*env)->GetFieldID(env, jDBCEmbeddedConnectionClassID, "lastServerResponse", "Lnl/cwi/monetdb/mcl/responses/IResponse;");

    tempLocalRef = (jobject) (*env)->FindClass(env, "nl/cwi/monetdb/mcl/responses/AutoCommitResponse");
    autoCommitResponseClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    autoCommitResponseConstructorID = (*env)->GetMethodID(env, autoCommitResponseClassID, "<init>", "(Z)V");

    tempLocalRef = (jobject) (*env)->FindClass(env, "nl/cwi/monetdb/mcl/responses/UpdateResponse");
    updateResponseClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    updateResponseConstructorID = (*env)->GetMethodID(env, updateResponseClassID, "<init>", "(II)V");

    tempLocalRef = (jobject) (*env)->FindClass(env, "nl/cwi/monetdb/embedded/tables/MonetDBTableColumn");
    monetDBTableColumnClassID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    //public MonetDBTableColumn(String columnType, String columnName, int columnDigits, int columnScale, String defaultValue, boolean isNullable)
    monetDBTableColumnConstructorID = (*env)->GetMethodID(env, monetDBTableColumnClassID, "<init>", "(Ljava/lang/String;Ljava/lang/String;IILjava/lang/String;Z)V");

    tempLocalRef = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/mapping/MonetDBToJavaMapping");
    mappingEnumID = (jclass) (*env)->NewGlobalRef(env, tempLocalRef);
    (*env)->DeleteLocalRef(env, tempLocalRef);

    getEnumValueID = (*env)->GetStaticMethodID(env, mappingEnumID, "GetJavaMappingFromMonetDBString", "(Ljava/lang/String;)Lnl/cwi/monetdb/embedded/mapping/MonetDBToJavaMapping;");
    jclass tableClass = (*env)->FindClass(env, "nl/cwi/monetdb/embedded/tables/MonetDBTable");
    getConnectionID = (*env)->GetFieldID(env, tableClass, "connection", "Lnl/cwi/monetdb/embedded/env/MonetDBEmbeddedConnection;");
    getConnectionLongID = (*env)->GetFieldID(env, monetDBEmbeddedConnectionClassID, "connectionPointer", "J");
    getSchemaID = (*env)->GetFieldID(env, tableClass, "tableSchema", "Ljava/lang/String;");
    getTableID = (*env)->GetFieldID(env, tableClass, "tableName", "Ljava/lang/String;");
    (*env)->DeleteLocalRef(env, tableClass);

    bigDecimalToStringID = (*env)->GetMethodID(env, bigDecimalClassID, "toPlainString", "()Ljava/lang/String;");
    setBigDecimalScaleID = (*env)->GetMethodID(env, bigDecimalClassID, "setScale", "(II)Ljava/math/BigDecimal;");
    dateToLongID = (*env)->GetMethodID(env, dateClassID, "getTime", "()J");
    timeToLongID = (*env)->GetMethodID(env, timeClassID, "getTime", "()J");
    timestampToLongID = (*env)->GetMethodID(env, timestampClassID, "getTime", "()J");
}

void releaseIDS(JNIEnv *env) {
    /* Embedded database environment Classes */

    (*env)->DeleteGlobalRef(env, monetDBEmbeddedExceptionClassID);
    (*env)->DeleteGlobalRef(env, monetDBEmbeddedConnectionClassID);
    (*env)->DeleteGlobalRef(env, jDBCEmbeddedConnectionClassID);
    (*env)->DeleteGlobalRef(env, queryResultSetID);
    (*env)->DeleteGlobalRef(env, monetDBTableClassID);

    /* Java MonetDB mappings constructors */

    (*env)->DeleteGlobalRef(env, javaObjectClassID);
    (*env)->DeleteGlobalRef(env, byteArrayClassID);
    (*env)->DeleteGlobalRef(env, bigDecimalClassID);
    (*env)->DeleteGlobalRef(env, dateClassID);
    (*env)->DeleteGlobalRef(env, timeClassID);
    (*env)->DeleteGlobalRef(env, timestampClassID);
    (*env)->DeleteGlobalRef(env, gregorianCalendarClassID);
    (*env)->DeleteGlobalRef(env, stringClassID);

    /* JDBC Embedded Connection */

    (*env)->DeleteGlobalRef(env, autoCommitResponseClassID);
    (*env)->DeleteGlobalRef(env, updateResponseClassID);

    /* MonetDB Table */

    (*env)->DeleteGlobalRef(env, monetDBTableColumnClassID);
    (*env)->DeleteGlobalRef(env, mappingEnumID);
}

/* Embedded database environment Classes */

jmethodID getMonetDBEmbeddedDatabaseConstructorID() {
    return monetDBEmbeddedDatabaseConstructorID;
}

jclass getMonetDBEmbeddedExceptionClassID() {
    return monetDBEmbeddedExceptionClassID;
}

jclass getMonetDBEmbeddedConnectionClassID() {
    return monetDBEmbeddedConnectionClassID;
}

jmethodID getMonetDBEmbeddedConnectionConstructorID() {
    return monetDBEmbeddedConnectionConstructorID;
}

jclass getJDBCEmbeddedConnectionClassID() {
    return jDBCEmbeddedConnectionClassID;
}

jmethodID getJDBCDBEmbeddedConnectionConstructorID() {
    return jDBCDBEmbeddedConnectionConstructorID;
}

jclass getQueryResultSetID() {
    return queryResultSetID;
}

jmethodID getQueryResultSetConstructorID() {
    return queryResultSetConstructorID;
}

jclass getMonetDBTableClassID() {
    return monetDBTableClassID;
}

jmethodID getMonetDBTableClassConstructorID() {
    return monetDBTableClassConstructorID;
}

jfieldID getMonetDBEmbeddedConnectionPointerID() {
    return monetDBEmbeddedConnectionPointerID;
}

jfieldID getConnectionResultPointerID() {
    return connectionResultPointerID;
}

/* Java MonetDB mappings constructors */

jclass getJavaObjectClassID() {
    return javaObjectClassID;
}

jclass getByteArrayClassID() {
    return byteArrayClassID;
}

jclass getBigDecimalClassID() {
    return bigDecimalClassID;
}

jmethodID getBigDecimalConstructorID() {
    return bigDecimalConstructorID;
}

jclass getDateClassID() {
    return dateClassID;
}

jmethodID getDateConstructorID() {
    return dateConstructorID;
}

jclass getTimeClassID() {
    return timeClassID;
}

jmethodID getTimeConstructorID() {
    return timeConstructorID;
}

jclass getTimestampClassID() {
    return timestampClassID;
}

jmethodID getTimestampConstructorID() {
    return timestampConstructorID;
}

jclass getGregorianCalendarClassID() {
    return gregorianCalendarClassID;
}

jmethodID getGregorianCalendarConstructorID() {
    return gregorianCalendarConstructorID;
}

jmethodID getGregorianCalendarSetterID() {
    return gregorianCalendarSetterID;
}

jclass getStringClassID() {
    return stringClassID;
}

jmethodID getStringByteArrayConstructorID() {
    return stringByteArrayConstructorID;
}

/* JDBC Embedded Connection */

jfieldID getServerResponsesID() {
    return serverResponsesID;
}

jfieldID getLastErrorID() {
    return lastErrorID;
}

jfieldID getLastResultSetPointerID() {
    return lastResultSetPointerID;
}

jfieldID getServerHeaderResponseID() {
    return serverHeaderResponseID;
}

jfieldID getLastServerResponseParametersID() {
    return lastServerResponseParametersID;
}

jfieldID getLastServerResponseID() {
    return lastServerResponseID;
}

jclass getAutoCommitResponseClassID() {
    return autoCommitResponseClassID;
}

jmethodID getAutoCommitResponseConstructorID() {
    return autoCommitResponseConstructorID;
}

jclass getUpdateResponseClassID() {
    return updateResponseClassID;
}

jmethodID getUpdateResponseConstructorID() {
    return updateResponseConstructorID;
}

/* MonetDB Table */

jclass getMonetDBTableColumnClassID() {
    return monetDBTableColumnClassID;
}

jmethodID getMonetDBTableColumnConstructorID() {
    return monetDBTableColumnConstructorID;
}

jclass getMappingEnumID() {
    return mappingEnumID;
}

jmethodID getGetEnumValueID() {
    return getEnumValueID;
}

jfieldID getGetConnectionID() {
    return getConnectionID;
}

jfieldID getGetConnectionLongID() {
    return getConnectionLongID;
}

jfieldID getGetSchemaID() {
    return getSchemaID;
}

jfieldID getGetTableID() {
    return getTableID;
}

jmethodID getBigDecimalToStringID() {
    return bigDecimalToStringID;
}

jmethodID getSetBigDecimalScaleID() {
    return setBigDecimalScaleID;
}

jmethodID getDateToLongID() {
    return dateToLongID;
}

jmethodID getTimeToLongID() {
    return timeToLongID;
}

jmethodID getTimestampToLongID() {
    return timestampToLongID;
}
