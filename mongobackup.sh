#!/bin/bash -x
#
## FILE: mongobackup.sh
##
## DESCRIPTION: Comprehensive mongodb backup/restore script
##
## AUTHOR: Dennis Ruzeski (denniruz@gmail.com)
##
## Creation Date: 2.5.2016
##
## Last Modified: 2.5.2016
##
## VERSION: 1.0
##
## Begin script
##
# Define some variables

_hostname="$(/bin/hostname)"
_nodetype="$(/opt/cronos/bin/user-data.py -k nodetype)"
_backup_dir="/mnt/backups/mongodb"
_s3path="/backups/mongodb/nonprod/$(/bin/date +%Y)/$(/bin/date +%m)/$(/bin/date +%d)/$(/bin/date +%H)"
_logfile="/var/log/mongo/mongobackup.log"
_mongo_backup_cmd="/usr/bin/mongodump"
_mongo_restore_cmd="/usr/bin/mongorestore"
_backup_name="${_nodetype}-${_hostname}-${_db_name}"
_s3_logfile="/var/lib/mongo/s3_list"
declare -a _db_list=( $(awk -F\" '!/local/ {print $2}' <(mongo localhost:27018 --eval "printjson(db.getMongo().getDBNames())")) )
# Set some functions
usage() {
    cat <<EOF
    usage: $0 options
    Backup or restore Mongo instances or databases

    OPTIONS:
      -u    Upload backups to s3
      -h    Show this message
      -b    Backup all databases
      -r    Restore 
      -d    Backup a specific database

      EXAMPLES:
      Backup all databases locally:
      mongobackup.sh -b
      
      Backup all databases and upload tarballs to s3:
      mongobackup -b -u
      
      Backup a single database and upload to s3:
      mongobackup -d <database_name> -u

      mongobackup -r will open an interactive dialog to select a database to restore. 
      More functionality with restores to follow.


EOF
}

if [[ $# -eq 0 ]]
then
    usage
    exit 0
fi

restore() {
    echo "Under Construction"
}

backup() {
    if [[ ${_nodetype} =~ ^ar ]] || [[ ${_nodetype} =~ ^tm ]]
    then
        _mongoopts=" --ssl"
    else
        _mongoopts=""
    fi
    if [[ ${_single_db_backup} == "" ]]
    then
        _db_opts=""
    else
        _db_opts=" -d ${_single_db_backup}"
    fi
    ${_mongo_backup_cmd} -h localhost:27018 ${_mongoopts} ${_db_opts} -o ${_backup_dir} >> ${_logfile} 2>&1
}

archive() {
    for _db_name in ${_db_list[@]}
    do 
        echo ${_db_name}
	tar jcf ${_backup_dir}/${_nodetype}-${_hostname}-${_db_name}.tar.bz2 ${_backup_dir}/${_db_name} >> ${_logfile} 2>&1
        if [[ ${_upload2aws} == "1" ]]
        then
            /opt/cronos/bin/s3Put.py -e -f ${_backup_dir}/${_nodetype}-${_hostname}-${_db_name}.tar.bz2 -o ${_s3path}/${_nodetype}-${_hostname}-${_db_name}.tar.bz2 >> ${_logfile} 2>&1
            # Log backups to a local file for easy restore
            echo ${_s3path}/${_nodetype}-${_hostname}-${_db_name}.tar.bz2 >> ${_s3_logfile}
        fi
    done
}

# Parse command line
while getopts "hrubd:" _OPTION
do
    case ${_OPTION} in
        b)  _backup="1"
            echo "Backing up all Mongo Databases" >&2
            ;;
        r)  echo "under construction" >&2
            ;;
        d)  _single_db_backup="${OPTARG}"
            echo "Backing up ${_single_db_backup}" >&2
            ;;
        u)  _upload2aws="1"
            ;;
        h)  usage
            ;;
        ?) echo "Invalid Option: -${OPTARG}" >&2
            usage
            ;;
    esac
done


# The meat and potatoes
if [[ ${_backup} == "1" ]] || [[ ${_single_db_backup} != "" ]]  
then
    backup
elif [[ ${_upload2aws} == "1" ]]
then
  #  backup
    archive
fi

