#!/bin/bash

include bash.utils.System;

function bash.apps.Svn()
{
	return 0;
}

#============================
# Usage: Svn.setRepository $1
#	$1 - SVN repository
#============================
function Svn.setRepository()
{
	__SVN_REPO=$1;
}

#====================
# Usage: Svn.checkout
#====================
function Svn.checkout()
{
	Logger.note "SVN checkout: $__SVN_REPO";
	System.obtainRootPrivs;
	#sudo svn update;
	#sudo svn co "$SVN_REPO";
}

#==================
# Usage: Svn.update
#==================
function Svn.update()
{
	Logger.note "SVN update";
	System.obtainRootPrivs;
	svn update || Logger.fatal "SVN Update failed.";
}

#Call the constructor
bash.apps.Svn;