/*
 * KeyDebounce.h
 *
 *  Created on: Oct 22, 2015
 *      Author: daniel
 */

#ifndef SOURCES_INTRO_COMMON_KEYDEBOUNCE_H_
#define SOURCES_INTRO_COMMON_KEYDEBOUNCE_H_

/*!
 * \brief Kicks the debouncing state machine.
 */
void KEYDBNC_Process(void);

/*!
 * \brief Driver initialization.
 */
void KEYDBNC_Init(void);

/*!
 * \brief Driver de-initialization.
 */
void KEYDBNC_Deinit(void);

#endif /* SOURCES_INTRO_COMMON_KEYDEBOUNCE_H_ */
