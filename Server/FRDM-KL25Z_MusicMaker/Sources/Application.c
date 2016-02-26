/*
 * Application.c
 *      Author: Erich Styger
 */
#include "pl_conf.h"
#include "Application.h"
#include "FRTOS1.h"
#include "Shell.h"
#include "VS1053.h"
#include "RNet_App.h"
#include "player.h"

void APP_Run(void) {
  SHELL_Init(); /* initialize shell */
  VS_Init(); /* initialize VS1053B module */
#if RADIO
  RNETA_Init();
#endif
#if PLAYER
  PLR_Init();
#endif
  FRTOS1_vTaskStartScheduler();
}

