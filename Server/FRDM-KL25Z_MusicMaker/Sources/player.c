/*
 * player.c
 *
 *  Created on: 24.02.2016
 *      Author: flavio
 */

#include "pl_conf.h"
#if PLAYER
#include "player.h"
#include "VS1053.h"
#include "UTIL1.h"
#include "FRTOS1.h"
#include "FAT1.h"
#include "CS1.h"
#include "blinky_light.h"
#include "TmDt1.h"


#define TASKDEL_MS 2

static bool turning = FALSE;
static int32_t turntime = 0;

static bool playback = FALSE;
static FIL fp;
uint32_t config_turnval = 1;
//static SemaphoreHandle_t feedSem;

void clearPlayback(void) {
	CS1_CriticalVariable()
	;
	CS1_EnterCritical()
	;
	playback = FALSE;
	CS1_ExitCritical()
	;
}

void setPlayback(void) {
	CS1_CriticalVariable()
	;
	CS1_EnterCritical()
	;
	playback = TRUE;
	CS1_ExitCritical()
	;
}

bool PlaybackIsSet(void) {
	bool tmp;
	CS1_CriticalVariable()
	;
	CS1_EnterCritical()
	;
	tmp = playback;
	CS1_ExitCritical()
	;
	return tmp;
}

void feedDataStream(void) {
	uint8_t readBuf[32];
	uint8_t res = ERR_OK;
	UINT bytesRead = 0;

	//FRTOS1_xSemaphoreTakeRecursive(feedSem, portMAX_DELAY);

	if (!PlaybackIsSet()) {
		return; // paused or stopped
	}


	// Feed the hungry buffer! :)
	if (VS_Ready()) {
		//for (;;) { /* breaks */
			if (FAT1_read(&fp, readBuf, sizeof(readBuf), &bytesRead) != FR_OK) {
				//FRTOS1_xSemaphoreGiveRecursive(feedSem);
				PLR_StopPlayback();
				return;
			}

			if (bytesRead == 0) { /* end of file? */
				// must be at the end of the file, wrap it up!
				//FRTOS1_xSemaphoreGiveRecursive(feedSem);
				PLR_StopPlayback();
				return;
			}

			VS_SendData(readBuf, sizeof(readBuf));
	}


	//FRTOS1_xSemaphoreGiveRecursive(feedSem);

}

uint8_t PLR_StartNewFile(const char* filename, bool turn) {

	uint16_t data;
	if(turn == TRUE){
		turning = TRUE;
		turntime = (config_turnval*1000)/TASKDEL_MS; /*Turn in taskcycles*/
	}
	//Stop actual playback
	PLR_StopPlayback();

	VS_ReadRegister(VS_MODE, &data);
	data = (data | VS_MODE_SM_LINE1 | VS_MODE_SM_SDINEW);
	VS_WriteRegister(VS_MODE, data);

	/*resync*/
	VS_WriteRegister(VS_WRAMADDR, 0x1E29);
	VS_WriteRegister(VS_WRAM, 0);

	if (FAT1_open(&fp, filename, FA_READ) != FR_OK) {
		return ERR_FAILED;
	}

	/*As in datasheet explained set twice 0 in REG_DECODETIME to set back playback time*/
	VS_WriteRegister(VS_DECODE_TIME, 0x00);
	VS_WriteRegister(VS_DECODE_TIME, 0x00);

	setPlayback();

	while (!VS_Ready()) {
		//wait until DREQ high
	}

	return ERR_OK;

}

uint8_t PLR_PlayFullFile(const char* filename) {
	uint8_t err = ERR_OK;
	err = PLR_StartNewFile(filename,FALSE);
	if (err != ERR_OK) {
		return err;
	}
	while (PlaybackIsSet()) {
		feedDataStream();
	}
	return ERR_OK;
}

uint8_t PLR_PausePlayback(bool pause) {

	if (pause) {
		clearPlayback();
	} else {
		if (fp.obj.fs != 0) {
			setPlayback();
		}
	}

	return ERR_OK;
}

uint8_t PLR_StopPlayback(void) {
	uint16_t data;

	clearPlayback();
	FAT1_close(&fp);

	VS_ReadRegister(VS_MODE, &data);
	data = (data | VS_MODE_SM_LINE1 | VS_MODE_SM_SDINEW | VS_MODE_SM_CANCEL);
	VS_WriteRegister(VS_MODE, data);
}

static uint8_t PrintStatus(const CLS1_StdIOType *io) {
	return VS_PrintStatus(io);
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

	CLS1_SendHelpStr((unsigned char*) "  player pause on",
			(unsigned char*) "player pause on \r\n", io->stdOut);

	CLS1_SendHelpStr((unsigned char*) "  player pause off",
			(unsigned char*) "player pause off \r\n", io->stdOut);

	CLS1_SendHelpStr((unsigned char*) "  player stop",
			(unsigned char*) "player stop \r\n", io->stdOut);

	CLS1_SendHelpStr((unsigned char*) "  player setval <val>",
			(unsigned char*) "Sets time duration of alarmlight ", io->stdOut);
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
			sizeof("player volume ") - 1) == 0) {
		*handled = TRUE;
		p = cmd + sizeof("player volume ") - 1;
		if (UTIL1_xatoi(&p, &val32u) == ERR_OK) {
			return VS_SetVolume((uint16_t) val32u);
		} else {
			CLS1_SendStr("Failed reading volume", io->stdErr);
			return ERR_FAILED;
		}
	} else if (UTIL1_strncmp((char* )cmd, "player play ",
			sizeof("player play ") - 1) == 0) {
		*handled = TRUE;
		p = cmd + sizeof("player play ") - 1;
		return PLR_StartNewFile(p,FALSE);
	}else if (UTIL1_strncmp((char* )cmd, "player pause on",
			sizeof("player pause on") - 1) == 0) {
		*handled = TRUE;
		PLR_PausePlayback(TRUE);
	}else if (UTIL1_strncmp((char* )cmd, "player pause off",
			sizeof("player pause off") - 1) == 0) {
		*handled = TRUE;
		PLR_PausePlayback(FALSE);
	}else if (UTIL1_strncmp((char* )cmd, "player stop",
			sizeof("player stop") - 1) == 0) {
		*handled = TRUE;
		PLR_StopPlayback();
	}else if (UTIL1_strncmp((char*)cmd, "player setval", sizeof("player setval")-1)==0) {
	    p = cmd+sizeof("player setval")-1;
	    uint8_t res = UTIL1_xatoi(&p, &val32u);
	    if (res==ERR_OK) {
	      config_turnval = val32u;
	      *handled = TRUE;
	    }
	}

	return ERR_OK;
}

void turnlight(void){
	if(turning == TRUE){
		if(turntime > 0){
			blinky_light_SetVal();
			turntime--;
		}else{
			turning = FALSE;
			turntime = 0;
			blinky_light_ClrVal();
		}
	}
}

static portTASK_FUNCTION( playerTask, pvParameters) {

	(void) pvParameters; /* not used */

	//VS_SetVolume(0x2014);
	VS_SetVolume(0x0000);
	for (;;) {
		turnlight();
        feedDataStream();
		FRTOS1_vTaskDelay(TASKDEL_MS / portTICK_RATE_MS);
	}
}

void PLR_Deinit(void) {

}

void PLR_Init(void) {
	//feedSem = FRTOS1_xSemaphoreCreateRecursiveMutex();
	blinky_light_ClrVal();

	if (FRTOS1_xTaskCreate(playerTask, "Player", configMINIMAL_STACK_SIZE+200,
			NULL, tskIDLE_PRIORITY+3, NULL) != pdPASS) {
		for (;;) {
		} /* error */
	}
	  //FRTOS1_vQueueAddToRegistry(feedSem, "feedSem");
}
#endif
