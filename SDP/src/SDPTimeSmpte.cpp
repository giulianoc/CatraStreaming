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


#include "SDPTimeSmpte.h"
#include <stdio.h>


SDPTimeSmpte:: SDPTimeSmpte (void): SDPTimeBase ()

{

}


SDPTimeSmpte:: ~SDPTimeSmpte (void)

{

}


Error SDPTimeSmpte:: init (const char *pTimeSmpteBuffer)

{

	if (SDPTimeBase:: init (SDPTimeBase:: SDP_TIMETYPE_SMPTE) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPTIMEBASE_INIT_FAILED);

		return err;
	}

	if (strchr (pTimeSmpteBuffer, '.') != (char *) NULL)
	{
		if (sscanf (pTimeSmpteBuffer,
			"%lu:%lu:%lu:%lu.%lu",
			_ulHours,
			_ulMinutes,
			_ulSeconds,
			_ulFrames,
			_ulSubFrames) != 5)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SSCANF_FAILED);

			return err;
		}
	}
	else
	{
		if (sscanf (pTimeSmpteBuffer,
			"%lu:%lu:%lu:%lu",
			_ulHours,
			_ulMinutes,
			_ulSeconds,
			_ulFrames) != 4)
		{
			Error err = SDPErrors (__FILE__, __LINE__,
				SDP_SSCANF_FAILED);

			return err;
		}

		_ulSubFrames		= 0;
	}

	/**
		The internally stored time in rounded seconds. The
		fraction that is expressed in terms of frames and
		subframes is ignored.
	*/
	_ulNptTimeInSeconds		=
		_ulHours * 3600 + _ulMinutes * 60 + _ulSeconds;
	_ulFraction				= 0;
	_bIsNow					= false;


	return errNoError;
}


Error SDPTimeSmpte:: init (
	unsigned long ulHours,
	unsigned long ulMinutes,
	unsigned long ulSeconds,
	unsigned long ulFrames,
	unsigned long ulSubFrames)

{

	if (SDPTimeBase:: init (SDPTimeBase:: SDP_TIMETYPE_SMPTE) != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPTIMEBASE_INIT_FAILED);

		return err;
	}

	_ulHours		= ulHours;
	_ulMinutes		= ulMinutes;
	_ulSeconds		= ulSeconds;
	_ulFrames		= ulFrames;
	_ulSubFrames	= ulSubFrames;

	/**
		The internally stored time in rounded seconds. The
		fraction that is expressed in terms of frames and
		subframes is ignored.
	*/
	_ulNptTimeInSeconds		=
		_ulHours * 3600 + _ulMinutes * 60 + _ulSeconds;
	_ulFraction				= 0;
	_bIsNow					= false;


	return errNoError;
} 


Error SDPTimeSmpte:: finish (void)

{

	if (SDPTimeBase:: finish () != errNoError)
	{
		Error err = SDPErrors (__FILE__, __LINE__,
			SDP_SDPTIMEBASE_FINISH_FAILED);

		return err;
	}


	return errNoError;
} 


