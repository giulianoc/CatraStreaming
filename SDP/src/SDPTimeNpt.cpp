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


#include "SDPTimeNpt.h"
#include <stdio.h>
#include <stdlib.h>


SDPTimeNpt:: SDPTimeNpt (void): SDPTimeBase ()

{

}


SDPTimeNpt:: ~SDPTimeNpt (void)

{

}


Error SDPTimeNpt:: init (const char *pTimeNptBuffer)

{

	if (SDPTimeBase:: init (SDPTimeBase:: SDP_TIMETYPE_NPT) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPTIMEBASE_INIT_FAILED);

		return err;
	}

	if (!strncmp (pTimeNptBuffer, SDP_NOW, SDP_NOWLENGTH))
	{
		_bIsNow					= true;
		_ulNptTimeInSeconds		= 0;
		_ulFraction				= 0;
	}
	else if (strchr (pTimeNptBuffer, ':') != (char *) NULL)
	{
		unsigned long			ulHours;
		unsigned long			ulMinutes;
		unsigned long			ulSeconds;


		_bIsNow				= false;

		if (strchr (pTimeNptBuffer, '.') == (char *) NULL)
		{
			if (sscanf (pTimeNptBuffer, "%lu:%lu:%lu",
				&ulHours, &ulMinutes, &ulSeconds) != 3)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SSCANF_FAILED);

				return err;
			}

			_ulNptTimeInSeconds		=
				ulHours * 3600 + ulMinutes * 60 + ulSeconds;

			_ulFraction				= 0;
		}
		else
		{
			if (sscanf (pTimeNptBuffer, "%lu:%lu:%lu.%lu",
				&ulHours, &ulMinutes, &ulSeconds, &_ulFraction) != 4)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SSCANF_FAILED);

				return err;
			}

			_ulNptTimeInSeconds		=
				ulHours * 3600 + ulMinutes * 60 + ulSeconds;
		}
	}
	else
	{
		_bIsNow				= false;

		if (strchr (pTimeNptBuffer, '.') == (char *) NULL)
		{
			_ulNptTimeInSeconds		= atol (pTimeNptBuffer);

			_ulFraction				= 0;
		}
		else
		{
			if (sscanf (pTimeNptBuffer, "%lu.%lu",
				&_ulNptTimeInSeconds, &_ulFraction) != 2)
			{
				Error err = SDPErrors (__FILE__, __LINE__,
					SDP_SSCANF_FAILED);

				return err;
			}
		}
	}


	return errNoError;
}


Error SDPTimeNpt:: finish (void)

{

	if (SDPTimeBase:: finish () != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPTIMEBASE_FINISH_FAILED);

		return err;
	}


	return errNoError;
}


