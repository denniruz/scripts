#!/bin/bash

include bash.io.Filesystem;

#===================================
# Usage: upper $1
#	$1 - var to switch uppercase
#===================================
function upper()
{
	eval "$1=\"`echo $1 | tr '[:lower:]' '[:upper:]'`\"" || Logger.fatal "Helper upper() failed.";
	return 0;
}

#===================================
# Usage: lower $1
#	$1 - var to switch lowercase
#===================================
function lower()
{
	eval "$1=\"`echo $1 | tr '[:upper:]' '[:lower:]'`\"" || Logger.fatal "Helper lower() failed.";
	return 0;
}

#===================================
# Usage: removeEmptyLines $1
#	$1 - var to switch lowercase
#	$2 - value of that variable
#===================================
function bash.removeEmptyLines()
{
	if [ -n "$1" ]; then
		local __HELPER_DATA="`echo "$2" | grep -v '^$'`"
		eval "$1='$__HELPER_DATA'" || Logger.fatal "Helper removeEmptyLines() failed.";
	fi

	return 0;
}
