#!/bin/bash

#for i in 54.236.100.141 54.236.90.182 205.139.46.232 205.139.46.233
for i in 10.4.0.10 205.139.46.232 205.139.46.233
do
echo
echo $i
echo
 dig @$i accounts.vocalocity.com axfr | grep -i soa
 dig @$i media.vocalocity.com axfr | grep -i soa
 dig @$i accounts.vocalos.com axfr | grep -i soa
 dig @$i media.vocalos.com axfr | grep -i soa
echo


sleep 1
done


