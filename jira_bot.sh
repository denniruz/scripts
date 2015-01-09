#!/bin/bash
# 
## FILE: 
##
## DESCRIPTION: 
##
## AUTHOR: Dennis Ruzeski (denniruz@gmail.com)
##
## Creation Date: 
##
## Last Modified: 
## 
## VERSION: 1.0
##
## USAGE: 
## 
## TODO:  
##       
## Begin script
# Set some variables-
JIRA="vocalocity.atlassian.net"
JIRA_USER="druzeski"
JIRA_PASS="tinfish83"
release="${1}

/opt/atlassian-cli/jira.sh \
            --server ${JIRA} \
            --user ${JIRA_USER} \
            --password ${JIRA_PASS} \
            -a createIssue \
            --project PCR \
            --type Task \
            --summary "Deploy ${release} in production" \
            --description "deploy puppet release ${release} "      
