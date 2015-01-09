#!/bin/bash

include bash.io.Filesystem;

function bash.xml.Simple()
{
	__XML_PARSER="awk -f $__BF_LIB/xml/xmlparser.awk"

	__XBEGIN="BEGIN"
	__XEND="END"
	__XATTR="ATTRIB"
	__XVAL="VALUE"
	__XDATA="DATA"

	#Need to use this filter somehow
	#__DATA_FILTER="sed -e '/^data \(\\n\)*\s*$/d' -e 's@^\(data \)\(\\n\)*@\1@g' -e 's@$\(\\n\)*\s*@@g'"
	#Cant even use this filter, because of escaping. Need more time to figure this one out
	#__DATA_FILTER="sed -e '/^data \(\\n\)\s*\$/d'"

	return 0;
}

#==============================
# Usage: XML.loadFromFile
#	$1 - path to the xml file
#==============================
function XML.loadFromFile()
{
	Filesystem.checkFile "$1"
	#Run the parser
	#In case of an error, $__XML_DATA will contain the error string
	__XML_DATA=`$__XML_PARSER $1`
	if [ "$?" -ne 0 ]; then
		test -z "$__XML_DATA" || Logger.error "$__XML_DATA";
		Logger.fatal "Could not parse XML file '$1'"
	fi
	#Filter the output
	echo "$__DATA_FILTER"
	REGEXP='/^DATA \(\\n\)*\s*$/d'
	__XML_DATA="`echo "$__XML_DATA" | sed -e "$REGEXP"`"
	XML.parse "$__XML_DATA" \
		|| Logger.fatal "Could not interpret the XML data loaded from file '$1'".
}

#==============================
# Usage: XML.parse
#	$1 - parses an XML string
#==============================
function XML.parse()
{
	echo "---------- DATA ----------"
	for line in "$1"; do
		echo "$1"
	done


	echo "--------------------------"
}

#Call the constructor
bash.xml.Simple

