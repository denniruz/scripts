#!/bin/bash
USERNAME="amc_admin"  
PASSWORD=";lkajsdf09aje 894e2"
INSTANCE="arnoldmedia.jira.com"
LOCATION=
 
TODAY=`date +%Y%m%d`
COOKIE_FILE_LOCATION=jiracookie
curl --silent -u $USERNAME:$PASSWORD --cookie-jar $COOKIE_FILE_LOCATION https://${INSTANCE}/Dashboard.jspa --output /dev/null
BKPMSG=`curl -s --cookie $COOKIE_FILE_LOCATION --header "X-Atlassian-Token: no-check" -H "X-Requested-With: XMLHttpRequest" -H "Content-Type: application/json"  -X POST https://${INSTANCE}/rest/obm/1.0/runbackup -d '{"cbAttachments":"true" }' `
rm $COOKIE_FILE_LOCATION
 
if [ `echo $BKPMSG | grep -i backup | wc -l` -ne 0 ]; then
    exit
fi

sleep 14400
 
for (( c=1; c<=20; c++ ))
do
    wget --user=$USERNAME --password=$PASSWORD --spider https://${INSTANCE}/webdav/backupmanager/JIRA-backup-${TODAY}.zip >/dev/null 2>/dev/null
    OK=$?
    if [ $OK -eq 0 ]; then
        break
    fi
    sleep 900
done
 
if [ $OK -ne 0 ];
then
    exit
else
    wget --user=$USERNAME --password=$PASSWORD -t 0 --retry-connrefused https://${INSTANCE}/webdav/backupmanager/JIRA-backup-${TODAY}.zip -P $LOCATION >/dev/null 2>/dev/null
fi
