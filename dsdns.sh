#!/bin/bash
# 
## FILE: check_pdns.sh
##
## DESCRIPTION: A remote check for DNS resolution
##
## AUTHOR: Dennis Ruzeski (denniruz@gmail.com)
##
## Creation Date: 1/23/2013
##
## Last Modified: 1/25/2013
## 
## VERSION: 1.0
##
## USAGE: ./check_pdns.sh -c <host> <record_type> <DNS_server>
##
## TODO: Add an option for output type (nagios check, command line), Add improved AXFR checks
#
# Set some variables
PROGNAME=`basename $0`
REVISION=1.00_0
DIG_CMD=/usr/bin/dig
E_OPTERROR=85
NO_ARGS=2
# This script requires dig
if [[ ! -e ${DIG_CMD} ]]; then
    echo "This script requires that dig is installed"
    exit 2
fi
# Check for proper CL ARGS
if [[ $# -ne "${E_OPTERROR}" ]]
    _usage
    exit 
    
_revision() {
	echo $PROGNAME $REVISION
}

_usage() {
	echo "Usage:"
	echo "	${PROGNAME} <host> <type> <server>"
}
_check_dns() {
    "${DIG_CMD}" "${1}" @"${2}" | grep -q NOERROR
    if [[ $? -eq 0 ]]; then    
        echo "OK: DNS Query returned with no errors"
        exit 0
    else
        echo "CRITICAL: DNS Query FAILED!"
        exit 2

_check_query_time() {
    _check_dns 
    if [[ $? -eq 0 ]];then
        if ${DIG_CMD}" "${1}" @"${2}" |awk '/time:/ {print $4}'
	
}

