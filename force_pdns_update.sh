#!/bin/bash

for i in 10.60.1.197 10.60.2.67 54.236.100.141 54.236.90.182 205.139.46.232 205.139.46.233


do
echo
echo $i
 pdns_control notify-host accounts.vocalocity.com  $i
 pdns_control notify-host media.vocalocity.com  $i
 pdns_control notify-host accounts.vocalos.com  $i
 pdns_control notify-host media.vocalos.com  $i
 sleep 3
done


# 10.60.6.40 newpdns1 # 54.236.90.182
# 10.60.4.28 newpdns2 # 54.236.100.141

echo "sleeping 30 seconds"
sleep 30
echo sleep_is_over

for i in 54.236.100.141 54.236.90.182 205.139.46.232 205.139.46.233
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


