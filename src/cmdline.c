/** Copyright (C) Armador Technologies, 2017
 *  Author: Jose Luis Tallon <jltallon@armador.xyz>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdio.h>
#include <popt.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

#include "initc.h"
#include "initc_misc.h"
#include "initc_common.h"	
#include "initc_signals.h"


#ifndef _
  #define _(x)	x
#endif


/*
struct poptOption {
	const char * longName; // may be NULL
	char shortName;        // may be '\0'
	int argInfo;
	void * arg;            // depends on argInfo
	int val;               // 0 means don't return, just update flag
	char * descrip;        // description for autohelp -- may be NULL
	char * argDescrip;     // argument description for autohelp
};
*/


static const struct poptOption options[]= {
	{ "command",	'c', 	POPT_ARG_VAL|POPT_ARGFLAG_ONEDASH,
		&g_nMode, MODE_SINGLE,
		"Run in single-child mode.\n\t\t\tIn this mode, signals are only proxied to the direct child and not any of its descendants.",
		""
	},
	{ "rewrite",	'r', 	POPT_ARG_STRING,
		NULL, 		'r',
		"Rewrite signal S to R before proxying. "
		"To ignore(not proxy) a signal, specify IGNORE.\n"
		"\t\t\tSupports both numerical and symbolic (e.g. 'SIGHUP') specs.",
		"S:R (multiple)"
	},
	{ "foreground",	'f',POPT_ARG_NONE,
		&g_bInitC_foreground, 0,
		"Do not detach from controlling TTY before forking (mostly for debugging)."
		""
	},
#ifndef NDEBUG
	{ "verbose",	'v',	POPT_ARG_NONE,
		&g_bVerbose, 0,
		"Be verbose.",
		""
	},
#endif
#ifdef DEBUG
	{ "debug",		'd', 	POPT_ARG_INT,
		&g_nDebugLevel,	0,
		"Specify debug level. Outputs to stderr.",
		"INT (0-10)"
	},
#endif
	{ "version",	'V',	POPT_ARG_NONE|POPT_ARGFLAG_ONEDASH,
		NULL,	'V',
		"Show version information and exit.",
		""
	},
	{ NULL, 'h', POPT_ARG_NONE|POPT_ARGFLAG_ONEDASH|POPT_ARGFLAG_DOC_HIDDEN, NULL, 'h', "","" },
	POPT_AUTOHELP
	POPT_TABLEEND		//{NULL,0,0,NULL,0,NULL}
};





// Forward decls
static void _parse_rewrite_signal(const char* arg);
static void help_rewrite_sig();
static void initc_usage(void* ctx);


void*
initc_parse_cmdline(int argc,const char** argv)
{
	
 poptContext ctx=
	poptGetContext(NULL,argc,argv,
					options,
					POPT_CONTEXT_NO_EXEC
	);
	
	poptSetOtherOptionHelp(ctx, "[OPTIONS] -- <command-plus-args>");

	
	if( argc < 2 )
		initc_usage(ctx);
	
	/*
	 * Actual command line parsing happens here.
	 * First call to poptGetNextOpt() populates all options with non-null "args"
	 * Remaining options return the corresponding "val" in rc
	 */
 const char* pszArg=NULL;
 int rc;
 
	while( (rc=poptGetNextOpt(ctx)) >= 0) 
	{
		switch (rc)
		{
		case 'r':
			pszArg = poptGetOptArg(ctx);
			_parse_rewrite_signal(pszArg);
			xfree(pszArg);
			break;
		
		case 'h':
			initc_usage(ctx);
			exit(0);
			break;
		
		case 'V':
			fprintf(stderr,PREFIX" v"VERSION"\n");
			exit(0);
			break;
		
		default:
			// Huh ???
			break;
		}
		
	}
	
	if( -1 != rc )	// some error occured ....
	{
// 		fatal(1,"%s: %s\n",
			  // poptBadOption(ctx,0), poptStrerror(ret));
			  // poptPrintUsage(ctx,stderr,0);
			  // return 1;
		
	}
	

	return ctx;
}

void
initc_free_cmdline(void* ctx)
{
	poptFreeContext(ctx);
}

int
initc_nextopt(void* ctx)
{
	if( NULL == ctx )
		return -1;
	
	return poptGetNextOpt(ctx);
}

const char*
initc_optarg(void* ctx)
{
	if( NULL == ctx )
		return NULL;
	
	return poptGetOptArg(ctx);
}


const char**
initc_getArgs(void* ctx)
{
	if( NULL == ctx )
		return NULL;
	
	return poptGetArgs(ctx);
}


static void
_parse_rewrite_signal(const char* arg)
{
	int signum, newsig;
	char *signame=NULL, *signew=NULL;
	
	if ( sscanf(arg, "%d:%d", &signum, &newsig) == 2 
		&& (signum>=1 && signum<=MAX_SIGNAL)
		&& (newsig>=0 && newsig<=MAX_SIGNAL) ) 
	{
		// Setup signal rewrite
		initc_signal_translate(signum,newsig);
	}
	else if ( sscanf(arg, "%m[A-Z12]:%m[A-Z12]", &signame, &signew) == 2 )
	{
		signum = initc_parse_signame(signame);
		newsig = initc_parse_signame(signew);
		
		if(	(signum>=1 && signum<=MAX_SIGNAL)
			&& (newsig>=0 && newsig<=MAX_SIGNAL)
		)
		{
			// Setup signal rewrite
			initc_signal_translate(signum,newsig);
		}
		
		xfree(signame);
		xfree(signew);
	}
	else
	{
		help_rewrite_sig();
		return;
	}
	
	// DEBUG("Parsed '%s': %d -> %d", arg,signum, newsig);
}



//////////////////////////////////////////////////////////////////////////////

static void
help_rewrite_sig()
{
	fprintf(stderr,PREFIX" v"VERSION"\n");
	
}

static void
initc_usage(void* ctx)
{
	poptPrintUsage(ctx,stderr,0);
	exit(1);
}
