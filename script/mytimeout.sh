#!/bin/bash
###############################################################################
# Script to abort a command after a given time is passed and it didnt return
#   (a custom message is printed)
# Author: Marcos Daniel V. Baroni
# Date: Jan 10, 2015
###############################################################################
if [[ $# -le 2 ]] ; then
	echo "usage ./mytimeout <time> <abort message> <command>"
	echo " <time>: #s for seconds"
	echo "         #m for minutes"
	echo ""
	echo "   Ex.: ./mytimeout 60s \"Tooo loooong\" ./scip -f knap.zpl"
    exit 0
fi

maxsecs=$1
shift
msg=$1
shift
command=$*

ret=`timeout ${maxsecs} $command`
if [[ -n $ret ]]; then
    echo $ret
else
    echo $msg
fi

