#!/bin/bash

include bash.utils.System;

function bash.io.Filesystem()
{
	return 0;
}

#====================================
# Usage: Filesystem.delete $1 $2
#	$1 - file/directory to delete
#	$2 - sudo
#====================================
function Filesystem.delete()
{
	local __FILE=`readlink -f $1`;
	if [ -e "$__FILE" ]; then
		if [ "$__FILESYSTEM_DRY" = true ]; then
			Logger.verbose "Deleting file: $__FILE [DRY-RUN=ON]";
		else
			Logger.verbose "Deleting file: $__FILE";
			if [ "$2" = "sudo" ]; then
				System.obtainRootPrivs;
				sudo rm -rf $__FILE || Logger.fatal "File '$__FILE' could not be deleted.";
			else
				rm -rf "$__FILE" || Logger.fatal "File '$__FILE' could not be deleted.";
			fi
		fi
		Logger.verbose "File deleted successfully.";
	else
		Logger.warn "File '$__FILE' does not even exist.";
	fi
}

#================================
# Usage: Filesystem.link $1 $2 $3
#	$1 - what
#	$2 - to where
#	$3 - sudo
#================================
function Filesystem.link()
{
	if [ "$3" = "sudo" ]; then
		sudo ln -s "$1" "$2";
	else
		ln -s "$1" "$2";
	fi
}

#=============================
# Usage: Filesystem.mkdir $1
#	$1 - directory to make
#=============================
function Filesystem.mkdir()
{
	mkdir -p "$1"
	if [ ! -d "$1" ]; then
		Logger.fatal "Unable to create directory: $1";
	fi
}

#=============================
# Usage: Filesystem.makeDir $1
#	$1 - directory to make
#=============================
function Filesystem.makeDir()
{
	Filesystem.mkdir "$@"
	return $?
}


#==============================
# Usage: Filesystem.checkDir $1
#	$1 - directory to check
#==============================
function Filesystem.checkDir()
{
	if [ ! -d "$1" ]; then
		Logger.warn "Directory doesn't exist: $1";
		return 1;
	elif [ ! -r "$1" ]; then
		Logger.fatal "You cannot access this directory: $1";
	fi
	return 0;
}

#===============================
# Usage: Filesystem.checkFile $1
#	$1 - file to check
#===============================
function Filesystem.checkFile()
{
	if [ ! -f "$1" ]; then
		Logger.fatal "Could not find file: $1";
	elif [ ! -r "$1" ]; then
		Logger.fatal "You cannot access file: $1";
	fi
}

#===============================
# Usage: Filesystem.checkFor $1
#	$1 - file or dir to check
#===============================
function Filesystem.checkFor()
{
	if [ ! -e "$1" ]; then
		Logger.fatal "Could not find file or directory: $1";
	elif [ ! -r "$1" ]; then
		Logger.fatal "Cannot access file or directory: $1. No read privileges.";
	fi
}

#===========================
# Usage Filesystem.dryRun $1
#	$1 - 1/0
#===========================
function Filesystem.dryRun()
{
	eval "__FILESYSTEM_DRY=$1";
}

#Calling the constructor
bash.io.Filesystem
