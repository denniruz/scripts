#!/bin/bash

die () { echo -e "$*" >&2; exit 1; }

cnt=0
sshConfig="$HOME/.ssh/config"

[ -e "$sshConfig" ] || die "ERROR: ssh_config $sshConfig missing"

hostArray=( `grep ^Host $sshConfig | awk '{print $2}'` )

if [ -z "$1" ]; then
    for i in "${hostArray[@]}"
    do
        echo "$cnt -- $i"
        let "cnt = cnt + 1"
    done

    read pick
else
    pick="$1"
    shift
fi



ssh ${hostArray[$pick]} $@

