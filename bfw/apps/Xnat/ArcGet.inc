#!/bin/bash
#**************************************************
# Xnat.ArcGet - a replacement of the original
#		ArcGet Utility for XNAT.
#
# This ArcGet tool provides the means to download
# an XNAT Scan Session from an XNAT server. It will
# also provide information on the time it took to
# connect, as well as report any errors along with
# diagnostics data.
#**************************************************

include bash.io.Filesystem;
include bash.net.Http;

function bash.apps.Xnat.ArcGet()
{
	__ARCGET_HOST=""
	#Proxy has the form: "host:port"
	__ARCGET_PROXY=""
	__ARCGET_USERNAME=""
	__ARCGET_PASSWORD=""
	__ARCGET_XMLSEARCH_PAGE="/axis/XMLSearch.jws"
	__ARCGET_SERVICE_SESSION_PAGE="/axis/CreateServiceSession.jws"
	__ARCGET_VELOCITY_SEARCH_PAGE="/axis/VelocitySearch.jws"
	#When initiating the download, need to append the Scan Session ID to the following variable:
	__ARCGET_DOWNLOAD_SCAN_PAGE="/app/template/ArcGet.vm/quality/ALL/id/"
	__ARCGET_COOKIE_NAME="JSESSIONID"
	__ARCGET_COOKIE_VALUE=""
	__ARCGET_SCAN_LIST=""
	__ARCGET_VALID_SCAN_LIST=""
	__ARCGET_SCAN_NAME=""
	__ARCGET_SERVICE_SESSION_DATA='<?xml version="1.0" encoding="UTF-8"?>
									<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
										<soapenv:Body><execute soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"/>
										</soapenv:Body>
									</soapenv:Envelope>'

	__ARCGET_VELOCITY_SEARCH_DATA='<?xml version="1.0" encoding="UTF-8"?>
									<soapenv:Envelope xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
										<soapenv:Body>
											<search soapenv:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/">
												<arg0></arg0>
												<arg1 xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">xnat:mrSessionData.ID</arg1>
												<arg2 xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">=</arg2>
												<arg3 xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">%__ARCGET_SCAN_NAME%</arg3>
												<arg4 xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">xnat:mrSessionData</arg4>
												<arg5 xsi:type="soapenc:string" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/">xnat_mrSessionData_brief.vm</arg5>
											</search>
										</soapenv:Body>
									</soapenv:Envelope>'

	__ARCGET_TEMP_DATA_FILE="/tmp/bash.apps.Xnat.ArcGet.data.$$.temp"
	__ARCGET_TEMP_RESPONSE_FILE="/tmp/bash.apps.Xnat.ArcGet.response.$$.temp"
	__ARCGET_ARCHIVE_EXTENSION=".zip"
	System.Has "egrep" || Logger.fatal "The 'egrep' utility was not found on your system. Please update your environment."
	System.Has "sort" || Logger.fatal "The 'sort' utility was not found on your system. Please update your environment."
	System.Has "uniq" || Logger.fatal "The 'uniq' utility was not found on your system. Please update your environment."
	Xnat.ArcGet.cleanup;
}

#===========================
# Usage: Xnat.ArcGet.setHost
#	$1 - Hostname
#===========================
function Xnat.ArcGet.setHost()
{
	__ARCGET_HOST="$1"
	Http.setHost "$1"
}

#===============================
# Usage: Xnat.ArcGet.setUsername
#	$1 - Hostname
#===============================
function Xnat.ArcGet.setUsername()
{
	__ARCGET_USERNAME="$1"
	Http.setUsername "$1"
}

#===========================================
# Usage: Xnat.ArcGet.getUsername
#	$1 - Variable to store the username into
#===========================================
function Xnat.ArcGet.getUsername()
{
	eval "$1='${__ARCGET_USERNAME}'"
}

#===============================
# Usage: Xnat.ArcGet.setPassword
#	$1 - Password
#===============================
function Xnat.ArcGet.setPassword()
{
	__ARCGET_PASSWORD="$1"
	Http.setPassword "$1"
}

#===============================================
# Usage: Xnat.ArcGet.getHost
#	$1 - Variable to store the Hostname into
#===============================================
function Xnat.ArcGet.getHost()
{
	eval "$1='$__ARCGET_HOST'"
}

#===============================================
# Usage: Xnat.ArcGet.loadFromFile
#	$1 - Path to the sessions file
#===============================================
function Xnat.ArcGet.loadFromFile()
{
	Logger.verbose "Checking if file '$1' exists..."
	Filesystem.checkFile "$1"

	__ARCGET_SCAN_LIST="`cat "$1" | tr ' ' "\n" | sort | uniq`"
}

#===============================================
# Usage: Xnat.ArcGet.setPassFile
#	$1 - Path to the passfile
#		No parameter = clear user/pass
#===============================================
function Xnat.ArcGet.setPassFile()
{
	if [ -n "$1" ]; then
		Filesystem.checkFile "$1" || Logger.fatal "Unable to access the file containing authentication details: '$1'"
		local __ARCGET_USER
		__ARCGET_USER="`cat "$1" | grep 'username=' | head -1 | cut -d '=' -f 2`"
		local __ARCGET_PASS
		__ARCGET_PASS="`cat "$1" | grep 'password=' | head -1 | cut -d '=' -f 2`"
		if [ -n "${__ARCGET_USER}" ]; then
			Xnat.ArcGet.setUsername "${__ARCGET_USER}"
			Xnat.ArcGet.setPassword "${__ARCGET_PASS}"
		else
			Logger.error "Could not find any username/password details in the specified file: '$1'. You may encounter errors."
		fi
	else
		__ARCGET_USERNAME=""
		__ARCGET_PASSWORD=""
	fi
}

#==============================
# Usage: Xnat.ArcGet.addScans
#	$1 - Scan Session ID
#==============================
function Xnat.ArcGet.addScans()
{
	if [ -n "$1" ]; then
		__ARCGET_SCAN_LIST="$ARCGET_SCAN_LIST $1"
	else
		Logger.error "Cannot add an empty Scan Session ID."
		return 1
	fi
}

#==================================
# Usage: Xnat.ArcGet.setDownloadDir
#==================================
function Xnat.ArcGet.setDownloadDir
{
	if [ -n "$1" ]; then
		Filesystem.checkDir "$1" || \
			Logger.fatal "Please specify a valid download directory."
		__ARCGET_DOWNLOAD_DIR="$1"
	fi
}

#==================================
# Usage: Xnat.ArcGet.setFileName
#==================================
function Xnat.ArcGet.setFileName
{
	if [ -n "$1" ]; then
		__ARCGET_FILE_NAME="$1"
	else
		__ARCGET_FILE_NAME=""
	fi
}

#==========================================
# Usage: Xnat.ArcGet.getScans
#	$1 - Variable to hold all scans
#==========================================
function Xnat.ArcGet.getScans
{
	eval "$1='$__ARCGET_SCAN_LIST'"
}

#================================
# Usage: Xnat.ArcGet.checkService
#================================
function Xnat.ArcGet.checkService()
{
	Http.reset
	if [ -z "${__ARCGET_HOST}" ]; then
		Logger.fatal "Please specify the host to connect to."
	fi
	Http.setHost "${__ARCGET_HOST}"
	Http.setPage "${__ARCGET_XMLSEARCH_PAGE}"
	Http.disableCookies
	if [ -n "${__ARCGET_PROXY}" ]; then
		Http.setProxy "${__ARCGET_PROXY}"
		Http.enableProxy
	fi
	Http.checkURL
	RET="$?"
	#Retrieve additional information on error
	if [ "$?" -ne 0 ]; then
		local __ARCGET_RESPONSE_CODE
		local __ARCGET_REPONSE_MESSAGE

		#Obtain the server response code and message
		Http.getResponse __ARCGET_RESPONSE_CODE __ARCGET_RESPONSE_MESSAGE

		Logger.error "Could not retrieve the URL. (Server said: '${__ARCGET_RESPONSE_CODE} ${__ARCGET_RESPONSE_MESSAGE}')"

		return 1
	fi

	Http.cleanup

	return "$RET"
}

#========================================
# Usage: Xnat.ArcGet.setServiceSessionId
#	$1 - Service Session ID
#========================================
function Xnat.ArcGet.setServiceSessionId()
{
	Logger.verbose "Setting the Service Session ID to '$1'"
	__ARCGET_COOKIE_VALUE="$1"
}

function Xnat.ArcGet.createServiceSession()
#================================================
# Usage: Xnat.ArcGet.createServiceSession
#	$1 - Variable to hold the Service Session ID
#			returned by the server
#================================================
{
	local __ARCGET_COOKIE_VALUE
	local __BF_GLOBAL_LOG
	Logger.getLogFile __BF_GLOBAL_LOG

	Http.reset
	if [ -z "$__ARCGET_HOST" ]; then
		Logger.fatal "Please specify the host to connect to."
	fi
	Http.setHost "$__ARCGET_HOST"
	Http.setMethod "POST"
	Http.setData "$__ARCGET_SERVICE_SESSION_DATA"
	if [ -n "$__ARCGET_USERNAME" ]; then
		Http.setUsername "$__ARCGET_USERNAME"
		Http.setPassword "$__ARCGET_PASSWORD"
	fi
	Http.setPage "${__ARCGET_SERVICE_SESSION_PAGE}"
	Http.addHeader 'SOAPAction:\"\"'
	Http.enableCookies
	Http.disableCache
	if [ -n "${__ARCGET_PROXY}" ]; then
		Http.setProxy "${__ARCGET_PROXY}"
		Http.enableProxy
	fi

	#Save data to a temporary file
	echo "$__ARCGET_SERVICE_SESSION_DATA" >"$__ARCGET_TEMP_DATA_FILE" 2>"$__BF_GLOBAL_LOG"
	if [ "$?" -ne 0 ]; then
		Logger.fatal "Could not save POST data to '${__ARCGET_TEMP_DATA_FILE}'."
	fi

	Http.setDataFile "${__ARCGET_TEMP_DATA_FILE}"
	Http.connect
	if [ "$?" -ne 0 ]; then
		local __ARCGET_RESPONSE_CODE
		local __ARCGET_REPONSE_MESSAGE

		#Obtain the server response code and message
		Http.getResponse __ARCGET_RESPONSE_CODE __ARCGET_RESPONSE_MESSAGE

		Logger.verbose "The Server returned the following response: '${__ARCGET_RESPONSE_CODE} ${__ARCGET_RESPONSE_MESSAGE}')"
		if [ "${__ARCGET_RESPONSE_CODE}" -eq 500 ]; then
			Logger.note "Please check your username and password."
		fi

		return 1
	fi

	#Try to retrieve the special cookie
	Http.getCookie "${__ARCGET_COOKIE_NAME}" __ARCGET_COOKIE_VALUE
	if [ -z "${__ARCGET_COOKIE_VALUE}" ]; then
		Logger.fatal "Could not find the '${__ARCGET_COOKIE_NAME}' cookie. Please examine the file '${__HTTP_CONNECT_LOG}'."
		return 1
	fi

	#Return the cookie, if the user desired so
	if [ -n "$1" ]; then
		eval "$1='${__ARCGET_COOKIE_VALUE}'"
	fi

	#Return 0 to indicate the successful acquisition of the service session id
	return 0
}

#==============================================
# Usage: Xnat.ArcGet.checkScans
#==============================================
function Xnat.ArcGet.checkScans()
{
	local __BF_GLOBAL_LOG
	local __ARCGET_NOT_FOUND
	local __ARCGET_COUNT
	local __ARCGET_VERBOSE_MODE
	#Number of scan sessions not found
	__ARCGET_NOT_FOUND=0
	#Number of loops
	__ARCGET_COUNT=0

	Logger.getVerboseMode __ARCGET_VERBOSE_MODE

	Logger.getLogFile __BF_GLOBAL_LOG

	Http.reset
	if [ -z "$__ARCGET_HOST" ]; then
		Logger.fatal "Please specify the host to connect to."
	fi

	#Set connection parameters
	Http.setMethod "POST"
	Http.setHost "$__ARCGET_HOST"
	Http.setPage "${__ARCGET_VELOCITY_SEARCH_PAGE}"
	Http.disableCache
	if [ -n "${__ARCGET_PROXY}" ]; then
		Http.setProxy "${__ARCGET_PROXY}"
		Http.enableProxy
	fi
	Http.enableCookies
	Http.setCookie "$__ARCGET_COOKIE_NAME" "$__ARCGET_COOKIE_VALUE"
	Http.addHeader 'SOAPAction:\"\"'

	Http.setData "$__ARCGET_VELOCITY_SEARCH_DATA"
	if [ -n "${__ARCGET_USERNAME}" ]; then
		Http.setUsername "${__ARCGET_USERNAME}"
		Http.setPassword "${__ARCGET_PASSWORD}"
	fi

	#Check if there's any data to search for
	if [ -n "${__ARCGET_SCAN_LIST}" ]; then


		Logger.setVerboseMode off

		Logger.note "#\tStatus\t\tLabel\t\tAccession\tSubject\t\tScans"

		for __ARCGET_SCAN_NAME in ${__ARCGET_SCAN_LIST}; do

			if [ -z "${__ARCGET_SCAN_NAME}" ]; then
				echo "it is empty"
				#Skip empty scan session names
				continue
			fi

			__ARCGET_COUNT="`expr ${__ARCGET_COUNT} + 1`"

			Logger.debug "---> Verifying scan session '${__ARCGET_SCAN_NAME}'"

			#Clean all temporary files before attempting anything
			Http.cleanup

			#Save data to a temporary file
			echo "${__ARCGET_VELOCITY_SEARCH_DATA}" | sed -e 's@\%__ARCGET_SCAN_NAME\%@'${__ARCGET_SCAN_NAME}'@g' >"${__ARCGET_TEMP_DATA_FILE}" 2>"${__BF_GLOBAL_LOG}"

			if [ "$?" -ne 0 ]; then
				Logger.fatal "Could not save POST data to '${__ARCGET_TEMP_DATA_FILE}'."
			fi

			Http.setDataFile "${__ARCGET_TEMP_DATA_FILE}"

			#Perform the HTTP request
			Http.download "${__ARCGET_TEMP_RESPONSE_FILE}" || \
				Logger.fatal "Could not check the scan session '${__ARCGET_SCAN_NAME}'.";

			#Check the return code
			if [ "$?" -ne 0 ]; then
				local __ARCGET_RESPONSE_CODE
				local __ARCGET_REPONSE_MESSAGE

				#Obtain the server response code and message
				Http.getResponse __ARCGET_RESPONSE_CODE __ARCGET_RESPONSE_MESSAGE

				Logger.error "Could not find the scan session '${__ARCGET_SCAN_NAME}' (Server said: '${__ARCGET_RESPONSE_CODE} ${__ARCGET_RESPONSE_MESSAGE}')";

				return 1
			fi

			local SESSION_STATUS
			#Checking if the server said 'No matches found'
			SESSION_STATUS="`cat ${__ARCGET_TEMP_RESPONSE_FILE} | grep '<searchReturn xsi:type="xsd:string">No Matches Found.</searchReturn>' | wc -l`"
			if [ "$SESSION_STATUS" -ne 0 ]; then
				Logger.note "${__ARCGET_COUNT}\tNot Found\t${__ARCGET_SCAN_NAME}"
				__ARCGET_NOT_FOUND="`expr ${__ARCGET_NOT_FOUND} + 1`"
			else
				#Retrieving scan session details
				Xnat.ArcGet.getScanSessionDetails __ARCGET_SESSION_ID __ARCGET_ACCESSION_ID __ARCGET_SESSION_SUBJECT_ID __ARCGET_SCAN_COUNT
				Logger.note "${__ARCGET_COUNT}\tFound\t\t${__ARCGET_SESSION_ID}\t\t${__ARCGET_ACCESSION_ID}\t${__ARCGET_SESSION_SUBJECT_ID}\t\t${__ARCGET_SCAN_COUNT}"
				#Add the session to the list of found sessions
				__ARCGET_VALID_SCAN_LIST="${__ARCGET_VALID_SCAN_LIST} ${__ARCGET_SESSION_ID}"
			fi

		done

		#Enable verbose logging
		Logger.setVerboseMode "${__ARCGET_VERBOSE_MODE}"

		#Leave an empty space

		if [ "${__ARCGET_NOT_FOUND}" -gt 0 ]; then
			Logger.note ""
			Logger.warn "${__ARCGET_NOT_FOUND} out of ${__ARCGET_COUNT} scan session(s) could not be found!"
		else
			Logger.verbose ""
			Logger.verbose "${__ARCGET_COUNT} scan session(s) found."
		fi

	else
		Logger.fatal "Please specify at least one scan session."
	fi

	Xnat.ArcGet.cleanup

	if [ "${__ARCGET_NOT_FOUND}" -eq "${__ARCGET_COUNT}" ]; then
		return 1
	fi

	return 0
}

#======================================================
# Usage: Xnat.ArcGet.getScanSessionDetails $1 $2 $3 $4
#	$1 - Variable to store the Scan Session ID in
#	$2 - Variable to store the Accession number in
#	$3 - Variable to store the Subject ID in
#	$4 - Variable to store the number of scans into
#======================================================
function Xnat.ArcGet.getScanSessionDetails
{
	local SCAN_ID
	local ACCESS_NUM
	local SUBJECT_ID
	local SCAN_COUNT

	#Retrieve the information from the temporary response file
	SCAN_ID="`cat ${__ARCGET_TEMP_RESPONSE_FILE} | grep 'SESSION ID:' | sed -r -e 's@.*\s(.+)$@\1@g'`"
	ACCESS_NUM="`cat ${__ARCGET_TEMP_RESPONSE_FILE} | grep 'ACCESSION :' | sed -r -e 's@.*\s(.+)$@\1@g'`"
	SUBJECT_ID="`cat ${__ARCGET_TEMP_RESPONSE_FILE} | grep 'SUBJECT ID:' | sed -r -e 's@.*\s(.+)$@\1@g'`"
	SCAN_COUNT="`cat ${__ARCGET_TEMP_RESPONSE_FILE} | egrep '^[0-9]+\s+\w+\s+(usable|unusable)$' | wc -l`"

	#Return the Scan Session ID
	if [ -n "$1" ]; then
		eval "$1='$SCAN_ID'"
	fi

	#Return the Accession Number
	if [ -n "$2" ]; then
		eval "$2='$ACCESS_NUM'"
	fi

	#Return the Subject ID
	if [ -n "$3" ]; then
		eval "$3='$SUBJECT_ID'"
	fi

	#Return the number of scans
	if [ -n "$4" ]; then
		eval "$4='$SCAN_COUNT'"
	fi
}


#=========================================
# Usage: Xnat.ArcGet.downloadValidScans
#	Downloads all valid scan sessions
#=========================================
function Xnat.ArcGet.downloadValidScans()
{
	Xnat.ArcGet.download "${__ARCGET_VALID_SCAN_LIST}"
	return $?
}

#=========================================
# Usage: Xnat.ArcGet.download
#	$1 - List of scan sessions to download (optional)
#	Returns 0 on Success, 1 on Failure
#	and -1 (or 255) on empty list
#=========================================
function Xnat.ArcGet.download
{
	local __ARCGET_LIST
	local __ARCGET_COUNT=0
	local __ARCGET_FAILED_COUNT=0
	local __ARCGET_SESSION_FILENAME=""

	#If get list of sessions from parameter
	#if no parameter was specified, get the
	#default list
	if [ -z "$1" ]; then
		__ARCGET_LIST="${__ARCGET_SCAN_LIST}"
	else
		__ARCGET_LIST="$1"
	fi

	#Check if there are any sessions to download
	if [ -z "$__ARCGET_LIST" ]; then
		return -1;
	fi

	#Initialize the HTTP module
	Http.reset
	if [ -z "$__ARCGET_HOST" ]; then
		Logger.fatal "Please specify the host to connect to."
	fi

	#Set connection parameters
	Http.setMethod "POST"
	Http.setHost "${__ARCGET_HOST}"
	#Http.setPage is used later, in the loop, because it uses the session id
	Http.disableCache
	if [ -n "${__ARCGET_PROXY}" ]; then
		Http.setProxy "${__ARCGET_PROXY}"
		Http.enableProxy
	fi
	Http.enableCookies
	Http.setCookie "${__ARCGET_COOKIE_NAME}" "${__ARCGET_COOKIE_VALUE}"

	if [ -n "${__ARCGET_USERNAME}" ]; then
		Http.setUsername "${__ARCGET_USERNAME}"
		Http.setPassword "${__ARCGET_PASSWORD}"
	fi

	#Process all scan sessions
	for __ARCGET_SESSION in ${__ARCGET_LIST}; do

		#Skip empty session ids
		if [ -z "${__ARCGET_SESSION}" ]; then
			continue;
		fi

		Http.setPage "${__ARCGET_DOWNLOAD_SCAN_PAGE}${__ARCGET_SESSION}"

		#Build an appropriate filename taking into account user options
		#Checking to see if the user specified a filename
		if [ -n "${__ARCGET_FILE_NAME}" ]; then
			#Check to see if the user also specified a directory
			if [ -n "${__ARCGET_DOWNLOAD_DIR}" ]; then
				__ARCGET_SESSION_FILENAME="${__ARCGET_DOWNLOAD_DIR}/${__ARCGET_FILE_NAME}"
			else
				#If no directory specified, save it in the wget default dir
				__ARCGET_SESSION_FILENAME="${__ARCGET_FILE_NAME}"
			fi
		else
			#if the user has not specified a filename
			#but did specify a download directory
			if [ -n "${__ARCGET_DOWNLOAD_DIR}" ]; then
				#Then use the current session name +extension as the filename, and save it in the download directory
				__ARCGET_SESSION_FILENAME="${__ARCGET_DOWNLOAD_DIR}/${__ARCGET_SESSION}${__ARCGET_ARCHIVE_EXTENSION}"
			else
				#otherwise, save it in the current directory as use the session name+extension as the filename
				__ARCGET_SESSION_FILENAME="${__ARCGET_SESSION}${__ARCGET_ARCHIVE_EXTENSION}"
			fi
		fi

		#Set the name of the file
		Http.setFileName "${__ARCGET_SESSION_FILENAME}"

		#Perform the HTTP request
		Http.download || \
			Logger.fatal "Could not download the scan session '${__ARCGET_SESSION}'.";

		#Check the return code
		if [ "$?" -ne 0 ]; then
			local __ARCGET_RESPONSE_CODE
			local __ARCGET_REPONSE_MESSAGE

			#Obtain the server response code and message
			Http.getResponse __ARCGET_RESPONSE_CODE __ARCGET_RESPONSE_MESSAGE

			Logger.error "Could not download the scan session '${__ARCGET_SESSION}' (Server said: '${__ARCGET_RESPONSE_CODE} ${__ARCGET_RESPONSE_MESSAGE}')";
			Logger.note "Continuing..."
			__ARCGET_FAILED_COUNT=`expr ${__ARCGET_FAILED_COUNT} + 1`
			continue
		fi

		Logger.note "${__ARCGET_SESSION} -> '${__ARCGET_SESSION_FILENAME}'"
	done

	if [ "${__ARCGET_FAILED_COUNT}" -gt 0 ]; then
		Logger.warning "Failed to download ${__ARCGET_FAILED_COUNT} scan sessions."
		return 1
	else
		return 0
	fi
}

#======================================
# Usage: Xnat.ArcGet.setArchiveExtension
#	$1 - File extension. default: .zip
#======================================
function Xnat.ArcGet.setArchiveExtension()
{
	if [ -n "$1" ]; then
		__ARCGET_ARCHIVE_EXTENSION="$1"
	else
		__ARCGET_ARCHIVE_EXTENSION=".zip"
	fi
}

#======================================
# Usage: Xnat.ArcGet.getArchiveExtension
#	$1 - Variable that will hold the
#		file extension.
#======================================
function Xnat.ArcGet.getArchiveExtension()
{
	eval "$1='${__ARCGET_ARCHIVE_EXTENSION}'"
}

#======================================
# Usage: Xnat.ArcGet.setProxy
#	$1 - Proxy:Port
#======================================
function Xnat.ArcGet.setProxy
{
	__ARCGET_PROXY="$1"
}

#======================================
# Usage: Xnat.ArcGet.getProxy
#	$1 - Variable to store the proxy
#		settings into
#======================================
function Xnat.ArcGet.getProxy
{
	eval "$1='${__ARCGET_PROXY}'"
}

#======================================
# Usage: Xnat.ArcGet.cleanup
#		Cleans all the temporary files
#		Does not report any errors
#======================================
function Xnat.ArcGet.cleanup()
{
	if [ -f "$__ARCGET_TEMP_DATA_FILE" ]; then
		rm -f "$__ARCGET_TEMP_DATA_FILE" >/dev/null 2>&1
	fi

	if [ -f "$__ARCGET_TEMP_RESPONSE_FILE" ]; then
		rm -f "$__ARCGET_TEMP_RESPONSE_FILE" >/dev/null 2>&1
	fi

	Http.cleanup
}

#Call the constructor
bash.apps.Xnat.ArcGet;