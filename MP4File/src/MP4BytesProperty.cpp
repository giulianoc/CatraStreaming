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

#include "MP4BytesProperty.h"
#include "FileIO.h"
#include <stdio.h>
#ifdef WIN32
#else
	#include <unistd.h>
#endif


MP4BytesProperty:: MP4BytesProperty (void): MP4Property ()

{

	_pucValues			= (unsigned char **) NULL;

}


MP4BytesProperty:: ~MP4BytesProperty (void)

{

	if (_pucValues != (unsigned char **) NULL)
	{
		long			lInstanceIndex;

		for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
			lInstanceIndex++)
			delete [] (_pucValues [lInstanceIndex]);

		if (_pucValues != (unsigned char **) NULL)
			delete [] _pucValues;

		_pucValues			= (unsigned char **) NULL;
	}

}


#ifdef WIN32
	Error MP4BytesProperty:: init (const char *pName, long lInstancesNumber,
		Boolean_p pbIsImplicitProperty, __int64 ullBytesNumber,
		unsigned char **pucValues, Tracer_p ptTracer)
#else
	Error MP4BytesProperty:: init (const char *pName, long lInstancesNumber,
		Boolean_p pbIsImplicitProperty, unsigned long long ullBytesNumber,
		unsigned char **pucValues, Tracer_p ptTracer)
#endif

{

	long					lIntegerIndex;


	if (_pucValues != (unsigned char **) NULL)
	{
		long			lInstanceIndex;

		for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
			lInstanceIndex++)
			delete [] (_pucValues [lInstanceIndex]);

		if (_pucValues != (unsigned char **) NULL)
			delete [] _pucValues;

		_pucValues			= (unsigned char **) NULL;
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

	if (pucValues == (unsigned char **) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

    // exclude '\0'
	_ullBytesNumber					= ullBytesNumber;

	if ((_pucValues = new unsigned char * [_lInstancesNumber]) ==
		(unsigned char **) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		if ((_pucValues [lIntegerIndex] =
			new unsigned char [(unsigned int) _ullBytesNumber]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			while (--lIntegerIndex >= 0)
			{
				delete [] ((_pucValues) [lIntegerIndex]);
				(_pucValues) [lIntegerIndex]		= (unsigned char *) NULL;
			}
			delete [] (_pucValues);
			_pucValues							= (unsigned char **) NULL;

			return err;
		}
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		memcpy (_pucValues [lIntegerIndex], pucValues [lIntegerIndex],
			(size_t) ullBytesNumber);
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4BytesProperty:: init (const char *pName, long lInstancesNumber,
		__int64 ullBytesNumber, FileReader_p pfFile, Tracer_p ptTracer)
#else
	Error MP4BytesProperty:: init (const char *pName, long lInstancesNumber,
		unsigned long long ullBytesNumber, FileReader_p pfFile, Tracer_p ptTracer)
#endif

{

	long						lIntegerIndex;
	Boolean_t					bIsImplicitProperty;
	#ifdef WIN32
		__int64					ullCharsRead;
	#else
		unsigned long long		ullCharsRead;
	#endif


	if (_pucValues != (unsigned char **) NULL)
	{
		long			lInstanceIndex;

		for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
			lInstanceIndex++)
			delete [] (_pucValues [lInstanceIndex]);

		if (_pucValues != (unsigned char **) NULL)
			delete [] _pucValues;

		_pucValues			= (unsigned char **) NULL;
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

	_ullBytesNumber				= ullBytesNumber;

	if ((_pucValues = new unsigned char * [_lInstancesNumber]) ==
		(unsigned char **) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		if ((_pucValues [lIntegerIndex] =
			new unsigned char [(unsigned int) _ullBytesNumber]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			while (--lIntegerIndex >= 0)
			{
				delete [] ((_pucValues) [lIntegerIndex]);
				(_pucValues) [lIntegerIndex]		= (unsigned char *) NULL;
			}
			delete [] (_pucValues);
			_pucValues								= (unsigned char **) NULL;

			return err;
		}
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber;
		lIntegerIndex++)
	{
		if (pfFile -> readBytes (_pucValues [lIntegerIndex],
			_ullBytesNumber, true, &ullCharsRead) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEREADER_READBYTES_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber;
				lIntegerIndex++)
			{
				delete [] ((_pucValues) [lIntegerIndex]);
				(_pucValues) [lIntegerIndex]		= (unsigned char *) NULL;
			}
			delete [] (_pucValues);
			_pucValues								= (unsigned char **) NULL;

			return err;
		}
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4BytesProperty:: init (const char *pName, long lInstancesNumber,
		__int64 ullBytesNumber, const unsigned char *pucBuffer,
		Tracer_p ptTracer)
#else
	Error MP4BytesProperty:: init (const char *pName, long lInstancesNumber,
		unsigned long long ullBytesNumber, const unsigned char *pucBuffer,
		Tracer_p ptTracer)
#endif

{

	long						lIntegerIndex;
	Boolean_t					bIsImplicitProperty;


	if (_pucValues != (unsigned char **) NULL)
	{
		long			lInstanceIndex;

		for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
			lInstanceIndex++)
			delete [] (_pucValues [lInstanceIndex]);

		if (_pucValues != (unsigned char **) NULL)
			delete [] _pucValues;

		_pucValues			= (unsigned char **) NULL;
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

	_ullBytesNumber				= ullBytesNumber;

	if ((_pucValues = new unsigned char * [_lInstancesNumber]) ==
		(unsigned char **) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber; lIntegerIndex++)
	{
		if ((_pucValues [lIntegerIndex] =
			new unsigned char [(unsigned int) _ullBytesNumber]) ==
			(unsigned char *) NULL)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_NEW_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			while (--lIntegerIndex >= 0)
			{
				delete [] ((_pucValues) [lIntegerIndex]);
				(_pucValues) [lIntegerIndex]		= (unsigned char *) NULL;
			}
			delete [] (_pucValues);
			_pucValues								= (unsigned char **) NULL;

			return err;
		}
	}

	for (lIntegerIndex = 0; lIntegerIndex < _lInstancesNumber;
		lIntegerIndex++)
	{
		memcpy (_pucValues [lIntegerIndex],
			pucBuffer + (lIntegerIndex * _ullBytesNumber),
			(size_t) _ullBytesNumber);
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4BytesProperty:: getData (
		__int64 ullOffset, __int64 llBytesToRead,
		long lIndex, unsigned char *pucBuffer)
#else
	Error MP4BytesProperty:: getData (
		unsigned long long ullOffset, long long llBytesToRead,
		long lIndex, unsigned char *pucBuffer)
#endif

{

	if (pucBuffer == (unsigned char *) NULL ||
		ullOffset < 0 ||
		(llBytesToRead < 0 && llBytesToRead != -1) ||
		llBytesToRead > _ullBytesNumber ||
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

	if (llBytesToRead == -1)
		llBytesToRead				= _ullBytesNumber;

	memcpy (pucBuffer, (_pucValues [lIndex]) + ullOffset,
		(size_t) llBytesToRead);


	return errNoError;
}


/*
#ifdef WIN32
	Error MP4BytesProperty:: appendDataToRTPPacket (
		__int64 ullOffset, __int64 llBytesToRead,
		long lIndex, RTPPacket_p prpRTPPacket) const
#else
	Error MP4BytesProperty:: appendDataToRTPPacket (
		unsigned long long ullOffset, long long llBytesToRead,
		long lIndex, RTPPacket_p prpRTPPacket) const
#endif

{

	if (prpRTPPacket == (RTPPacket_p) NULL ||
		(llBytesToRead < 0 && llBytesToRead != -1) ||
		llBytesToRead > _ullBytesNumber ||
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

	if (llBytesToRead == -1)
		llBytesToRead				= _ullBytesNumber;

	if (prpRTPPacket -> appendData (
		(_pucValues [lIndex]) + ullOffset, llBytesToRead) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_RTPPACKET_APPENDDATA_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/


#ifdef WIN32
	Error MP4BytesProperty:: getPointerToData (
		unsigned long ulOffset, long lIndex,
		unsigned char **pucBuffer, __int64 *pullBytesNumber)
#else
	Error MP4BytesProperty:: getPointerToData (
		unsigned long ulOffset, long lIndex,
		unsigned char **pucBuffer, unsigned long long *pullBytesNumber)
#endif

{

	if (pucBuffer == (unsigned char **) NULL ||
		#ifdef WIN32
			pullBytesNumber == (__int64 *) NULL ||
		#else
			pullBytesNumber == (unsigned long long *) NULL ||
		#endif
		ulOffset < 0 ||
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

	*pullBytesNumber			= _ullBytesNumber - ulOffset;

	*pucBuffer					= (_pucValues [lIndex]) + ulOffset;


	return errNoError;
}


#ifdef WIN32
	Error MP4BytesProperty:: setData (unsigned char *pucValue,
		__int64 ullBytesNumber, long lIndex,
		long *plSizeChangedInBytes)
#else
	Error MP4BytesProperty:: setData (unsigned char *pucValue,
		unsigned long long ullBytesNumber, long lIndex,
		long *plSizeChangedInBytes)
#endif

{

	#ifdef WIN32
		__int64					llIncrement;
	#else
		long long				llIncrement;
	#endif

	if (pucValue == (unsigned char *) NULL ||
		lIndex < 0 ||
		plSizeChangedInBytes == (long *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	*plSizeChangedInBytes			= 0;

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

	if (_lInstancesNumber > 1 && ullBytesNumber != _ullBytesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	delete [] _pucValues [lIndex];
	_pucValues [lIndex]					= (unsigned char *) NULL;

	llIncrement			= ullBytesNumber - _ullBytesNumber;

	_ullBytesNumber		= ullBytesNumber;

	if ((_pucValues [lIndex] = new unsigned char [(unsigned int) _ullBytesNumber]) ==
		(unsigned char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	memcpy (_pucValues [lIndex], pucValue, (size_t) _ullBytesNumber);

	*plSizeChangedInBytes			= llIncrement;


	return errNoError;
}


#ifdef WIN32
	Error MP4BytesProperty:: appendData (unsigned char *pucValue,
		__int64 ullBytesNumber, long lIndex,
		unsigned long *pulSizeChangedInBytes)
#else
	Error MP4BytesProperty:: appendData (unsigned char *pucValue,
		unsigned long long ullBytesNumber, long lIndex,
		unsigned long *pulSizeChangedInBytes)
#endif

{

	unsigned char			*pucLocalValue;


	if (pucValue == (unsigned char *) NULL ||
		lIndex < 0 ||
		pulSizeChangedInBytes == (unsigned long *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

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

	if (_lInstancesNumber > 1 && ullBytesNumber != _ullBytesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	pucLocalValue				= _pucValues [lIndex];

	if ((_pucValues [lIndex] = new unsigned char [
		(unsigned int) (_ullBytesNumber + ullBytesNumber)]) ==
			(unsigned char *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NEW_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	memcpy (_pucValues [lIndex], pucLocalValue, (size_t) _ullBytesNumber);
	delete [] pucLocalValue;

	memcpy ((_pucValues [lIndex]) + _ullBytesNumber, pucValue,
		(size_t) ullBytesNumber);

	_ullBytesNumber					+= ullBytesNumber;

	*pulSizeChangedInBytes			= ullBytesNumber;


	return errNoError;
}


Error MP4BytesProperty:: write (int iFileDescriptor)

{

	long					lInstanceIndex;
	#ifdef WIN32
		__int64				llBytesWritten;
	#else
		long long			llBytesWritten;
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
		if (FileIO:: writeBytes (iFileDescriptor, (
			_pucValues [lInstanceIndex]), _ullBytesNumber,
			&llBytesWritten) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_FILEIO_WRITEBYTES_FAILED);
			_ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			return err;
		}
	}


	return errNoError;
}


Error MP4BytesProperty:: getSize (unsigned long *pulSize)

{

	if (_bIsImplicitProperty)
		*pulSize				= 0;
	else
		*pulSize				= _lInstancesNumber * _ullBytesNumber;


	return errNoError;
}


Error MP4BytesProperty:: getType (char *pType)

{

	strcpy (pType, "Bytes");


	return errNoError;
}


Error MP4BytesProperty:: appendDump (Buffer_p pbBuffer)

{

	unsigned long				ulSize;
	char						pHexadecimal [MP4_MAXLONGLENGTH];
	unsigned long				ulVisiblesBytes;
	long						lInstanceIndex;


	if (getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

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

	for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
		lInstanceIndex++)
	{
		for (ulVisiblesBytes = 0; ulVisiblesBytes <
			(_ullBytesNumber < 20 ? _ullBytesNumber : 20); ulVisiblesBytes++)
		{
			sprintf (pHexadecimal, "%02lX",
				(long) (_pucValues [lInstanceIndex][ulVisiblesBytes]));

			if (pbBuffer -> append (pHexadecimal) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (_ullBytesNumber > 20)
		{
			if (pbBuffer -> append ("...") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				_ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				return err;
			}
		}

		if (lInstanceIndex != _lInstancesNumber - 1)
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

	if (pbBuffer -> append (" <binary data: ") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbBuffer -> append (ulSize) != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pbBuffer -> append (" byte(s)>") != errNoError)
	{
		Error err = ToolsErrors (__FILE__, __LINE__,
			TOOLS_BUFFER_APPEND_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
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


Error MP4BytesProperty:: printOnStdOutput (void)

{

	unsigned long				ulSize;
	char						pHexadecimal [MP4_MAXLONGLENGTH];
	unsigned long				ulVisiblesBytes;
	long						lInstanceIndex;


	if (getSize (&ulSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4BYTESPROPERTY_GETSIZE_FAILED);
		_ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		return err;
	}

	std:: cout << _pName << " = ";

	for (lInstanceIndex = 0; lInstanceIndex < _lInstancesNumber;
		lInstanceIndex++)
	{
		for (ulVisiblesBytes = 0; ulVisiblesBytes <
			(_ullBytesNumber < 20 ? _ullBytesNumber : 20); ulVisiblesBytes++)
		{
			sprintf (pHexadecimal, "%02lX",
				(long) (_pucValues [lInstanceIndex][ulVisiblesBytes]));
			std:: cout << pHexadecimal;
		}

		if (_ullBytesNumber > 20)
			std:: cout << "...";

		if (lInstanceIndex != _lInstancesNumber - 1)
			std:: cout << ", ";
	}

	std:: cout << " <binary data: " << ulSize << " byte(s)>";

	if (_bIsImplicitProperty)
		std:: cout << " (implicit)";
	std:: cout << std:: endl;
	std:: cout. flush ();


	return errNoError;
}

