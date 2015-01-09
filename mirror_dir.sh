#!/bin/bash 
dirs=$(/usr/bin/find /usr -type d -print)
for directory in ${dirs}; do
    mkdir -p /home/druzeski/${directory}
done

