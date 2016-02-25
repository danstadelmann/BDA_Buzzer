/*
 * Event.h
 *
 *  Created on: Oct 8, 2015
 *      Author: daniel
 */

#ifndef SOURCES_INTRO_COMMON_EVENT_H_
#define SOURCES_INTRO_COMMON_EVENT_H_

#include "Platform.h"

#if PL_CONFIG_HAS_EVENTS

typedef enum EVNT_Handle {
	EVNT_STARTUP, // System startup event
#if PL_CONFIG_NOF_KEYS >= 1
  EVNT_SW1_PRESSED,
  EVNT_SW1_LPRESSED,
  EVNT_SW1_RELEASED,
#endif
#if PL_CONFIG_NOF_KEYS >= 2
  EVNT_SW2_PRESSED,
  EVNT_SW2_LPRESSED,
  EVNT_SW2_RELEASED,
#endif
#if PL_CONFIG_NOF_KEYS >= 3
  EVNT_SW3_PRESSED,
  EVNT_SW3_LPRESSED,
  EVNT_SW3_RELEASED,
#endif
#if PL_CONFIG_NOF_KEYS >= 4
  EVNT_SW4_PRESSED,
  EVNT_SW4_LPRESSED,
  EVNT_SW4_RELEASED,
#endif
#if PL_CONFIG_NOF_KEYS >= 5
  EVNT_SW5_PRESSED,
  EVNT_SW5_LPRESSED,
  EVNT_SW5_RELEASED,
#endif
#if PL_CONFIG_NOF_KEYS >= 6
  EVNT_SW6_PRESSED,
  EVNT_SW6_LPRESSED,
  EVNT_SW6_RELEASED,
#endif
#if PL_CONFIG_NOF_KEYS >= 7
  EVNT_SW7_PRESSED,
  EVNT_SW7_LPRESSED,
  EVNT_SW7_RELEASED,
#endif
#if PL_CONFIG_HAS_TETRIS
  EVNT_TETRIS_LEFT,
  EVNT_TETRIS_RIGHT,
  EVNT_TETRIS_UP,
  EVNT_TETRIS_DOWN,
  EVNT_TETRIS_DROP,
#endif
#if PL_CONFIG_HAS_SNAKE
  EVNT_SNAKE_START_PAUSE,
  EVNT_SNAKE_UP,
  EVNT_SNAKE_DOWN,
  EVNT_SNAKE_LEFT,
  EVNT_SNAKE_RIGHT,
#endif
#if PL_CONFIG_HAS_REFLECTANCE
  EVNT_START_CALIB,
  EVNT_STOP_CALIB,
#endif
	EVNT_NOF_EVENTS // has to be the last event!
} EVNT_Handle;

/**
 * Sets an event.
 * param event: The handle of the event to set
 */
void EVNT_SetEvent(EVNT_Handle event);

/**
 * Clears an event.
 * param event: The event handle of the event to clear
 */
void EVNT_ClearEvent(EVNT_Handle event);

/**
 * Returns the status of the selected event
 * param event: the event handler of the event to check
 * returns true if the event is set, false otherwise
 */
bool EVNT_EventIsSet(EVNT_Handle event);

/**
 * Returns the status of an event. As a side effect, the event gets cleared.
 * param event: The event handler of the event to check and clear.
 * returns true if the event is set, false otherwise
 */
bool EVNT_EventIsSetAutoClear(EVNT_Handle event);

/**
 * Routine to check if an event is pending. If an event is pending, the event is cleared and the provided callback is called.
 * param *callback: Callback routine to be called. The event handle is passed as an argument to the callback.
 */
void EVNT_HandleEvent(void (*callback)(EVNT_Handle), bool clearEvent);

/**
 * Event module initialization
 */
void EVNT_Init(void);

/**
 * Event module de-initialization
 */
void EVNT_Deinit(void);

#endif /* PL_HAS_EVENTS */

#endif /* SOURCES_INTRO_COMMON_EVENT_H_ */
