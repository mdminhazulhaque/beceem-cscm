#!/bin/sh
### BEGIN INIT INFO
# Provides:          beceem-cscm
# Required-Start:    $network $local_fs $remote_fs
# Required-Stop:     $network $local_fs $remote_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Starts Beceem CSCM Daemon
# Description:       This service starts Beceem CSCM Daemon, so you can control Beceem WiMAX Devices using cscm
### END INIT INFO

# Author: Muhammad Minhazul Haque <mdminhazulhaque@gmail.com>

PATH=/sbin:/usr/sbin:/bin:/usr/bin
DESC='Starts Beceem CSCM Daemon'
NAME=beceem-cscm
DAEMON=/usr/bin/wimaxd
DAEMON_ARGS="-c /etc/beceem-cscm/beceem-cscm.conf"
SCRIPTNAME=/etc/init.d/beceem-cscm

# Exit if the package is not installed
[ -x $DAEMON ] || exit 0

do_start()
{
	PID=$(pidof wimaxd)
	if [ $? -eq 0 ]
	then
	  echo "Stopping beceem-cscm daemon"
	  /usr/bin/wimaxc server stop
	  sleep 1
	fi
	echo "Starting beceem-cscm daemon"
	$DAEMON $DAEMON_ARGS
}

do_stop()
{
	PID=$(pidof wimaxd)
	if [ $? -eq 0 ]
	then
	  echo "Stopping beceem-cscm daemon"
	  /usr/bin/wimaxc server stop
	  sleep 1
	else
	  echo "beceem-cscm daemon is not running"
	fi
}

do_reload()
{
	do_stop
	sleep 1
	do_start
}

do_status_check()
{
	PID=$(pidof wimaxd)
	if [ $? -eq 0 ]
	then
	  echo "beceem-cscm daemon is running"
	else
	  echo "beceem-cscm daemon is not running"
	fi
}

case "$1" in
  start)
	do_start
	;;
  stop)
	do_stop
	;;
  status)
	do_status_check
       ;;
  restart|reload)
	do_reload
       ;;
  *)
	echo "Usage: $SCRIPTNAME {start|stop|status|restart|reload}" >&2
	exit 3
	;;
esac

:
