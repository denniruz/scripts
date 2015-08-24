#!/bin/bash 
# Backup github repos in an organization
#
_CURL_CMD="/usr/bin/curl"
_RUBY_CMD="/usr/bin/ruby"
_ORGANIZATION="ArnoldMediaConsulting"
#
# Read in Username and Password
echo -n " Username: "
read _USERNAME
echo -n " Password: "
read _PASSWORD
# 
${_CURL_CMD} -u ${_USERNAME}:${_PASSWORD} -s https://api.github.com/orgs/${_ORGANIZATION}/repos?per_page=200 | ${_RUBY_CMD} -rubygems -e 'require json; JSON.load(STDIN.read). each { |repo| %x[git clone #{repo[ssh_url]} ]}' 2>&1 



