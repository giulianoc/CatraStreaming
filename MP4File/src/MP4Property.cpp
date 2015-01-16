/*
 Copyright (C) Giuliano Catrambone (giuliano.catrambone@catrasoftware.it)

 This program is free software; you can redistribute it and/or 
 modify it under the terms of the GNU General Public License 
 as published by the Free Software Foundation; either 
 version 2 of the License, or (at your option) any later 
 version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 Commercial use other than under the terms of the GNU General Public
 License is allowed only after express negotiation of conditions
 with the authors.
*/

#include "MP4Property.h"
#include <stdio.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif


MP4Property:: MP4Property (void)

{

	_lInstancesNumber	= 0;
}


MP4Property:: ~MP4Property (void)

{

	_lInstancesNumber	= 0;
}


Error MP4Property:: init (const char *pName, long lInstancesNumber,
	Boolean_p pbIsImplicitProperty, Tracer_p ptTracer)

{

	if (pName == (const char *) NULL ||
		strlen (pName) >= MP4_MAXPROPERTYNAMELENGTH ||
		lInstancesNumber < 0 ||
		// pbIsImplicitProperty == (Boolean_p) NULL ||
		ptTracer == (Tracer_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_ptTracer				= ptTracer;

	strcpy (_pName, pName);

	if (pbIsImplicitProperty != (Boolean_p) NULL)
		_bIsImplicitProperty		= *pbIsImplicitProperty;

	_lInstancesNumber				= lInstancesNumber;


	return errNoError;
}


Error MP4Property:: getName (Buffer_p pbValue)

{

	if (pbValue == (Buffer_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (pbValue -> setBuffer (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}

