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

#include "MP4StringLastProperty.h"
#include "FileIO.h"
#include <stdio.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif


MP4StringLastProperty:: MP4StringLastProperty (void): MP4Property ()

{

	_pValues			= (char **) NULL;

}


MP4StringLastProperty:: ~MP4StringLastProperty (void)

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


Error MP4StringLastProperty:: init (const char *pName,
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

	_ulBytesNumber					= strlen (pValue);

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
		memcpy ((_pValues [lIntegerIndex]), pValue, _ulBytesNumber);
	}


	return errNoError;
}


Error MP4StringLastProperty:: init (const char *pName,
	unsigned long ulBytesNumber, FileReader_p pfFile,
	Tracer_p ptTracer)

{

	Boolean_t				bIsImplicitProperty;
	Error_t					errReadChars;
	#ifdef WIN32
		__int64					ullCharsRead;
	#else
		unsigned long long		ullCharsRead;
	#endif


	bIsImplicitProperty						= false;

	if (pfFile == (FileReader_p) NULL)
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

	_ulBytesNumber						= ulBytesNumber;

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

	if ((_pValues = new char * [_lInstancesNumber]) == (char **) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((_pValues [0] = new char [_ulBytesNumber]) == (char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		delete [] (_pValues);
		_pValues									= (char **) NULL;

		return err;
	}

	if ((errReadChars = pfFile -> readChars (_pValues [0], _ulBytesNumber,
		true, &ullCharsRead)) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_FILEREADER_READCHARS_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		delete [] (_pValues [0]);

		delete [] (_pValues);
		_pValues									= (char **) NULL;

		return err;
	}


	return errNoError;
}


Error MP4StringLastProperty:: getValue (Buffer_p pbValue, long lIndex)

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

	if (pbValue -> setBuffer (_pValues [lIndex], _ulBytesNumber) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_SETBUFFER_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4StringLastProperty:: setValue (Buffer_p pbValue, long lIndex,
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

	lIncrement					= ((unsigned long) (*pbValue)) -
		_ulBytesNumber;

	_ulBytesNumber				= ((unsigned long) (*pbValue));

	if ((_pValues [lIndex] = new char [_ulBytesNumber]) == (char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	memcpy (_pValues [lIndex], (const char *) *pbValue, _ulBytesNumber);

	*plSizeChangedInBytes			= lIncrement;


	return errNoError;
}


Error MP4StringLastProperty:: write (int iFileDescriptor)

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


Error MP4StringLastProperty:: getSize (unsigned long *pulSize)

{

	if (_bIsImplicitProperty)
		*pulSize				= 0;
	else
		*pulSize				= _lInstancesNumber * _ulBytesNumber;


	return errNoError;
}


Error MP4StringLastProperty:: getType (char *pType)

{

	strcpy (pType, "StringLast");


	return errNoError;
}


Error MP4StringLastProperty:: appendDump (Buffer_p pbBuffer)

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
		if (pbBuffer -> append (_pValues [lIntegerIndex],
			_ulBytesNumber) != errNoError)
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


Error MP4StringLastProperty:: printOnStdOutput (void)

{

	long					lIntegerIndex;
	long					lCharIndex;


	std:: cout << _pName << " = ";

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		for (lCharIndex = 0; lCharIndex < _ulBytesNumber; lCharIndex++)
			std:: cout << _pValues [lIntegerIndex][lCharIndex];

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

