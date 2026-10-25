/* $Id: portlistingparse.h,v 1.10 2014/11/01 10:37:32 nanard Exp $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2011-2015 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */
#ifndef PORTLISTINGPARSE_H_INCLUDED
#define PORTLISTINGPARSE_H_INCLUDED

#include "miniupnpc_declspec.h"
 
#include "miniupnpctypes.h"

#ifdef __cplusplus
extern "C" {
#endif

 
 
 
 
 
 
 
 
 
 
 
 
typedef enum { PortMappingEltNone,
       PortMappingEntry, NewRemoteHost,
       NewExternalPort, NewProtocol,
       NewInternalPort, NewInternalClient,
       NewEnabled, NewDescription,
       NewLeaseTime } portMappingElt;

struct PortMapping {
	struct PortMapping * l_next;	 
	UNSIGNED_INTEGER leaseTime;
	unsigned short externalPort;
	unsigned short internalPort;
	char remoteHost[64];
	char internalClient[64];
	char description[64];
	char protocol[4];
	unsigned char enabled;
};

struct PortMappingParserData {
	struct PortMapping * l_head;	 
	portMappingElt curelt;
};

MINIUPNP_LIBSPEC void
ParsePortListing(const char * buffer, int bufsize,
                 struct PortMappingParserData * pdata);

MINIUPNP_LIBSPEC void
FreePortListing(struct PortMappingParserData * pdata);

#ifdef __cplusplus
}
#endif

#endif
