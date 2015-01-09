#!/bin/bash

include bash.utils.System;
include bash.utils.Logger;

function bash.Framework
{
	Logger.setSmartIndent true;
	Logger.setVerboseMode off
	Logger.setDebugMode off
}

#===============================================================================
# Usage: routeArguments $@
#
# Description:
#	routeArguments performs the following actions on the script's arguments:
#	1. Checks if the argument is an equality e.g. -a=b, --a=b or a=b
#		a. If so, expresses 'A=b' before the script is run
#	2. Checks if the argument is a flag e.g. -c, --c or c
#		a. If so, calls the respective function cAction()
#
# Return: number of arguments routed
#===============================================================================
function routeArguments()
{
	ARGC=0
	index=0;
	__BF_HAS_ROUTER=1

	local i

	for i in $@; do
		#If there's an argument
		if [ -n $i ]; then
			#Strip dashes
			ARG=`echo $i | sed -e 's@-@@g' 2>/dev/null`
			#Check for key=value pair
			if [ "`echo $ARG | grep '=' | wc -l`" == "1"  ]; then
				VAR=`echo $ARG | cut -d '=' -f 1 | tr '[:lower:]' '[:upper:]'`
				VALUE=`echo $ARG | cut -d '=' -f 2`
				if [ -n "$VAR" ] && [ -n "$VALUE" ]; then
					eval "$VAR='$VALUE'"
				fi
			else
				#Create actions array.
				actions[$index]="${ARG}Action";
				index=`expr $index + 1`;
				ARGC=`expr $ARGC + 1`
			fi
		fi
	done

	#Call Actions
	nActions=${#actions[@]};

	#Show Help by default
	if [ "$nActions" -eq 0 ]; then
		helpAction
	else
		#Execute onActionStart event
		if [ "`declare -f | egrep '^bash.onActionStart \(\)' | wc -l`" -gt 0 ]; then
			__BF_HAS_MAIN=1
			bash.onActionStart $ARGV $__BF_LIB;
		fi

		#Call all the actions
		#This function is using 'shift' to avoid using a variable such as $i, which
		#caused problems due to scoping
		bash.executeActions "${actions[@]}"

		#Execute onActionEnd event
		if [ "`declare -f | egrep '^bash.onActionEnd \(\)' | wc -l`" -gt 0 ]; then
			__BF_HAS_MAIN=1
			bash.onActionEnd $ARGV $__BF_LIB;
		fi

	fi

	return $ARGC
}

#=========================================================
# Usage: bash.executeActions "action1 action2 ... actionN"
#=========================================================
function bash.executeActions()
{
	while (( "$#" )); do
		eval "$1"
		shift
	done
}

#===========================================
# Usage: Option <Name> [Value]
#===========================================
function Option()
{
	if [ -n "$1" ]; then
		eval "$1='$2'"
	fi
}

#===========================================
# Usage: -verbose
#	Will enable verbose message output
#===========================================
function verboseAction()
{
	Logger.setVerboseMode on
}

#===========================================
# Usage: -debug
#	Will enable debug message output
#===========================================
function debugAction()
{
	Logger.setDebugMode on
}

#===========================================
# Usage: showHelp
#
# Description:
#	This function displays help messages
#===========================================
function helpAction()
{
	if [ -n "$TITLE" ]; then
		echo "$TITLE"
		echo
	fi
	echo "Usage: $0 [options] [actions]"
	echo
	echo "Options:"
	cat "$0" | egrep '^Option' | grep -iv 'TITLE' | sed -r -e 's@^Option\s+((\w|\d)+)(\s.*)?$@\t--\1=@g' | tr '[[:upper:]]' '[[:lower:]]'
	echo
	echo "Actions:"
	cat "$0" | grep -B 1 '^function' | grep -B 1 'Action()' | grep -v '\-\-' | sed -e '1!G;h;$!d' | perl -n -e 'if ($_ =~ s/^function (.+)Action\(\)/-$1/) { chomp($_);print "\t$_";} else {printf("%20s","$_");}' | perl -p -e 's/\#(.+)$/\t$1/'
	echo "	-verbose	Enable verbose output"
	echo "	-debug		Enable debug output"
	echo "	-help		Display the help page"
	echo
}

#BF cleaning event
function bash.onClean()
{
	Logger.verbose "Cleaning up..."
	if [ -f "${__BF_LOCK_FILE}" ]; then
		Filesystem.delete "${__BF_LOCK_FILE}"
	fi
}

#Call the constructor
bash.Framework;
