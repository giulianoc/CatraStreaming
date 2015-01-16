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

#include "MP4StringCountedProperty.h"
#include "FileIO.h"
#include <stdio.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif


MP4StringCountedProperty:: MP4StringCountedProperty (void): MP4Property ()

{

	_pValues			= (char **) NULL;

}


MP4StringCountedProperty:: ~MP4StringCountedProperty (void)

{

	if (_pValues != (char **) NULL)
	{
		long			lInstanceIndex;

		for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
			lInstanceIndex++)
			delete [] (_pValues [lInstanceIndex]);

		if (_pValues != (char **) NULL)
			delete [] _pValues;

		_pValues			= (char **) NULL;
	}
}


Error MP4StringCountedProperty:: init (const char *pName,
	Boolean_p pbIsImplicitProperty, Boolean_t bUnicode,
	Boolean_t bExpandedCount, char **pValues, Tracer_p ptTracer)

{

	long					lIntegerIndex;


	if (MP4Property:: init (pName, 1, pbIsImplicitProperty,
		ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (pValues == (char **) NULL ||
		pValues [0] == (char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_pValues != (char **) NULL)
	{
		long			lInstanceIndex;

		for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
			lInstanceIndex++)
			delete [] (_pValues [lInstanceIndex]);

		if (_pValues != (char **) NULL)
			delete [] _pValues;

		_pValues			= (char **) NULL;
	}

	_bUnicode					= bUnicode;
	_bExpandedCount				= bExpandedCount;

    // exclude '\0'
	_ulBytesNumber					= strlen (pValues [0]);

	if ((_pValues = new char * [_lInstancesNumber]) == (char **) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		if ((_pValues [lIntegerIndex] = new char [_ulBytesNumber + 1]) ==
			(char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			while (--lIntegerIndex >= 0)
			{
				delete [] ((_pValues) [lIntegerIndex]);
				(_pValues) [lIntegerIndex]			= (char *) NULL;
			}
			delete [] (_pValues);
			_pValues								= (char **) NULL;

			return err;
		}
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		memcpy ((_pValues [lIntegerIndex]), pValues [lIntegerIndex],
			_ulBytesNumber);
		_pValues [lIntegerIndex][_ulBytesNumber]				= '\0';
	}


	return errNoError;
}


Error MP4StringCountedProperty:: init (const char *pName,
	Boolean_t bUnicode, Boolean_t bExpandedCount, unsigned long *pulBytesNumber,
	FileReader_p pfFile, Tracer_p ptTracer)

{

	long						lIntegerIndex;
	Boolean_t					bIsImplicitProperty;
	unsigned long				ulCharsNumber;
	unsigned long				ulBytesNumberForCounted;
	#ifdef WIN32
		__int64					ullCharsRead;
	#else
		unsigned long long		ullCharsRead;
	#endif


	bIsImplicitProperty						= false;

	if (pulBytesNumber == (unsigned long *) NULL ||
		pfFile == (FileReader_p) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (MP4Property:: init (pName, 1, &bIsImplicitProperty,
		ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (_pValues != (char **) NULL)
	{
		long			lInstanceIndex;

		for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
			lInstanceIndex++)
			delete [] (_pValues [lInstanceIndex]);

		if (_pValues != (char **) NULL)
			delete [] _pValues;

		_pValues			= (char **) NULL;
	}

	_bUnicode					= bUnicode;
	_bExpandedCount				= bExpandedCount;

	ulCharsNumber				= 0;

	ulBytesNumberForCounted		= 0;

	if (_bExpandedCount)
	{
		unsigned long				ulTemp;


		do
		{
			if (pfFile -> readNetUnsignedInt8Bit (&ulTemp) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_READNETUNSIGNEDINT8BIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			ulCharsNumber			+= ulTemp;

			ulBytesNumberForCounted++;
		}
		while (ulTemp == 255);
	}
	else
	{
		if (pfFile -> readNetUnsignedInt8Bit (&ulCharsNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_READNETUNSIGNEDINT8BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		ulBytesNumberForCounted++;
	}

	_ulBytesNumber					= ulCharsNumber * (_bUnicode ? 2 : 1);

	*pulBytesNumber					= _ulBytesNumber + ulBytesNumberForCounted;

	if ((_pValues = new char * [_lInstancesNumber]) ==
		(char **) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		if ((_pValues [lIntegerIndex] = new char [_ulBytesNumber +
			1]) == (char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			while (--lIntegerIndex >= 0)
			{
				delete [] ((_pValues) [lIntegerIndex]);
				(_pValues) [lIntegerIndex]		= (char *) NULL;
			}
			delete [] (_pValues);
			_pValues								= (char **) NULL;

			return err;
		}
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		if (pfFile -> readChars (_pValues [lIntegerIndex],
			_ulBytesNumber, true, &ullCharsRead) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_READCHARS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber;
				lIntegerIndex++)
			{
				delete [] ((_pValues) [lIntegerIndex]);
				(_pValues) [lIntegerIndex]		= (char *) NULL;
			}
			delete [] (_pValues);
			_pValues								= (char **) NULL;

			return err;
		}

		_pValues [lIntegerIndex][_ulBytesNumber]				= '\0';
	}


	return errNoError;
}


Error MP4StringCountedProperty:: getValue (Buffer_p pbValue,
	long lIndex)

{

	if (pbValue == (Buffer_p) NULL ||
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

	if (pbValue -> setBuffer (_pValues [lIndex]) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4StringCountedProperty:: setValue (Buffer_p pbValue, long lIndex,
	long *plSizeChangedInBytes)

{

	long					lIncrement;
	unsigned long			ulBytesNumberForCountedNow;
	unsigned long			ulBytesNumberForCountedBefore;


	if (pbValue == (Buffer_p) NULL ||
		lIndex < 0)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*plSizeChangedInBytes					= 0;

	// lIndex will be 0
	if (lIndex >= _lInstancesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	// initialize ulBytesNumberForCountedBefore
	{
		unsigned long						ulCharsNumber;


		ulBytesNumberForCountedBefore			= 0;

		ulCharsNumber				= _ulBytesNumber / (_bUnicode ? 2 : 1);

		if (_bExpandedCount)
		{
			while (ulCharsNumber >= 0xFF)
			{
				// FileIO:: writeNetUnsignedInt8Bit 0xFF
				ulBytesNumberForCountedBefore				+= 1;

				ulCharsNumber								-= 0xFF;
			}

			// FileIO:: writeNetUnsignedInt8Bit ulCharsNumber
			ulBytesNumberForCountedBefore				+= 1;
		}
		else
		{
			// FileIO:: writeNetUnsignedInt8Bit ulCharsNumber
			ulBytesNumberForCountedBefore				+= 1;
		}
	}

	delete [] _pValues [lIndex];
	_pValues [lIndex]				= (char *) NULL;

	// initialize ulBytesNumberForCountedNow
	{
		unsigned long						ulCharsNumber;


		ulBytesNumberForCountedNow			= 0;

		ulCharsNumber				= ((unsigned long) (*pbValue)) / (_bUnicode ? 2 : 1);

		if (_bExpandedCount)
		{
			while (ulCharsNumber >= 0xFF)
			{
				// FileIO:: writeNetUnsignedInt8Bit 0xFF
				ulBytesNumberForCountedNow				+= 1;

				ulCharsNumber							-= 0xFF;
			}

			// FileIO:: writeNetUnsignedInt8Bit ulCharsNumber
			ulBytesNumberForCountedNow				+= 1;
		}
		else
		{
			// FileIO:: writeNetUnsignedInt8Bit ulCharsNumber
			ulBytesNumberForCountedNow				+= 1;
		}
	}

	lIncrement						= (ulBytesNumberForCountedNow -
		ulBytesNumberForCountedBefore) + (((unsigned long) (*pbValue)) - _ulBytesNumber);

	// exclude '\0'
	_ulBytesNumber					= ((unsigned long) (*pbValue));

	if ((_pValues [lIndex] = new char [_ulBytesNumber + 1]) == (char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	strcpy (_pValues [lIndex], (const char *) *pbValue);

	*plSizeChangedInBytes				= lIncrement;


	return errNoError;
}


Error MP4StringCountedProperty:: write (int iFileDescriptor)

{

	unsigned long				ulCharsNumber;
	#ifdef WIN32
		__int64					llBytesWritten;
	#else
		long long				llBytesWritten;
	#endif


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

	ulCharsNumber				= _ulBytesNumber / (_bUnicode ? 2 : 1);

	if (_bExpandedCount)
	{
		while (ulCharsNumber >= 0xFF)
		{
			if (FileIO:: writeNetUnsignedInt8Bit (iFileDescriptor,
				0xFF) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEIO_WRITENETUNSIGNEDINT8BIT_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}

			ulCharsNumber				-= 0xFF;
		}

		if (FileIO:: writeNetUnsignedInt8Bit (iFileDescriptor,
			ulCharsNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITENETUNSIGNEDINT8BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}
	else
	{
		if (ulCharsNumber > 255)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STRINGCOUNTEDPROPERTY_CHARSNUMBERWRONG);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		if (FileIO:: writeNetUnsignedInt8Bit (iFileDescriptor,
			ulCharsNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITENETUNSIGNEDINT8BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}

	if (_ulBytesNumber > 0)
	{
		if (FileIO:: writeChars (iFileDescriptor, _pValues [0],
			_ulBytesNumber, &llBytesWritten) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITECHARS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error MP4StringCountedProperty:: getSize (unsigned long *pulSize)

{

	unsigned long						ulCharsNumber;


	*pulSize					= 0;

	if (_bIsImplicitProperty)
		;
	else
	{
		ulCharsNumber				= _ulBytesNumber / (_bUnicode ? 2 : 1);

		if (_bExpandedCount)
		{
			while (ulCharsNumber >= 0xFF)
			{
				// FileIO:: writeNetUnsignedInt8Bit 0xFF
				(*pulSize)					+= 1;

				ulCharsNumber				-= 0xFF;
			}

			// FileIO:: writeNetUnsignedInt8Bit ulCharsNumber
			(*pulSize)				+= 1;
		}
		else
		{
			// FileIO:: writeNetUnsignedInt8Bit ulCharsNumber
			(*pulSize)				+= 1;
		}

		(*pulSize)				+= _lInstancesNumber * _ulBytesNumber;
	}


	return errNoError;
}


Error MP4StringCountedProperty:: getType (char *pType)

{

	strcpy (pType, "StringCounted");


	return errNoError;
}


Error MP4StringCountedProperty:: appendDump (Buffer_p pbBuffer)

{

	long									lIntegerIndex;


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
		if (pbBuffer -> append (_pValues [lIntegerIndex]) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		break;

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


Error MP4StringCountedProperty:: printOnStdOutput (void)

{

	long									lIntegerIndex;


	std:: cout << _pName << " = ";

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		std:: cout << _pValues [lIntegerIndex];

		break;

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

