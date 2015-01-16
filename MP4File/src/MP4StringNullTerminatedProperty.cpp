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

#include "MP4StringNullTerminatedProperty.h"
#include "FileIO.h"
#include <stdio.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif


MP4StringNullTerminatedProperty:: MP4StringNullTerminatedProperty (void):
	MP4Property ()

{

	_pValues			= (char **) NULL;

}


MP4StringNullTerminatedProperty:: ~MP4StringNullTerminatedProperty (void)

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


Error MP4StringNullTerminatedProperty:: init (const char *pName,
	Boolean_p pbIsImplicitProperty, unsigned long *pulBytesNumber,
	char *pValue, Tracer_p ptTracer)

{

	long					lIntegerIndex;


	if (MP4Property:: init (pName, 1, pbIsImplicitProperty, ptTracer) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (pValue == (char *) NULL)
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

    // In this case include '\0'. I found some files where,
	// when we have a string at the end of the file, it doesn't contain
	// the '\0', this means that in this case _ulBytesNumber doesn't include
	// the '\0'.
	_ulBytesNumber					= strlen (pValue) + 1;

	*pulBytesNumber					= _ulBytesNumber;

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
		if ((_pValues [lIntegerIndex] = new char [_ulBytesNumber]) ==
			(char *) NULL)
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
		strcpy ((_pValues [lIntegerIndex]), pValue);
	}


	return errNoError;
}


Error MP4StringNullTerminatedProperty:: init (const char *pName,
	unsigned long *pulBytesNumber, FileReader_p pfFile, Tracer_p ptTracer)

{

	Buffer_t				bString;
	char					pStringChar [1];
	Boolean_t				bIsImplicitProperty;
	Error_t					errReadChars;
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

	if (MP4Property:: init (pName, 1, &bIsImplicitProperty, ptTracer) !=
		errNoError)
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

	if (bString. init () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_INIT_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (*pulBytesNumber = 0; ; (*pulBytesNumber)++)
	{
		if ((errReadChars = pfFile -> readChars (pStringChar, 1,
			true, &ullCharsRead)) != errNoError)
		{
			if ((long) errReadChars == TOOLS_ACTIVATION_WRONG)
				break;
			else
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_FILEREADER_READCHARS_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (*pStringChar == '\0')
			break;
		else
		{
			if (bString. append ((const char *) pStringChar, 1) !=
				errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (bString. finish () != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_FINISH_FAILED);
					_ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	// this error means end of file. In this case we didn't find the '\0'
	// at the end of the string and we don't need to increment pulBytesNumber
	if ((long) errReadChars != TOOLS_ACTIVATION_WRONG)
		(*pulBytesNumber)++;

	// Be carefull that in the case
	// of '(long) errReadChars != TOOLS_ACTIVATION_WRONG', _ulBytesNumber
	// could not include the '\0'
	_ulBytesNumber						= *pulBytesNumber;

	if ((_pValues = new char * [_lInstancesNumber]) == (char **) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// we allocate '_ulBytesNumber + 1' bytes because _ulBytesNumber could not
	// include the '\0' in case
	// of '(long) errReadChars != TOOLS_ACTIVATION_WRONG'. Also in this case
	// (_ulBytesNumber doesn't include '\0') _pValues [0] will include the '\0'
	// at the end.
	if ((_pValues [0] = new char [_ulBytesNumber + 1]) == (char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] (_pValues);
		_pValues									= (char **) NULL;

		if (bString. finish () != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_FINISH_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	strcpy (_pValues [0], (const char *) bString);

	if (bString. finish () != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_FINISH_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete [] (_pValues);
		_pValues								= (char **) NULL;
	}


	return errNoError;
}


Error MP4StringNullTerminatedProperty:: getValue (Buffer_p pbValue, long lIndex)

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


Error MP4StringNullTerminatedProperty:: setValue (Buffer_p pbValue, long lIndex,
	long *plSizeChangedInBytes)

{

	long				lIncrement;


	if (pbValue == (Buffer_p) NULL ||
		lIndex < 0)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*plSizeChangedInBytes			= 0;

	// lIndex will be 0
	if (lIndex >= _lInstancesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	delete [] _pValues [lIndex];
	_pValues [lIndex]					= (char *) NULL;

	// +1 is for '\0'
	lIncrement					= (((unsigned long) (*pbValue)) + 1) - _ulBytesNumber;

	// include '\0'
	_ulBytesNumber				= ((unsigned long) (*pbValue)) + 1;

	if ((_pValues [lIndex] = new char [_ulBytesNumber]) == (char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	strcpy (_pValues [lIndex], (const char *) *pbValue);

	*plSizeChangedInBytes			= lIncrement;


	return errNoError;
}


Error MP4StringNullTerminatedProperty:: write (int iFileDescriptor)

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

	// _lInstancesNumber should be 1
	for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
		lInstanceIndex++)
	{
		if (FileIO:: writeChars (iFileDescriptor,
			_pValues [lInstanceIndex], _ulBytesNumber,
			&llBytesWritten) != errNoError)
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


Error MP4StringNullTerminatedProperty:: getSize (unsigned long *pulSize)

{

	if (_bIsImplicitProperty)
		*pulSize				= 0;
	else
		*pulSize				= _lInstancesNumber * _ulBytesNumber;


	return errNoError;
}


Error MP4StringNullTerminatedProperty:: getType (char *pType)

{

	strcpy (pType, "StringNullTerminated");


	return errNoError;
}


Error MP4StringNullTerminatedProperty:: appendDump (Buffer_p pbBuffer)

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


Error MP4StringNullTerminatedProperty:: printOnStdOutput (void)

{

	long									lIntegerIndex;


	std:: cout << _pName << " = ";

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		std:: cout << _pValues [lIntegerIndex];

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

