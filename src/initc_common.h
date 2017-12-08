/** Copyright (C) Armador Technologies, 2017
 *  Author: Jose Luis Tallon <jltallon@armador.xyz>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _INITC_COMMON__INCLUDED_
#define _INITC_COMMON__INCLUDED_



#include <stdbool.h>
#include <sys/types.h>

//#define DEBUG_INITC	"initc:DEBUG: "
#define PREFIX	"initc: "



enum initc_mode { 
	MODE_UNKNOWN	= 0,
	MODE_SINGLE		= 1,
	MODE_DEFAULT	= 2,
	MODE_SUPERVISOR	= 3
};


struct initcService {
		
	
};


#ifndef UNUSED
  #ifdef __GNUC__
    #define UNUSEDFUNC __attribute__((unused))
  #endif
  #define UNUSED(x) (void)x
#endif


#ifdef GCC
  #define _NORETURN __attribute__ ((noreturn))
#else
  #define _NORETURN
#endif


#endif
