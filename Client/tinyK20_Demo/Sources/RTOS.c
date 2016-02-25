/*
 * RTOS.c
 *
 *  Created on: Oct 23, 2015
 *      Author: daniel
 */


#include "Platform.h"
#if PL_CONFIG_HAS_RTOS
#include "RTOS.h"
#include "FRTOS1.h"
#include "Event.h"
#include "Keys.h"
#include "Application.h"

static void AppTask(void* param) {
  /* EVNT_SetEvent(EVNT_STARTUP); set startup event */
  for(;;) {
#if PL_CONFIG_HAS_EVENTS
  #if PL_CONFIG_EVENTS_AUTO_CLEAR
  EVNT_HandleEvent(APP_EvntHandler, TRUE);
  #else
  EVNT_HandleEvent(APP_EvntHandler, FALSE);
  #endif
#endif
#if PL_CONFIG_HAS_KEYS && PL_CONFIG_NOF_KEYS>0 && !PL_CONFIG_HAS_KBI
    KEY_Scan(); /* scan keys */
#endif
    FRTOS1_vTaskDelay(10/portTICK_RATE_MS);
  }
}

void RTOS_Run(void) {
  FRTOS1_vTaskStartScheduler();  /* does usually not return! */
}

void RTOS_Init(void) {
  /*! \todo Add tasks here */
  if (FRTOS1_xTaskCreate(AppTask, (signed portCHAR *)"App", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
    for(;;){} /* error */
  }
}

void RTOS_Deinit(void) {
  /* nothing needed */
}

#endif /* PL_HAS_RTOS */

