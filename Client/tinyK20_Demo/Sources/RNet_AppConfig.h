/*
 * RNet_AppConfig.h
 *
 *  Created on: Nov 27, 2015
 *      Author: daniel
 */

#ifndef SOURCES_INTRO_COMMON_RNET_APPCONFIG_H_
#define SOURCES_INTRO_COMMON_RNET_APPCONFIG_H_

/*! type ID's for application messages */
typedef enum {
  RAPP_MSG_TYPE_STDIN = 0x00,
  RAPP_MSG_TYPE_STDOUT = 0x01,
  RAPP_MSG_TYPE_STDERR = 0x02,
  RAPP_MSG_TYPE_ACCEL = 0x03,
  RAPP_MSG_TYPE_DATA = 0x04,
  RAPP_MSG_TYPE_JOYSTICK_XY = 0x05,
  RAPP_MSG_TYPE_JOYSTICK_BTN = 0x54, /* Joystick button message (data is one byte: 'A', 'B', ... 'F' and 'K') */
} RAPP_MSG_Type;

#endif /* SOURCES_INTRO_COMMON_RNET_APPCONFIG_H_ */
