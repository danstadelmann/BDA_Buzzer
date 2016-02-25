/*
 * RTOS.h
 *
 *  Created on: Oct 23, 2015
 *      Author: daniel
 */

#ifndef SOURCES_INTRO_COMMON_RTOS_H_
#define SOURCES_INTRO_COMMON_RTOS_H_

/*! \brief Runs the scheduler */
void RTOS_Run(void);

/*! \brief Initializes the RTOS module */
void RTOS_Init(void);

/*! \brief De-Initializes the RTOS module */
void RTOS_Deinit(void);

/*!
 * \brief Short button press
 */
void RTOS_ButtonSW1Press(void);

#endif /* SOURCES_INTRO_COMMON_RTOS_H_ */
