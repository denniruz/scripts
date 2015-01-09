#!/bin/bash

source function_amazon_env_cred.sh

#export EC2_HOME=/root/ec2-api-tools
#export JAVA_HOME=/usr

SERVER=`uname -n | cut -d. -f1`
DATE=`date +%m-%d-%Y`
DATEONEWEEKAGO=`date -d '1 year ago' +%Y-%m-%dT%H:%M:%S+0000`
OUTFILE=/tmp/amazon-morning_check-$DATE.$SERVER

#cat $OUTFILE | mail -s 'Daily Report - Amazon EC2 Instance Health Check' david.zielinski@vocalocity.com

echo "***** AMAZON EC2 INSTANCE HEALTH CHECK *****" > $OUTFILE
echo >> $OUTFILE

function get_amazon_instance_health {
  echo "========================" >> $OUTFILE
  echo $1 >> $OUTFILE
  echo "========================" >> $OUTFILE
  #OUTPUT=`ec2-describe-instance-status --filter "event.not-before=$DATEONEWEEKAGO"` >> $OUTFILE
  OUTPUT=`ec2-describe-instance-status --filter "event.description=*"` >> $OUTFILE
  if [ -z "$OUTPUT" ]; then
    echo "All instances in this environment are healthy." >> $OUTFILE
  else
    echo $OUTPUT >> $OUTFILE
  fi
  echo >> $OUTFILE
}

#amz1_aws
#get_amazon_instance_health 'Production (AMZ1)'
aptela_aws
get_amazon_instance_health 'Aptela'
#dev_aws
#get_amazon_instance_health 'Development'
#lincdoc_aws
#get_amazon_instance_health 'LincDoc'
#neo_aws
#get_amazon_instance_health 'NEO'
#www_aws
#get_amazon_instance_health 'WWW'

#cat $OUTFILE | mail -s 'Daily Report - Amazon EC2 Instance Health Check' david.zielinski@vocalocity.com
cat $OUTFILE
