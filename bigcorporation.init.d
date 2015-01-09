#!/bin/bash
#
# chkconfig: 345 99 01
# description: Starts the Big Corporation Prepaid SMI instance
#

# Source function library.
# Find out how we where called.

INSTANCE_NAME=`basename $0`
## NJS: Following single line added to deal with basename not following multiple symlinks and basename == S99<instance> instead of just <instance> :
INSTANCE_NAME=$(echo "$INSTANCE_NAME" | sed -e 's/^[SK][0-9][0-9]//g')
INSTANCE_NAME_UPPER=$(echo "$INSTANCE_NAME" | tr '[:lower:]' '[:upper:]')

. /etc/rc.d/init.d/functions
if [ -f /etc/sysconfig/bigcorporation-$INSTANCE_NAME-alternatives ]; then
    . /etc/sysconfig/bigcorporation-$INSTANCE_NAME-alternatives
fi
. /etc/sysconfig/bigcorporation

# Allow for instance specific configuration options
if [ -f "/etc/sysconfig/bigcorporation-$INSTANCE_NAME" ]
then
    . /etc/sysconfig/bigcorporation-$INSTANCE_NAME
fi

eval JNP_PORT=\$${INSTANCE_NAME_UPPER}_JNP_PORT
eval SERVER_NAME=\$${INSTANCE_NAME_UPPER}_SERVER_NAME
DATE=$(date +%Y-%m-%d-%H-%M-%S)
prog=$INSTANCE_NAME

if [ "$LOGNAME" = "$JBOSSUS" ]
then
  SU="eval"
fi

function bind_port(){
sed -i -e "s/<attribute name=\"ServerName\"><\/attribute>/<attribute name=\"ServerName\">$SERVER_NAME<\/attribute>/" $INST_HOME/conf/jboss-service.xml
}

function cleanup() {
  rm -rf $INST_HOME/data
  rm -rf $INST_HOME/work
  rm -rf $INST_HOME/tmp
}

function start() {

  if [ -f "$INST_LOCK_FILE" ]
  then
    RETVAL=1
    echo -n $"$INSTANCE_NAME already running: "
    echo_failure
    echo
    return 0
  fi

  echo -n $"Starting JBoss for $INSTANCE_NAME: "
  bind_port
  cleanup

  daemon `$SU "$CMD_START -b $BINDIP -c $INSTANCE_NAME >$LOG 2>&1 &"`
  RETVAL=$?
  echo

  sleep 4
  INSTANCE_PID=$(ps -u jboss -ww -o user,pid,command | grep "/opt/${INSTANCE_NAME}" | grep 'Dprogram.name=run.sh' | grep 'bin/java ' |grep 'Xms' | grep 'Xmx' | grep "c ${INSTANCE_NAME}" | awk '{ gsub(/[[:space:]]*/,"",$2); print $2 }')

  [ $RETVAL -eq 0 ] && touch $INST_LOCK_FILE && echo "$INSTANCE_PID" > $INST_PID_FILE
  return $RETVAL
}

function stop() {
  # Grab the instance PID.
  if [ -e $INST_PID_FILE ]
  then
	 INSTANCE_PID=`cat $INST_PID_FILE`
  fi

  # Check if the instance is running
  if [ ! -e /proc/$INSTANCE_PID/status ] && [ ! -e $INST_LOCK_FILE ] && [ ! -e $INST_PID_FILE ]
  then
      echo -n $"$INSTANCE_NAME is not running: "
      echo_failure
      echo
      return 0
  fi

  echo -n $"Stopping JBoss for $INSTANCE_NAME: "

  # Try stopping the instance via the JNP Port
  $SU "$CMD_STOP -s ${BINDIP}:${JNP_PORT} $CMD_STOP_OPTS" &> /dev/null &
  TWIDDLE_PID=$!
  disown $TWIDDLE_PID
  COUNTER=0
  while [ $COUNTER -lt $TWIDDLE_TIMEOUT ]
    do
    if [ ! -e /proc/$TWIDDLE_PID/status ] && [ ! -e /proc/$INSTANCE_PID/status ]
    then
      RETVAL=0
      break
    else
      RETVAL=1
    fi
    sleep 1
    let COUNTER=COUNTER+1
  done

  if [ -e /proc/$TWIDDLE_PID/status ]
    then
    kill -9 $TWIDDLE_PID > /dev/null
  fi

  if [ $RETVAL ==  "0" ]
  then
    echo_success
    echo
    cleanup
    rm -f $INST_LOCK_FILE
    rm -f $INST_PID_FILE
    return $RETVAL
  else
    echo "Shutting down instance via JNP did not work... trying Kill."
    kill -9 $INSTANCE_PID

	 sleep 5

    if [ ! -e /proc/$INSTANCE_PID/status ]
    then
      echo_success
      cleanup
      rm -f $INST_LOCK_FILE
      rm -f $INST_PID_FILE
    	return 0
    else
		echo_failure
		return 1
	 fi
  fi

  if [ -e /proc/$INSTANCE_PID/status ] || [ -e $INST_LOCK_FILE ] || [ -e $INST_PID_FILE ]
  then
    echo_failure
	 return 1
  fi

}


function restart() {
stop
sleep 5
start
}

function showenv() {
  set
}

case "$1" in
  start)
    start
    ;;
  stop)
    stop
    ;;
  restart)
    restart
    RETVAL=$?
    ;;
  status)
    status $INSTANCE_NAME
    RETVAL=$?
    ;;
  *)
    echo "usage: $0 (start|startwithlog|stop|restart|showenv|status)"
    RETVAL=3
esac

exit $RETVAL
