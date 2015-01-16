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


#include "RTPHintPacketData.h"
#include <sys/types.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif
#include <assert.h>
#include <stdio.h>



RTPHintPacketData:: RTPHintPacketData (void)

{

}


RTPHintPacketData:: ~RTPHintPacketData (void)

{

}



RTPHintPacketData:: RTPHintPacketData (const RTPHintPacketData &)

{

	assert (1==0);

	// to do

}


RTPHintPacketData &RTPHintPacketData:: operator = (const RTPHintPacketData &)

{

	assert (1==0);

	// to do

	return *this;

}


Error RTPHintPacketData:: init (Tracer_p ptTracer)

{

	unsigned long					ulPropertiesSize;


	if (ptTracer == (Tracer_p) NULL)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_ACTIVATION_WRONG);

		return err;
	}

	_ptTracer						= ptTracer;

	if (createProperties (&ulPropertiesSize) != errNoError)
	{
		Error err = RTPErrors (__FILE__, __LINE__,
			RTP_RTPHINTPACKETDATA_CREATEPROPERTIES_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error RTPHintPacketData:: finish (void)

{


	return errNoError;
}

