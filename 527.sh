#!/bin/bash
set -e
# Set CHROOT directory name
BASE="/nginx"
file="$@"
 
sync_suppot_libs(){
	local d="$1"         	# JAIL ROOT
	local pFILE="$2"        # copy bin file libs
	local files=""
	local _cp="/bin/cp"
 
	# get rid of blanks and (0x00007fff0117f000)
	files="$(ldd $pFILE |  awk '{ print $3 }' | sed -e '/^$/d' -e '/(*)$/d')"
 
	for i in $files
	do
	  dcc="${i%/*}"	# get dirname only
	  [ ! -d ${d}${dcc} ] && mkdir -p ${d}${dcc}
	  ${_cp} -f $i ${d}${dcc}
	done
 
	# Works with 32 and 64 bit ld-linux
	sldl="$(ldd $pFILE | grep 'ld-linux' | awk '{ print $1}')"
	sldlsubdir="${sldl%/*}"
	[ ! -f ${d}${sldl} ] && ${_cp} -f ${sldl} ${d}${sldlsubdir}
}
 
usage(){
	echo "Syntax : $0 /usr/local/nginx/sbin/nginx"
	echo "Example: $0 /usr/bin/php5-cgi"
	exit 1
}
 
[ $# -eq 0 ] && usage
[ ! -d $BASE ] && mkdir -p $BASE
 
# copy all files
for f in $file
do
	sync_suppot_libs "${BASE}" "${f}"
done
