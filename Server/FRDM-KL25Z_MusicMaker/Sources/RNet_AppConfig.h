/**
 * \file
 * \brief This is a configuration file for the RNet stack
 * \author (c) 2013 Erich Styger, http://mcuoneclipse.com/
 * \note MIT License (http://opensource.org/licenses/mit-license.html)
 *
 * Here the stack can be configured using macros.
 */

#ifndef __RNET_APP_CONFIG__
#define __RNET_APP_CONFIG__

#include "pl_conf.h"
/*! type ID's for application messages */
typedef enum {
  RAPP_MSG_TYPE_STDIN = 0x40,
  RAPP_MSG_TYPE_STDOUT = 0x41,
  RAPP_MSG_TYPE_STDERR = 0x42,
  RAPP_MSG_TYPE_HONK = 0x4,
 } RAPP_MSG_Type;

#endif /* __RNET_APP_CONFIG__ */
