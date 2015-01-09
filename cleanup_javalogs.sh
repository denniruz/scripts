#!/bin/sh

## Java logs
find /usr/local/jboss/standalone/log/  -mtime +3  -name \*log.2\* -exec rm {} \;

## Old core files
find /usr/local/jboss/bin/  -mtime +4  -name core.\* -exec rm {} \;

##  Old CallLogs

find /usr/local/vocalocity/vocalos/callLogs/  -type f -mtime +15  -exec rm -f {} \;

## Recording cache
find /usr/local/vocalocity/vocalos/recording-cache/ -type f -mtime +9  -exec rm {} \;

## /var/tmp/archive cleanup
find /var/tmp/archive/ -type f -name \*.pi\*  -mtime +21  -exec rm {} \;

find /usr/local/vocalocity/vocalos/log/  -type f -name \*log\*  -mtime +8  -exec rm {} \;

## /usr/local/vocalocity/hdap/ cleanup
find /usr/local/vocalocity/hdap/  -type f -name \*.log\*  -mtime +8  -exec rm {} \;


find /usr/local/vocalocity/vocalos/log/  -type f -name \*log\*  -mtime +8  -exec rm {} \;

## /usr/local/jboss/bin/hdap/ cleanup
find /usr/local/vocalocity/hdap/  -type f -name \*.log\*  -mtime +8  -exec rm {} \;

## /var/log/jboss-as/restarts/ cleanup
find /var/log/jboss-as/restarts/  -type f -name jboss.console-\*  -mtime +5  -exec rm {} \;

## /usr/local/jboss/bin/reports/logs/ cleanup
find /usr/local/jboss/bin/reports/logs/  -type f -name jboss.console-\*  -mtime +9  -exec rm {} \;

## Find /tmp/ files in media
#  SoundRecorder4954280508360669975.mp3
find /tmp/ -type f -name  \*mp3 -mtime +3 -exec rm {} \;
find /tmp/ -type f -name  \*ulaw -mtime +3 -exec rm {} \;
