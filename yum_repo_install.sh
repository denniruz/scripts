#!/bin/sh
# vim:tw=0:et:sw=4:ts=4

#echo "the repository bootstrap is down for maintainance. Please check back in 1 hour."
#[ -n "$DEBUG" ] || exit 1

# The purpose of this script is to download and install the appropriate 
# repository RPM. This RPM will set up the Dell yum repositories on your 
# system. This script will also install the Dell GPG keys used to sign 
# Dell RPMS.

# these two variables are replaced by the perl script 
# with the actual server name and directory.
SERVER="http://linux.dell.com"
# mind the trailing slash here...
REPO_URL="/repo/hardware/latest/"

# change to 0 to disable check of repository RPM sig.
CHECK_REPO_SIGNATURE=1

REPO_RPM_NAME="dell-omsa-repository"
REPO_FRIENDLY_NAME="Dell OMSA repository"

# these are 'eval'-ed to do var replacement later.
GPG_KEY[${#GPG_KEY[*]}]='${SERVER}${REPO_URL}RPM-GPG-KEY-dell'
GPG_KEY[${#GPG_KEY[*]}]='${SERVER}${REPO_URL}RPM-GPG-KEY-libsmbios'

##############################################################################
#  Should not need to edit anything below this point
##############################################################################

set -e
[ -z "$DEBUG" ] || set -x

get_dist_version(){
    local REL_RPM rpmq
    # let user override... unwise but necessary for testing
    ([ -z "$dist_base" ] && [ -z "$dist_ver" ] && [ -z "$dist" ]) || return 0
    dist_base=unknown
    dist_ver=
    rpmq='rpm --qf %{name}-%{version}-%{release}\n -q'
    if $rpmq --whatprovides redhat-release >/dev/null 2>&1; then
        REL_RPM=$($rpmq --whatprovides redhat-release 2>/dev/null | tail -n1)
        VER=$(rpm -q --qf "%{version}\n" $REL_RPM)
        REDHAT_RELEASE=$VER

        # RedHat: format is 3AS, 4AS, 5Desktop... strip off al alpha chars
        # Centos/SL: format is 4.1, 5.1, 5.2, ... strip off .X chars
        dist_base=el
        dist_ver=${VER%%[.a-zA-Z]*}

        if echo $REL_RPM | grep -q centos-release; then
            CENTOS_RELEASE=$VER
        elif echo $REL_RPM | grep -q sl-release; then
            # Scientific Linux (RHEL recompile)
            SCIENTIFIC_RELEASE=$VER
        elif echo $REL_RPM | grep -q fedora-release; then
            FEDORA_RELEASE=$(rpm --eval "%{fedora}")
            dist_base=f
        	dist_ver=${FEDORA_RELEASE}
        elif echo $REL_RPM | grep -q enterprise-linux; then
            # this is for Oracle Enterprise Linux (probably 4.x)
            ORACLE_RELEASE=$VER
        elif echo $REL_RPM | grep -q enterprise-release; then
            # this is for Oracle Enterprise Linux 5+
            ORACLE_RELEASE=$VER
        fi

    elif $rpmq --whatprovides sles-release >/dev/null 2>&1; then
        REL_RPM=$($rpmq --whatprovides sles-release 2>/dev/null | tail -n1)
        SLES_RELEASE=$(rpm -q --qf "%{version}\n" $REL_RPM)
		dist_base=sles
        # SLES 11 is 11.1, strip off .X chars
        dist_ver=${SLES_RELEASE%%[.a-zA-Z]*}

    elif $rpmq --whatprovides sled-release >/dev/null 2>&1; then
        REL_RPM=$($rpmq --whatprovides sled-release 2>/dev/null | tail -n1)
        SLES_RELEASE=$(rpm -q --qf "%{version}\n" $REL_RPM)
		dist_base=sles
        dist_ver=${SLES_RELEASE}

    # This comes after sles-release because sles also defines suse-release
    elif $rpmq --whatprovides suse-release >/dev/null 2>&1; then
        REL_RPM=$($rpmq --whatprovides suse-release 2>/dev/null | tail -n1)
        SUSE_RELEASE=$(rpm -q --qf "%{version}\n" $REL_RPM)
		dist_base=suse
        # SLES 11 is 11.1, strip off .X chars
        dist_ver=${SUSE_RELEASE%%[.a-zA-Z]*}
    fi

    dist=$dist_base$dist_ver
}

install_gpg_key() {
    eval GPG_KEY_URL=$1
    echo "Downloading GPG key: ${GPG_KEY_URL}"
    GPG_FN=$(mktemp /tmp/GPG-KEY-$$-XXXXXX)
    trap "rm -f $GPG_FN" EXIT HUP QUIT TERM
    wget -q -O ${GPG_FN} ${GPG_KEY_URL}
    email=$(gpg -v ${GPG_FN} 2>/dev/null  | grep 1024D | perl -p -i -e 's/.*<(.*)>/\1/')
    HAVE_KEY=0
    for key in $(rpm -qa | grep gpg-pubkey)
    do
        if rpm -qi $key | grep -q "^Summary.*$email"; then 
            HAVE_KEY=1; 
            break; 
        fi
    done
    if [ $HAVE_KEY = 1 ]; then
        i=$(( $i + 1 ))
        echo "    Key already exists in RPM, skipping"
        continue
    fi

    echo "    Importing key into RPM."
    rpm --import ${GPG_FN}
    if [ $? -ne 0 ]; then
        echo "GPG-KEY import failed."
        echo "   Either there was a problem downloading the key,"
        echo "   or you do not have sufficient permissions to import the key."
        exit 1
    fi
    rm -f $GPG_FN
    trap - EXIT HUP QUIT TERM
}

install_rpm() {
    # download repo rpm
    local RPM_URL=$1
    RPM_FILENAME=$(mktemp /tmp/repo-rpm-$$-XXXXXX)
    trap "rm -f $RPM_FILENAME" EXIT HUP QUIT TERM
    wget -O $RPM_FILENAME -q ${RPM_URL}
    if [ ! -e ${RPM_FILENAME} ]; then
        echo "Failed to download RPM: ${RPM_URL}"
        exit 1
    fi
    
    if [ "$CHECK_REPO_SIGNATURE" = "1" ]; then
        if ! rpm -K ${RPM_FILENAME} > /dev/null 2>&1; then
            echo "Repository RPM Failed GPG check! $RPM_URL" 
            exit 1
        fi
    fi

    echo "Installing repository rpm: $RPM_URL"
    export IN_BOOTSTRAP_ALREADY=1
    rpm -U --replacepkgs ${RPM_FILENAME}

    rm -f $RPM_FILENAME
    trap - EXIT HUP QUIT TERM
}

write_repo() {
    cat > $1 <<-EOF
		[dell-omsa-indep]
		name=${REPO_FRIENDLY_NAME} - Hardware independent
		type=rpm-md
		mirrorlist=${SERVER}${REPO_URL}mirrors.cgi?osname=${replace_dist}&basearch=$replace_basearch&native=1&dellsysidpluginver=\$dellsysidpluginver
		gpgcheck=1
		gpgkey=${SERVER}${REPO_URL}RPM-GPG-KEY-dell
		    ${SERVER}${REPO_URL}RPM-GPG-KEY-libsmbios
		enabled=1
		failover=priority
		bootstrapurl=${SERVER}${REPO_URL}bootstrap.cgi
		
		
		[dell-omsa-specific]
		name=${REPO_FRIENDLY_NAME} - Hardware specific
		type=rpm-md
		mirrorlist=${SERVER}${REPO_URL}mirrors.cgi?osname=${replace_dist}&basearch=$replace_basearch&native=1&sys_ven_id=\$sys_ven_id&sys_dev_id=\$sys_dev_id&dellsysidpluginver=\$dellsysidpluginver
		gpgcheck=1
		gpgkey=${SERVER}${REPO_URL}RPM-GPG-KEY-dell
		    ${SERVER}${REPO_URL}RPM-GPG-KEY-libsmbios
		enabled=1
		failover=priority
		bootstrapurl=${SERVER}${REPO_URL}bootstrap.cgi
EOF
}

install_all_gpg_keys() {
    local i=0
    while [ $i -lt ${#GPG_KEY[*]} ]; do
        install_gpg_key ${GPG_KEY[$i]}
        i=$(( $i + 1 ))
    done
}

# if bootstrap is being called from within RPM %post, dont do any operations
# that call rpm. Can be set via cmline or set for us.
if [ "$1" == "recursive_rpm" ]; then
    RECURSIVE_RPM=1
fi

# sets $dist
get_dist_version

if [ "${dist}" = "unknown" ]; then
    echo "Unable to determine that you are running an OS I know about."
    echo "Handled OSs include Red Hat Enterprise Linux and CentOS,"
    echo "Fedora Core and Novell SuSE Linux Enterprise Server and OpenSUSE"
    exit 1
fi

basearch=$(uname -i)

REPO_FULL_URL=${SERVER}${REPO_URL}mirrors.cgi/osname=${dist}\&basearch=$basearch\&native=1
replace_basearch=$basearch
replace_dist=$dist

[ "$RECURSIVE_RPM" == "1" ] || install_all_gpg_keys

case $dist in
    sles*|suse*)
        if [ -e /usr/bin/rug ]; then
            # rug deadlocks if called recursively
            rug sd dell-omsa-hwindep ||:
            rug sd dell-omsa-hw ||:
            yes | rug sa -t ZYPP $REPO_FULL_URL\&redirpath=/ dell-omsa-hwindep
            rug subscribe dell-omsa-hwindep
        elif [ -e /usr/bin/zypp ]; then
            zypp sa -t YUM $REPO_FULL_URL\&redirpath=/ dell-omsa-hwindep
	elif [ -e /usr/bin/zypper ]; then
            zypper rs dell-omsa-hwindep
            zypper rs dell-omsa-hw
            zypper as -t YUM $REPO_FULL_URL\&redirpath=/ dell-omsa-hwindep
        fi
        ;;
    el4)
        mkdir -p /etc/yum.repos.d ||:
        write_repo /etc/yum.repos.d/$REPO_RPM_NAME.repo
        RHN_SOURCES=/etc/sysconfig/rhn/sources
        if [ -e ${RHN_SOURCES} ]; then
            if ! grep -q "^#DONT UPDATE dell-omsa-hwindep" ${RHN_SOURCES} > /dev/null 2>&1; then
                # remove existing config
                perl -n -i -e "print if not /^#BEGIN dell-omsa-hwindep/ ... /^#END dell-omsa-hwindep/" ${RHN_SOURCES}
            
                # add updated config unless user specifies not to
                echo "#BEGIN dell-omsa-hwindep" >> ${RHN_SOURCES}
                echo "yum dell-omsa-repository ${REPO_FULL_URL}\&redirpath=/" >> ${RHN_SOURCES}
                echo "yum-mirror dell-omsa-repository $REPO_FULL_URL" >> ${RHN_SOURCES}
                echo "#END dell-omsa-hwindep" >> ${RHN_SOURCES}
            fi
        fi
        ;;
    el[5-9]*|f[0-9]*)
        replace_basearch=\$basearch
        replace_dist=$dist_base\$releasever
        echo "Write repository configuration"
        mkdir -p /etc/yum.repos.d ||:
        write_repo /etc/yum.repos.d/$REPO_RPM_NAME.repo
        ;;
esac



if [ "$RECURSIVE_RPM" != "1" ]; then
    echo "Downloading repository RPM"
    rpm -e dell-hw-indep-repository > /dev/null 2>&1 ||:
    rpm -e dell-hw-specific-repository > /dev/null 2>&1 ||:
    install_rpm $(wget -q -O- ${REPO_FULL_URL}\&getrpm=${REPO_RPM_NAME} | head -n1)
fi


# install plugins
case $dist in
    sles*|suse*)
        if [ "$RECURSIVE_RPM" != "1" ]; then
            echo "Installing yum plugins for system id"

            if [ -e /usr/bin/rug ]; then
                rug install -y yum-dellsysid
            else
		zypper install -y libsmbios2
                zypper install -y  --force-resolution yum-dellsysid
            fi

            sys_ven_id=0x1028
            sys_dev_id=$( /usr/sbin/getSystemId | grep '^System ID:' | cut -d: -f2 | tr A-Z a-z | perl -p -i -e 's/\s*//')

            HW_SPEC_URL=${REPO_FULL_URL}\&sys_ven_id=$sys_ven_id\&sys_dev_id=$sys_dev_id\&dellsysidpluginver=\$dellsysidpluginver\&redirpath=/
            if [ -e /usr/bin/rug ]; then
                # rug deadlocks if called recursively
                yes | rug sa -t ZYPP $HW_SPEC_URL dell-omsa-hw
                rug subscribe dell-omsa-hw
            elif [ -e /usr/bin/zypp ]; then
                zypp sa -t YUM $HW_SPEC_URL dell-omsa-hw
            elif [ -e /usr/bin/zypper ]; then
                zypper as -t YUM $HW_SPEC_URL dell-omsa-hw
            fi

        fi
        ;;
    el4)
        if [ "$RECURSIVE_RPM" != "1" ]; then
            echo "Installing yum plugins for system id"
            if [ -x /usr/bin/yum ]; then
                yum -y install yum-dellsysid
            else
                up2date -i yum-dellsysid
            fi
        fi
        sys_ven_id=0x1028
        sys_dev_id=$( /usr/sbin/getSystemId | grep '^System ID:' | cut -d: -f2 | tr A-Z a-z | perl -p -i -e 's/\s*//')

        # need to confirm path below
        #rm -f /var/spool/up2date/

        RHN_SOURCES=/etc/sysconfig/rhn/sources
        if [ -e ${RHN_SOURCES} ]; then
            if ! grep -q "^#DONT UPDATE dell-omsa-hwspec" ${RHN_SOURCES} > /dev/null 2>&1; then
                # remove existing config
                perl -n -i -e "print if not /^#BEGIN dell-omsa-hwspec/ ... /^#END dell-omsa-hwspec/" ${RHN_SOURCES}
            
                # add updated config unless user specifies not to
                echo "#BEGIN dell-omsa-hwspec" >> ${RHN_SOURCES}

                HW_SPEC_URL=${REPO_FULL_URL}\&sys_ven_id=$sys_ven_id\&sys_dev_id=$sys_dev_id\&dellsysidpluginver=\$dellsysidpluginver
                echo "yum dell-omsa-hwspecific-repository ${HW_SPEC_URL}\&redirpath=/" >> ${RHN_SOURCES}
                echo "yum-mirror dell-omsa-hwspecific-repository $HW_SPEC_URL" >> ${RHN_SOURCES}
                echo "#END dell-omsa-hwspec" >> ${RHN_SOURCES}
            fi
        fi
        ;;
    el[5-9]*|f[0-9]*)
        if [ "$RECURSIVE_RPM" != "1" ]; then
            echo "Installing yum plugins for system id"
            yum -y install yum-dellsysid
            # - have to re-init the yum cache as the repo changes under us when
            # we install the plugin 
            # - deal with bug in recent RHEL 5 releases where they broke yum
            # clean in rhnplugin. :(
            yum --disableplugin=rhnplugin clean all
        fi
        ;;
esac

echo "Done!"
exit 0
