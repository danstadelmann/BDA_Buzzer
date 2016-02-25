/*
 * Keys.c
 *
 *  Created on: Oct 8, 2015
 *      Author: daniel
 *      This module implements a generic keyboard driver for up to 7 Keys.
 * 		It is using macros for maximum flexibility with minimal code overhead.
 */

#include "Platform.h"
#if PL_CONFIG_HAS_KEYS
  #include "Keys.h"
#if PL_CONFIG_HAS_EVENTS
  #include "Event.h"
#endif
#if PL_CONFIG_HAS_DEBOUNCE
	#include "KeyDebounce.h"
#endif


void KEY_Scan(void) {
  /*! check handling all keys */
#if PL_CONFIG_HAS_DEBOUNCE
  KEYDBNC_Process();
#else
  #if PL_CONFIG_NOF_KEYS >= 1
    if (KEY1_Get()) { /* key pressed */
      EVNT_SetEvent(EVNT_SW1_PRESSED);
    }
  #endif
  #if PL_CONFIG_NOF_KEYS >= 2
    if (KEY2_Get()) { /* key pressed */
      EVNT_SetEvent(EVNT_SW2_PRESSED);
    }
  #endif
  #if PL_CONFIG_NOF_KEYS >= 3
    if (KEY3_Get()) { /* key pressed */
      EVNT_SetEvent(EVNT_SW3_PRESSED);
    }
  #endif
  #if PL_CONFIG_NOF_KEYS >= 4
    if (KEY4_Get()) { /* key pressed */
      EVNT_SetEvent(EVNT_SW4_PRESSED);
    }
  #endif
  #if PL_CONFIG_NOF_KEYS >= 5
    if (KEY5_Get()) { /* key pressed */
      EVNT_SetEvent(EVNT_SW5_PRESSED);
    }
  #endif
  #if PL_CONFIG_NOF_KEYS >= 6
    if (KEY6_Get()) { /* key pressed */
      EVNT_SetEvent(EVNT_SW6_PRESSED);
    }
  #endif
  #if PL_CONFIG_NOF_KEYS >= 7
    if (KEY7_Get()) { /* key pressed */
      EVNT_SetEvent(EVNT_SW7_PRESSED);
    }
  #endif
#endif
}

/*! \brief Key driver initialization */
void KEY_Init(void) {
  /* nothing needed for now */
}

/*! \brief Key driver de-initialization */
void KEY_Deinit(void) {
  /* nothing needed for now */
}
#endif /* PL_HAS_KEYS */



