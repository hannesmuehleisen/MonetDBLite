#!/usr/bin/env python -O

# The contents of this file are subject to the MonetDB Public License
# Version 1.1 (the "License"); you may not use this file except in
# compliance with the License. You may obtain a copy of the License at
# http://monetdb.cwi.nl/Legal/MonetDBLicense-1.1.html
#
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
# License for the specific language governing rights and limitations
# under the License.
#
# The Original Code is the MonetDB Database System.
#
# The Initial Developer of the Original Code is CWI.
# Portions created by CWI are Copyright (C) 1997-July 2008 CWI.
# Copyright August 2008-2009 MonetDB B.V.
# All Rights Reserved.

""" Script to test database capabilities and the DB-API interface
    for functionality and memory leaks.

    Adapted from a script by M-A Lemburg and taken from the MySQL python driver.

"""
from time import time
import array
import unittest


class DatabaseTest(unittest.TestCase):

    db_module = None
    connect_args = ()
    connect_kwargs = dict()
    create_table_extra = ''
    rows = 10
    debug = False

    def setUp(self):
        import gc
        db = self.db_module.connect(*self.connect_args, **self.connect_kwargs)
        self.connection = db
        self.cursor = db.cursor()
        self.BLOBText = ''.join([chr(i) for i in range(256)] * 100);
        self.BLOBUText = u''.join([unichr(i) for i in range(16384)])
        self.BLOBBinary = self.db_module.Binary(''.join([chr(i) for i in range(256)] * 16))

    leak_test = True

    def tearDown(self):
        if self.leak_test:
            import gc
            del self.cursor
            orphans = gc.collect()
            self.failIf(orphans, "%d orphaned objects found after deleting cursor" % orphans)

            del self.connection
            orphans = gc.collect()
            self.failIf(orphans, "%d orphaned objects found after deleting connection" % orphans)

    def table_exists(self, name):
        try:
            self.cursor.execute('select * from %s where 1=0' % name)
        except:
            return False
        else:
            return True

    def quote_identifier(self, ident):
        return '"%s"' % ident

    def new_table_name(self):
        i = id(self.cursor)
        while True:
            name = self.quote_identifier('tb%08x' % i)
            if not self.table_exists(name):
                return name
            i = i + 1

    def create_table(self, columndefs):

        """ Create a table using a list of column definitions given in
            columndefs.

            generator must be a function taking arguments (row_number,
            col_number) returning a suitable data object for insertion
            into the table.

        """
        self.table = self.new_table_name()
        self.cursor.execute('CREATE TABLE %s (%s) %s' %
                            (self.table,
                             ',\n'.join(columndefs),
                             self.create_table_extra))

    def check_data_integrity(self, columndefs, generator):
        # insert
        self.create_table(columndefs)
        insert_statement = ('INSERT INTO %s VALUES (%s)' %
                            (self.table,
                             ','.join(['%s'] * len(columndefs))))
        data = [ [ generator(i,j) for j in range(len(columndefs)) ]
                 for i in range(self.rows) ]
        if self.debug:
            print data
        self.cursor.executemany(insert_statement, data)
        self.connection.commit()
        # verify
        self.cursor.execute('select * from %s' % self.table)
        l = self.cursor.fetchall()
        if self.debug:
            print l
        self.assertEquals(len(l), self.rows)
        try:
            for i in range(self.rows):
                for j in range(len(columndefs)):
                    self.assertEquals(l[i][j], generator(i,j))
        finally:
            if not self.debug:
                self.cursor.execute('drop table %s' % (self.table))

    def test_transactions(self):
        columndefs = ( 'col1 INT', 'col2 VARCHAR(255)')
        def generator(row, col):
            if col == 0: return row
            else: return ('%i' % (row%10))*255
        self.create_table(columndefs)
        insert_statement = ('INSERT INTO %s VALUES (%s)' %
                            (self.table,
                             ','.join(['%s'] * len(columndefs))))
        data = [ [ generator(i,j) for j in range(len(columndefs)) ]
                 for i in range(self.rows) ]
        self.cursor.executemany(insert_statement, data)
        # verify
        self.connection.commit()
        self.cursor.execute('select * from %s' % self.table)
        l = self.cursor.fetchall()
        self.assertEquals(len(l), self.rows)
        for i in range(self.rows):
            for j in range(len(columndefs)):
                self.assertEquals(l[i][j], generator(i,j))
        delete_statement = 'delete from %s where col1=%%s' % self.table
        self.cursor.execute(delete_statement, (0,))
        self.cursor.execute('select col1 from %s where col1=%s' % \
                            (self.table, 0))
        l = self.cursor.fetchall()
        self.failIf(l, "DELETE didn't work")
        self.connection.rollback()
        self.cursor.execute('select col1 from %s where col1=%s' % \
                            (self.table, 0))
        l = self.cursor.fetchall()
        self.failUnless(len(l) == 1, "ROLLBACK didn't work")
        self.cursor.execute('drop table %s' % (self.table))

    def test_truncation(self):
        columndefs = ( 'col1 INT', 'col2 VARCHAR(255)')
        def generator(row, col):
            if col == 0: return row
            else: return ('%i' % (row%10))*((255-self.rows/2)+row)
        self.create_table(columndefs)
        insert_statement = ('INSERT INTO %s VALUES (%s)' %
                            (self.table,
                             ','.join(['%s'] * len(columndefs))))

        try:
            self.cursor.execute(insert_statement, (0, '0'*256))
        except Warning:
            if self.debug: print self.cursor.messages
        except self.connection.DataError:
            pass
        else:
            self.fail("Over-long column did not generate warnings/exception with single insert")

        self.connection.rollback()

        try:
            for i in range(self.rows):
                data = []
                for j in range(len(columndefs)):
                    data.append(generator(i,j))
                self.cursor.execute(insert_statement,tuple(data))
        except Warning:
            if self.debug: print self.cursor.messages
        except self.connection.DataError:
            pass
        else:
            self.fail("Over-long columns did not generate warnings/exception with execute()")

        self.connection.rollback()

        try:
            data = [ [ generator(i,j) for j in range(len(columndefs)) ]
                     for i in range(self.rows) ]
            self.cursor.executemany(insert_statement, data)
        except Warning:
            if self.debug: print self.cursor.messages
        except self.connection.DataError:
            pass
        else:
            self.fail("Over-long columns did not generate warnings/exception with executemany()")

        self.connection.rollback()
        self.cursor.execute('drop table %s' % (self.table))

    def test_CHAR(self):
        # Character data
        def generator(row,col):
            return ('%i' % ((row+col) % 10)) * 255
        self.check_data_integrity(
            ('col1 char(255)','col2 char(255)'),
            generator)

    def test_INT(self):
        # Number data
        def generator(row,col):
            return row*row
        self.check_data_integrity(
            ('col1 INT',),
            generator)

    def test_DECIMAL(self):
        # DECIMAL
        def generator(row,col):
            from decimal import Decimal
            return Decimal("%d.%02d" % (row, col))
        self.check_data_integrity(
            ('col1 DECIMAL(5,2)',),
            generator)

    def test_DATE(self):
        ticks = time()
        def generator(row,col):
            return self.db_module.DateFromTicks(ticks+row*86400-col*1313)
        self.check_data_integrity(
                 ('col1 DATE',),
                 generator)

    def test_TIME(self):
        ticks = time()
        def generator(row,col):
            return self.db_module.TimeFromTicks(ticks+row*86400-col*1313)
        self.check_data_integrity(
                 ('col1 TIME',),
                 generator)

    def test_DATETIME(self):
        ticks = time()
        def generator(row,col):
            return self.db_module.TimestampFromTicks(ticks+row*86400-col*1313)
        self.check_data_integrity(
                 ('col1 DATETIME',),
                 generator)

    def test_TIMESTAMP(self):
        ticks = time()
        def generator(row,col):
            return self.db_module.TimestampFromTicks(ticks+row*86400-col*1313)
        self.check_data_integrity(
                 ('col1 TIMESTAMP',),
                 generator)

    def test_fractional_TIMESTAMP(self):
        ticks = time()
        def generator(row,col):
            return self.db_module.TimestampFromTicks(ticks+row*86400-col*1313+row*0.7*col/3.0)
        self.check_data_integrity(
                 ('col1 TIMESTAMP',),
                 generator)

    def test_LONG(self):
        def generator(row,col):
            if col == 0:
                return row
            else:
                return self.BLOBUText # 'BLOB Text ' * 1024
        self.check_data_integrity(
                 ('col1 INT','col2 LONG'),
                 generator)

    def test_TEXT(self):
        def generator(row,col):
            return self.BLOBUText # 'BLOB Text ' * 1024
        self.check_data_integrity(
                 ('col2 TEXT',),
                 generator)

    def test_LONG_BYTE(self):
        def generator(row,col):
            if col == 0:
                return row
            else:
                return self.BLOBBinary # 'BLOB\000Binary ' * 1024
        self.check_data_integrity(
                 ('col1 INT','col2 LONG BYTE'),
                 generator)

    def test_BLOB(self):
        def generator(row,col):
            if col == 0:
                return row
            else:
                return self.BLOBBinary # 'BLOB\000Binary ' * 1024
        self.check_data_integrity(
                 ('col1 INT','col2 BLOB'),
                 generator)

