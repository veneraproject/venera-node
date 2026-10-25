#!/bin/sh
 
# (c) 2010 Reuben Hawkins
upnpc -s | sed -n -e 's/^ExternalIPAddress = \([0-9.]*\)$/\1/p'
