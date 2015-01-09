#!/bin/bash
#**************************************************
# bash.net.Http - a wrapper around various HTTP
#		 utilities available in bash
#**************************************************

include bash.io.Filesystem;
include bash.utils.Logger;

function bash.net.Http()
{
	__HTTP_WGET="wget"
	System.Has $__HTTP_WGET || Logger.fatal "Could not find the '$__HTTP_WGET' utility on your system."

	__HTTP_WGET_PARAMS=""
	__HTTP_COOKIE_FILE="/tmp/bash.net.Http.cookies.$$"
	__HTTP_POST_DATA_FILE="/tmp/bash.net.Http.POST.data.$$"

	#Cookies are enabled by default
	Http.enableCookies

	#Get current user's home directory
	System.getHomeDir __HTTP_USER_HOMEDIR

	__HTTP_WGET_LOG="/tmp/wget.$$.log"
	__HTTP_CONNECT_LOG="/tmp/bash.net.Http.connect.$$.log"

	#Set the location of wgetrc
	__HTTP_WGETRC="$__HTTP_USER_HOMEDIR/.wgetrc"

	#Users must either supply an URL
	__HTTP_URL=""
	#or
	#Supply URL details, such as host and port
	__HTTP_PREFIX="http://"
	__HTTP_HOST=""
	__HTTP_PORT="80"
	__HTTP_PAGE="/"

	#Data will either be appended to the GET request, or POST'ed, depending on the request method
	__HTTP_DATA=""
	__HTTP_DATA_FILE=""

	#Saving options
	__HTTP_FILE_NAME=""
	__HTTP_DOWNLOAD_DIR=""

	#Other HTTP options
	__HTTP_METHOD="GET"

	#Block identifiers for data stored in .wgetrc
	__HTTP_WGET_BLOCK_AUTH="AUTH"

	#Clear any temporary files
	Http.cleanup
}

#===========================================
# Usage: Http.getHost
#	$1 - Variable to hold the Host value
#===========================================
function Http.getHost()
{
	eval "$1='$__HTTP_HOST'"
}


#===========================================
# Usage: Http.getPort
#	$1 - Variable to hold the Port value
#===========================================
function Http.getPort()
{
	eval "$1='$__HTTP_PORT'"
}


#===========================================
# Usage: Http.getPrefix
#	$1 - Variable to hold the Prefix value
#===========================================
function Http.getPrefix()
{
	eval "$1='$__HTTP_PREFIX'"
}

#===========================================
# Usage: Http.getPage
#	$1 - Variable to hold the Page value
#===========================================
function Http.getPrefix()
{
	eval "$1='$__HTTP_PAGE'"
}


#===========================================
# Usage: Http.getMethod
#	$1 - Variable to hold the Method value
#===========================================
function Http.getMethod()
{
	eval "$1='$__HTTP_METHOD'"
}

#===========================================
# Usage: Http.getData
#	$1 - Variable to hold the Data value
#===========================================
function Http.getData()
{
	eval "$1='$__HTTP_DATA'"
}

#===========================================
# Usage: Http.getDataFile
#	$1 - Variable to hold the DataFile value
#===========================================
function Http.getDataFile()
{
	eval "$1='$__HTTP_DATA_FILE'"
}

#===========================================
# Usage: Http.setHost
#	$1 - Hostname
#===========================================
function Http.setHost()
{
	__HTTP_HOST="$1"
}

#===========================================
# Usage: Http.setPort
#	$1 - Port number (default: 80)
#===========================================
function Http.setPort()
{
	if [ -z "$1" ]; then
		__HTTP_PORT="80"
	else
		__HTTP_PORT="$1"
	fi
}

#===========================================
# Usage: Http.setProxyHost
#	$1 - Host:Port (Example: localhost:8080)
#===========================================
function Http.setProxy()
{
	export http_proxy="$1"
}

#===========================================
# Usage: Http.disableProxy
#	$1 - Proxy Port
#===========================================
function Http.disableProxy()
{
	__HTTP_USE_PROXY="--no-proxy"
}

#===========================================
# Usage: Http.disableProxy
#	$1 - Proxy Port
#===========================================
function Http.enableProxy()
{
	__HTTP_USE_PROXY=""
}

#===========================================
# Usage: Http.setPrefix
#	$1 - Prefix (default: http://)
#===========================================
function Http.setPrefix()
{
	if [ -z "$1" ]; then
		__HTTP_PREFIX="http://"
	else
		__HTTP_PREFIX="$1"
	fi
}

#===========================================
# Usage: Http.setPage
#	$1 - Page URL, relative to the host
#		(default: /)
#===========================================
function Http.setPage()
{
	if [ -z "$__HTTP_PAGE" ]; then
		__HTTP_PAGE="/"
	else
		__HTTP_PAGE="$1"
	fi
}


#===========================================
# Usage: Http.setMethod
#	$1 - HTTP method (default: GET)
#		values: POST, post, Post
#		all others result in GET
#===========================================
function Http.setMethod()
{
	if [ "$1" = "POST" ] || [ "$1" = "post" ] || [ "$1" = "Post" ]; then
		__HTTP_METHOD="POST"
	else
		__HTTP_METHOD="GET"
	fi
}

#===========================================
# Usage: Http.setData
#	$1 - Data to be sent via POST or GET
#===========================================
function Http.setData()
{
	__HTTP_DATA="$1"
}

#===========================================
# Usage: Http.setDataFile
#	$1 - Path to the file containing POST data
#	This will only be used with POST
#===========================================
function Http.setDataFile()
{
	Filesystem.checkFile "$1" || Logger.fatal "File '$1' does not exist or is not accessible."
	__HTTP_DATA_FILE="$1"
	Http.setWGetParam "--post-file=$1"
}

#===========================
# Usage: Http.setWGetParam
#	$1 - Parameter=Value
#		OR
#	$1 - Parameter
#	$2 - Value
#===========================
function Http.setWGetParam()
{
	__HTTP_WGET_PARAMS="$__HTTP_WGET_PARAMS $1 $2"
}

#===========================
# Usage: Http.logHeaders
#	$1 - true or empty to enable, false to disable
#===========================
function Http.logHeaders()
{
	__HTTP_LOG_HEADERS="-d -S";

	if [ "$1" = "false" ] || [ "$1" = "False" ] || [ "$1" = "FALSE" ]; then
		__HTTP_LOG_HEADERS="";
	fi
}

#===========================
# Usage: Http.setDownloadLog
#	$1 - File path
#===========================
function Http.setDownloadLog()
{
	if [ -z "$1" ]; then
		return 0
	fi

	__HTTP_WGET_LOG="$1"
}

#==============================================
# Usage: Http.getDownloadLog
#	$1 - Variable to store the file path in
#==============================================
function Http.getDownloadLog()
{
	eval "$1='$__HTTP_WGET_LOG'"
}

#===========================
# Usage: Http.enableCookies
#===========================
function Http.enableCookies()
{
	__HTTP_COOKIES="--keep-session-cookies --save-cookies=$__HTTP_COOKIE_FILE"

	if [ -f "$__HTTP_COOKIE_FILE" ]; then
		__HTTP_COOKIES="$__HTTP_COOKIES --load-cookies $__HTTP_COOKIE_FILE"
	fi
}

#===========================
# Usage: Http.disableCookies
#===========================
function Http.disableCookies()
{
	__HTTP_COOKIES="--no-cookies"
}

#===========================
# Usage: Http.addHeader
#		$1 - Raw header parameter and value
#===========================
function Http.addHeader
{
	if [ -n "$1" ]; then
		Http.setWGetParam "--header \"$1\""
	fi
}

#===========================
# Usage: Http.disableCache
#===========================
function Http.disableCache()
{
	Http.addHeader "Cache-Control: no-cache"
	Http.addHeader "Pragma: no-cache"
}


#=============================================
# Usage: Http.getCookie
#	$1 - Cookie name
#	$2 - Variable to store the cookie into
#=============================================
function Http.getCookie()
{
	if [ -z "$1" ]; then
		Logger.error "You must specify a name for the Cookie you'd like to retrieve."
		return 1
	fi

	if [ -z "$2" ]; then
		Logger.error "You must specify a variable to store the Cookie into."
		return 1
	fi

	__HTTP_TEMP_COOKIE_VALUE=`cat "$__HTTP_COOKIE_FILE" | grep -i "$1" | head -1 | sed -r -e 's@^.+\s([[:alnum:]]+)$@\1@gi'`
	eval "$2='$__HTTP_TEMP_COOKIE_VALUE'"
}

#===========================
# Usage: Http.setCookie
#	$1 - Cookie name
#	$2 - Cookie value
#===========================
function Http.setCookie
{
	if [ -z "$1" ]; then
		Logger.warn "You must specify a name for the Cookie you'd like to send."
		return 1
	fi
	if [ -z "$2" ]; then
		Logger.warn "You must specify a value for the Cookie you'd like to send."
		return 1
	fi

	Http.disableCookies
	Http.setWGetParam "--header \"Cookie: $1=$2\""
}

#===========================
# Usage: Http.setURL
#	$1 - URL
#===========================
function Http.setURL
{
	__HTTP_URL="$1"
}

#=============================================
# Usage: Http.unsetURL
# Same as calling Http.setURL with no parameters.
#=============================================
function unsetURL
{
	Http.setURL
}

#==============================
# Usage: Http.getURL
#	$1 - Variable to store the URL into
#==============================
function Http.getURL
{
	if [ -z "$__HTTP_URL" ] && [ -n "$__HTTP_HOST" ]; then
		Http.buildURL
	fi
	eval "$1='$__HTTP_URL'"
}

#==============================
# Usage: Http.buildURL
#==============================
function Http.buildURL
{
	if [ "$__HTTP_METHOD" = "GET" ]; then
		__HTTP_URL="${__HTTP_PREFIX}${__HTTP_HOST}:${__HTTP_PORT}${__HTTP_PAGE}?${__HTTP_DATA}"
	else
		__HTTP_URL="${__HTTP_PREFIX}${__HTTP_HOST}:${__HTTP_PORT}${__HTTP_PAGE}"
	fi
}

#==============================
# Usage: Http.connect
#		$1 - URL to connect to (optional)
#==============================
function Http.connect()
{
	if [ -n "$1" ]; then
		Http.setURL "$1"
	fi

	#Do not store the downloaded file
	Http.setWGetParam -O "/dev/null"
	Http.download
}

#================================
# Usage: Http.setFileName
#	$1 - name or path to the file
#================================
function Http.setFileName
{
	if [ -n "$1" ]; then
		Logger.debug "Setting filename to '$1'"
		__HTTP_FILE_NAME="$1"
	else
		Logger.debug "Clearing the filename."
		__HTTP_FILE_NAME=""
	fi
}

#==============================
# Usage: Http.setDownloadDir
#	$1  - Download directory
#==============================
function Http.setDownloadDir
{
	if [ -n "$1" ]; then
		Logger.verbose "Checking directory '$1'..."
		Filesystem.checkDir "$1"
		__HTTP_DOWNLOAD_DIR="$1"
	else
		__HTTP_DOWNLOAD_DIR="."
	fi
}

#===========================
# Usage: Http.download
#	$1 - Local filename (optional)
#===========================
function Http.download()
{
	local __HTTP_DL_URL
	local __BF_GLOBAL_LOGFILE=""

	#Build the URL if there is no value assigned to it
	Http.getURL __HTTP_DL_URL

	#Check to see if the URL was set
	if [ -z "$__HTTP_DL_URL" ]; then
		Logger.error "Cannot download file: No URL was given."
		return 1
	fi

	#Check to see if there is a filename given
	if [ -n "$1" ]; then
		__HTTP_FILE_NAME="$1"
	fi

	Logger.getLogFile __HTTP_BF_LOGFILE

	#Deleting previously recorded contents from ~/.wgetrc
	sed -i -e '/#++BF/,/#--BF/d' $__HTTP_WGETRC

	#Check to see if there is a need to store the username and password
	if [ -n "$__HTTP_USERNAME" ]; then
		#Securing username and password
		chmod 700 $__HTTP_WGETRC

		#Write the data into .wgetrc
		echo >> $__HTTP_WGETRC
		echo "#++BF" >> $__HTTP_WGETRC
		echo "http_user=$__HTTP_USERNAME" >> $__HTTP_WGETRC
		echo "http_password=$__HTTP_PASSWORD" >> $__HTTP_WGETRC
		echo "#--BF" >> $__HTTP_WGETRC
	fi

	#Set the download file path
	if [ -n "${__HTTP_FILE_NAME}" ]; then
		Http.setWGetParam -O "${__HTTP_FILE_NAME}"
	else
		#Set the download directory and leave the default filename intact
		if [ -n "${__HTTP_DOWNLOAD_DIR}" ]; then
			setWGetParam -P "${__HTTP_DOWNLOAD_DIR}"
		fi
	fi

	if [ "$__HTTP_METHOD" = "GET" ]; then
		Logger.verbose "Requesting file from '$__HTTP_DL_URL'";
		__HTTP_WGET_COMMAND="$__HTTP_WGET $__HTTP_LOG_HEADERS $__HTTP_COOKIES -o \"$__HTTP_CONNECT_LOG\" $__HTTP_USE_PROXY $__HTTP_WGET_PARAMS \"$__HTTP_DL_URL\" >>$__HTTP_BF_LOGFILE 2>&1"
		echo "WGET Command: '${__HTTP_WGET_COMMAND}'" >>/tmp/command.txt
		eval "${__HTTP_WGET_COMMAND}"
	else
		Logger.verbose "Posting data to '$__HTTP_DL_URL'";
		#Post the request
		__HTTP_WGET_COMMAND="$__HTTP_WGET $__HTTP_LOG_HEADERS $__HTTP_COOKIES -o \"$__HTTP_CONNECT_LOG\" $__HTTP_USE_PROXY $__HTTP_WGET_PARAMS \"$__HTTP_DL_URL\" >>$__HTTP_BF_LOGFILE 2>&1"
		echo "WGET Command: '${__HTTP_WGET_COMMAND}'" >>/tmp/command.txt
		#Run the command
		eval "${__HTTP_WGET_COMMAND}"
	fi

	if [ "$?" -ne 0 ]; then
		Logger.error "Could not download from URL '$__HTTP_DL_URL'"
		return 1
	fi

	#Check the log file for the server response
	Http.getResponse __HTTP_RESPONSE_CODE __HTTP_RESPONSE_MESSAGE
	RET="$?"
	if [ "$RET" -eq "0" ]; then
		Logger.verbose "Request succeeded."
	else
		Logger.fatal "Request failed! (Server said: '$__HTTP_RESPONSE_CODE $__HTTP_RESPONSE_MESSAGE'"
	fi

	if [ -n "$1" ]; then
		Logger.verbose "Download complete. File saved as '$1'"
	fi

	return 0
}

#===========================================================================
# Usage: Http.storeAuth
# 	Stores the username and password into .wgetrc, so that they would not
#	be visible as wget parameters when running 'ps'
#===========================================================================
function Http.storeAuth
{
	#Securing username and password
	chmod 700 $__HTTP_WGETRC

	#Write the data into .wgetrc
	echo >> $__HTTP_WGETRC
	echo "#++BF++${__HTTP_WGET_BLOCK_AUTH}" >> $__HTTP_WGETRC
	echo "http_user=$__HTTP_USERNAME" >> $__HTTP_WGETRC
	echo "http_password=$__HTTP_PASSWORD" >> $__HTTP_WGETRC
	echo "#--BF--${__HTTP_WGET_BLOCK_AUTH}" >> $__HTTP_WGETRC
}

#=======================================
# Usage: Http.clearStoredSettings
#	$1 - Name of the block to delete
#		example: "AUTH"
#		default: erase all blocks
#=======================================
function Http.clearStoredSettings
{
	#Deleting previously recorded contents from ~/.wgetrc
	sed -i -e '/#++BF++$1/,/#--BF--$1/d' $__HTTP_WGETRC
}

#===========================
# Usage: Http.checkURL
#	$1 - URL to check (optional)
#===========================
function Http.checkURL()
{
	local __BF_GLOBAL_LOGFILE=""
	local __HTTP_DL_URL=""
	__HTTP_RESPONSE_CODE=""

	if [ -n "$1" ]; then
		Http.setURL "$1"
	fi

	#Build the URL if there is no value assigned to it
	Http.getURL __HTTP_DL_URL

	#Check to see if the URL was set
	if [ -z "$__HTTP_DL_URL" ]; then
		Logger.error "Cannot check URL: No URL was given."
		return 1
	fi

	#Get the Bash Framework's log file
	Logger.getLogFile __HTTP_BF_LOGFILE


	#Check to see if there is a need to store the username and password
	if [ -n "$__HTTP_USERNAME" ]; then
		Http.storeAuth
	fi

	#Send downloaded file to /dev/null
	Http.setWGetParam "-O" "/dev/null"

	#Enable header logging
	Http.logHeaders true

	#Enable WGET log file
	Http.setWGetParam -o "$__HTTP_CONNECT_LOG"

	if [ "$__HTTP_METHOD" = "GET" ]; then
		Logger.verbose "Retrieving URL '$__HTTP_DL_URL'...";
		__HTTP_WGET_COMMAND="$__HTTP_WGET $__HTTP_LOG_HEADERS $__HTTP_COOKIES -o \"$__HTTP_CONNECT_LOG\" $__HTTP_USE_PROXY $__HTTP_WGET_PARAMS \"$__HTTP_DL_URL\" >>$__HTTP_BF_LOGFILE 2>&1"
		#Run the command
		eval "${__HTTP_WGET_COMMAND}"
	else
		Logger.verbose "Posting data to '$__HTTP_DL_URL'";
		__HTTP_WGET_COMMAND="$__HTTP_WGET $__HTTP_LOG_HEADERS $__HTTP_COOKIES -o \"$__HTTP_CONNECT_LOG\" $__HTTP_USE_PROXY $__HTTP_WGET_PARAMS \"$__HTTP_DL_URL\" >>$__HTTP_BF_LOGFILE 2>&1"
		#Run the command
		eval "${__HTTP_WGET_COMMAND}"
	fi

	if [ "$?" -ne 0 ]; then
		Logger.error "Could not request URL: '$__HTTP_DL_URL'"
		return 1
	fi

	#Check the log file for the server response
	Http.getResponse __HTTP_RESPONSE_CODE __HTTP_RESPONSE_MESSAGE
	RET="$?"
	if [ "$RET" -eq "0" ]; then
		Logger.verbose "Request succeeded."
	else
		Logger.error "Request failed! (Server said: '$__HTTP_RESPONSE_CODE $__HTTP_RESPONSE_MESSAGE'"
	fi

	return $RET
}

#=============================
# Usage: Http.getResponse
#	$1 - Response Code
#	$2 - Response Message
#	$3 - HTTP version
#=============================
function Http.getResponse()
{
	local __HTTP_SERVER_RESPONSE_CODE=""
	local __HTTP_SERVER_RESPONSE_MESSAGE=""
	local __HTTP_SERVER_RESPONSE_VERSION=""

	#Check that the connection log exists
	if ! [ -f "${__HTTP_CONNECT_LOG}" ]; then
		Logger.error "Could not find file '${__HTTP_CONNECT_LOG}.'"
		return 1
	fi

	#Look for the response code
	#search beginning with the end, to get the last response code
	__HTTP_SERVER_RESPONSE_VERSION=`tac ${__HTTP_CONNECT_LOG} | grep 'HTTP/' | sed -r -n 's@^.*HTTP/([0-9]+\.[0-9])\s+([0-9]+)\s+(.+)$@\1@gp' | head -1`
	__HTTP_SERVER_RESPONSE_CODE=`tac ${__HTTP_CONNECT_LOG} | grep 'HTTP/' | sed -r -n 's@^.*HTTP/([0-9]+\.[0-9])\s+([0-9]+)\s+(.+)$@\2@gp' | head -1`
	__HTTP_SERVER_RESPONSE_MESSAGE=`tac ${__HTTP_CONNECT_LOG} | grep 'HTTP/' | sed -r -n 's@^.*HTTP/([0-9]+\.[0-9])\s+([0-9]+)\s+(.+)$@\3@gp' | head -1`

	if [ -z "${__HTTP_SERVER_RESPONSE_CODE}" ]; then
		Logger.fatal "Could not retrieve the response code from file '${__HTTP_CONNECT_LOG}'."
		return 1
	fi

	[ -n "$1" ] && eval "$1='$__HTTP_SERVER_RESPONSE_CODE'"
	[ -n "$2" ] && eval "$2='$__HTTP_SERVER_RESPONSE_MESSAGE'"
	[ -n "$3" ] && eval "$3='$__HTTP_SERVER_RESPONSE_VERSION'"

	if [ "$__HTTP_RESPONSE_CODE" = "200" ]; then
		return 0
	else
		return 1
	fi
}

#===========================
# Usage: Http.setUsername
#	$1 - HTTP Username
#===========================
function Http.setUsername()
{
	__HTTP_USERNAME="$1"
}

#===========================
# Usage: Http.setPassword
#	$1 - HTTP Password
#===========================
function Http.setPassword()
{
	__HTTP_PASSWORD="$1"
}

#=========================================
# Usage: Http.reset
#		Resets the connection parameters
#=========================================
function Http.reset()
{
	#Call the Http constructor to reset all preferences
	bash.net.Http
}

#======================================
# Usage: Http.cleanup
#		Cleans all the temporary files
#		Does not report any errors
#======================================
function Http.cleanup()
{
	if [ -f "$__HTTP_COOKIE_FILE" ]; then
		rm -f "$__HTTP_COOKIE_FILE" >/dev/null 2>&1
	fi
	if [ -f "$__HTTP_POST_DATA_FILE" ]; then
		rm -f "$__HTTP_POST_DATA_FILE" >/dev/null 2>&1
	fi
	if [ -f "$__HTTP_CONNECT_LOG" ]; then
		rm -f "$__HTTP_CONNECT_LOG" >/dev/null 2>&1
	fi
}

#Call the constructor
bash.net.Http;