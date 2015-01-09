#!/bin/bash

include bash.utils.System;

function bash.apps.Pgsql()
{
	__PGSQL_DUMPDIR="/home/postgres";
}

#============================================
# Usage: Pgsql.setDumpDir $1
#	$1 - alternate directory for db dumps
#============================================
function Pgsql.setDumpDir()
{
	Filesystem.checkDir "$1" || Filesystem.mkdir "$1";
	eval "__PGSQL_DUMPDIR=$1";
}

#========================================
# Usage: Pgsql.setAdmin $1
#	$1 - PGSQL administrative account
#========================================
function Pgsql.setAdmin()
{
	eval "__PGSQL_ADMIN=$1";
}

#===============================
# Usage: Pgsql.setDB $1
#	$1 - PGSQL database name
#===============================
function Pgsql.setDB()
{
	eval "__PGSQL_DB=$1";
}

#================================
# Usage: Pgsql.setHome $1
#	$1 - Pgsql home directory
#================================
function Pgsql.setHome()
{
	Filesystem.checkDir "$1";
	eval "__PGSQL_HOME=$1";
}

#======================
# Usage: Pgsql.backupDB
#======================
function Pgsql.backupDB()
{
	Logger.note "Attempting to backup Postgres database: $__PGSQL_DB";
	Pgsql.checkForDB;
	if [ "$?" -eq "0" ]; then
		if [ -z "$__PGSQL_DUMPDIR" ]; then
			Logger.fatal "You must set the Dump directory: Pgsql.setDumpDir";
		fi
		__PGSQL_TIME=`date +%Y_%m_%d-%H_%M_%S`
		Logger.note "Backing up the Postgres Database: $__PGSQL_DB to $__PGSQL_DUMPDIR/$__PGSQL_DB-backup.$__PGSQL_TIME.sql";
		System.obtainRootPrivs;
		sudo su - "$__PGSQL_ADMIN" -c \
			"$__PGSQL_HOME/bin/pg_dump $__PGSQL_DB > $__PGSQL_DUMPDIR/$__PGSQL_DB-backup.$__PGSQL_TIME.sql" || \
			Logger.fatal "Database backup failed.";
		Logger.note "Database backup successful.";
	fi
}

#====================
# Usage: Pgsql.dropDB
#====================
function Pgsql.dropDB()
{
	Logger.warn "Attempting to drop Postgres database: $__PGSQL_DB";
	Pgsql.checkForDB;
	if [ "$?" -eq "0" ]; then
		System.obtainRootPrivs;
		sudo su - "$__PGSQL_ADMIN" -c "$__PGSQL_HOME/bin/dropdb $__PGSQL_DB" || \
			Logger.fatal "Database drop was unsuccessful.";
		Logger.note "Database dropped successfully.";
	fi
}

#======================
# Usage: Pgsql.vaccumDB
#======================
function Pgsql.vaccumDB()
{
	Logger.warn "Vacuuming the Postgres Database: $__PGSQL_DB";
	# ... to be implemented :)
	Logger.note "Database vacuumed successfully.";
}

#======================
# Usage: Pgsql.createDB
#======================
function Pgsql.createDB()
{
	Logger.note "Creating Pgsql database: $__PGSQL_DB";
	System.obtainRootPrivs;
	sudo su - "$__PGSQL_ADMIN" -c "$__PGSQL_HOME/bin/createdb $__PGSQL_DB" || \
		Logger.fatal "Database creation failed.";
	sudo su - "$__PGSQL_ADMIN" -c "$__PGSQL_HOME/bin/createlang plpgsql $__PGSQL_DB" || \
		Logger.fatal "Adding database language failed.";
	Logger.note "Pgsql database created successfully.";
}

#===========================
# Usage: Pgsql.runSQL $1
#	$1 - SQL file to run
#===========================
function Pgsql.runSql()
{
	Logger.note "Running SQL: $1";
	System.obtainRootPrivs;
	sudo su - "$__PGSQL_ADMIN" -c "$__PGSQL_HOME/bin/psql -f $1 -d $__PGSQL_DB" \
		|| Logger.fatal "Running SQL failed.";
	Logger.note "SQL run successful.";
}

#========================
# Usage: Pgsql.checkForDB
#========================
function Pgsql.checkForDB()
{
	__DB_EXIST=`sudo su - "$__PGSQL_ADMIN" -c "$__PGSQL_HOME/bin/psql -l | grep $__PGSQL_DB"`;
	if [ ! "$__DB_EXIST" ]; then
		Logger.warn "Database $__PGSQL_DB does not even exist.";
		return 1;
	fi
	return 0;
}

#Call the constructor
bash.apps.Pgsql;