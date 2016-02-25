/*
 * player.h
 *
 *  Created on: 24.02.2016
 *      Author: flavio
 */

#ifndef SOURCES_PLAYER_H_
#define SOURCES_PLAYER_H_
#include "CLS1.h"
#

/*!
 * \brief Module command line parser
 * \param cmd Pointer to the command
 * \param handled Return value if the command has been handled by parser
 * \param io Shell standard I/O handle
 * \return Error code, ERR_OK if everything is OK
 */
uint8_t PLR_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);

void PLR_Init(void);

void PLR_Deinit(void);
#endif /* SOURCES_PLAYER_H_ */
