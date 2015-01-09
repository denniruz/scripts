#!/bin/bash
echo "Content-type: text/html"
echo ""
saveIFS=$IFS
IFS='=&'
parm=($QUERY_STRING)
IFS=$saveIFS
__IP=${parm[1]}
__NETMASK=${parm[3]}
__GATEWAY=${parm[5]}
__FQDN=${parm[7]}
__ROLE=${parm[9]}
__C_ID=${parm[11]}
# Uncomment these for Command Line testing.
#echo "${parm[@]}"
#__IP=10.10.10.10
#__NETMASK=255.255.0.0
#__GATEWAY=10.10.0.1 
#__FQDN=tester.atldc.vocalocity.com
#__ROLE=cache
#__C_ID=69
#
echo ""
echo "install"
echo "url --url http://10.2.0.103/distro/centos55_x86_64/"
echo "lang en_US.UTF-8"
echo "keyboard us"
echo "xconfig --startxonboot"
echo "network --device eth0 --bootproto static --ip ${__IP} --netmask ${__NETMASK} --gateway ${__GATEWAY} --nameserver 10.4.0.31,10.4.0.32 --hostname ${__FQDN}"
echo 'rootpw --iscrypted $1$aLB8v3tx$J0.PuiIWNDDWe/gTPgv0O.'
echo "firewall --disabled"
echo "authconfig --enableshadow --enablemd5"
echo "selinux --permissive"
echo "timezone --utc UTC"
echo "bootloader --location=mbr --driveorder=sda --append="rhgb quiet""
echo "clearpart --linux --initlabel"
case ${__ROLE} in 
	controller )
	echo "part /boot --fstype ext3 --size=250"
	echo "part pv.13 --size=100 --grow"
	echo "volgroup VolGroup00 --pesize=32768 pv.13"
	echo "logvol / --fstype ext3 --name=LogVolRoot --vgname=VolGroup00 --size=14336"
	echo "logvol /home --fstype ext3 --name=LogVolHome --vgname=VolGroup00 --size=26624"
	echo "logvol /usr/local --fstype ext3 --name=LogVolUsrLocal --vgname=VolGroup00 --size=992"
	echo "logvol /usr --fstype ext3 --name=LogVolUsr --vgname=VolGroup00 --size=4992"
	echo "logvol /var --fstype ext3 --name=LogVolVar --vgname=VolGroup00 --size=31744"
	echo "logvol /tmp --fstype ext3 --name=LogVolTmp --vgname=VolGroup00 --size=992"
	echo "logvol /opt --fstype ext3 --name=LogVolOpt --vgname=VolGroup00 --size=992"
	echo "logvol swap --fstype swap --name=LogVolSwap --vgname=VolGroup00 --size=4000"
	;;
	hdap|hdap-ui )
	echo "part /boot --fstype ext3 --size=250"
	echo "part pv.13 --size=100 --grow"
	echo "volgroup VolGroup00 --pesize=32768 pv.13"
	echo "logvol / --fstype ext3 --name=LogVolRoot --vgname=VolGroup00 --size=1984"
	echo "logvol /home --fstype ext3 --name=LogVolHome --vgname=VolGroup00 --size=992"
	echo "logvol /usr/local --fstype ext3 --name=LogVolUsrLocal --vgname=VolGroup00 --size=76800"
	echo "logvol /usr --fstype ext3 --name=LogVolUsr --vgname=VolGroup00 --size=4992"
	echo "logvol /var --fstype ext3 --name=LogVolVar --vgname=VolGroup00 --size=4992"
	echo "logvol /tmp --fstype ext3 --name=LogVolTmp --vgname=VolGroup00 --size=992"
	echo "logvol /opt --fstype ext3 --name=LogVolOpt --vgname=VolGroup00 --size=992"
	echo "logvol swap --fstype swap --name=LogVolSwap --vgname=VolGroup00 --size=4000"
	;;
	outboundproxy )
	echo "part /boot --fstype ext3 --size=250"
	echo "part pv.13 --size=100 --grow"
	echo "volgroup VolGroup00 --pesize=32768 pv.13"
	echo "logvol / --fstype ext3 --name=LogVolRoot --vgname=VolGroup00 --size=1984"
	echo "logvol /home --fstype ext3 --name=LogVolHome --vgname=VolGroup00 --size=992"
	echo "logvol /usr/local --fstype ext3 --name=LogVolUsrLocal --vgname=VolGroup00 --size=10240"
	echo "logvol /usr --fstype ext3 --name=LogVolUsr --vgname=VolGroup00 --size=4992"
	echo "logvol /var --fstype ext3 --name=LogVolVar --vgname=VolGroup00 --size=24992"
	echo "logvol /tmp --fstype ext3 --name=LogVolTmp --vgname=VolGroup00 --size=992"
	echo "logvol /opt --fstype ext3 --name=LogVolOpt --vgname=VolGroup00 --size=992"
	echo "logvol swap --fstype swap --name=LogVolSwap --vgname=VolGroup00 --size=4000"
	;;
	inboundproxy )
	echo "part /boot --fstype ext3 --size=250"
	echo "part pv.13 --size=100 --grow"
	echo "volgroup VolGroup00 --pesize=32768 pv.13"
	echo "logvol / --fstype ext3 --name=LogVolRoot --vgname=VolGroup00 --size=1984"
	echo "logvol /home --fstype ext3 --name=LogVolHome --vgname=VolGroup00 --size=992"
	echo "logvol /usr/local --fstype ext3 --name=LogVolUsrLocal --vgname=VolGroup00 --size=992"
	echo "logvol /usr --fstype ext3 --name=LogVolUsr --vgname=VolGroup00 --size=4992"
	echo "logvol /var --fstype ext3 --name=LogVolVar --vgname=VolGroup00 --size=4992"
	echo "logvol /tmp --fstype ext3 --name=LogVolTmp --vgname=VolGroup00 --size=992"
	echo "logvol /opt --fstype ext3 --name=LogVolOpt --vgname=VolGroup00 --size=992"
	echo "logvol swap --fstype swap --name=LogVolSwap --vgname=VolGroup00 --size=4000"
	;;
	registrar )
	echo "part /boot --fstype ext3 --size=250"
	echo "part pv.13 --size=100 --grow"
	echo "volgroup VolGroup00 --pesize=32768 pv.13"
	echo "logvol / --fstype ext3 --name=LogVolRoot --vgname=VolGroup00 --size=2048"
	echo "logvol /home --fstype ext3 --name=LogVolHome --vgname=VolGroup00 --size=5017"
	echo "logvol /usr/local --fstype ext3 --name=LogVolUsrLocal --vgname=VolGroup00 --size=992"
	echo "logvol /usr --fstype ext3 --name=LogVolUsr --vgname=VolGroup00 --size=4992"
	echo "logvol /var --fstype ext3 --name=LogVolVar --vgname=VolGroup00 --size=39936"
	echo "logvol /tmp --fstype ext3 --name=LogVolTmp --vgname=VolGroup00 --size=992"
	echo "logvol /opt --fstype ext3 --name=LogVolOpt --vgname=VolGroup00 --size=992"
	echo "logvol swap --fstype swap --name=LogVolSwap --vgname=VolGroup00 --size=4000"
	;;
esac
echo ""
echo "reboot --eject"
echo ""
echo "%packages"
echo "@base"
echo "@core"
echo "@editors"
echo "@gnome-desktop"
echo "@graphical-internet"
echo "@legacy-software-support"
echo "@mail-server"
echo "@system-tools"
echo "@text-internet"
echo "@base-x"
echo "keyutils"
echo "trousers"
echo "fipscheck"
echo "device-mapper-multipath"
echo "postfix"
echo "audit"
echo "net-snmp-utils"
echo "sysstat"
echo "curl"
echo "sblim-sfcc"
echo "sblim-sfcb"
echo "libxslt"
echo "kernel-devel"
echo "gcc"
echo "xorg-x11-server-Xnest"
echo "-lftp"
echo "-words"
echo "-mgetty"
echo "-dosfstools"
echo "-ftp"
echo "rsync"
echo "-nano"
echo "-finger"
echo "-autofs"
echo "-rdate"
echo "-conman"
echo "-pcmciautils"
echo "-mtools"
echo "-dos2unix"
echo "-tcpdump"
echo "-rsh"
echo "-rp-pppoe"
echo "-unix2dos"
echo "-mtr"
echo "-mkbootdisk"
echo "-irda-utils"
echo "-ypbind"
echo "-jwhois"
echo "-rdist"
echo "-nc"
echo "-telnet"
echo "-bluez-utils"
echo "-talk"
echo "-evolution"
echo "-nspluginwrapper"
echo "-evolution-webcal"
echo "-ekiga"
echo "-evolution-connector"
echo "-dovecot"
echo "-spamassassin"
echo "-vnc"
echo "-zisofs-tools"
echo "-nmap"
echo "-xdelta"
echo "-samba-client"
echo "-bluez-hcidump"
echo "-bluez-gnome"
echo "-slrn"
echo "-fetchmail"
echo "-cadaver"
echo ""
echo "%post --log=/root/kickstart_%post.log"
echo ""
echo "### Update all packages on the system"
echo 'YUM="/usr/bin/yum -y"'
echo '$YUM update'
echo '/bin/echo "All System Packages Updated"'
echo ""
echo "### Backup /etc/redhat-release"
echo "/bin/cp /etc/redhat-release /etc/redhat-release_`date +%m%d%y%H%M`"
echo '/bin/echo "/etc/redhat-release Backup Complete"'
echo ""
echo "### Add Red Hat Language to /etc/redhat-release"
echo '/bin/echo "Red Hat Enterprise Linux release 5 (Tikanga)" > /etc/redhat-release'
echo '/bin/echo "/etc/redhat-release Updated to Red Hat"'
echo ""
echo "### Download Openmange 7.0 repo"
echo "/usr/bin/wget http://10.2.0.103/neo/openmanage/OM-SrvAdmin-Dell-Web-LX-7.0.0-4614_A00.tar.gz -O /usr/local/OM-SrvAdmin-Dell-Web-LX-7.0.0-4614_A00.tar.gz"
echo "/bin/tar xzf /usr/local/OM-SrvAdmin-Dell-Web-LX-7.0.0-4614_A00.tar.gz -C /usr/local/"
echo "/usr/local/setup.sh -x -a"
echo '$YUM install perl-Net-SNMP'
echo "/bin/rm -fr /usr/local/linux"
echo "/bin/rm -f /usr/loca/setup.sh /usr/local/COPYRIGHT.txt /usr/local/license.txt /usr/local/OM-SrvAdmin-Dell-Web-LX-7.0.0-4614_A00.tar.gz"
echo '/bin/echo "Dell Openmanage Repo Added"'
echo ""
echo "### Install OpenMange Packages"
echo '$YUM install srvadmin-all'
echo '/bin/echo "OpenManage Installation Complete"'
echo ""
echo "### Disable Openmange repo"
echo "/bin/sed -i 's/enabled=1/enabled=0/' /etc/yum.repos.d/dell-omsa-repository.repo"
echo '/bin/echo "OpenManage Repo Disabled"'
echo ""
echo "### Install Security Package"
echo "RPM="rpm -ivh""
echo '$RPM http://10.2.0.103/neo/centos5/system_security-1.3-0.el5.noarch.rpm'
echo '/bin/echo "Security Package Installed"'
echo "### Set Server Time"
echo "/sbin/service ntpd stop"
echo "/usr/sbin/ntpdate time.vocalocity.com"
echo "/sbin/service ntpd restart"
echo ""
echo "### Create Facts File for Puppet"
case ${__ROLE} in
	hdap_ui )
	echo "/bin/touch /etc/facts.txt"
	echo '/bin/echo "role=hdap_ui" >> /etc/facts.txt'
	echo '/bin/echo "site=savvis" >> /etc/facts.txt'
	echo '/bin/echo "environment=prod" >> /etc/facts.txt'
	echo '/bin/echo "run_crons=false" >> /etc/facts.txt'
	;;
	hdap|cache )
	echo "/bin/touch /etc/facts.txt"
	echo '/bin/echo "role=hdap" >> /etc/facts.txt'
	echo '/bin/echo "site=savvis" >> /etc/facts.txt'
	echo '/bin/echo "environment=prod" >> /etc/facts.txt'
	echo "/bin/echo "cluster_id=${__C_ID}""
	;;
	outboundproxy )
	echo "/bin/touch /etc/facts.txt"
	echo '/bin/echo "role=outboundproxy" >> /etc/facts.txt'
	echo '/bin/echo "site=savvis" >> /etc/facts.txt'
	echo '/bin/echo "environment=prod" >> /etc/facts.txt'
	echo "/bin/echo "obp_cluster_id=${__C_ID}""
	;;
	inboundproxy )
	echo "/bin/touch /etc/facts.txt"
	echo '/bin/echo "role=intboundproxy" >> /etc/facts.txt'
	echo '/bin/echo "site=savvis" >> /etc/facts.txt'
	echo '/bin/echo "environment=prod" >> /etc/facts.txt'
	echo "/bin/echo "ibp_cluster_id=${__C_ID}""
	;;
	registrar )
	echo "/bin/touch /etc/facts.txt"
	echo '/bin/echo "role=registrar" >> /etc/facts.txt'
	echo '/bin/echo "site=savvis" >> /etc/facts.txt'
	echo '/bin/echo "environment=prod" >> /etc/facts.txt'
	echo "/bin/echo "registrar_cluster_id=${__C_ID}""
	;;
	controller )
	echo /bin/touch /etc/facts.txt
	echo /bin/echo "role=controller" >> /etc/facts.txt
	echo /bin/echo "site=savvis" >> /etc/facts.txt
	echo /bin/echo "environment=prod" >> /etc/facts.txt
	echo /bin/echo "cluster_id=${__C_ID}"
	;;
esac	
echo '/bin/echo "/etc/facts.txt created"'
echo ""
echo "### Insert PuppetMaster IP into the /etc/hosts File"
echo "/bin/cp /etc/hosts /etc/hosts_`date +%m%d%y%H%M`"
echo '/bin/echo "10.2.0.100     atlsvppmstr01.atldc.vocalocity.com       atlsvppmstr01" >> /etc/hosts'
echo '/bin/echo "10.2.0.100     puppet.atldc.vocalocity.com       puppet" >> /etc/hosts'
echo '/bin/echo "/etc/hosts updated for Puppet"'
echo ""
echo "### Insert NAS IP Addresses in /etc/hosts file"
echo '/bin/echo "10.2.2.21     atlsvpnas01.atldc.vocalocity.com       atlsvpnas01" >> /etc/hosts'
echo '/bin/echo "10.2.2.23     atlsvpnas03.atldc.vocalocity.com       atlsvpnas03" >> /etc/hosts'
echo '/bin/echo "/etc/hosts updated for NFS"'
echo ""
echo ""
echo "#Install for Puppet and Ruby"
echo "rpm -Uvh http://dl.fedoraproject.org/pub/epel/5/i386/epel-release-5-4.noarch.rpm --quiet 2>&1 | /usr/bin/tee /root/puppet_post.log" 
echo '$YUM install subversion git make gcc libstdc++-devel curl curl* curl-devel libxml2 libxml2* libxml2-devel libxslt libxslt-devel zlib-devel.x86_64 zlib.x86_64 ncompress.x86_64 readline-devel 2>&1 | /usr/bin/tee /root/puppet_post.log'
echo ""
echo "#install ruby 2>&1 | /usr/bin/tee /root/puppet_post.log"
echo "/bin/sed -i -e 's/gpgcheck=1/gpgcheck=0/g' /etc/yum.conf 2>&1 | /usr/bin/tee /root/puppet_post.log"
echo "wget -c http://10.2.0.103/neo/puppet/ruby-enterprise-edition.rpm 2>&1 | /usr/bin/tee /root/puppet_post.log"
echo '$YUM install ruby-enterprise-edition.rpm 2>&1 | /usr/bin/tee /root/puppet_post.log'
echo ". /etc/profile.d/ree-path-set.sh 2>&1 | /usr/bin/tee /root/puppet_post.log"
echo ""
echo "# Create puppet.conf 2>&1 | /usr/bin/tee /root/puppet_post.log"
echo "/bin/mkdir /etc/puppet 2>&1 | /usr/bin/tee /root/puppet_post.log"
echo "/opt/ruby-enterprise-1.8.7-2012.02/bin/puppet agent --genconfig --pluginsync --server=atlsvppmstr01.atldc.vocalocity.com > /etc/puppet/puppet.conf 2>&1 | /usr/bin/tee /root/puppet_post.log"
echo ""
case $__ROLE in
	cache|outboundproxy|inboundproxy|hdap-ui|registrar|controller )
	echo "# Create bond interface"
	echo 'echo 'ETH0_HWADDR='$(/sbin/ifconfig eth0 |/bin/awk '"'"'/HWaddr/ {print '$NF'}'"'"')'''
	echo 'echo 'ETH2_HWADDR='$(/sbin/ifconfig eth2 |/bin/awk '"'"'/HWaddr/ {print '$NF'}'"'"')'''
	echo ""
	echo "echo "#Bond0 Config File""
	echo "echo "DEVICE=bond0" > /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-bond0"
        echo "echo "BOOTPROTO=none" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-bond0"
        echo "echo "ONBOOT=yes" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-bond0"
        echo "echo "NETMASK=${__NETMASK}" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-bond0"
        echo "echo "IPADDR=${__IP}" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-bond0"
        echo "echo "USERCTL=no" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-bond0"
        echo "echo "BONDING_OPTS=mode=1 miimon=100" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-bond0"
	echo ""
	echo "echo "#Eth0 Config File""
	echo "echo "DEVICE=eth0" > /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth0"
        echo "echo "BOOTPROTO=none" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth0"
        echo "echo "ONBOOT=yes" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth0"
        echo "echo "MASTER=bond0" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth0"
        echo "echo "SLAVE=yes" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth0"
        echo "echo "USERCTL=no" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth0"
        echo 'echo "HWADDR=${ETH0_HWADDR}" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth0'
	echo ""
	echo "echo "#Eth2 Config File""
	echo "echo "DEVICE=eth2" > /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth2"
        echo "echo "BOOTPROTO=none" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth2"
        echo "echo "ONBOOT=yes" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth2"
        echo "echo "MASTER=bond0" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth2"
        echo "echo "SLAVE=yes" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth2"
        echo "echo "USERCTL=no" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth2"
        echo 'echo "HWADDR=${ETH2_HWADDR}" >> /mnt/sysimage/etc/sysconfig/network-scripts/ifcfg-eth2'
	echo ""
	echo "echo "#Load the bonding driver""
	echo "echo "alias bond0 bonding" >> /mnt/sysimage/etc/modprobe.conf"
	;;
esac
echo "%end"
echo ""
