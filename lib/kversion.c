/** Copyright (C) Armador Technologies, 2017
 *  Author: Jose Luis Tallon <jltallon@armador.xyz>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "kversion.h"

#include <stdio.h>
#include <errno.h>
#include <sys/utsname.h>



#define KVER_INT(kvi)	(((kvi).major<<24)|((kvi).minor<<16)|((kvi).release<<8)|(kvi).patch)



long kversion_get(/*out*/krnlver* pkv)
{
	if( NULL == pkv )
		return -1;

  struct utsname un;
	if( uname(&un) < 0 )
		return -1;

	if( 4 == sscanf(un.release,"%hhu.%hhu.%hhu.%hhu", &pkv->major, &pkv->minor, &pkv->release, &pkv->patch) )
	{
		return KVER_INT(*pkv);
	} else
	if( 3 == sscanf(un.release,"%hhu.%hhu.%hhu", &pkv->major, &pkv->minor, &pkv->release) )
	{
		pkv->patch=0;
		return KVER_INT(*pkv);
	}

	errno=EINVAL;
	return -1;
}


/*
void	kversion_init(krnlver* v)
{
	memset(v,0,sizeof(krnlver))
}
*/
