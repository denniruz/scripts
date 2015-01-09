#!/bin/bash
. "/opt/local/xnat/.NRG/bash/include";

include bash.Framework;
include bash.apps.Xnat;
include bash.apps.Pgsql;
include bash.apps.Tomcat;

function main() 
{
	# --- sets
	Logger.setVerbosity 0;
	Tomcat.setWebapp "xnat";
	Tomcat.setHome "/opt/local/tomcat";
	Xnat.setHome "/opt/local/xnat";
	Xnat.setDeployment "xnat";
	Tomcat.getWebappPath PATH;
	Xnat.setWebappPath "$PATH";
	Pgsql.setHome "/opt/local/postgres"
	Pgsql.setAdmin "postgres";
	Pgsql.setDB "xnat";
	
	# --- overrides
	Logger.setLogFile "/tmp/xnat-vm.log";
	Pgsql.setDumpDir "/opt/local/shared/Dumps";
	Xnat.setLogDir "/opt/local/shared/Logs/xnat";
}

#Purge existing Xnat source
function purgeAction() 
{
	Logger.note "Called purgeAction()";
	Tomcat.stop;
	Xnat.dropDeployment;
	Tomcat.dropWebapp;
	Pgsql.backupDB
	Pgsql.dropDB
	Logger.note "Purged successfully.";
	exit 0;
}

#Clean existing Xnat source
function cleanAction() 
{
	Logger.note "Called cleanAction()";
	Tomcat.stop;
	Tomcat.dropWebapp;
	Logger.note "Cleaned successfully.";
	exit 0;
}

#Build Xnat from existing source
function setupAction()
{
	Logger.note "Called setupAction()";
	Tomcat.stop;
	Xnat.fix;
	Xnat.patch;
	Pgsql.createDB
	Xnat.setup;
	Xnat.getDefaultSql SQL;
	Pgsql.runSql "$SQL";
	Xnat.storeDefaultXml;
	Tomcat.start;
	Logger.note "Setup was successful.";
	exit 0;
}

#Restart Tomcat
function restartTomcatAction() 
{
	Logger.note "Caller restartTomcatAction()";
	Tomcat.restart;
	Logger.note "Restarting Tomcat was successful.";
	exit 0;
}

#Install Xnat from scratch
function installAction()
{
	Logger.note "Called installAction()";
	Xnat.retrieve;
	Xnat.patch;
	Pgsql.createDB;
	Xnat.setup;
	Xnat.getDefaultSql SQL;
	Pgsql.runSql $SQL;
	Xnat.storeDefaultXml;
	Tomcat.start;
	Logger.note "Installation was successful.";
	exit 0;
}

#Update Xnat
function updateAction()
{
	Logger.note "Called updateAction()";
	Xnat.update
	Tomcat.start
	Logger.note "Update was successful.";
	exit 0;
}

#Remove Xnat entirely
function obliterateAction()
{
	Logger.note "Called obliterateAction()";
	Tomcat.stop;
	Xnat.dropXnat;
	Tomcat.dropWebapp;
	Pgsql.backupDB
	Pgsql.dropDB;
	Logger.note "Obliteration was successful.";
	exit 0;
}

dispatch;
