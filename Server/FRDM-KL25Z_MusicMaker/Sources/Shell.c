/*
 * Shell.c
 *      Author: Erich Styger
 */

#include "pl_conf.h"
#include "Shell.h"
#include "Application.h"
#include "FRTOS1.h"
#include "CLS1.h"
#include "LEDR.h"
#include "LEDG.h"
#include "FAT1.h"
#include "VS1053.h"
#include "player.h"

#if RADIO
#include "RApp.h"
#include "RNet_App.h"
#include "RNetConf.h"
#include "RStdIO.h"
#endif

static const CLS1_ParseCommandCallback CmdParserTable[] = { CLS1_ParseCommand,
#if FRTOS1_PARSE_COMMAND_ENABLED
		FRTOS1_ParseCommand,
#endif
#if FAT1_PARSE_COMMAND_ENABLED
		FAT1_ParseCommand,
#endif
		VS_ParseCommand,
#if PLAYER
		PLR_ParseCommand,
#endif
#if TmDt1_PARSE_COMMAND_ENABLED
		TmDt1_ParseCommand,
#endif
#if RADIO
		RNET1_ParseCommand, RNETA_ParseCommand,
#endif
		NULL /* sentinel */
};

static portTASK_FUNCTION(ShellTask, pvParameters) {
	bool cardMounted = FALSE;
	static FAT1_FATFS fileSystemObject;
	unsigned char buf[48];
#if RADIO
	static unsigned char radio_cmd_buf[48];
	radio_cmd_buf[0] = '\0';
	CLS1_ConstStdIOType *ioRemote = RSTDIO_GetStdioRx();

#endif
	CLS1_ConstStdIOTypePtr ioLocal = CLS1_GetStdio();

	(void) pvParameters; /* not used */
	buf[0] = '\0';
	(void) CLS1_ParseWithCommandTable((unsigned char*) CLS1_CMD_HELP, ioLocal,
			CmdParserTable);
	FAT1_Init();
	for (;;) {
		(void) FAT1_CheckCardPresence(&cardMounted, "0" /* drive */,
				&fileSystemObject, CLS1_GetStdio());
		if (cardMounted) {
			LEDG_On();
			LEDR_Off();
		} else {
			LEDG_Off();
			LEDR_On();
		}
		(void) CLS1_ReadAndParseWithCommandTable(buf, sizeof(buf),
				CLS1_GetStdio(), CmdParserTable);
#if RADIO
		RSTDIO_Print(ioLocal); /* dispatch incoming messages */
		(void) CLS1_ReadAndParseWithCommandTable(radio_cmd_buf,
				sizeof(radio_cmd_buf), ioRemote, CmdParserTable);
#endif
		FRTOS1_vTaskDelay(50/portTICK_RATE_MS);
	}
}

void SHELL_Init(void) {
	if (FRTOS1_xTaskCreate(ShellTask, "Shell", configMINIMAL_STACK_SIZE+200,
			NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
		for (;;) {
		} /* error */
	}
}

