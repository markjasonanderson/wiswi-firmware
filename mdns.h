/*
 * mdns.h
 *
 *  Created on: 19 Sep 2014
 *      Author: manderson
 */

#ifndef MDNS_H_
#define MDNS_H_

typedef struct mDNS
{
  char *service_name;
  char *c_text;
  unsigned short port;
  unsigned long ttl;
  unsigned long flags;
} mDNS;

int mdns_register(void);
int mdns_intitialise(void);

#endif /* MDNS_H_ */
