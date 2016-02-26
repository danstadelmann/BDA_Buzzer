/*
 * Application.c
 *      Author: Erich Styger
 */
#include "Platform.h"
#include "Application.h"
#include "LED1.h"
#include "CLS1.h"
#include "WAIT1.h"
#include "UTIL1.h"
#include "FRTOS1.h"
#include "CDC1.h"
#include "Shell.h"
#if PL_USE_HW_RTC
  #include "RTC1.h"
#endif

#if PL_CONFIG_HAS_KEYS
void APP_KeyEvntHandler(EVNT_Handle event) {
	#if PL_CONFIG_HAS_RADIO
		uint8_t data;
	#endif
  switch(event) {
  #if PL_CONFIG_NOF_KEYS >= 1 /* A */
    case EVNT_SW1_PRESSED:
		#if PL_CONFIG_HAS_RADIO
    		data = 'A';
    		(void)RAPP_SendPayloadDataBlock(&data, sizeof(char_t), RAPP_MSG_TYPE_DATA, RNETA_GetDestAddr(), RPHY_PACKET_FLAGS_REQ_ACK);
		#endif
    	#if !PL_CONFIG_EVENTS_AUTO_CLEAR
    		EVNT_ClearEvent(EVNT_SW1_PRESSED);
    	#endif
    break;

    case EVNT_SW1_RELEASED:
    #if !PL_CONFIG_EVENTS_AUTO_CLEAR
      EVNT_ClearEvent(EVNT_SW1_RELEASED);
    #endif
    break;
  #endif

  #if PL_CONFIG_NOF_KEYS >= 2 /* B */
    case EVNT_SW2_PRESSED:
	#if PL_CONFIG_HAS_RADIO
    	data = 'B';
    	(void)RAPP_SendPayloadDataBlock(&data, sizeof(char_t), RAPP_MSG_TYPE_JOYSTICK_BTN, RNETA_GetDestAddr(), RPHY_PACKET_FLAGS_REQ_ACK);
	#endif
    #if !PL_CONFIG_EVENTS_AUTO_CLEAR
      EVNT_ClearEvent(EVNT_SW2_PRESSED);
    #endif
    case EVNT_SW2_RELEASED:
    #if !PL_CONFIG_EVENTS_AUTO_CLEAR
      EVNT_ClearEvent(EVNT_SW2_RELEASED);
    #endif
      break;
  #endif
    default:
      break;
  }
}

void APP_EvntHandler(EVNT_Handle event) {
	#if PL_CONFIG_HAS_KEYS
		APP_KeyEvntHandler(event);
	#endif
}

void APP_Run(void) {
  PL_Init();
#if PL_CONFIG_HAS_RTOS
  RTOS_Run();
#endif
  PL_Deinit();
}

