/* initc v1.0
 * (C) Armador Technologies, 2017
 * Author: Jose Luis Tallon <jltallon@armador.xyz>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#include "environ.h"

#include "initc_common.h"
#include "initc_signals.h"
#include "cmdline.h"

#include <kversion.h>

bool 	g_bInitC_use_setsid = true;
bool	g_bInitC_foreground = false;
bool	g_bVerbose = false;
int		g_nDebugLevel = 0;
int		g_nMode = MODE_DEFAULT;

pid_t	g_pidMainChild = 0;


// We're Linux only :)
#include <sys/prctl.h>

// fwd decl
void initc_detach(void);
void initc_attach(void);



int main(int argc, char* argv[])
{
 sigset_t allsigs;
 int rc;


 
	// Reset all signals
	initc_setup_signals(&allsigs);
	
	void* ctx = initc_parse_cmdline(argc,(const char**)argv);
	
	
 krnlver kvi;
	long kv = kversion_get(&kvi);

	if( g_bVerbose )
	{
		fprintf(stderr,PREFIX"Running on kernel %u.%u(%u)\n", 
				kvi.major,kvi.minor,kvi.release);
	}
	
	UNUSED(kv);
	
	// Ensure proper operation when being debugged
	if( 1 != getpid() )
	{
		// Linux 3.4+
  #ifdef PR_SET_CHILD_SUBREAPER
		if( prctl(PR_SET_CHILD_SUBREAPER,0UL,0,0,0) < 0 )
			exit(254);
  #endif
	}
	// We ARE init, so no need to close all fds (shouldn't be open anyway)...?
	

#if defined(PR_SET_NAME)
	
	rc = prctl(PR_SET_NAME,"init", 0,0,0);
	if( rc < 0 )exit(0);

#endif

	if( MODE_SINGLE == g_nMode )
		g_bInitC_use_setsid = false;	// does not make sense ....
	
	if( g_bInitC_use_setsid )
	{
		// Setup default behaviour for signals
		initc_translate_ifndef(SIGTSTP, SIGSTOP);
		initc_translate_ifndef(SIGTTOU, SIGSTOP);
		initc_translate_ifndef(SIGTTIN, SIGSTOP);
	}

	/*
	 * If foreground, don't use setsid()
	 * otherwise, bash will not be able to do it itself ;)
	 */
 	if( g_bInitC_foreground )
	{
		g_bInitC_use_setsid = false;
	}
	else
	{
 		initc_detach();
	}
	
const char* const* command = initc_getArgs(ctx);

	if( NULL == command )
	{
		fprintf(stderr,PREFIX"No command specified.\n");
		exit(1);
	}
	
	if( !command[0] || !strlen(command[0]) )
	{
		fprintf(stderr,PREFIX"Args error\n");
		exit(3);
	}

	
	/* Prepare "spawn"
	 * Fork & setup
	 */
 pid_t child = fork();
	if (child < 0)
	{
		fprintf(stderr,PREFIX"Unable to fork. Exiting.\n");
		return 1;
		
	}
	else if( 0 == child )
	{
		// CHILD process
		initc_unblock_signals(&allsigs);
		
		if( g_bInitC_use_setsid )
		{
			if( setsid() < 0 )
			{
				fprintf(stderr,PREFIX"Unable to setsid(): errno=%d (%s). Exiting.\n",
					errno, strerror(errno)
				);
				exit(1);
			}
			
			// reattach CTTY if foreground
			if( g_bInitC_foreground )
				initc_attach();
			//TRACE(9,"setsid complete.\n");
		}

		
		execvp(command[0], (char * const*)command);


		// If this point is reached, exec failed: we should exit nonzero
		// Let's check errno and act accordingly
		
		//PRINTERR("%s: %s\n", cmd[0], strerror(errno));
		
		int status = 192;
		switch(errno )
		{
		 case ENOENT:
			if( g_bVerbose )
			{
				//trace("")
			}
			status = 127;
			break;
			
		 case EACCES:
			status = 126;
			break;
		}
		// fatal("exec %s failed: %s", argv[0], strerror(errno));
		return status;
	}
	else
	{
		// PARENT process
		g_pidMainChild = child;
	
		// TRACE("Child spawned with PID %d.\n", child_pid);
	

		/* MAIN LOOP
		* Forever: wait for all signals, handle them
		*/
	 int signum;
		for (;;)
		{
			signum=0;
			sigwait(&allsigs, &signum);
			initc_signal_handler(signum);
		}
	
	}
	
    return 0;	// NOTREACHED
}


void initc_detach(void)
{
	// detach self from controlling tty
	if( ioctl(STDIN_FILENO, TIOCNOTTY) < 0 )
	{
		// 	TRACE("Unable to detach from controlling tty (errno=%d %s).\n",
		// 		errno, strerror(errno)
		// 	);
	}
	return;
}

void initc_attach(void)
{
	if( ioctl(STDIN_FILENO, TIOCSCTTY, NULL) == -1) 
	{
		// TRACE("Unable to attach to controlling tty (errno=%d %s).\n",
		//		errno, strerror(errno)
		//	);
	}
	return;
}
