#!/bin/bash

include bash.io.Filesystem;
include bash.apps.Svn;

function bash.apps.Xnat;
{
	Filesystem.dryRun false;
}

#===========================
# Usage: Xnat.dropDeployment
#===========================
function Xnat.dropDeployment()
{
	Logger.note "Preparing to drop the Xnat deployment directory: $__XNAT_HOME/deployments/$__XNAT_DEPLOY";
	Filesystem.delete "$__XNAT_HOME/deployments/$__XNAT_DEPLOY" sudo;
	Logger.note "Deployment dropped successfully.";
}

#===============================
# Usage: Xnat.setHome $1
#	$1 - Xnat root directory
#===============================
function Xnat.setHome()
{
	eval "__XNAT_HOME=$1";
}

#=============================
# Usage: Xnat.setDeployment $1
#	$1 - deployment name
#=============================
function Xnat.setDeployment()
{
	eval "__XNAT_DEPLOY=$1";
}

#=====================
# Usage: Xnat.dropXnat
#=====================
function Xnat.dropXnat()
{
	Logger.note "Preparing to drop Xnat: $__XNAT_HOME";
	Filesystem.delete "$__XNAT_HOME" sudo;
	Logger.note "Xnat dropped successfully";
}

#=================
# Usage Xnat.patch
#=================
function Xnat.patch()
{
	Logger.note "Preparing to patch Xnat using: $__XNAT_HOME/patches/*"
	System.pushd "$__XNAT_HOME";
	Filesystem.checkDir "patches";
	for i in `ls -1 patches | tr '\n' ' '`;
	do
		OUTPUT=`sudo patch -p1 -N -u < "patches/$i"`;
		if [ "$?" -ne 0 ]; then
			FAILED=`echo $OUTPUT | grep 'FAILED'`
			APPLIED=`echo $OUTPUT | grep 'previously applied'`
			if [ -n "$FAILED" ]; then
				Logger.fatal "Patch '$i' could not be applied."
			elif [ -n "$APPLIED" ]; then
				Logger.warn "Patch '$i' has already been applied."
			else
				Logger.warn "Patch '$i' returned a non-zero error code."
			fi
		else
			Logger.note "Patch '$i' has been applied successfully."
		fi
	done
	System.popd;
	Logger.note "Patching Xnat completed successfully.";
}

#==================
# Usage: Xnat.setup
#==================
function Xnat.setup()
{
	Logger.note "Setting up XNAT (running Setup.sh)";
	System.obtainRootPrivs;
	System.pushd "$__XNAT_HOME";
	sudo -E bin/setup.sh -Ddeploy=true;
	System.popd;
	Logger.note "Xnat setup completed successfully.";

}

#==========================
# Usage: Xnat.setWebapp
#	$1 - Webapp path
#==========================
function Xnat.setWebapp()
{
	__XNAT_WEBAPP="$1";
}

#==========================
# Usage: Xnat.getWebapp
#	$1 - Webapp path
#==========================
function Xnat.getWebapp()
{
	eval "$1=$__XNAT_WEBAPP";
}



#===================
# Usage: Xnat.update
#===================
function Xnat.update()
{
	Logger.note "Updating XNAT (running Setup.sh)";
	System.obtainRootPrivs;
	System.pushd "$__XNAT_HOME";
	Svn.update
	Xnat.patch
	sudo -E bin/update.sh -Ddeploy=true;
	System.popd;
	Logger.note "Xnat update completed successfully.";

}

#===================================
# Usage: Xnat.setLogDir
#	$1 - alternate log directory
#===================================
function Xnat.setLogDir()
{
	__XNAT_LOGDIR="$1";
}

#=================================
# Usage: Xnat.rerouteLogs
#	$1 - destination directory
#=================================
function Xnat.rerouteLogs()
{
	echo "Original log directory: $1"
	test -n "$1" || Logger.fatal "You must set the logs directory when using Xnat.rerouteLogs"

	__XNAT_DESTLOGDIR="$1"

	Logger.note "Preparing to reroute XNAT logs.";

	if [ ! -n "$__XNAT_LOGDIR" ]; then
		Logger.warn "You must call Xnat.setLogDir before calling Xnat.rerouteLogs"
		return
	fi

	if [ -d "$__XNAT_DESTLOGDIR" ]; then
		Logger.note "Directory '$__XNAT_DESTLOGDIR' does not exist."
		Filesystem.mkdir "$__XNAT_DESTLOGDIR"
	fi

	Logger.note "Rerouting Xnat Webapp Logging from $__XNAT_LOGDIR to $__XNAT_DESTLOGDIR"

	if [ ! -L "$__XNAT_LOGDIR" ]; then
		Filesystem.delete "$__XNAT_LOGDIR" sudo;
		Filesystem.link "$__XNAT_LOGDIR" "$__XNAT_DESTLOGDIR" sudo;
		Xnat.setLogDir "$__XNAT_DESTLOGDIR";
	fi
}

#================================
# Usage: Xnat.setPatchRepo $1
#	$1 - SVN Patch Repository
#================================
function Xnat.setPatchRepo()
{
	__PATCH_REPO="$1";
}

#======================
# Usage: Xnat.setBranch
#======================
function Xnat.setBranch()
{
	__XNAT_BRANCH="$1";
}

#===============================
# Usage: Xnat.setXnatRepo $1
#	$1 - SVN Xnat Repository
#===============================
function Xnat.setXnatRepo()
{
	__XNAT_REPO="$1";
	Xnat.setPatchRepo "$__XNAT_REPO/patches";
}

#==============================
# Usage: Xnat.getInstallSql $1
#	$1 - variable to assign
#==============================
function Xnat.getDefaultSql()
{
	Filesystem.checkFile "$__XNAT_HOME/deployments/$__XNAT_DEPLOY/sql/$__XNAT_DEPLOY.sql";
	eval "$1=$__XNAT_HOME/deployments/$__XNAT_DEPLOY/sql/$__XNAT_DEPLOY.sql";
}

#==============================
# Usage: Xnat.getUpdateSql $1
#	$1 - variable to assign
#==============================
function Xnat.getUpdateSql()
{
	Filesystem.checkFile "$__XNAT_HOME/deployments/$__XNAT_DEPLOY/sql/$__XNAT_DEPLOY-update.sql";
	eval "$1=$__XNAT_HOME/deployments/$__XNAT_DEPLOY/sql/$__XNAT_DEPLOY-update.sql";
}

#============================
# Usage: Xnat.storeDefaultXml
#============================
function Xnat.storeDefaultXml()
{
	Logger.note "Storing default XML into Xnat";
	System.pushd "$__XNAT_HOME/deployments";
	../bin/StoreXML -project "$__XNAT_DEPLOY" -l $__XNAT_DEPLOY/security/security.xml -allowDataDeletion true -u admin -p admin || \
		Logger.fatal "Unable to store XML.";
	../bin/StoreXML -project "$__XNAT_DEPLOY" -dir ../projects/"$__XNAT_DEPLOY"/work/field_groups -allowDataDeletion true \
		-u admin -p admin || \
		Logger.fatal "Unable to store XML.";
	Logger.note "Default XML stored into Xnat successfully.";
}

#=====================
# Usage: Xnat.retrieve
#=====================
function Xnat.retrieve()
{
	Logger.note "Retrieving Xnat";
	Logger.note "Repository: $__XNAT_REPO";
	Logger.note "Branch: $__XNAT_BRANCH";
	#...to be implemented
	Logger.note "Xnat retrieved successfully";
}

#================
# Usage: Xnat.fix
#================
function Xnat.fix()
{
	Logger.note "Fixing Xnat"
	System.pushd "$__XNAT_HOME/pipeline"
	Svn.update || Logger.fatal "Could not perform SVN update.";
	System.popd
}

#Call the constructor
bash.apps.Xnat;