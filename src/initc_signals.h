/** Copyright (C) Armador Technologies, 2017
 *  Author: Jose Luis Tallon <jltallon@armador.xyz>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _INITC_SIGNALS__H_
#define _INITC_SIGNALS__H_


// Assume we are Ok mapping 1-31, since 32 and above are real-time signals in Linux
// (cfr. <include/linux/signal.h> within the Linux kernel source; checked at v4.12)
#define MAX_SIGNAL 31 /*(SIGRTMIN-1)*/



// fwd decl
struct sigset_t;

void	initc_setup_signals(sigset_t* pss);

void	initc_block_all_signals();

void 	initc_block_signals(sigset_t*);
void 	initc_unblock_signals(sigset_t*);
void 	initc_signal_handler(int signum);


bool	initc_signal_translate(int signum,int translated);
bool	initc_translate_ifndef(int signum, int newsig);
bool	initc_signal_ignore(int signum);


int					initc_parse_signame(const char const* signame);
const char const*	initc_print_signame(int signum);


#endif /* !defined(_INITC_SIGNALS__H_) */
