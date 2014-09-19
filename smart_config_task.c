#include "smart_config_task.h"

// Simplelink includes
#include "simplelink.h"

//Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"

//Common interface includes
#include "pinmux.h"
#include "gpio_if.h"
#include "common.h"
#include "uart_if.h"
#include "log.h"
#include "mdns.h"



// Application specific status/error codes
typedef enum{
  // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
  LAN_CONNECTION_FAILED = -0x7D0,
  DEVICE_NOT_IN_STATION_MODE = LAN_CONNECTION_FAILED - 1,
  STATUS_CODE_MAX = -0xBB8
} e_AppStatusCodes;

unsigned long  g_ulStatus = 0;//SimpleLink Status
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID

int smart_config_connect();
static void InitialiseSmartConfigTask();

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
  if(!pWlanEvent)
  {
    return;
  }
  switch(pWlanEvent->Event)
  {
    case SL_WLAN_CONNECT_EVENT:
      {
        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

        //
        // Information about the connected AP (like name, MAC etc) will be
        // available in 'slWlanConnectAsyncResponse_t'
        // Applications can use it if required
        //
        //  slWlanConnectAsyncResponse_t *pEventData = NULL;
        // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
        //

        // Copy new connection SSID and BSSID to global parameters
        memcpy(g_ucConnectionSSID,pWlanEvent->EventData.
            STAandP2PModeWlanConnected.ssid_name,
            pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
        memcpy(g_ucConnectionBSSID,
            pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
            SL_BSSID_LENGTH);

        log_info("[WLAN EVENT] STA Connected to the AP: %s , "
            "BSSID: %x:%x:%x:%x:%x:%x",
            g_ucConnectionSSID,g_ucConnectionBSSID[0],
            g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
            g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
            g_ucConnectionBSSID[5]);
      }
      break;

    case SL_WLAN_DISCONNECT_EVENT:
      {
        slWlanConnectAsyncResponse_t*  pEventData = NULL;

        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
        CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

        pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

        // If the user has initiated 'Disconnect' request,
        //'reason_code' is SL_USER_INITIATED_DISCONNECTION
        if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
        {
          log_info("[WLAN EVENT]Device disconnected from the AP: %s, "
              "BSSID: %x:%x:%x:%x:%x:%x on application's "
              "request ",
              g_ucConnectionSSID,g_ucConnectionBSSID[0],
              g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
              g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
              g_ucConnectionBSSID[5]);
        }
        else
        {
          log_info("[WLAN ERROR]Device disconnected from the AP AP: %s, "
              "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! ",
              g_ucConnectionSSID,g_ucConnectionBSSID[0],
              g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
              g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
              g_ucConnectionBSSID[5]);
        }
        memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
        memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
      }
      break;

    default:
      {
        log_info("[WLAN EVENT] Unexpected event [0x%x]",
            pWlanEvent->Event);
      }
      break;
  }
}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
  if(!pNetAppEvent)
  {
    return;
  }

  switch(pNetAppEvent->Event)
  {
    case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
      {
        SlIpV4AcquiredAsync_t *pEventData = NULL;

        SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

        //Ip Acquired Event Data
        pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

        //Gateway IP address
        g_ulGatewayIP = pEventData->gateway;

        log_info("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
            "Gateway=%d.%d.%d.%d",
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,0),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,0));
      }
      break;

    default:
      {
        log_info("[NETAPP EVENT] Unexpected event [0x%x] ",
            pNetAppEvent->Event);
      }
      break;
  }
}


void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
    SlHttpServerResponse_t *pHttpResponse)
{
  // Unused in this application
}

void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
  if(!pDevEvent)
  {
    return;
  }

  //
  // Most of the general errors are not FATAL are are to be handled
  // appropriately by the application
  //
  log_info("[GENERAL EVENT] - ID=[%d] Sender=[%d]",
      pDevEvent->EventData.deviceEvent.status,
      pDevEvent->EventData.deviceEvent.sender);
}


void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
  if(!pSock)
  {
    return;
  }

  //
  // This application doesn't work w/ socket - Events are not expected
  //
  switch( pSock->Event )
  {
    case SL_SOCKET_TX_FAILED_EVENT:
      switch( pSock->EventData.status )
      {
        case SL_ECLOSE:
          log_info("[SOCK ERROR] - close socket (%d) operation "
              "failed to transmit all queued packets",
              pSock->EventData.sd);
          break;
        default:
          log_info("[SOCK ERROR] - TX FAILED  :  socket %d , "
              "reason (%d) ",
              pSock->EventData.sd, pSock->EventData.status);
      }
      break;

    default:
      log_info("[SOCK EVENT] - Unexpected Event [%x0x]", \
          pSock->Event);
  }
}

static void InitialiseSmartConfigTask()
{
  g_ulStatus = 0;
  g_ulGatewayIP = 0;
  memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
  memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
}

static long setup_network()
{
  SlVersionFull   ver = {0};
  _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

  unsigned char ucVal = 1;
  unsigned char ucConfigOpt = 0;
  unsigned char ucConfigLen = 0;
  unsigned char ucPower = 0;

  long result = -1;
  long lMode = -1;

  // Get the device's version-information
  ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
  ucConfigLen = sizeof(ver);
  result = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt,
      &ucConfigLen, (unsigned char *)(&ver));
  ASSERT_ON_ERROR(result);

  log_info("Host Driver Version: %s",SL_DRIVER_VERSION);
  log_info("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d",
      ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
      ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
      ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
      ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
      ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

  // Set connection policy to Auto + SmartConfig 
  //      (Device's default connection policy)
  result = sl_WlanPolicySet(SL_POLICY_CONNECTION,
      SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
  ASSERT_ON_ERROR(result);

  // Enable DHCP client
  result = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
  ASSERT_ON_ERROR(result);

  // Disable scan
  ucConfigOpt = SL_SCAN_POLICY(0);
  result = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
  ASSERT_ON_ERROR(result);

  // Set Tx power level for station mode
  // Number between 0-15, as dB offset from max power - 0 will set max power
  ucPower = 0;
  result = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
      WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
  ASSERT_ON_ERROR(result);

  // Set PM policy to normal
  result = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
  ASSERT_ON_ERROR(result);

  // Unregister mDNS services
  result = sl_NetAppMDNSUnRegisterService(0, 0);
  ASSERT_ON_ERROR(result);

  // Remove  all 64 filters (8*8)
  memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
  result = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
      sizeof(_WlanRxFilterOperationCommandBuff_t));
  ASSERT_ON_ERROR(result);
}

int smart_config_connect()
{
  unsigned char policyVal;
  long result = -1;

  if (!IS_CONNECTED(g_ulStatus)) 
  {
    //set AUTO policy
    result = sl_WlanPolicySet(  SL_POLICY_CONNECTION,
        SL_CONNECTION_POLICY(1,0,0,0,1),
        &policyVal,
        1 /*PolicyValLen*/);
    ASSERT_ON_ERROR(result);

    // Start SmartConfig
    // This example uses the unsecured SmartConfig method
    //
    result = sl_WlanSmartConfigStart(0,                /*groupIdBitmask*/
        SMART_CONFIG_CIPHER_NONE,    /*cipher*/
        0,                           /*publicKeyLen*/
        0,                           /*group1KeyLen*/
        0,                           /*group2KeyLen */
        NULL,                        /*publicKey */
        NULL,                        /*group1Key */
        NULL);                       /*group2Key*/
    ASSERT_ON_ERROR(result);

    // Wait for WLAN Event
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus)))
    {
#ifndef SL_PLATFORM_MULTI_THREADED
      _SlNonOsMainLoopTask();
#endif
    }
    //
    // Turn ON the RED LED to indicate connection success
    //
    GPIO_IF_LedOn(MCU_RED_LED_GPIO);
    //wait for few moments
    MAP_UtilsDelay(80000000);
    //reset to default AUTO policy
    result = sl_WlanPolicySet(  SL_POLICY_CONNECTION,
        SL_CONNECTION_POLICY(1,0,0,0,0),
        &policyVal,
        1 /*PolicyValLen*/);
    ASSERT_ON_ERROR(result);
  }
  return SUCCESS;
}

void SmartConfigTask(void *pvParameters)
{
  long result = -1;

  log_info("Smart Config Task is running");
  InitialiseSmartConfigTask();

  CLR_STATUS_BIT_ALL(g_ulStatus);
  
  //Start simplelink
  result = sl_Start(0,0,0);
  if (result < 0 || ROLE_STA != result)
  {
    log_info("Failed to start the device ");
    LOOP_FOREVER();
  }

  setup_network();
  log_info("Device started as STATION ");

  mdns_register();

  /* Connect to our AP using SmartConfig method */
  result = smart_config_connect();
  if(result < 0)
  {
    ERR_PRINT(result);
  }    
  LOOP_FOREVER();
}
