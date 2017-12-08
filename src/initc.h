/** Copyright (C) Armador Technologies, 2017
 *  Author: Jose Luis Tallon <jltallon@armador.xyz>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef __INITC__H_
#define __INITC__H_


#include <stdbool.h>
#include <sys/types.h>


/* Globals exported from main
 * Used to pass global state among modules (e.g. operating mode)
 */

extern bool g_bInitC_use_setsid;
extern bool	g_bInitC_foreground;
extern bool	g_bVerbose;
extern int	g_nMode;
extern int	g_nDebugLevel;

extern pid_t g_pidMainChild;


#define VERSION	"0.2b"



#endif /* __INITC__H_ */
