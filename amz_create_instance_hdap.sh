#!/bin/bash

# chad crapware .02v
# Updated by Dave Z
# Makes AMZ instances like whoa.

# NOTE - This script isn't perfect yet. And the documentation isn't very good... It just sort of works for me.
# This only makes HDAP call handlers reliably right now.
# This also requires you to export the proper EC2 security credentials before you run this.
# Future updates will include support for other roles - transcoders and caches are first in line.

# Usage:
# ./amz_create_instance_hdap.sh ROLE ZONE FQDN AMI PUPPETMASTER TYPE CLUSTER-ID VPC-ID
#
# ROLE - The type of server to be created
# ZONE - The region the instance will be added to. This also determines the server's VPC/subnet ID.
# FQDN - The instance's fully qualified hostname
# AMI - The ID of the AMI to use in creating the server.
# PUPPETMASTER - FQDN of the server's target puppetmaster.
# TYPE - The server size, as defined by Amazon EC2's type list.  Default is m1.large.  Other options: c1.xlarge, m2.xlarge, etc...
# CLUSTER-ID - The AMZ1 cluster for the new instance.
# SG-ID - The security group ID.  Default is sg-c9eaf1a5 (Application_Zone) and this probably won't have to change much.

# Usage example:
# ./amz_create_instance_hdap.sh hdap us-east-1a amz1vpc1call99.amz1.vocalocity.com ami-a986f3c0 amz1ppmstr1.amz1.vocalocity.com c1.xlarge amz1vpc4 sg-c9eaf1a5

# $1 = ROLE (aka hdap)
# $2 = ZONE (aka: us-east-1d)
# $3 = FQDN (aka: amz1vpc4call100.amz1.neo.vocalocity.com)
# $4 = AMI (aka: ami-5a385933)
# $5 = puppet master (aka: amz1ppmstr1.amz1.vocalocity.com)
# $6 = size (aka: m1.large)
# $7 = Cluster ID (aka: amz1vpc4)
# $8 = Security Group (aka: sg-c9eaf1a5) 


# This aligns zone 1a, 1b, 1d to proper SGID
# THIS IS RELEVANT TO AMZ1 VPC ONLY
if [[ "$2" == us-east-1a ]]; then
SUBNET=subnet-79b11e12
fi
if [[ "$2" == us-east-1b ]]; then
SUBNET=subnet-8cd2e2e4
fi
if [[ "$2" == us-east-1d ]]; then
SUBNET=subnet-930dd8fe
fi
#

ROLE=$1
ZONE=$2
HOSTNAME=$3
AMI=$4
SIZE=$6
CLUSTERID=$7
SECGROUP=$8

# static stuff env stuff for aws
EC2_HOME=/cygdrive/c/AWS/EC2/ec2-api-tools-1.6.7.3
AWS_IAM_HOME=${EC2_HOME}/bin
JAVA_HOME=/usr
EC2_PRIVATE_KEY=${EC2_HOME}/pk-XROC6KTXIS6DY32JMPNOWPHWGI32CJ4G.pem
EC2_CERT=${EC2_HOME}/cert-XROC6KTXIS6DY32JMPNOWPHWGI32CJ4G.pem
USER_FACT=user-data-flatfile
export SECGROUP CLUSTERID ROLE ZONE SUBNET SIZE HOSTNAME AMI USER_FACT EC2_CERT EC2_PRIVATE_KEY JAVA_HOME AWS_IMA_HOME EC2_HOME

# do stuff now
cat > $USER_FACT <<USERDATA
:facts:
  environment: prod
  site: amz1
  role: $1
  puppetmaster: $5
  cluster_id: $7
:actions:
- :type: :sethostname
  :hostname: $HOSTNAME
- :type: :puppetagent
  :genconfig: true
  :run: agent --test
USERDATA

# call api scripts with above vars
echo Creating instance...
ec2-run-instances -t $SIZE -k amz1 -f $USER_FACT -s $SUBNET -z $ZONE -g $SECGROUP $AMI > output
#$AWS_IAM_HOME/ec2-create-tags `cat output |grep -v RESERVATION|awk -F " " '{print $2}'|head -1` -t Name=`grep ":hostname:" $USER_FACT|cut -d: -f3|cut -d. -f1|cut -d" " -f2` > nameout
echo Setting Tag 'Name' to $HOSTNAME...
ec2-create-tags `cat output | grep -v RESERVATION | awk -F " " '{print $2}' | head -1` -t Name=`grep ":hostname:" user-data-flatfile | cut -d: -f3 | cut -d " " -f2` > nameout

# done
echo Done.
exit 0

