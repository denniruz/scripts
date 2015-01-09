#!/bin/bash

include bash.io.Filesystem;
include bash.utils.System;

function bash.apps.Tomcat()
{
	return 0;
}

#=========================
# Usage: Tomcat.dropWebapp
#	$1 - Webapp name
#=========================
function Tomcat.dropWebapp()
{
	local __TOMCAT_WEBAPP="$1"
	local __TOMCAT_WEBAPP_PATH;
	Tomcat.getWebappPath __TOMCAT_WEBAPP_PATH
	Logger.note "Preparing to drop Tomcat webapp directory: $__TOMCAT_HOME/webapps/$__TOMCAT_WEBAPP";
	Filesystem.delete "$__TOMCAT_WEBAPP_PATH/$__TOMCAT_WEBAPP" sudo \
		&& Logger.note "Webapp dropped successfully.";
}

#==============================
# Usage: Tomcat.getWebappPath()
#	$1 - var to return
#==============================
function Tomcat.getWebappPath()
{
	eval "$1=$__TOMCAT_HOME/webapps";
}

#=================================
# Usage: Tomcat.setHome $1
#	$1 - Tomcat home directory
#=================================
function Tomcat.setHome()
{
	eval "__TOMCAT_HOME=$1";
}

#===================
# Usage: Tomcat.stop
#===================
function Tomcat.stop()
{
	Logger.note "Stopping Tomcat: sudo /etc/init.d/tomcat stop";
	System.obtainRootPrivs;
	Filesystem.checkFile "/etc/init.d/tomcat";
	sudo /etc/init.d/tomcat stop &> /dev/null || \
		Logger.fatal "Tomcat failed to stop.";
	Logger.note "Tomcat stopped successfully.";
}

#==========================
# Usage: Tomcat.start
#	$1 - "debug" on/off
#==========================
function Tomcat.start()
{
	#is debug on?
	if [ "$1" = "debug" ]; then
		__TOMCAT_ARG="debug";
	else
		__TOMCAT_ARG="start";
	fi

	Filesystem.checkFile "/etc/init.d/tomcat";
	Logger.note "Starting Tomcat: sudo /etc/init.d/tomcat $__TOMCAT_ARG";
	System.obtainRootPrivs;
	sudo /etc/init.d/tomcat "$__TOMCAT_ARG" &> /dev/null || \
		Logger.fatal "Tomcat failed to start.";
	Logger.note "Tomcat started successfully.";
}

#==============================
# Usage: Tomcat.restart
#	--debug=true - debug on
#==============================
function Tomcat.restart()
{
	__TOMCAT_ARG="";
	if [ "$__TOMCAT_DEBUG" = true ]; then
		__TOMCAT_ARG="debug";
	fi
	Tomcat.stop;
	Tomcat.start $__TOMCAT_ARG;
}

#=====================
# Usage: Tomcat.getPid
#=====================
function Tomcat.getPid()
{
	__TOMCAT_PID=`ps -ef  | grep "$__TOMCAT_HOME"/bin/bootstrap.jar | grep -v grep | awk '{print $2}'`;
}

#Call the constructor
bash.apps.Tomcat;