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

#include "MP4StringFixedProperty.h"
#include "FileIO.h"
#include <stdio.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif


MP4StringFixedProperty:: MP4StringFixedProperty (void): MP4Property ()

{

	_pValues			= (char **) NULL;

}


MP4StringFixedProperty:: ~MP4StringFixedProperty (void)

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


Error MP4StringFixedProperty:: init (const char *pName, long lInstancesNumber,
	Boolean_p pbIsImplicitProperty, unsigned long ulBytesNumber,
	char **pValues, Tracer_p ptTracer)

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

	if (pValues == (char **) NULL)
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

    // exclude '\0'
	_ulBytesNumber					= ulBytesNumber;

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
				(_pValues) [lIntegerIndex]				= (char *) NULL;
			}
			delete [] (_pValues);
			_pValues							= (char **) NULL;

			return err;
		}
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		strncpy ((_pValues [lIntegerIndex]), pValues [lIntegerIndex],
			ulBytesNumber);
		_pValues [lIntegerIndex][ulBytesNumber]				= '\0';
	}


	return errNoError;
}


Error MP4StringFixedProperty:: init (const char *pName, long lInstancesNumber,
	unsigned long ulBytesNumber, FileReader_p pfFile, Tracer_p ptTracer)

{

	long						lIntegerIndex;
	Boolean_t					bIsImplicitProperty;
	#ifdef WIN32
		__int64					ullCharsRead;
	#else
		unsigned long long		ullCharsRead;
	#endif


	bIsImplicitProperty						= false;

	if (ulBytesNumber < 0 ||
		pfFile == (FileReader_p) NULL)
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

	_ulBytesNumber				= ulBytesNumber;

	if ((_pValues = new char * [_lInstancesNumber]) == (char **) NULL)
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
				(_pValues) [lIntegerIndex]			= (char *) NULL;
			}
			delete [] (_pValues);
			_pValues								= (char **) NULL;

			return err;
		}
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber;
		lIntegerIndex++)
	{
		if (pfFile -> readChars (_pValues [lIntegerIndex],
			_ulBytesNumber, true, &ullCharsRead) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_READCHARS_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber;
				lIntegerIndex++)
			{
				delete [] ((_pValues) [lIntegerIndex]);
				(_pValues) [lIntegerIndex]			= (char *) NULL;
			}
			delete [] (_pValues);
			_pValues									= (char **) NULL;

			return err;
		}

		_pValues [lIntegerIndex][_ulBytesNumber]				= '\0';
	}


	return errNoError;
}


Error MP4StringFixedProperty:: getValue (char *pValue,
	unsigned long ulBufferLength, long lIndex)

{

	if (pValue == (char *) NULL ||
		_ulBytesNumber > ulBufferLength - 1 ||
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

	memcpy (pValue, _pValues [lIndex], _ulBytesNumber);
	pValue [_ulBytesNumber]					= '\0';


	return errNoError;
}


Error MP4StringFixedProperty:: getValue (Buffer_p pbValue, long lIndex)

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


Error MP4StringFixedProperty:: getValue (unsigned char *pucValue, long lIndex)

{

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

	memcpy (pucValue, _pValues [lIndex], _ulBytesNumber);


	return errNoError;
}


Error MP4StringFixedProperty:: setValue (Buffer_p pbValue, long lIndex,
	unsigned long *pulSizeChangedInBytes)

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

	*pulSizeChangedInBytes			= 0;

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
		// see the other setValue in the same case
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NOTIMPLEMENTEDYET);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (strlen ((const char *) *pbValue) != _ulBytesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	strncpy (_pValues [lIndex], (const char *) *pbValue, _ulBytesNumber);
	_pValues [lIndex][_ulBytesNumber]				= '\0';


	return errNoError;
}


Error MP4StringFixedProperty:: write (int iFileDescriptor)

{

	long						lInstanceIndex;
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

	for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
		lInstanceIndex++)
	{
		if (FileIO:: writeChars (iFileDescriptor, _pValues [lInstanceIndex],
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


Error MP4StringFixedProperty:: getSize (unsigned long *pulSize)

{

	if (_bIsImplicitProperty)
		*pulSize				= 0;
	else
		*pulSize				= _lInstancesNumber * _ulBytesNumber;


	return errNoError;
}


Error MP4StringFixedProperty:: getType (char *pType)

{

	strcpy (pType, "StringFixed");


	return errNoError;
}


Error MP4StringFixedProperty:: appendDump (Buffer_p pbBuffer)

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

		// break;

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


Error MP4StringFixedProperty:: printOnStdOutput (void)

{

	long									lIntegerIndex;


	std:: cout << _pName << " = ";

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		std:: cout << _pValues [lIntegerIndex];

		// break;

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

