#!/bin/bash

include bash.io.Filesystem;

function bash.System
{
	return 0;
}

#===================================
# Usage: System.setSingleInstance $1
#	$1 - Path to the lock file the lock file contains the
#		ID of the running process on the first line
#===================================
function System.setSingleInstance()
{
	if [ -f "$1" ]; then
		local __SYSTEM_PID=`cat "$1" | head -1`
		if [ -z "${__SYSTEM_PID}" ]; then
			Logger.verbose "Could not find a Process ID inside the lock file '$1'."
			Filesystem.delete "$1"
		else
			#Check if the process is still running
			local __SYS_CHECK_PID=`ps -o pid -p "$__SYSTEM_PID" | tail -1 | sed -e 's@\s@@g'`

			#Exit if another instance is already running
			if [ "${__SYS_CHECK_PID}" = "${__SYSTEM_PID}" ]; then
				Logger.fatal "Another instance is already running. Please wait until it is done and run this script again."
				exit 1
			fi

			Logger.verbose "A previous instance failed to clean up after itself. Perhaps it died unexpectedly."
			#If execution reached this point, then the lock file is present, it contains
			#a PID, but the process is no longer in the memory. Perhaps it died prematurely.
			#Try to delete the lock file and continue execution
			Filesystem.delete "$1"
		fi
	fi

	#Store the current PID in the Lock file
	echo "$$" > "$1"
	local RET="$?"
	__BF_LOCK_FILE="$1"

	return $?
}

#==============================
# Usage: System.getTimeStamp $1
#	$1 - Variable to assign
#==============================
function System.getTimeStamp()
{
	System.Has 'whoami' || Logger.fatal "Could not find the 'whoami' utility on your system."
	__SYSTEM_TIME=`date +'[%Y.%m.%d ~ %H:%M:%S]'`;
	eval "$1='$__SYSTEM_TIME'";
}

#==============================
# Usage: System.obtainRootPrivs
#==============================
function System.obtainRootPrivs()
{
	Logger.note "Asking for root priviledges";
	sudo -p "Please Enter Password: " sleep 0 || \
		Logger.fatal "Failed to obtain root priviledges.";
}

#==================================
# Usage: System.pushd $1
#	$1 - directory to push into
#==================================
function System.pushd()
{
	Filesystem.checkDir "$1";
	pushd "$1" &> /dev/null;
}

#===================
# Usage: System.popd
#===================
function System.popd()
{
	popd &> /dev/null || \
		Logger.fatal "Failed to pop up from prior System.pushd()";
}

#=============================================
# Usage: System.Has
#	$1 - Utility to check for in the PATH
#=============================================
function System.Has()
{
	if [ -z "$1" ]; then
		Logger.error "Cannot check if the utility exists: no name given."
		return 1
	fi

	which "$1" >/dev/null 2>&1
	if [ "$?" -ne 0 ]; then
		return 1
	fi

	return 0
}

#==================================================================
# Usage: System.getCurrentUser
#	$1 - Variable to store the current username in
#==================================================================
function System.getCurrentUser
{
	__SYSTEM_CURUSER=`whoami`
	eval "$1='$__SYSTEM_CURUSER'"
}

#==================================================================
# Usage: System.getHomeDir
#	$1 - Variable to store the current user's home directory in
#==================================================================
function System.getHomeDir
{
	__SYSTEM_HOMEDIR=`echo ~`
	eval "$1='$__SYSTEM_HOMEDIR'"
}

#Call the constructor
bash.io.Filesystem;

