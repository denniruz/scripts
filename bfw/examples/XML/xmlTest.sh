#!/bin/bash
. ../../include

include bash.Framework
include bash.xml.Simple

function main()
{
	XML_FILE="./test3.xml"
}

function loadXMLAction()
{
	XML.loadFromFile $XML_FILE
}

dispatch
