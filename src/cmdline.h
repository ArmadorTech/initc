/** Copyright (C) Armador Technologies, 2017
 *  Author: Jose Luis Tallon <jltallon@armador.xyz>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef __CMDLINE__H_
#define __CMDLINE__H_





void*			initc_parse_cmdline(int argc,const char** argv);
void 			initc_free_cmdline(void* ctx);

int				initc_nextopt(void* ctx);
const char*		initc_optarg(void* ctx);

const char**	initc_getArgs(void* ctx);




#endif // __CMDLINE__H_