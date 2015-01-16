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

#include "MP4UInteger16BitsProperty.h"
#include "FileIO.h"
#include <stdio.h>
#ifdef WIN32
	// #include <winsock2.h>
#else
	#include <unistd.h>
	#include <netinet/in.h>
#endif


MP4UInteger16BitsProperty:: MP4UInteger16BitsProperty (void): MP4Property ()

{

	_pulValues			= (unsigned long *) NULL;

}


MP4UInteger16BitsProperty:: ~MP4UInteger16BitsProperty (void)

{

	if (_pulValues != (unsigned long *) NULL)
	{
		delete [] _pulValues;

		_pulValues			= (unsigned long *) NULL;
	}

}


Error MP4UInteger16BitsProperty:: init (const char *pName,
	long lInstancesNumber, Boolean_p pbIsImplicitProperty,
	unsigned long *pulValues, Tracer_p ptTracer)

{

	long					lIntegerIndex;


	if (MP4Property:: init (pName, lInstancesNumber, pbIsImplicitProperty,
		ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (pulValues == (unsigned long *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_pulValues != (unsigned long *) NULL)
	{
		delete [] _pulValues;

		_pulValues			= (unsigned long *) NULL;
	}

	if ((_pulValues = new unsigned long [_lInstancesNumber]) ==
		(unsigned long *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		_pulValues [lIntegerIndex]				= pulValues [lIntegerIndex];
	}


	return errNoError;
}


Error MP4UInteger16BitsProperty:: init (const char *pName,
	long lInstancesNumber, FileReader_p pfFile, Tracer_p ptTracer)

{

	long						lIntegerIndex;
	Boolean_t					bIsImplicitProperty;


	bIsImplicitProperty						= false;

	if (pfFile == (FileReader_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (MP4Property:: init (pName, lInstancesNumber, &bIsImplicitProperty,
		ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_pulValues != (unsigned long *) NULL)
	{
		delete [] _pulValues;

		_pulValues			= (unsigned long *) NULL;
	}

	if ((_pulValues = new unsigned long [_lInstancesNumber]) ==
		(unsigned long *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		if (pfFile -> readNetUnsignedInt16Bit (&(_pulValues [lIntegerIndex])) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_READNETUNSIGNEDINT16BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] (_pulValues);
			_pulValues					= (unsigned long *) NULL;

			return err;
		}
	}


	return errNoError;
}


Error MP4UInteger16BitsProperty:: init (const char *pName,
	long lInstancesNumber, const unsigned char *pucBuffer, Tracer_p ptTracer)

{

	long						lIntegerIndex;
	Boolean_t					bIsImplicitProperty;


	bIsImplicitProperty						= false;

	if (pucBuffer == (const unsigned char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (MP4Property:: init (pName, lInstancesNumber, &bIsImplicitProperty,
		ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_pulValues != (unsigned long *) NULL)
	{
		delete [] _pulValues;

		_pulValues			= (unsigned long *) NULL;
	}

	if ((_pulValues = new unsigned long [_lInstancesNumber]) ==
		(unsigned long *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		/*
		_pulValues [lIntegerIndex]			= (
			(pucBuffer [lIntegerIndex * 2] << 8) |
			pucBuffer [lIntegerIndex * 2 + 1]);
		*/
		_pulValues [lIntegerIndex]			= ntohs (
			*((unsigned short *) (pucBuffer + (lIntegerIndex * 2))));
	}


	return errNoError;
}


Error MP4UInteger16BitsProperty:: getValue (unsigned long *pulValue,
	long lIndex)

{

	if (pulValue == (unsigned long *) NULL ||
		lIndex < 0)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (lIndex >= _lInstancesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*pulValue					= _pulValues [lIndex];


	return errNoError;
}


Error MP4UInteger16BitsProperty:: getValue (unsigned char *pucValue,
	long lIndex)

{

	// unsigned short					usValue;


	if (pucValue == (unsigned char *) NULL ||
		lIndex < 0)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (lIndex >= _lInstancesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	/*
	usValue					= _pulValues [lIndex];

	pucValue [0]			= (usValue >> 8) & 0xFF;
	pucValue [1]			= usValue & 0xFF;
	*/

	*((unsigned short *) pucValue)			= htons (
		(unsigned short) (_pulValues [lIndex]));


	return errNoError;
}


Error MP4UInteger16BitsProperty:: setValue (unsigned long ulValue, long lIndex,
	unsigned long *pulSizeChangedInBytes)

{

	if (lIndex < 0)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*pulSizeChangedInBytes			= 0;

	// lIndex could be equal to _lInstancesNumber
	// In this case we may add a value
	if (lIndex > _lInstancesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (lIndex == _lInstancesNumber)
	{
		unsigned long				*pulLocalValues;
		long						lIntegerIndex;
		long						lIncrement;


		if ((pulLocalValues = new unsigned long [_lInstancesNumber + 1]) ==
			(unsigned long *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			return err;
		}

		for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber;
			lIntegerIndex++)
		{
			pulLocalValues [lIntegerIndex]				=
				_pulValues [lIntegerIndex];
		}

		pulLocalValues [lIntegerIndex]				= ulValue;

		_lInstancesNumber							+= 1;

		delete [] _pulValues;

		_pulValues									= pulLocalValues;

		lIncrement									= 2;

		*pulSizeChangedInBytes						= lIncrement;
	}
	else
		_pulValues [lIndex]				= ulValue;


	return errNoError;
}


Error MP4UInteger16BitsProperty:: incrementValue (long lIndex, long lIncrement)

{

	if (lIndex < 0)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (lIndex >= _lInstancesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	_pulValues [lIndex]					+= lIncrement;


	return errNoError;
}



Error MP4UInteger16BitsProperty:: write (int iFileDescriptor)

{

	long					lInstanceIndex;


	if (_bIsImplicitProperty)
	{
		/*
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
		*/

		return errNoError;
	}

	for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
		lInstanceIndex++)
	{
		if (FileIO:: writeNetUnsignedInt16Bit (iFileDescriptor,
			_pulValues [lInstanceIndex]) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITENETUNSIGNEDINT16BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error MP4UInteger16BitsProperty:: getSize (unsigned long *pulSize)

{

	if (_bIsImplicitProperty)
		*pulSize				= 0;
	else
		*pulSize				= _lInstancesNumber * 2;


	return errNoError;
}


Error MP4UInteger16BitsProperty:: getType (char *pType)

{

	strcpy (pType, "UInteger16Bits");


	return errNoError;
}


Error MP4UInteger16BitsProperty:: appendDump (Buffer_p pbBuffer)

{

	long									lIntegerIndex;
	char									pHexadecimal [MP4_MAXLONGLENGTH];


	if (pbBuffer -> append (_pName) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbBuffer -> append (" = ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		if (pbBuffer -> append (_pulValues [lIntegerIndex]) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		sprintf (pHexadecimal, "0x%lX", _pulValues [lIntegerIndex]);

		if (pbBuffer -> append (" (") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbBuffer -> append (pHexadecimal) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (pbBuffer -> append (")") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (lIntegerIndex != _lInstancesNumber - 1)
		{
			if (pbBuffer -> append (", ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}
	}

	if (_bIsImplicitProperty)
	{
		if (pbBuffer -> append (" (implicit)") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (pbBuffer -> append ("\n") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4UInteger16BitsProperty:: printOnStdOutput (void)

{

	long									lIntegerIndex;
	char									pHexadecimal [MP4_MAXLONGLENGTH];


	std:: cout << _pName << " = ";

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		std:: cout << _pulValues [lIntegerIndex];
		sprintf (pHexadecimal, "0x%lX", _pulValues [lIntegerIndex]);
		std:: cout << " (" << pHexadecimal << ")";

		if (lIntegerIndex != _lInstancesNumber - 1)
		{
			std:: cout << ", ";
		}
	}

	if (_bIsImplicitProperty)
		std:: cout << " (implicit)";
	std:: cout << std:: endl;
	std:: cout. flush ();


	return errNoError;
}

