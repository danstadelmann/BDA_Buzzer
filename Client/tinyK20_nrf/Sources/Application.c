/*
 * Application.c
 *      Author: Erich Styger
 */
#include "Platform.h"
#include "Application.h"
#include "FRTOS1.h"
#include "Shell.h"
#include "LED1.h"
#include "RNet_App.h"
#include "RApp.h"
#include "SHP.h"
#include "BUZ.h"

void APP_DebugPrint(uint8_t *str) {
	/* dummy */
}

static void led_task(void *param) {
	(void) param;
	uint8_t data = 'A';
	(void) RAPP_SendPayloadDataBlock(&data, sizeof(char_t),
					RAPP_MSG_TYPE_DATA, RNETA_GetDestAddr(),
					RPHY_PACKET_FLAGS_REQ_ACK);
	for (;;) {
		SHP_SetVal();
		LED1_On();
		for(int i= 0; i < 10; i++){
			WAIT1_Waitms(200);
			if(BUZ_GetVal()){
				(void) RAPP_SendPayloadDataBlock(&data, sizeof(char_t),
									RAPP_MSG_TYPE_DATA, RNETA_GetDestAddr(),
									RPHY_PACKET_FLAGS_REQ_ACK);
			}

		}
		SHP_ClrVal();
		vTaskDelay(pdMS_TO_TICKS(1000));
	} /* for */
}

void APP_Run(void) {
	SHELL_Init();
#if PL_CONFIG_HAS_RADIO
	RNETA_Init();
#endif
	if (xTaskCreate(led_task, "Led", configMINIMAL_STACK_SIZE, NULL,
			tskIDLE_PRIORITY, NULL) != pdPASS) {
		for (;;) {
		} /* error! probably out of memory */
	}
	vTaskStartScheduler();
}

