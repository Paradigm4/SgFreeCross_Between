#!/bin/bash
#Quick restart script for dev use
LIBNAME="sgf_cross_between"
iquery -aq "unload_library('$LIBNAME')" > /dev/null 2>&1
set -e

DBNAME="mydb"
#This is easily sym-linkable: ~/scidb

SCIDB_INSTALL=$SCIDB_INSTALL_PATH
export SCIDB_THIRDPARTY_PREFIX="/opt/scidb/15.7"

mydir=`dirname $0`
pushd $mydir
make SCIDB=$SCIDB_INSTALL

scidb.py stopall $DBNAME 
sudo cp lib$LIBNAME.so ${SCIDB_INSTALL}/lib/scidb/plugins/
scidb.py startall $DBNAME 

iquery -aq "load_library('$LIBNAME')"
