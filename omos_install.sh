#!/bin/bash
# 
## FILE: omos_install.sh 
##
## DESCRIPTION: Install and configure Openmanage on SC Openstack nodes
##
## AUTHOR: Dennis Ruzeski (denniruz@gmail.com)
##
## Creation Date: 7/21/2015
##
## Last Modified: 7/22/2015
## 
## VERSION: 1.0
##
## USAGE: omos_install.sh
## 
## TODO:  Test and deploy
##       
## Begin script
# 
# Set some variables
_OPTERROR=85
_RACADM=/opt/dell/srvadmin/sbin/racadm
_PASSWORD=<password>
#
#
# Functions-
# 
_usage() {
    echo "Usage: $(basename $0) install | setpassword"
    echo "install will create and sign the repo and instasll openmanage"
    echo "setpassword will reset the root password"
}

_what_os() {
    if [[ -f /etc/centos-release ]] ; then 
        _OS=CENTOS
    else
        _OS=UBUNTU
    fi
}
check_om_repo() {
    [[ -f /etc/apt/sources.list.d/linux.dell.com.sources.list ]]
}

install_om_repo() {
    check_om_repo
    if [[ $? -eq 0 ]] ; then 
        apt-get install srvadmin-all
    else
        echo 'deb http://linux.dell.com/repo/community/ubuntu precise openmanage' | \
        sudo tee -a /etc/apt/sources.list.d/linux.dell.com.sources.list 
        gpg --keyserver pool.sks-keyservers.net --recv-key 1285491434D8786F 
        gpg -a --export 1285491434D8786F | sudo apt-key add - 
        apt-get update
        apt-get -y install srvadmin-all
    fi
}

confirm_root_index() {
    if [[ -f ${_RACADM} ]] ; then 
        _user=$(${_RACADM} getconfig -g cfgUserAdmin -i 2 |awk -F= 'NR==1{print $2}')
        if [[ ${_user} != "root" ]] ; then
            echo "Root User is not INDEX 2 - Please correct this manually"
            exit 1
        fi
        else 
            echo "racadm is not installed"
            exit 1
    fi
}

change_root_password() {
    if [[ -f ${_RACADM} ]] ; then
    ${_RACADM} config -g cfgUserAdmin -o cfgUserAdminPassword -i 2 ${_PASSWORD}
    fi
}

if [[ $# -ne 1 ]] ; then
    _usage
    exit 2
fi

case $1 in 
    install )
        install_om_repo ;;
    setpassword )
#        confirm_root_index 
        change_root_password ;;
    checkom )
        check_om_installed ;;
    * )
        _usage ;;
esac


