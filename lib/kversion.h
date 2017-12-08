/** Copyright (C) Armador Technologies, 2017
 *  Author: Jose Luis Tallon <jltallon@armador.xyz>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _KVERSION__H_
#define _KVERSION__H_


#include <stdint.h>


typedef struct {
	uint8_t	major;		// e.g. 2 / 3-
	uint8_t	minor;		// e.g. 6 / y
	uint8_t release;	// e.g.	y / z
	uint8_t patch;		// e.g. t
} krnlver;


/*void	kversion_init(krnlver* v);*/
long 	kversion_get(krnlver* v);


#define KVERSION(x,y,z)	(((x)<<24)|((y)<<16)|((z)<<8))


#endif
