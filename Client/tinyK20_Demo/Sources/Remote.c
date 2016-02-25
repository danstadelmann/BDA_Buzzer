/*
a * Remote.c
 *
 *  Created on: Nov 27, 2015
 *      Author: daniel
 */

#include "Platform.h" /* interface to the platform */
#if PL_CONFIG_HAS_REMOTE
#include "Remote.h"
#include "FRTOS1.h"
#include "CLS1.h"
#include "UTIL1.h"
#include "Shell.h"
#if PL_CONFIG_HAS_PID
  #include "Pid.h"
#endif
#if PL_CONFIG_HAS_MOTOR
  #include "Motor.h"
#endif
#if PL_CONFIG_HAS_RADIO
  #include "RNet_App.h"
  #include "RNet_AppConfig.h"
#endif
#if PL_CONFIG_HAS_DRIVE
  #include "Drive.h"
#endif
#if PL_CONFIG_HAS_LED
  #include "LED.h"
#endif
#if PL_CONFIG_HAS_JOYSTICK
  #include "AD1.h"
#endif
#if PL_CONFIG_HAS_SHELL
	#include "Shell.h"
#endif
#if PL_CONFIG_HAS_BUZZER
	#include "Buzzer.h"
#endif
#if PL_CONFIG_HAS_LINE_FOLLOW
	#include "LineFollow.h"
#endif
#if PL_CONFIG_HAS_LINE_MAZE
	#include "Maze.h"
#endif

#if PL_CONFIG_HAS_DRIVE
static bool NITRO = FALSE;
#endif
static bool REMOTE_isOn = FALSE;
static bool REMOTE_isVerbose = FALSE;
static bool REMOTE_useJoystick = TRUE;
#if PL_CONFIG_HAS_JOYSTICK
static uint16_t midPointX, midPointY;
#endif

#if PL_CONFIG_CONTROL_SENDER
static int8_t ToSigned8Bit(uint16_t val, bool isX) {
  int32_t tmp;

  if (isX) {
    tmp = (int32_t)val-midPointX;
  } else {
    tmp = (int32_t)val-midPointY;
  }
  if (tmp>0) {
    tmp = (tmp*128)/0x7fff;
  } else {
    tmp = (-tmp*128)/0x7fff;
    tmp = -tmp;
  }
  if (tmp<-128) {
    tmp = -128;
  } else if (tmp>127) {
    tmp = 127;
  }
  return (int8_t)tmp;
}

static uint8_t APP_GetXY(uint16_t *x, uint16_t *y, int8_t *x8, int8_t *y8) {
  uint8_t res;
  uint16_t values[2];

  res = AD1_Measure(TRUE);
  if (res!=ERR_OK) {
    return res;
  }
  res = AD1_GetValue16(&values[0]);
  if (res!=ERR_OK) {
    return res;
  }
  if (x!=NULL) {
    *x = values[0];
  }
  if (y!=NULL) {
    *y = values[1];
  }
  /* transform into -128...127 with zero as mid position */
  if (x8!=NULL) {
    *x8 = ToSigned8Bit(values[0], TRUE);
  }
  if (y8!=NULL) {
    *y8 = ToSigned8Bit(values[1], FALSE);
  }
  return ERR_OK;
}

static void RemoteTask (void *pvParameters) {
  (void)pvParameters;
#if PL_CONFIG_HAS_JOYSTICK
  (void)APP_GetXY(&midPointX, &midPointY, NULL, NULL);
#endif
  FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
  for(;;) {
    if (REMOTE_isOn) {
#if PL_CONFIG_HAS_JOYSTICK
      if (REMOTE_useJoystick) {
        uint8_t buf[2];
        int16_t x, y;
        int8_t x8, y8;

        /* send periodically messages */
        APP_GetXY(&x, &y, &x8, &y8);
        buf[0] = x8;
        buf[1] = y8;
        if (REMOTE_isVerbose) {
          uint8_t txtBuf[48];

          UTIL1_strcpy(txtBuf, sizeof(txtBuf), (unsigned char*)"TX: x: ");
          UTIL1_strcatNum8s(txtBuf, sizeof(txtBuf), x8);
          UTIL1_strcat(txtBuf, sizeof(txtBuf), (unsigned char*)" y: ");
          UTIL1_strcatNum8s(txtBuf, sizeof(txtBuf), y8);
          UTIL1_strcat(txtBuf, sizeof(txtBuf), (unsigned char*)" to addr 0x");
    #if RNWK_SHORT_ADDR_SIZE==1
          UTIL1_strcatNum8Hex(txtBuf, sizeof(txtBuf), RNETA_GetDestAddr());
    #else
          UTIL1_strcatNum16Hex(txtBuf, sizeof(txtBuf), RNETA_GetDestAddr());
    #endif
          UTIL1_strcat(txtBuf, sizeof(txtBuf), (unsigned char*)"\r\n");
          SHELL_SendString(txtBuf);
        }
        (void)RAPP_SendPayloadDataBlock(buf, sizeof(buf), RAPP_MSG_TYPE_JOYSTICK_XY, RNETA_GetDestAddr(), RPHY_PACKET_FLAGS_REQ_ACK);
        LED1_Neg();
      }
#endif
      FRTOS1_vTaskDelay(200/portTICK_RATE_MS);
    } else {
      FRTOS1_vTaskDelay(1000/portTICK_RATE_MS);
    }
  } /* for */
}
#endif

#if PL_CONFIG_HAS_MOTOR
static void REMOTE_HandleMotorMsg(int16_t speedVal, int16_t directionVal, int16_t z) {
  #define MIN_VALUE  350 /* old: 250 - values below this value are ignored */
  #define Nitro		 2
  #define Zuercher	 4	 /* gerade aus -> schnell */

  if (!REMOTE_isOn) {
    return;
  }
  if (z<-900) { /* have a way to stop motor: turn FRDM USB port side up or down */
#if PL_CONFIG_HAS_DRIVE
    DRV_SetSpeed(0, 0);
#else
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
#endif
  } else if ((directionVal>MIN_VALUE || directionVal<-MIN_VALUE) && (speedVal>MIN_VALUE || speedVal<-MIN_VALUE)) {
    int16_t speed, speedL, speedR;

    speed = speedVal*2;
    if (directionVal<0) {
      if (speed<0) {
        speedR = speed+(directionVal);
      } else {
        speedR = speed-(directionVal);
      }
      speedL = speed;
    } else {
      speedR = speed;
      if (speed<0) {
        speedL = speed-(directionVal);
      } else {
        speedL = speed+(directionVal);
      }
    }
#if PL_CONFIG_HAS_DRIVE
    if(NITRO){
    	DRV_SetSpeed(speedL*Nitro, speedR*Nitro);
    }else {
    	DRV_SetSpeed(speedL, speedR);
    }
#else
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), speedL);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), speedR);
#endif
  } else if (speedVal>100 || speedVal<-100) { /* speed */
#if PL_CONFIG_HAS_DRIVE
	if(NITRO){
		DRV_SetSpeed(speedVal*3*Nitro, speedVal*3*Nitro); /* Zuercher & Nitro esch chli vell */
	} else {
		DRV_SetSpeed(speedVal*Zuercher, speedVal*Zuercher);
	}
#else
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), -speedVal/SCALE_DOWN);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), -speedVal/SCALE_DOWN);
#endif
  } else if (directionVal>110 || directionVal<-110) { /* direction */
#if PL_CONFIG_HAS_DRIVE
	  if(NITRO){
		  DRV_SetSpeed(directionVal*Nitro, -directionVal*Nitro);
	  } else {
		  DRV_SetSpeed(directionVal, -directionVal);
	  }
#else
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), -directionVal/SCALE_DOWN);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), (directionVAl/SCALE_DOWN));
#endif
  } else { /* device flat on the table? */
#if PL_CONFIG_HAS_DRIVE
    DRV_SetSpeed(0, 0);
#else
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
#endif
  }
}
#endif

#if PL_CONFIG_HAS_MOTOR
static int16_t scaleJoystickTo1K(int8_t val) {
  /* map speed from -128...127 to -1000...+1000 */
  int tmp;

  if (val>0) {
    tmp = ((val*10)/127)*100;
  } else {
    tmp = ((val*10)/128)*100;
  }
  if (tmp<-1000) {
    tmp = -1000;
  } else if (tmp>1000) {
    tmp = 1000;
  }
  return tmp;
}
#endif

uint8_t REMOTE_HandleRemoteRxMessage(RAPP_MSG_Type type, uint8_t size, uint8_t *data, RNWK_ShortAddrType srcAddr, bool *handled, RPHY_PacketDesc *packet) {
#if PL_CONFIG_HAS_SHELL
  uint8_t buf[48];
#endif
  uint8_t val;
  int16_t x, y, z;

  (void)size;
  (void)packet;
  switch(type) {
#if PL_CONFIG_HAS_MOTOR
    case RAPP_MSG_TYPE_JOYSTICK_XY: /* values are -128...127 */
      {
        int8_t x, y;
        int16_t x1000, y1000;

        *handled = TRUE;
        x = *data; /* get x data value */
        y = *(data+1); /* get y data value */
        if (REMOTE_isVerbose) {
          UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"x/y: ");
          UTIL1_strcatNum8s(buf, sizeof(buf), (int8_t)x);
          UTIL1_chcat(buf, sizeof(buf), ',');
          UTIL1_strcatNum8s(buf, sizeof(buf), (int8_t)y);
          UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
          SHELL_SendString(buf);
        }
  #if 0 /* using shell command */
        UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"motor L duty ");
        UTIL1_strcatNum8s(buf, sizeof(buf), scaleSpeedToPercent(x));
        SHELL_ParseCmd(buf);
        UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"motor R duty ");
        UTIL1_strcatNum8s(buf, sizeof(buf), scaleSpeedToPercent(y));
        SHELL_ParseCmd(buf);
  #endif
        /* filter noise around zero */
        if (x>-5 && x<5) {
          x = 0;
        }
        if (y>-15 && y<5) {
          y = 0;
        }
        x1000 = scaleJoystickTo1K(x);
        y1000 = scaleJoystickTo1K(y);
        if (REMOTE_useJoystick) {
          REMOTE_HandleMotorMsg(y1000, x1000, 0); /* first param is forward/backward speed, second param is direction */
        }
      }
      break;
#endif
    case RAPP_MSG_TYPE_JOYSTICK_BTN:
      *handled = TRUE;
      val = *data; /* get data value */
#if PL_CONFIG_HAS_SHELL && PL_CONFIG_HAS_BUZZER && PL_CONFIG_HAS_REMOTE
      if (val=='F') { /* F button - disable remote, drive mode none */
    	  DRV_SetMode(DRV_MODE_NONE);
    	  REMOTE_SetOnOff(FALSE);
    	  SHELL_SendString("Remote OFF\r\n");
      } else if (val=='G') { /* center joystick button: enable remote */
        REMOTE_SetOnOff(TRUE);
		#if PL_CONFIG_HAS_DRIVE
        DRV_SetMode(DRV_MODE_SPEED);
		#endif
        SHELL_SendString("Remote ON\r\n");
      }
#if PL_CONFIG_HAS_LINE_MAZE
      else if (val=='A') { /* green 'A' button */
        SHELL_SendString("Button A pressed\r\n");
        // Start Maze solving
        if(!LF_IsFollowing()){
        	LF_StartFollowing();
        }
      } else if (val=='K') { /* green A button longpress -> 'K' */
          SHELL_SendString("Clear Maze\r\n");
          // Clear old maze solution, ready for restart
          MAZE_ClearSolution();
      } else if (val =='B'){ /* yellow 'B' button */
    	  SHELL_SendString("Right hand rule!\r\n");
    	  LF_SetRule(FALSE);
      } else if (val=='E') { /* button 'E' pressed */
    	  SHELL_SendString("Stop Following! \r\n");
    	  if(LF_IsFollowing()){
    		  LF_StopFollowing();
    	  }
      } else if (val=='D') { /* blue 'D' button */
    	  SHELL_SendString("Left hand rule!\r\n");
    	  LF_SetRule(TRUE);
      }
#endif
        else if (val=='C') { /* red 'C' button */
    	  NITRO = TRUE;
    	  SHELL_SendString("Nitrooooooo!!!\r\n");
    	  BUZ_Beep(1000,1000);
      } else if (val=='J') { /* button 'C' released */
    	  NITRO = FALSE;
    	  SHELL_SendString("Stop Nitro \r\n");
      }
#else
      *handled = FALSE; /* no shell and no buzzer? */
#endif
      break;

    default:
      break;
  } /* switch */
  return ERR_OK;
}

#if PL_CONFIG_HAS_JOYSTICK
static void StatusPrintXY(CLS1_ConstStdIOType *io) {
  uint16_t x, y;
  int8_t x8, y8;
  uint8_t buf[64];

  if (APP_GetXY(&x, &y, &x8, &y8)==ERR_OK) {
    UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"X: 0x");
    UTIL1_strcatNum16Hex(buf, sizeof(buf), x);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"(");
    UTIL1_strcatNum8s(buf, sizeof(buf), x8);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)") Y: 0x");
    UTIL1_strcatNum16Hex(buf, sizeof(buf), y);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)"(");
    UTIL1_strcatNum8s(buf, sizeof(buf), y8);
    UTIL1_strcat(buf, sizeof(buf), (unsigned char*)")\r\n");
  } else {
    UTIL1_strcpy(buf, sizeof(buf), (unsigned char*)"GetXY() failed!\r\n");
  }
  CLS1_SendStatusStr((unsigned char*)"  analog", buf, io->stdOut);
}
#endif

static void REMOTE_PrintHelp(const CLS1_StdIOType *io) {
  CLS1_SendHelpStr((unsigned char*)"remote", (unsigned char*)"Group of remote commands\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows remote help or status\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  on|off", (unsigned char*)"Turns the remote on or off\r\n", io->stdOut);
  CLS1_SendHelpStr((unsigned char*)"  verbose on|off", (unsigned char*)"Turns the verbose mode on or off\r\n", io->stdOut);
#if PL_CONFIG_HAS_JOYSTICK
  CLS1_SendHelpStr((unsigned char*)"  joystick on|off", (unsigned char*)"Use joystick\r\n", io->stdOut);
#endif
}

static void REMOTE_PrintStatus(const CLS1_StdIOType *io) {
  CLS1_SendStatusStr((unsigned char*)"remote", (unsigned char*)"\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  remote", REMOTE_isOn?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  joystick", REMOTE_useJoystick?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
  CLS1_SendStatusStr((unsigned char*)"  verbose", REMOTE_isVerbose?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
#if PL_CONFIG_HAS_JOYSTICK
  StatusPrintXY(io);
#endif
}

uint8_t REMOTE_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io) {
  uint8_t res = ERR_OK;

  if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_HELP)==0 || UTIL1_strcmp((char*)cmd, (char*)"remote help")==0) {
    REMOTE_PrintHelp(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)CLS1_CMD_STATUS)==0 || UTIL1_strcmp((char*)cmd, (char*)"remote status")==0) {
    REMOTE_PrintStatus(io);
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote on")==0) {
    REMOTE_isOn = TRUE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote off")==0) {
#if PL_CONFIG_HAS_MOTOR
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_LEFT), 0);
    MOT_SetSpeedPercent(MOT_GetMotorHandle(MOT_MOTOR_RIGHT), 0);
#endif
    REMOTE_isOn = FALSE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote verbose on")==0) {
    REMOTE_isVerbose = TRUE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote verbose off")==0) {
    REMOTE_isVerbose = FALSE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote joystick on")==0) {
    REMOTE_useJoystick = TRUE;
    *handled = TRUE;
  } else if (UTIL1_strcmp((char*)cmd, (char*)"remote joystick off")==0) {
    REMOTE_useJoystick = FALSE;
    *handled = TRUE;
  }
  return res;
}

bool REMOTE_GetOnOff(void) {
  return REMOTE_isOn;
}

void REMOTE_SetOnOff(bool on) {
  REMOTE_isOn = on;
}

void REMOTE_Deinit(void) {
  /* nothing to do */
}

/*! \brief Initializes module */
void REMOTE_Init(void) {
  REMOTE_isOn = TRUE;
  REMOTE_isVerbose = FALSE;
  REMOTE_useJoystick = TRUE;
#if PL_CONFIG_CONTROL_SENDER
  if (FRTOS1_xTaskCreate(RemoteTask, "Remote", configMINIMAL_STACK_SIZE+50, NULL, tskIDLE_PRIORITY, NULL) != pdPASS) {
    for(;;){} /* error */
  }
#endif
}
#endif /* PL_HAS_REMOTE */

