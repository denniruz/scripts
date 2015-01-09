#!/bin/bash
# 
# Quick script to create a function to check that all nfs mounts in fstab are mounted.

nfs_mounts=( `grep -v ^\# /etc/fstab |grep nfs |awk '{print $2}'` )

for mount_type in $nfs_mounts ; do 
	if [ `stat -f -c '%T' $mount_type` = nfs ]; then 
		echo "$mount_type is properly mounted\n" 
	else 
		echo "$mount_type is not mounted"
		echo "Please check that all nfs mounts are present before running jbossx"
		exit 0
	fi;
done



