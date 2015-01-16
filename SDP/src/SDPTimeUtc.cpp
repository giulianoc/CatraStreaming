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


#include "SDPTimeUtc.h"
#include <stdlib.h>


SDPTimeUtc:: SDPTimeUtc (void): SDPTimeBase ()

{

}


SDPTimeUtc:: ~SDPTimeUtc (void)

{

}


Error SDPTimeUtc:: init (const char *pTimeUtcBuffer)

{

	const char			*pT;
	const char			*pZ;
	const char			*pFraction;
	char				pBuffer	[
		SDP_MAXLONGLENGTH];


	if (SDPTimeBase:: init (SDPTimeBase:: SDP_TIMETYPE_UTC) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPTIMEBASE_INIT_FAILED);

		return err;
	}

	if ((pT = strchr (pTimeUtcBuffer, 'T')) == (char *) NULL ||
		pT - pTimeUtcBuffer != SDP_SDPUTCDATE_LENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPTIMEUTC_WRONGUTCBUFFER,
			1, pTimeUtcBuffer);

		return err;
	}

	if ((pZ = strchr (pTimeUtcBuffer, 'Z')) == (char *) NULL ||
		pZ - pT < SDP_SDPUTCTIME_LENGTH)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPTIMEUTC_WRONGUTCBUFFER,
			1, pTimeUtcBuffer);

		return err;
	}

	strncpy (pBuffer, pTimeUtcBuffer, 4);
	pBuffer [4]				= '\0';

	_ulYear			= atol (pBuffer);

	strncpy (pBuffer, pTimeUtcBuffer + 4, 2);
	pBuffer [2]				= '\0';

	_ulMonth		= atol (pBuffer);

	strncpy (pBuffer, pTimeUtcBuffer + 6, 2);
	pBuffer [2]				= '\0';

	_ulDay			= atol (pBuffer);

	strncpy (pBuffer, pTimeUtcBuffer + 9, 2);
	pBuffer [2]				= '\0';

	_ulHours		= atol (pBuffer);

	strncpy (pBuffer, pTimeUtcBuffer + 11, 2);
	pBuffer [2]				= '\0';

	_ulMinutes		= atol (pBuffer);

	strncpy (pBuffer, pTimeUtcBuffer + 13, 2);
	pBuffer [2]				= '\0';

	_ulSeconds		= atol (pBuffer);

	_ulNptTimeInSeconds		=
		_ulHours * 3600 + _ulMinutes * 60 + _ulSeconds;
	_bIsNow					= false;

	if ((pFraction = strchr (pTimeUtcBuffer, '.')) == (char *) NULL)
	{
		_ulFraction		= 0;
	}
	else
	{
		strcpy (pBuffer, pFraction + 1);
		pBuffer [strlen (pBuffer) - 1]			= '\0';

		_ulFraction		= atol (pBuffer);
	}


	return errNoError;
}


Error SDPTimeUtc:: finish (void)

{

	if (SDPTimeBase:: finish () != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPTIMEBASE_FINISH_FAILED);

		return err;
	}


    return errNoError;
}


