/*
 * mdns.c
 *
 *  Created on: 19 Sep 2014
 *      Author: manderson
 */
#include "mdns.h"
#include <stdio.h>

#include "log.h"
// Simplelink includes
#include "simplelink.h"

static mDNS mdns;

int mdns_intitialise(void)
{
  _u8 macAddressVal[SL_MAC_ADDR_LEN];
  _u8 macAddressLen = SL_MAC_ADDR_LEN;
  static char c_text[200];

  sl_NetCfgGet(SL_MAC_ADDRESS_GET,NULL,&macAddressLen,(_u8 *)macAddressVal);

  snprintf(c_text, sizeof(c_text), "id: %02x%02x%02x%02x%02x%02x",
    macAddressVal[0],macAddressVal[1],macAddressVal[2],
    macAddressVal[3],macAddressVal[4],macAddressVal[5]
  );

  mdns = (mDNS)
  {
    .service_name = "wiswi._http._tcp.local",
    .c_text = c_text,
    .port = 80,
    .ttl = 2000,
    .flags = 1
  };

  //TODO: need to dynamically work out the switch ids
  return sl_NetAppMDNSUnRegisterService(0, 0);
}

int mdns_register(void)
{
  long result = -1;
  mdns_intitialise();

  result = sl_NetAppMDNSRegisterService(
    mdns.service_name,
    (unsigned char)strlen(mdns.service_name),
    mdns.c_text,
    (unsigned char)strlen(mdns.c_text),
    mdns.port,
    mdns.ttl,
    mdns.flags);

  if(result == 0)
  {
    log_info("MDNS registered");
  }
  else
  {
    log_err("MDNS registration failed");
  }
}
