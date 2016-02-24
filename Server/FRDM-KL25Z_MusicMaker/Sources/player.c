/*
 * player.c
 *
 *  Created on: 24.02.2016
 *      Author: flavio
 */

#include "player.h"
#include "VS1053.h"
#include "UTIL1.h"
#include "FRTOS1.h"


#define PLR_DATA_QUEUE_LENGTH      4 /* number of items in queue, that's my buffer size */
#define PLR_DATA_SIZE   8 /* each item is a single drive command */
static xQueueHandle PLR_data_queue;

static uint8_t PrintStatus(const CLS1_StdIOType *io) {

	return ERR_OK;
}

static uint8_t PrintHelp(const CLS1_StdIOType *io) {
	CLS1_SendHelpStr((unsigned char*) "player",
			(unsigned char*) "Group of player commands\r\n", io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  help|status",
			(unsigned char*) "Print help or status information\r\n",
			io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  volume <number>",
			(unsigned char*) "Set volume, full: 0x0000, 0xFEFE silence\r\n",
			io->stdOut);
	CLS1_SendHelpStr((unsigned char*) "  play <file>",
			(unsigned char*) "Play song file\r\n", io->stdOut);
	return ERR_OK;
}

uint8_t PLR_ParseCommand(const unsigned char *cmd, bool *handled,
		const CLS1_StdIOType *io) {
	const uint8_t *p;
	uint32_t val32u;

	if (UTIL1_strcmp((char*)cmd, CLS1_CMD_HELP) == 0
			|| UTIL1_strcmp((char*)cmd, "player help") == 0) {
		*handled = TRUE;
		return PrintHelp(io);
	} else if ((UTIL1_strcmp((char*)cmd, CLS1_CMD_STATUS) == 0)
			|| (UTIL1_strcmp((char*)cmd, "player status") == 0)) {
		*handled = TRUE;
		return PrintStatus(io);
	} else if (UTIL1_strncmp((char* )cmd, "player volume ",
			sizeof("player volume ") - 1)
			== 0) {
		*handled = TRUE;
		p = cmd + sizeof("player volume ") - 1;
		if (UTIL1_xatoi(&p, &val32u) == ERR_OK) {
			return VS_SetVolume((uint16_t) val32u);
		} else {
			CLS1_SendStr("Failed reading volume", io->stdErr);
			return ERR_FAILED;
		}
	} else if (UTIL1_strncmp((char* )cmd, "player play ",
			sizeof("player play ") - 1)
			== 0) {
		*handled = TRUE;
		p = cmd + sizeof("player play ") - 1;
		return VS_PlaySong(p, io);
	}
	return ERR_OK;
}

static portTASK_FUNCTION( playerTask, pvParameters) {

	(void) pvParameters; /* not used */
	for (;;) {
		FRTOS1_vTaskDelay(50 / portTICK_RATE_MS);
	}
}

void PLR_Deinit(void) {

}

void PLR_Init(void) {
	PLR_data_queue = FRTOS1_xQueueCreate(PLR_DATA_QUEUE_LENGTH,PLR_DATA_SIZE);
	  if (PLR_data_queue==NULL) {
	    for(;;){} /* out of memory? */
	  }
	  if (FRTOS1_xTaskCreate(playerTask, "Player", configMINIMAL_STACK_SIZE+200, NULL, tskIDLE_PRIORITY+1, NULL) != pdPASS) {
	    for(;;){} /* error */
	  }
}
