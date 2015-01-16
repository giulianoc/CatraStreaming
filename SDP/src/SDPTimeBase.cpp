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


#include "SDPTimeBase.h"
#include <stdio.h>
#include <stdlib.h>


SDPTimeBase:: SDPTimeBase (void)

{


}


SDPTimeBase:: ~SDPTimeBase (void)

{

}


Error SDPTimeBase:: init (TimeType_t ttTimeType)

{

	_ttTimeType				= ttTimeType;
	_ulNptTimeInSeconds		= 0;
	_ulFraction				= 0;
	_bIsNow					= false;


	return errNoError;
}


Error SDPTimeBase:: finish (void)

{


	return errNoError;
}


const Error SDPTimeBase:: getTimeType (TimeType_p pttTimeType) const

{

	*pttTimeType		= _ttTimeType;


	return errNoError;
}


Error SDPTimeBase:: getTimeAsNpt (double *pdNptTime, Boolean_p pbIsNow) const

{

	char			pBuffer [SDP_MAXLONGLENGTH];


	sprintf (pBuffer, "%lu.%lu", _ulNptTimeInSeconds, _ulFraction);

	*pdNptTime		= atof (pBuffer);
	*pbIsNow		= _bIsNow;


    return errNoError;
}

