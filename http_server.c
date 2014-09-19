/*
 * http_server.c
 *
 *  Created on: 19 Sep 2014
 *      Author: manderson
 */

#include "http_server.h"

#include "log.h"
#include "simplelink.h"

static HttpServerState http_state;

HttpServerState *http_server_state_get() {
  return &http_state;
}

int http_server_initialise(void)
{
  http_state = (HttpServerState) {
    .running = 0,
    .port = 80
  };
}

int http_server_start(void)
{
  long result = -1;
  char false = 0;

  http_server_stop();

  //set the port
  sl_NetAppSet(
    SL_NET_APP_HTTP_SERVER_ID,
    NETAPP_SET_GET_HTTP_OPT_PORT_NUMBER,
    2, (_u8 *)&http_state.port);

  //turn off ROM http pages
  sl_NetAppSet(
    SL_NET_APP_HTTP_SERVER_ID,
    NETAPP_SET_GET_HTTP_OPT_ROM_PAGES_ACCESS,
    1, (_u8 *)&false);

  result = sl_NetAppStart(SL_NET_APP_HTTP_SERVER_ID);
  if(result < 0)
  {
      log_err("Failed to start the HTTP server");
  }
  http_state.running = 1;
  return 0;
}

int http_server_stop(void)
{
  long result = -1;
  result = sl_NetAppStop(SL_NET_APP_HTTP_SERVER_ID);
  if (result < 0)
  {
    log_err("Failed to stop the HTTP server");
    return result;
  }
  http_state.running = 0;
  return 0;
}

void SimpleLinkHttpServerCallback(
  SlHttpServerEvent_t *pSlHttpServerEvent,
  SlHttpServerResponse_t *pSlHttpServerResponse)
{
    unsigned char strLenVal = 0;

    if(!pSlHttpServerEvent || !pSlHttpServerResponse)
    {
        return;
    }

  /*  switch (pSlHttpServerEvent->Event)
    {
        case SL_NETAPP_HTTPGETTOKENVALUE_EVENT:
        {
          unsigned char status, *ptr;

          ptr = pSlHttpServerResponse->ResponseData.token_value.data;
          pSlHttpServerResponse->ResponseData.token_value.len = 0;
          if(memcmp(pSlHttpServerEvent->EventData.httpTokenName.data, GET_token,
                    strlen((const char *)GET_token)) == 0)
          {
            status = GPIO_IF_LedStatus(MCU_RED_LED_GPIO);
            strLenVal = strlen(LED1_STRING);
            memcpy(ptr, LED1_STRING, strLenVal);
            ptr += strLenVal;
            pSlHttpServerResponse->ResponseData.token_value.len += strLenVal;
            if(status & 0x01)
            {
              strLenVal = strlen(LED_ON_STRING);
              memcpy(ptr, LED_ON_STRING, strLenVal);
              ptr += strLenVal;
              pSlHttpServerResponse->ResponseData.token_value.len += strLenVal;
            }
            else
            {
              strLenVal = strlen(LED_OFF_STRING);
              memcpy(ptr, LED_OFF_STRING, strLenVal);
              ptr += strLenVal;
              pSlHttpServerResponse->ResponseData.token_value.len += strLenVal;
            }
            status = GPIO_IF_LedStatus(MCU_GREEN_LED_GPIO);
            strLenVal = strlen(LED2_STRING);
            memcpy(ptr, LED2_STRING, strLenVal);
            ptr += strLenVal;
            pSlHttpServerResponse->ResponseData.token_value.len += strLenVal;
            if(status & 0x01)
            {
              strLenVal = strlen(LED_ON_STRING);
              memcpy(ptr, LED_ON_STRING, strLenVal);
              ptr += strLenVal;
              pSlHttpServerResponse->ResponseData.token_value.len += strLenVal;
            }
            else
            {
              strLenVal = strlen(LED_OFF_STRING);
              memcpy(ptr, LED_OFF_STRING, strLenVal);
              ptr += strLenVal;
              pSlHttpServerResponse->ResponseData.token_value.len += strLenVal;
            }
            *ptr = '\0';
          }

        }
        break;

        case SL_NETAPP_HTTPPOSTTOKENVALUE_EVENT:
        {
          unsigned char led;
          unsigned char *ptr = pSlHttpServerEvent->EventData.httpPostData.token_name.data;

          if(memcmp(ptr, POST_token, strlen((const char *)POST_token)) == 0)
          {
            ptr = pSlHttpServerEvent->EventData.httpPostData.token_value.data;
            strLenVal = strlen(LED_STRING);
            if(memcmp(ptr, LED_STRING, strLenVal) != 0)
              break;
            ptr += strLenVal;
            led = *ptr;
            strLenVal = strlen(LED_ON_STRING);
            ptr += strLenVal;
            if(led == '1')
            {
              if(memcmp(ptr, LED_ON_STRING, strLenVal) == 0)
              {
                      GPIO_IF_LedOn(MCU_RED_LED_GPIO);
              }
              else
              {
                      GPIO_IF_LedOff(MCU_RED_LED_GPIO);
              }
            }
            else if(led == '2')
            {
              if(memcmp(ptr, LED_ON_STRING, strLenVal) == 0)
              {
                      GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
              }
              else
              {
                      GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
              }
            }

          }
        }
          break;
        default:
          break;
    }*/
}

