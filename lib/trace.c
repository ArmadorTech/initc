/** Copyright (C) Armador Technologies, 2017
 *  Author: Jose Luis Tallon <jltallon@armador.xyz>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdio.h>
#include <stdarg.h>


void _trace(char* format, ...)
{
 va_list ap;

	va_start(ap,format);

	vfprintf(stderr,format,ap);

	va_end(ap);
}


void _debug(char* format, ...)
{
 va_list ap;

	va_start(ap,format);

	vfprintf(stderr,format,ap);

	va_end(ap);
}

