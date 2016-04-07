/*
 * player.h
 *
 *  Created on: 24.02.2016
 *      Author: flavio
 */

#ifndef SOURCES_PLAYER_H_
#define SOURCES_PLAYER_H_
#include "pl_conf.h"
#if PLAYER
#include "CLS1.h"


/*!
 * \brief Starts a new file. If a current file is playing playback will be stopped
 * and reset.
 * \param filename pointer to the filename
 * \return standard errorcodes of pe_error.h
 */
uint8_t PLR_StartNewFile(const char* filename, bool turn);

/*!
 * \brief Plays a full file. Is blocking until the file is finished!
 * \param filename pointer to the filename
  * \return standard errorcodes of pe_error.h
 */
uint8_t PLR_PlayFullFile(const char* filename);

/*
 * \brief Pauses the playback. If no current file is open nothing happens.
 * \return standard errorcodes of pe_error.h
 */
uint8_t PLR_PausePlayback(bool pause);


/*
 * \brief Stops current playback. If no current file is open nothing happens.
 * \return standard errorcodes of pe_error.h
 */
uint8_t PLR_StopPlayback(void);

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
#endif
#endif /* SOURCES_PLAYER_H_ */
