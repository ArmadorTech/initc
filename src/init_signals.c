/** Copyright (C) Armador Technologies, 2017
 *  Author: Jose Luis Tallon <jltallon@armador.xyz>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <assert.h>

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "initc.h"
#include "initc_common.h"
#include "initc_misc.h"
#include "initc_signals.h"

/*
 * Signal rewriting logic.
 * Assume we are Ok mapping 1-31, since 32 and above are real-time signals in Linux
 * (cfr. <include/linux/signal.h> within the Linux kernel source; checked at v4.12)
 */


static const char const* _initc_signal_names[MAX_SIGNAL+1+1] = {
	//	POSIX signal	(default action)	[from Linux source]
	"IGNORE",	/* trick for initc_parse_signame */
	"SIGHUP",	/*terminate*/
	"SIGINT",	/*terminate*/
	"SIGQUIT",	/*coredump*/
	"SIGILL",	/*coredump*/
	"SIGTRAP",	/*coredump*/
	"SIGABRT/SIGIOT",	/*coredump*/
	"SIGBUS",	/*coredump*/
	"SIGFPE",	/*coredump*/
	"SIGKILL",	/*terminate !!*/
	"SIGUSR1",	/*terminate*/
	"SIGSEGV",	/*coredump*/
	"SIGUSR2",	/*terminate*/
	"SIGPIPE",	/*terminate*/
	"SIGALRM",	/*terminate*/
	"SIGTERM",	/*terminate*/	
	"SIGCHLD",	/*ignore*/
	"SIGCONT",  /*ignore(*)*/
	"SIGSTOP",	/*stop(*)(+)*/
	"SIGTSTP",	/*stop(*)*/
	"SIGTTIN",	/*stop(*)*/
	"SIGTTOU",	/*stop(*)*/
	"SIGURG",	/*ignore*/
	"SIGXCPU",	/*coredump*/
	"SIGXFSZ",  /*coredump*/
	"SIGVTALRM",/*terminate*/
	"SIGPROF",	/*terminate*/
	"SIGPOLL/SIGIO",	/*terminate*/
	"SIGSYS/SIGUNUSED",	/*coredump*/
	"SIGSTKFLT",	/*terminate*/
	"SIGWINCH",		/*ignore*/
	"SIGPWR",	/*terminate*/
	NULL
};


static const char const* _initc_sigzero = "<zero>";

// Signals are one-indexed. Index zero is unused.
static int
_initc_signal_xlat[MAX_SIGNAL+1] = { [0 ... MAX_SIGNAL] = -1 };


static int initc_signal_xlat(int sig)
{
	register int ts;
	
	// bounds + sanity check
	if(sig <= 0 || sig > MAX_SIGNAL) 
	{
		return /*(-1)*/sig;		// garbage in, garbage out
    } 
    else
	{
        if( (ts = _initc_signal_xlat[sig]) < 0 )
		{
			return sig;
        }
        else
		{
            // TRACE(DEBUG_INITC"Signal %d translated to %u.\n", signum, translated);
            return ts;
        }
    }
}



/*
 * Dummy signal handler for signals we actually care about :)
 * With a FreeBSD kernel (e.g. debian-kfreebsd), ignored signals cannot be 
 * waited on by `sigwait` (but they can be on Linux). We must provide a dummy handler.
 * https://lists.freebsd.org/pipermail/freebsd-ports/2009-October/057340.html
 */
static void 
initc_dummy_sighandler(int sn)
{
	UNUSED(sn);
	// Do nothing ....
}



/*
 * initC setup signals
 * 
 */
void
initc_setup_signals(sigset_t* pss)
{
	// block all signals ....
	sigfillset(pss);
	sigprocmask(SIG_BLOCK, pss, NULL);
	
	// Reset all signals to "do nothing"
	for (int i = 1; i <= MAX_SIGNAL; i++)
		signal(i, initc_dummy_sighandler);
	
	return;
}

void
initc_block_all_signals()
{
 sigset_t allsigs;
	
	// block all signals ....
	sigfillset(&allsigs);
	sigprocmask(SIG_BLOCK, &allsigs, NULL);
}

void
initc_block_signals(sigset_t* pss)
{ 
	// block all signals ....
	sigfillset(pss);
	sigprocmask(SIG_BLOCK, pss, NULL);
		
	return;
}

void
initc_unblock_signals(sigset_t* pss)
{	
	// unblock all signals ....
	sigprocmask(SIG_UNBLOCK, pss, NULL);
	
	return;
}

/*
 * initc_signal_translate() - setup signal translation
 */
bool
initc_signal_translate(int signum,int translated)
{
	// bounds check
	if(signum <= 0 || signum > MAX_SIGNAL) 
		return false;	// Huh ?

	// setup translation
	_initc_signal_xlat[signum] = translated;
	
	return true;
}

/*
 * initc_signal_ignore() - set signum to be ignored
 */
bool
initc_signal_ignore(int signum)
{
	// bounds check
	if(signum <= 0 || signum > MAX_SIGNAL) 
		return false;	// Huh ?
		
	
	_initc_signal_xlat[signum] = 0;	// IGNORE

	signal(signum, initc_dummy_sighandler);
	return true;
}


bool
initc_translate_ifndef(int signum, int sigdest)
{
	// bounds check
	if(signum <= 0 || signum > MAX_SIGNAL) 
		return false;	// Huh ?
		
	if( -1 == _initc_signal_xlat[signum] )
	{
		_initc_signal_xlat[signum] = sigdest;
		return true;	// rewritten
	}
	return false;
}


/*
 * Parse signal name into int;
 * Special case: "IGNORE" -> 0
 * Returns -1 on error
 */
int
initc_parse_signame(const char* const signame)
{
	if( !signame || !*signame )
		return -1;
	
	
	
	// 1-indexed + padding NULL at the end ...
	// cfr _initc_signal_names above
	for(int i=0; i < sizeof(_initc_signal_names)/sizeof(_initc_signal_names[0]); i++)
	{
		// IGNORE returned as signal 0; rest as expected
		if( !strcmp(signame,_initc_signal_names[i]) )
			return i;
	}
	return -1;
}

const char const*
initc_print_signame(int signum)
{
	// bounds check
	if( signum < 0 || signum > MAX_SIGNAL )
		return NULL;	// Huh ?
	
	// Special case for zero: input->IGNORE / output-><zero>
	if( 0 == signum )
		return _initc_sigzero;
		
	return _initc_signal_names[signum];
}

////////////////////////////////////////////////////////////////////////////////


static void initc_signal_forward(int sig)
{
    register int sn = initc_signal_xlat(sig);
    if( sn > 0 )
	{
		kill(g_bInitC_use_setsid? -g_pidMainChild : g_pidMainChild, sn);
        // TRACE(DEBUG_INITC"Forwarded signal %d to children (as %u)\n", sig,sn);
    }
    else 
	{
        // TRACE(DEBUG_INITC"Not forwarding signal %d to children (ignored).\n", sig);
    }
}


/*
 * initC signal handler.
 * Basically, forwards signals to child processes (optionally translating them)
 * In setsid mode (default), this means signaling the entire process group
 * In non-setsid mode, this is just signaling the primary child.
 *
 * Proxying the received signal is usually enough.
 * For job control, we need to sleep/wake ourselves too: This allows users to
 * run foreground processes using initC and control them using normal shell
 * job control features (e.g. Ctrl-Z + bg/fg -> SIGSTOP / SIGCONT).
 */
void initc_signal_handler(int signum)
{
	switch(signum)
	{
		case SIGCHLD: {
			int status, exit_status;
			pid_t killed_pid;
			
			// Cleanup children
			while( (killed_pid = waitpid(-1, &status, WNOHANG)) > 0)
			{
				if( WIFEXITED(status) )
				{
					exit_status = WEXITSTATUS(status);
				} 
				else
				{
					assert( WIFSIGNALED(status) );
					exit_status = 128 + WTERMSIG(status);
				}
				
				if( killed_pid == g_pidMainChild )
				{
					initc_signal_forward(SIGTERM);  // send SIGTERM to any remaining children
					exit(exit_status);
				}
			}
		}
		break;
		
		case SIGTSTP:
		case SIGTTOU:
		case SIGTTIN:
			initc_signal_forward(signum);
			kill(getpid(), SIGSTOP);
			
			break;
			
		default:
			// Forward signal to children
			initc_signal_forward(signum);
	}
}
