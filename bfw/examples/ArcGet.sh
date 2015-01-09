#!/bin/bash

. ../include

include bash.Framework;
include bash.apps.Xnat.ArcGet;
include bash.utils.Logger;
include bash.utils.System;

Option HOST
Option SESSION
Option SESSION_FILE
Option SAVE_AS
Option SAVE_DIR
Option PASSFILE "$HOME/.passfile"
Option LOG "/tmp/arcget.log"
Option EXTENSION
Option PROXY

function main()
{
	TITLE="ArcGet v.0.1"
}

#Downloads a file
function downloadAction()
{
	Logger.setLogFile "/tmp/arcget.log"

	#Checking input parameters
	if [ -z "$SESSION" ] && [ -z "$SESSION_FILE" ]; then
		Logger.fatal "Please specify either a session name or the path to the file containing a list of sessions."
		return 1
	fi

	#Check if the user has supplied a session file or a session name
	if [ -n "$SESSION_FILE" ] && [ -n "$SESSION" ]; then
		Logger.verbose "You have specified both a session and a session file. I will try to use both."
	fi

	#Check for a passfile
	if [ -z "$PASSFILE" ]; then
		Logger.fatal "Please specify your username and password in a file and supply them using the --passfile=<filename> option."
	fi

	#Set connection preferences
	Xnat.ArcGet.setHost "$HOST"
	Xnat.ArcGet.setProxy "$PROXY"

	#Check that the server offers ArcGet services
	Logger.note "Checking for service XMLSearch..."
	Xnat.ArcGet.setPassFile "$PASSFILE"
	Xnat.ArcGet.checkService || \
		Logger.fatal "Could not find the XMLSearch service on host $HOST."
	Logger.verbose "Service found."

	#Authenticate with XNAT
	Logger.note "Creating the service session..."
	local ServiceID
	Xnat.ArcGet.createServiceSession ServiceID || \
		Logger.fatal "Failed to create the service session."


	if [ -z "$ServiceID" ]; then
		Logger.fatal "XNAT didn't want to give you a cookie. See ya."
	fi

	Logger.debug "XNAT gave you a cookie: '$ServiceID'. Excellent!"

	#Verify all scans
	if [ -n "$SESSION" ]; then
		Xnat.ArcGet.addScans "$SESSION"
	fi

	if [ -n "$SESSION_FILE" ]; then
		Xnat.ArcGet.loadFromFile "$SESSION_FILE"
	fi

	if [ -n "$SAVE_DIR" ]; then
		Xnat.ArcGet.setDownloadDir "$SAVE_DIR"
	fi

	if [ -n "$SAVE_AS" ]; then
		Xnat.ArcGet.setFileName "$SAVE_AS"
	fi

	Xnat.ArcGet.setServiceSessionId $ServiceID
	Logger.note "Verifying all scans..."
	Xnat.ArcGet.checkScans || Logger.fatal "There are no sessions to download."

	Logger.note "Downloading scan sessions.."
	if [ -n "$EXTENSION" ]; then
		Xnat.ArcGet.setArchiveExtension "$EXTENSION"
	fi
	Xnat.ArcGet.downloadValidScans || Logger.fatal "Download failed."

	Logger.note "All scan sessions have been downloaded successfully."
}

dispatch;