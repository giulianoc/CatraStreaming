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

#include "MP4Float32BitsProperty.h"
#include "FileIO.h"
#include <stdio.h>
#ifdef WIN32
	// #include <winsock2.h>
#else
	#include <unistd.h>
	#include <netinet/in.h>
#endif


MP4Float32BitsProperty:: MP4Float32BitsProperty (void): MP4Property ()

{

	_pfValues			= (float *) NULL;
}


MP4Float32BitsProperty:: ~MP4Float32BitsProperty (void)

{

	if (_pfValues != (float *) NULL)
	{
		delete [] _pfValues;

		_pfValues			= (float *) NULL;
	}
}


Error MP4Float32BitsProperty:: init (const char *pName,
	long lInstancesNumber, Boolean_p pbIsImplicitProperty,
	float *pfValues, Tracer_p ptTracer)

{

	long					lIntegerIndex;


	if (_pfValues != (float *) NULL)
	{
		delete [] _pfValues;

		_pfValues			= (float *) NULL;
	}

	if (MP4Property:: init (pName, lInstancesNumber, pbIsImplicitProperty,
		ptTracer) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INIT_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if (pfValues == (float *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	if ((_pfValues = new float [_lInstancesNumber]) == (float *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		_pfValues [lIntegerIndex]		= pfValues [lIntegerIndex];
	}


	return errNoError;
}


Error MP4Float32BitsProperty:: init (const char *pName,
	long lInstancesNumber, FileReader_p pfFile, Tracer_p ptTracer)

{

	long						lIntegerIndex;
	Boolean_t					bIsImplicitProperty;


	if (_pfValues != (float *) NULL)
	{
		delete [] _pfValues;

		_pfValues			= (float *) NULL;
	}

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

	if ((_pfValues = new float [_lInstancesNumber]) == (float *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		if (pfFile -> readNetFloat32Bit (&(_pfValues [lIntegerIndex])) !=
			errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_READNETFLOAT32BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			delete [] (_pfValues);
			_pfValues					= (float *) NULL;

			return err;
		}
	}


	return errNoError;
}


Error MP4Float32BitsProperty:: init (const char *pName,
	long lInstancesNumber, const unsigned char *pucBuffer, Tracer_p ptTracer)

{

	long						lIntegerIndex;
	Boolean_t					bIsImplicitProperty;
	unsigned long				ulIntegerPart;
	unsigned long				ulFloatPart;


	if (_pfValues != (float *) NULL)
	{
		delete [] _pfValues;

		_pfValues			= (float *) NULL;
	}

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

	if ((_pfValues = new float [_lInstancesNumber]) == (float *) NULL)
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
		ulIntegerPart	=
			(pucBuffer [lIntegerIndex * 4] << 8) |
			(pucBuffer [lIntegerIndex * 4 + 1]);

		ulFloatPart	=
			(pucBuffer [lIntegerIndex * 4 + 2] << 8) |
			(pucBuffer [lIntegerIndex * 4 + 3]);
		*/
		ulIntegerPart	= ntohs (
			*((unsigned short *) (pucBuffer + (lIntegerIndex * 4))));

		ulFloatPart		= ntohs (
			*((unsigned short *) (pucBuffer + (lIntegerIndex * 4 + 2))));

		_pfValues [lIntegerIndex]				=
			(ulIntegerPart + (((float) ulFloatPart) / 0x10000));
	}


	return errNoError;
}


Error MP4Float32BitsProperty:: getValue (float *pfValue, long lIndex)

{

	if (pfValue == (float *) NULL ||
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

	*pfValue					= _pfValues [lIndex];


	return errNoError;
}


Error MP4Float32BitsProperty:: setValue (float fValue, long lIndex,
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
		float						*pfLocalValues;
		long						lIntegerIndex;
		long						lIncrement;


		if ((pfLocalValues = new float [_lInstancesNumber + 1]) ==
			(float *) NULL)
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
			pfLocalValues [lIntegerIndex]				=
				_pfValues [lIntegerIndex];
		}

		pfLocalValues [lIntegerIndex]				= fValue;

		_lInstancesNumber							+= 1;

		delete [] _pfValues;

		_pfValues									= pfLocalValues;

		lIncrement									= 4;

		*pulSizeChangedInBytes						= lIncrement;
	}
	else
		_pfValues [lIndex]				= fValue;


	return errNoError;
}


Error MP4Float32BitsProperty:: incrementValue (long lIndex, float fIncrement)

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

	_pfValues [lIndex]					+= fIncrement;


	return errNoError;
}



Error MP4Float32BitsProperty:: write (int iFileDescriptor)

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
		if (FileIO:: writeNetFloat32Bit (iFileDescriptor,
			_pfValues [lInstanceIndex]) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITENETFLOAT32BIT_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error MP4Float32BitsProperty:: getSize (unsigned long *pulSize)

{

	if (_bIsImplicitProperty)
		*pulSize				= 0;
	else
		*pulSize				= _lInstancesNumber * 4;


	return errNoError;
}


Error MP4Float32BitsProperty:: getType (char *pType)

{

	strcpy (pType, "Float32Bits");


	return errNoError;
}


Error MP4Float32BitsProperty:: appendDump (Buffer_p pbBuffer)

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
		if (pbBuffer -> append (_pfValues [lIntegerIndex]) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}

		sprintf (pHexadecimal, "%A", _pfValues [lIntegerIndex]);

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


Error MP4Float32BitsProperty:: printOnStdOutput (void)

{

	long									lIntegerIndex;
	char									pHexadecimal [MP4_MAXLONGLENGTH];


	std:: cout << _pName << " = ";

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		std:: cout << _pfValues [lIntegerIndex];
		sprintf (pHexadecimal, "%A", _pfValues [lIntegerIndex]);
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

