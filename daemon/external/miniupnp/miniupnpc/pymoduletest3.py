#! /usr/bin/python3
# MiniUPnP project
# Author : Thomas Bernard
# This Sample code is public domain.
# website : http://miniupnp.tuxfamily.org/

 
import miniupnpc
import sys

 
u = miniupnpc.UPnP()
print('inital(default) values :')
print(' discoverdelay', u.discoverdelay)
print(' lanaddr', u.lanaddr)
print(' multicastif', u.multicastif)
print(' minissdpdsocket', u.minissdpdsocket)
u.discoverdelay = 200;
 
 
print('Discovering... delay=%ums' % u.discoverdelay)
print(u.discover(), 'device(s) detected')
 
try:
  u.selectigd()
except Exception as e:
  print('Exception :', e)
  sys.exit(1)
 
print('local ip address :', u.lanaddr)
print('external ip address :', u.externalipaddress())
print(u.statusinfo(), u.connectiontype())

 
 
 

port = 0
proto = 'UDP'
 
i = 0
while True:
	p = u.getgenericportmapping(i)
	if p==None:
		break
	print(i, p)
	(port, proto, (ihost,iport), desc, c, d, e) = p
	 
	i = i + 1

print(u.getspecificportmapping(port, proto))

