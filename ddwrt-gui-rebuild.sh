#!/bin/bash

WEBDIR="$1"
DIR="$2"

if [ "$DIR" == "" ]
then
	DIR="fmk/rootfs"
fi

if [ "$WEBDIR" == "" ]
then
	WEBDIR="www"
fi

eval $(cat shared-ng.inc)
HTTPD="$DIR/usr/sbin/httpd"
WWW="$DIR/etc/www"
#KEYFILE="$DIR/webcomp.key"

echo -e "Firmware Mod Kit (ddwrt-gui-rebuild) $VERSION, (c)2011 Craig Heffner, Jeremy Collake\nhttp://www.bitsum.com\n"

if [ ! -d "$DIR" ] || [ "$1" == "-h" ] || [ "$1" == "--help" ]
then
	echo -e "Usage: $0 [input directory] [rootfs directory]\n"
	exit 1
fi

if [ ! -e "$HTTPD" ] || [ ! -e "$WWW" ]
then
	echo "Unable to locate httpd / www files in directory $DIR. Quitting..."
	exit 1
fi

# Restore!
./src/webcomp-tools/webdecomp --httpd="$HTTPD" --www="$WWW" --dir="$WEBDIR" --restore

