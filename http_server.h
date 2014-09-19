/*
 * http_server.h
 *
 *  Created on: 19 Sep 2014
 *      Author: manderson
 */

#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

typedef struct HttpServerState
{
  char running;
  int port;
} HttpServerState;

HttpServerState *http_server_state_get();

int http_server_stop(void);
int http_server_start(void);
int http_server_initialise(void);

#endif /* HTTP_SERVER_H_ */
