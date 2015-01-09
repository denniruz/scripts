#i!/bin/bash

include bash.utils.System;
include bash.io.Filesystem;
include bash.helpers.Helper;

function bash.utils.Logger()
{
	__LOGGER_VERBOSE=0
	__LOGGER_DEBUG=0

	__LOGGER_SILENCE_NONE=0
	__LOGGER_SILENCE_CONSOLE=1
	__LOGGER_SILENCE_FILE=2
	__LOGGER_SILENCE_ALL=3

	__LOGGER_FORMAT_NORMAL=0
	__LOGGER_FORMAT_DETAILED=1

	__LOGGER_SILENCE=${__LOGGER_SILENCE_NONE}
	__LOGGER_FORMAT=${__LOGGER_FORMAT_NORMAL}

	Logger.setLogFile "/tmp/log4b.log";
	Logger.setSmartIndent false;
	Logger.setLogFormat default;
	System.Has "column" || Logger.fatal "The 'column' utility was not found on your system. Please update your environment."
	System.Has "awk" || Logger.fatal "The 'awk' utility was not found on your system. Please update your environment."

}

#===================================================
# Usage: Logger.setLogFormat $1
#	$1 - type of logging prefixes (optional)
#		Values: "detailed", "normal"
#		Default: "normal"
#===================================================
function Logger.setLogFormat()
{

	__LOGGER_FORMAT_PARAM=`echo $1 | tr '[:lower:]' '[:upper:]'`

	case "${__LOGGER_FORMAT_PARAM}" in
		"NORMAL")	__LOGGER_FORMAT=${__LOGGER_FORMAT_NORMAL};
			;;
		"DETAILED")	__LOGGER_FORMAT=${__LOGGER_FORMAT_DETAILED};
			;;
		*)			__LOGGER_FORMAT=${__LOGGER_FORMAT_NORMAL};
			;;
	esac
}


#===================================================
# Usage: Logger.silence $1
#	$1 - type of silence (optional)
#		Values: "all", "none", "file", "console"
#		Default: "all"
#===================================================
function Logger.silence()
{
	__LOGGER_SILENCE_PARAM=`echo $1 | tr '[:lower:]' '[:upper:]'`

	case "${__LOGGER_SILENCE_PARAM}" in
		"NONE")		__LOGGER_SILENCE=${__LOGGER_SILENCE_NONE};
			;;
		"CONSOLE")	__LOGGER_SILENCE=${__LOGGER_SILENCE_CONSOLE};
			;;
		"FILE")		__LOGGER_SILENCE=${__LOGGER_SILENCE_FILE};
			;;
		"ALL")		__LOGGER_SILENCE=${__LOGGER_SILENCE_ALL};
			;;
		*)			__LOGGER_SILENCE=${__LOGGER_SILENCE_ALL};
			;;
	esac
}


#=====================================
# Usage: Logger.debug $1
#	$1 - text to be written to log
#=====================================
function Logger.debug()
{
	if [ "${__LOGGER_DEBUG}" -ne 0 ]; then
		Logger.write debug "$1"
	fi
}

#=====================================
# Usage: Logger.verbose $1
#	$1 - text to be written to log
#=====================================
function Logger.verbose()
{
	if [ "${__LOGGER_VERBOSE}" -ne 0 ]; then
		Logger.write note "$1"
	fi
}

#==================================
# Usage: Logger.note $1
#	$1 - Text to be written to log
#===================================
function Logger.note()
{
	Logger.write note "$1";
}

#=====================================
# Usage: Logger.error $1
#	$1 - text to be written to log
#=====================================
function Logger.error()
{
	Logger.write error "Error: $1";
}

#=====================================
# Usage: Logger.fatal $1
#	$1 - text to be written to log
#=====================================
function Logger.fatal()
{
	#A,B,A,C,A,B,B
	Logger.write fatal "Fatal Error: $1";
	exit 1;
}

#=====================================
# Usage: Logger.warn $1
#	$1 - text to be written to log
#=====================================
function Logger.warn()
{
	Logger.write warn "Warning: $1";
}

#=====================================
# Usage: Logger.write $1 $2
#	$1 - tag
#	$2 - text to be written to log
#=====================================
function Logger.write()
{
	#if global silence is on, return immediately
	if [ "${__LOGGER_SILENCE}" -eq "${__LOGGER_SILENCE_ALL}" ]; then
		return 0;
	fi

	#Format tag
	__LOGGER_TAG="[`echo $1 | tr '[:lower:]' '[:upper:]'`]";

	#Get system timestamp
	System.getTimeStamp __LOGGER_STAMP;

	#Smart indenting
	if [ "$__LOGGER_SMART_INDENT" = true ]; then
		Logger.indentManager;
	fi

	#Set visual aid (elucidate warn/error)
	case "$__LOGGER_TAG" in
		'NOTE')
			__LOGGER_TAG="[$__LOGGER_TAG]";
			;;
		'WARN')
			__LOGGER_TAG="[$__LOGGER_TAG]*";
			;;
		'ERROR')
			__LOGGER_TAG="[$__LOGGER_TAG]!";
			;;
		'FATAL')
			__LOGGER_TAG="[$__LOGGER_TAG]#";
			;;
		'DEBUG')
			__LOGGER_TAB="[$__LOGGER_TAG]%";
			;;
		*)
			__LOGGER_TAG="[$__LOGGER_TAG] ";
			;;
	esac

	__LOGGER_CALLER="`caller 1 | awk '{print $2}'`():";

	#Check for silence level
	if [ "${__LOGGER_SILENCE}" -ne "${__LOGGER_SILENCE_CONSOLE}" ] && \
	   [ "${__LOGGER_SILENCE}" -ne "${__LOGGER_SILENCE_ALL}" ]; then
		#stdout
		if [ ${__LOGGER_FORMAT} -eq ${__LOGGER_FORMAT_DETAILED} ]; then
			printf "${__LOGGER_TAB}${__LOGGER_CALLER} ${__LOGGER_TAG} $2\n";
		else
			printf "${__LOGGER_TAB}$2\n";
		fi
	fi

	#Check for silence level
	if [ "${__LOGGER_SILENCE}" -ne "${__LOGGER_SILENCE_FILE}" ] && \
	   [ "${__LOGGER_SILENCE}" -ne "${__LOGGER_SILENCE_ALL}" ]; then
		#logfile
		echo "$__LOGGER_STAMP $__LOGGER_TAG $__LOGGER_TAB$__LOGGER_CALLER $2" >>"$__LOGGER_FILE" 2>/dev/null;

		if [ "$?" -ne 0 ]; then
			echo "Unable to write log messages to '$__LOGGER_FILE.'"
		fi
	fi

	return 0;
}

#=========================================
# Usage Logger.setLogFile $1
#	$1 - redirect logging to this file
#=========================================
function Logger.setLogFile()
{
	__LOGGER_FILE=$1;
}

#==============================================
# Usage Logger.getLogFile $1
#	$1 - variable to store the filename in
#==============================================
function Logger.getLogFile()
{
	eval "$1='$__LOGGER_FILE'";
}

#===============================
# Usage: Logger.smartIndent $1
#	$1 - true/false = on/off
#===============================
function Logger.setSmartIndent()
{
	__LOGGER_SMART_INDENT=$1;
}

function Logger.indentManager()
{
	__LOGGER_STACK=0;

	local __LOGGER_CALLER='default';
	local __LOGGER_INDENT=2
	__LOGGER_TAB=" "

	if [ -n "$__BF_HAS_MAIN" ]; then
		__LOGGER_INDENT=`expr $__LOGGER_INDENT + 1`
	fi

	if [ -n "$__BF_HAS_DISPATCH" ]; then
		__LOGGER_INDENT=`expr $__LOGGER_INDENT + 1`
	fi

	if [ -n "$__BF_HAS_ROUTER" ]; then
		__LOGGER_INDENT=`expr $__LOGGER_INDENT + 2`
	fi

	while [ -n "$__LOGGER_CALLER" ]; do
		__LOGGER_CALLER=`caller $__LOGGER_STACK | awk '{print $2}'`;
		#echo "$__LOGGER_CALLER"
		__LOGGER_STACK=`expr $__LOGGER_STACK + 1`;
	done

	__LOGGER_TAB='';

	for((i=1; i<=`expr $__LOGGER_STACK - $__LOGGER_INDENT`; i++))
	do
		__LOGGER_TAB="$__LOGGER_TAB  ";
	done
}

#=============================
# Usage: Logger.setVerboseMode $1
#	$1 - Turns verbose messaging On/Off (optional)
#		Values: true/false, on/off
#		Default: on
#=============================
function Logger.setVerboseMode
{
	if [ -z "$1" ]; then
		__LOGGER_VERBOSE=1
	fi

	__LOGGER_VERBOSE_PARAM=`echo $1 | tr '[:lower:]' '[:upper:]'`

	case "${__LOGGER_VERBOSE_PARAM}" in
		'FALSE')	__LOGGER_VERBOSE=0;
			;;
		'OFF')		__LOGGER_VERBOSE=0;
			;;
		'0')		__LOGGER_VERBOSE=0;
			;;
		*)			__LOGGER_VERBOSE=1;
			;;
	esac
}

#=============================
# Usage: Logger.getVerboseMode $1
#	$1 - Variable to hold the verbose mode
#=============================
function Logger.getVerboseMode
{
	eval "$1='${__LOGGER_VERBOSE}'"
}

#=============================
# Usage: Logger.getDebugMode $1
#	$1 - Variable to hold the debug mode
#=============================
function Logger.getDebugMode
{
	eval "$1='${__LOGGER_DEBUG}'"
}

#=============================
# Usage: Logger.setDebugMode $1
#	$1 - Turns debug messages On/Off (optional)
#		Values: true/false, on/off
#		Default: on
#=============================
function Logger.setDebugMode
{
	if [ -z "$1" ]; then
		__LOGGER_DEBUG=1
	fi

	__LOGGER_DEBUG_PARAM=`echo $1 | tr '[:lower:]' '[:upper:]'`

	case "${__LOGGER_DEBUG_PARAM}" in
		'FALSE')	__LOGGER_DEBUG=0;
			;;
		'OFF')		__LOGGER_DEBUG=0;
			;;
		'0')		__LOGGER_DEBUG=0;
			;;
		*)			__LOGGER_DEBUG=1;
			;;
	esac
}


#Call the constructor
bash.utils.Logger
