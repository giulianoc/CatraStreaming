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

#include "MP4UInteger64BitsProperty.h"
#include "FileIO.h"
#include <stdio.h>
#include <iostream>
#ifdef WIN32
#else
	#include <unistd.h>
#endif


MP4UInteger64BitsProperty:: MP4UInteger64BitsProperty (void): MP4Property ()

{

	#ifdef WIN32
		_pullValues			= (__int64 *) NULL;
	#else
		_pullValues			= (unsigned long long *) NULL;
	#endif

}


MP4UInteger64BitsProperty:: ~MP4UInteger64BitsProperty (void)

{

	#ifdef WIN32
		if (_pullValues != (__int64 *) NULL)
	#else
		if (_pullValues != (unsigned long long *) NULL)
	#endif
	{
		delete [] _pullValues;

		#ifdef WIN32
			_pullValues			= (__int64 *) NULL;
		#else
			_pullValues			= (unsigned long long *) NULL;
		#endif
	}
}


#ifdef WIN32
	Error MP4UInteger64BitsProperty:: init (const char *pName,
		long lInstancesNumber, Boolean_p pbIsImplicitProperty,
		__int64 *pullValues, Tracer_p ptTracer)
#else
	Error MP4UInteger64BitsProperty:: init (const char *pName,
		long lInstancesNumber, Boolean_p pbIsImplicitProperty,
		unsigned long long *pullValues, Tracer_p ptTracer)
#endif

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

	#ifdef WIN32
		if (pullValues == (__int64 *) NULL)
	#else
		if (pullValues == (unsigned long long *) NULL)
	#endif
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	#ifdef WIN32
		if (_pullValues != (__int64 *) NULL)
	#else
		if (_pullValues != (unsigned long long *) NULL)
	#endif
	{
		delete [] _pullValues;

		#ifdef WIN32
			_pullValues			= (__int64 *) NULL;
		#else
			_pullValues			= (unsigned long long *) NULL;
		#endif
	}

	#ifdef WIN32
		if ((_pullValues = new __int64 [_lInstancesNumber]) ==
			(__int64 *) NULL)
	#else
		if ((_pullValues = new unsigned long long [_lInstancesNumber]) ==
			(unsigned long long *) NULL)
	#endif
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		_pullValues [lIntegerIndex]				= pullValues [lIntegerIndex];
	}


	return errNoError;
}


Error MP4UInteger64BitsProperty:: init (const char *pName,
	long lInstancesNumber, FileReader_p pfFile, Tracer_p ptTracer)

{

	long					lIntegerIndex;
	Boolean_t				bIsImplicitProperty;


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

	#ifdef WIN32
		if (_pullValues != (__int64 *) NULL)
	#else
		if (_pullValues != (unsigned long long *) NULL)
	#endif
	{
		delete [] _pullValues;

		#ifdef WIN32
			_pullValues			= (__int64 *) NULL;
		#else
			_pullValues			= (unsigned long long *) NULL;
		#endif
	}

	#ifdef WIN32
		if ((_pullValues = new __int64 [_lInstancesNumber]) ==
			(__int64 *) NULL)
	#else
		if ((_pullValues = new unsigned long long [_lInstancesNumber]) ==
			(unsigned long long *) NULL)
	#endif
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		if (pfFile -> readNetUnsignedInt64Bit (
			&(_pullValues [lIntegerIndex])) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_READNETUNSIGNEDINT64BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] (_pullValues);
			#ifdef WIN32
				_pullValues					= (__int64 *) NULL;
			#else
				_pullValues					= (unsigned long long *) NULL;
			#endif

			return err;
		}
	}


	return errNoError;
}


Error MP4UInteger64BitsProperty:: init (const char *pName,
	long lInstancesNumber, const unsigned char *pucBuffer, Tracer_p ptTracer)

{

	long					lIntegerIndex;
	Boolean_t				bIsImplicitProperty;


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

	#ifdef WIN32
		if (_pullValues != (__int64 *) NULL)
	#else
		if (_pullValues != (unsigned long long *) NULL)
	#endif
	{
		delete [] _pullValues;

		#ifdef WIN32
			_pullValues			= (__int64 *) NULL;
		#else
			_pullValues			= (unsigned long long *) NULL;
		#endif
	}

	#ifdef WIN32
		if ((_pullValues = new __int64 [_lInstancesNumber]) ==
			(__int64 *) NULL)
	#else
		if ((_pullValues = new unsigned long long [_lInstancesNumber]) ==
			(unsigned long long *) NULL)
	#endif
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		_pullValues [lIntegerIndex]			= (
			(pucBuffer [lIntegerIndex * 8] << 56) |
			(pucBuffer [lIntegerIndex * 8 + 1] << 48) |
			(pucBuffer [lIntegerIndex * 8 + 2] << 40) |
			(pucBuffer [lIntegerIndex * 8 + 3] << 32) |
			(pucBuffer [lIntegerIndex * 8 + 4] << 24) |
			(pucBuffer [lIntegerIndex * 8 + 5] << 16) |
			(pucBuffer [lIntegerIndex * 8 + 6] << 8) |
			pucBuffer [lIntegerIndex * 8 + 7]);
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4UInteger64BitsProperty:: getValue (__int64 *pullValue,
		long lIndex)
#else
	Error MP4UInteger64BitsProperty:: getValue (unsigned long long *pullValue,
		long lIndex)
#endif
{

	#ifdef WIN32
		if (pullValue == (__int64 *) NULL ||
			lIndex < 0)
	#else
		if (pullValue == (unsigned long long *) NULL ||
			lIndex < 0)
	#endif
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

	*pullValue					= _pullValues [lIndex];


	return errNoError;
}


Error MP4UInteger64BitsProperty:: getValue (unsigned char *pucValue,
	long lIndex)

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

	pucValue [0]			= (unsigned char) (((_pullValues [lIndex]) >> 56) & 0xFF);
	pucValue [1]			= (unsigned char) (((_pullValues [lIndex]) >> 48) & 0xFF);
	pucValue [2]			= (unsigned char) (((_pullValues [lIndex]) >> 40) & 0xFF);
	pucValue [3]			= (unsigned char) (((_pullValues [lIndex]) >> 32) & 0xFF);
	pucValue [4]			= (unsigned char) (((_pullValues [lIndex]) >> 24) & 0xFF);
	pucValue [5]			= (unsigned char) (((_pullValues [lIndex]) >> 16) & 0xFF);
	pucValue [6]			= (unsigned char) (((_pullValues [lIndex]) >> 8) & 0xFF);
	pucValue [7]			= (unsigned char) ((_pullValues [lIndex]) & 0xFF);


	return errNoError;
}


#ifdef WIN32
	Error MP4UInteger64BitsProperty:: setValue (__int64 ullValue,
		long lIndex, unsigned long *pulSizeChangedInBytes)
#else
	Error MP4UInteger64BitsProperty:: setValue (unsigned long long ullValue,
		long lIndex, unsigned long *pulSizeChangedInBytes)
#endif

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
		#ifdef WIN32
			__int64						*pullLocalValues;
		#else
			unsigned long long			*pullLocalValues;
		#endif
		long						lIntegerIndex;
		long						lIncrement;


		#ifdef WIN32
			if ((pullLocalValues =
				new __int64 [_lInstancesNumber + 1]) ==
				(__int64 *) NULL)
		#else
			if ((pullLocalValues =
				new unsigned long long [_lInstancesNumber + 1]) ==
				(unsigned long long *) NULL)
		#endif
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
			pullLocalValues [lIntegerIndex]				=
				_pullValues [lIntegerIndex];
		}

		pullLocalValues [lIntegerIndex]				= ullValue;

		_lInstancesNumber							+= 1;

		delete [] _pullValues;

		_pullValues									= pullLocalValues;

		lIncrement									= 8;

		*pulSizeChangedInBytes						= lIncrement;
	}
	else
		_pullValues [lIndex]				= ullValue;


	return errNoError;
}


#ifdef WIN32
	Error MP4UInteger64BitsProperty:: incrementValue (long lIndex,
		__int64 llIncrement)
#else
	Error MP4UInteger64BitsProperty:: incrementValue (long lIndex,
		long long llIncrement)
#endif

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

	_pullValues [lIndex]					+= llIncrement;


	return errNoError;
}



Error MP4UInteger64BitsProperty:: write (int iFileDescriptor)

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
		if (FileIO:: writeNetUnsignedInt64Bit (iFileDescriptor,
			_pullValues [lInstanceIndex]) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITENETUNSIGNEDINT64BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error MP4UInteger64BitsProperty:: getSize (unsigned long *pulSize)

{

	if (_bIsImplicitProperty)
		*pulSize				= 0;
	else
		*pulSize				= _lInstancesNumber * 8;


	return errNoError;
}


Error MP4UInteger64BitsProperty:: getType (char *pType)

{

	strcpy (pType, "UInteger64Bits");


	return errNoError;
}


Error MP4UInteger64BitsProperty:: appendDump (Buffer_p pbBuffer)

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
		if (pbBuffer -> append (_pullValues [lIntegerIndex]) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		sprintf (pHexadecimal, "0x%llX", _pullValues [lIntegerIndex]);

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


Error MP4UInteger64BitsProperty:: printOnStdOutput (void)

{

	long									lIntegerIndex;
	char									pHexadecimal [MP4_MAXLONGLENGTH];


	std:: cout << _pName << " = ";

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		#ifdef WIN32
			std:: cout << (unsigned long) (_pullValues [lIntegerIndex]);
		#else
			std:: cout << _pullValues [lIntegerIndex];
		#endif
		sprintf (pHexadecimal, "0x%llX", _pullValues [lIntegerIndex]);
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

