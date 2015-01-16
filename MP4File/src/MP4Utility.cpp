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


#include "MP4Utility.h"
#include "MP4DecoderConfigDescr.h"
#include "MP4DecoderSpecificDescr.h"
#include "MP4DrefAtom.h"
#include "MP4HintAtom.h"
#include "MP4InitialObjectDescr.h"
#include "MP4MdatAtom.h"
#include "MP4PaytAtom.h"
#include "MP4RtpAtom.h"
#include "MP4S263Atom.h"
#include "MP4SdpAtom.h"
#include "MP4SnroAtom.h"
#include "MP4StsdAtom.h"
#include "MP4TimsAtom.h"
#include "MP4TsroAtom.h"
#include "MP4UrlAtom.h"
#include "Convert.h"
#include <stdio.h>
#include <assert.h>
#ifdef WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
	#include <netinet/in.h>
#endif


MP4Utility:: MP4Utility (void)

{

}


MP4Utility:: ~MP4Utility (void)

{

}



MP4Utility:: MP4Utility (const MP4Utility &)

{

	assert (1==0);

	// to do

}


MP4Utility &MP4Utility:: operator = (const MP4Utility &)

{

	assert (1==0);

	// to do

	return *this;

}


#ifdef WIN32
	Error MP4Utility:: readFromMdatAtom (MP4RootAtom_p pmaRootAtom,
		__int64 ullFileOffset,
		unsigned char *pucBuffer, unsigned long ulBytesToRead,
		Tracer_p ptTracer)
#else
	Error MP4Utility:: readFromMdatAtom (MP4RootAtom_p pmaRootAtom,
		unsigned long long ullFileOffset,
		unsigned char *pucBuffer, unsigned long ulBytesToRead,
		Tracer_p ptTracer)
#endif

{

	char				pAtomPath [MP4F_MAXPATHNAMELENGTH];
	unsigned long		ulMdatIndex;
	MP4MdatAtom_p		pmaMdatAtom;
	Error_t				errGetData;
	MP4Atom_p			pmaAtom;


	if (pmaRootAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (ulMdatIndex = 0; ; ulMdatIndex++)
	{
		sprintf (pAtomPath, "mdat:%lu", ulMdatIndex);

		if (pmaRootAtom -> searchAtom (pAtomPath, false,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdat");
			// ptTracer -> trace (Tracer:: TRACER_LERRR,
			// 	(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaMdatAtom		= (MP4MdatAtom_p) pmaAtom;

		if ((errGetData = pmaMdatAtom -> getData (
			ullFileOffset, ulBytesToRead, pucBuffer)) != errNoError)
		{
			if ((long) errGetData != MP4F_ACTIVATION_WRONG)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4BYTESPROPERTY_GETDATA_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
			break;
	}

	if (pmaRootAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4Utility:: getPointerFromMdatAtom (MP4RootAtom_p pmaRootAtom,
		__int64 ullFileOffset,
		unsigned char **pucBuffer, __int64 *pullBytesToRead,
		Tracer_p ptTracer)
#else
	Error MP4Utility:: getPointerFromMdatAtom (MP4RootAtom_p pmaRootAtom,
		unsigned long long ullFileOffset,
		unsigned char **pucBuffer, unsigned long long *pullBytesToRead,
		Tracer_p ptTracer)
#endif

{

	char				pAtomPath [MP4F_MAXPATHNAMELENGTH];
	unsigned long		ulMdatIndex;
	MP4MdatAtom_p		pmaMdatAtom;
	Error_t				errGetData;
	MP4Atom_p			pmaAtom;


	if (pmaRootAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	for (ulMdatIndex = 0; ; ulMdatIndex++)
	{
		sprintf (pAtomPath, "mdat:%lu", ulMdatIndex);

		if (pmaRootAtom -> searchAtom (pAtomPath, false,
			&pmaAtom) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "mdat");
			// ptTracer -> trace (Tracer:: TRACER_LERRR,
			// 	(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaMdatAtom		= (MP4MdatAtom_p) pmaAtom;

		if ((errGetData = pmaMdatAtom -> getPointerToData (
			ullFileOffset, pucBuffer, pullBytesToRead)) != errNoError)
		{
			if ((long) errGetData != MP4F_ACTIVATION_WRONG)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4BYTESPROPERTY_GETPOINTERTODATA_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaRootAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
			break;
	}

	if (pmaRootAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: getMaxSampleSize (
	MP4StszAtom_p pmaStszAtom, unsigned long *pulMaxSampleSize,
	unsigned long *pulSamplesNumber, Tracer_p ptTracer)

{

	/*
	MP4StszAtom_p				pmsaStszAtom;
	MP4Atom_p					pmaAtom;
	*/
	unsigned long				ulSampleIndex;
	unsigned long				ulSampleSize;


	if (pulMaxSampleSize == (unsigned long *) NULL)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmaStszAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stsz:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:minf:0:stbl:0:stsz:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmsaStszAtom		= (MP4StszAtom_p) pmaAtom;
	*/

	if (pmaStszAtom -> getSamplesNumber (pulSamplesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStszAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaStszAtom -> getCommonSampleSize (&ulSampleSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStszAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulSampleSize == 0)
	{
		*pulMaxSampleSize				= 0;

		for (ulSampleIndex = 1; ulSampleIndex <= *pulSamplesNumber;
			ulSampleIndex++)
		{
			if (pmaStszAtom -> getSampleSizeTable (ulSampleIndex - 1,
				&ulSampleSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStszAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (ulSampleSize > *pulMaxSampleSize)
				*pulMaxSampleSize				= ulSampleSize;
		}
	}
	else
	{
		*pulMaxSampleSize				= ulSampleSize;
	}

	if (pmaStszAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}



	return errNoError;
}


Error MP4Utility:: getSampleSize (
	MP4StszAtom_p pmaStszAtom,
	unsigned long ulSampleIdentifier,
	unsigned long *pulSampleSize, Boolean_p pbIsCommonSampleSize,
	Tracer_p ptTracer)

{

	/*
	MP4StszAtom_p					pmsaStszAtom;
	MP4Atom_p						pmaAtom;
	*/


	if (pmaStszAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stsz:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_SEARCHATOM_FAILED, 1, "mdia:0:minf:0:stbl:0:stsz:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmsaStszAtom		= (MP4StszAtom_p) pmaAtom;
	*/

	if (pmaStszAtom -> getCommonSampleSize (pulSampleSize) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStszAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (*pulSampleSize == 0)
	{
		*pbIsCommonSampleSize				= false;

		if (pmaStszAtom -> getSampleSizeTable (ulSampleIdentifier - 1,
			pulSampleSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStszAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		*pbIsCommonSampleSize				= true;
	}

	if (pmaStszAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}



	return errNoError;
}


Error MP4Utility:: getSamplesNumber (
	MP4StszAtom_p pmaStszAtom,
	unsigned long *pulSamplesNumber, Tracer_p ptTracer)

{

	/*
	MP4StszAtom_p			pmaStszAtom;
	MP4Atom_p				pmaAtom;
	*/


	if (pmaStszAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stsz:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:minf:0:stbl:0:stsz:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaStszAtom		= (MP4StszAtom_p) pmaAtom;
	*/

	if (pmaStszAtom -> getSamplesNumber (pulSamplesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStszAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaStszAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: getRenderingOffsetFromSampleNumber (
	MP4CttsAtom_p pmaCttsAtom, unsigned long ulSampleIdentifier,
	unsigned long *pulRenderingOffset, Tracer_p ptTracer)

{

	/*
	MP4CttsAtom_p				pmaCttsAtom;
	MP4Atom_p					pmaAtom;
	*/
	unsigned long				ulEntriesNumber;
	unsigned long				ulEntryIndex;
	unsigned long				ulSampleCount;
	unsigned long				ulCurrentSampleIdentifier;


	if (pmaCttsAtom == (MP4CttsAtom_p) NULL)
	{
		*pulRenderingOffset			= 0;

		return errNoError;
	}

	if (pmaCttsAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:ctts:0", false,
		&pmaAtom) != errNoError)
	{
		*pulRenderingOffset			= 0;

		if (pmaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}
	else
		pmaCttsAtom		= (MP4CttsAtom_p) pmaAtom;
	*/

	if (pmaCttsAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaCttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulEntriesNumber == 0)
	{
		*pulRenderingOffset				= 0;

		if (pmaCttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}


		return errNoError;
	}

	ulCurrentSampleIdentifier			= 1;

	for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber; ulEntryIndex++)
	{
		if (pmaCttsAtom -> getEntries (2 * ulEntryIndex, &ulSampleCount) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaCttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (ulSampleIdentifier <= ulCurrentSampleIdentifier + ulSampleCount - 1)
			break;

		ulCurrentSampleIdentifier			+= ulSampleCount;
	}

	if (ulEntryIndex == ulEntriesNumber)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_ACTIVATION_WRONG);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaCttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaCttsAtom -> getEntries (2 * ulEntryIndex + 1, pulRenderingOffset) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaCttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaCttsAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: isSyncSample (
	MP4StssAtom_p pmaStssAtom,
	unsigned long ulSampleIdentifier, Boolean_p pbIsSyncSample,
	Tracer_p ptTracer)

{

	/*
	MP4StssAtom_p				pmaStssAtom;
	MP4Atom_p					pmaAtom;
	*/
	unsigned long				ulEntriesNumber;
	unsigned long				ulEntryIndex;
	unsigned long				ulLocalSampleIdentifier;


	if (pmaStssAtom == (MP4StssAtom_p) NULL)	// because the stss atom does not exist
	{
		*pbIsSyncSample			= true;

		return errNoError;
	}

	if (pmaStssAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stss:0", false,
		&pmaAtom) != errNoError)
	{
		*pbIsSyncSample			= true;

		if (pmaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}


		return errNoError;
	}
	else
		pmaStssAtom		= (MP4StssAtom_p) pmaAtom;
	*/

	if (pmaStssAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStssAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pbIsSyncSample			= false;

	for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber; ulEntryIndex++)
	{
		if (pmaStssAtom -> getSyncSampleTable (ulEntryIndex,
			&ulLocalSampleIdentifier) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStssAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (ulLocalSampleIdentifier == ulSampleIdentifier)
		{
			*pbIsSyncSample			= true;

			break;
		}
		else if (ulLocalSampleIdentifier > ulSampleIdentifier)
			break;
		else
			;
	}

	if (pmaStssAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: getPreviousSyncSample (MP4StssAtom_p pmaStssAtom,
	unsigned long ulSampleNumber, unsigned long *pulNewSampleNumber,
	Tracer_p ptTracer)

{

	/*
	MP4StssAtom_p				pmsaStssAtom;
	MP4Atom_p					pmaAtom;
	*/
	unsigned long				ulEntriesNumber;
	unsigned long				ulEntryIndex;
	unsigned long				ulLocalSampleNumber;


	if (pmaStssAtom == (MP4StssAtom_p) NULL)	// because the stss atom does not exist
	{
		*pulNewSampleNumber				= ulSampleNumber;

		return errNoError;
	}

	if (pmaStssAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stss:0", false,
		&pmaAtom) != errNoError)
	{
		*pulNewSampleNumber				= ulSampleNumber;

		if (pmaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}
	else
		pmsaStssAtom		= (MP4StssAtom_p) pmaAtom;
	*/

	if (pmaStssAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStssAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	*pulNewSampleNumber				= ulSampleNumber;

	for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber; ulEntryIndex++)
	{
		if (pmaStssAtom -> getSyncSampleTable (ulEntryIndex,
			&ulLocalSampleNumber) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStssAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (ulLocalSampleNumber <= ulSampleNumber)
			*pulNewSampleNumber				= ulLocalSampleNumber;
		else
			break;
	}

	if (pmaStssAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: getChunkFirstLastSample (
	MP4StscAtom_p pmaStscAtom, unsigned long ulChunkNumber,
	unsigned long *pulFirstSample, unsigned long *pulLastSample,
	Tracer_p ptTracer)

{

	/*
	MP4StscAtom_p				pmaStscAtom;
	MP4Atom_p					pmaAtom;
	*/
	unsigned long				ulEntriesNumber;
	unsigned long				ulTotalSamples;
	unsigned long				ulPrevSamplesPerChunk;
	unsigned long				ulSamplesPerChunk;



	if (pmaStscAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stsc:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:minf:0:stbl:0:stsc:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaStscAtom		= (MP4StscAtom_p) pmaAtom;
	*/

	if (pmaStscAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStscAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulEntriesNumber == 1)
	{
		if (pmaStscAtom -> getSampleToChunkTable (1,
			&ulSamplesPerChunk) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STSCATOM_GETSAMPLETOCHUNKTABLE_FAILED,
				1, (long) (1));
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		ulPrevSamplesPerChunk			=
			((ulChunkNumber - 1) * ulSamplesPerChunk);
		ulTotalSamples			= ulChunkNumber * ulSamplesPerChunk;
	}
	else
	{
		unsigned long				ulFirstChunk;
		unsigned long				ulEntryIndex;
		long						lPrevFirstChunk;
		long						lNumChunks;
		long						lNumSamplesInChunks;


		ulFirstChunk					= 0;
		ulSamplesPerChunk				= 0;
		ulTotalSamples					= 0;
		ulPrevSamplesPerChunk			= 0;

		for (ulEntryIndex = 0; ulEntryIndex < ulEntriesNumber;
			ulEntryIndex++)
		{
			ulPrevSamplesPerChunk			= ulSamplesPerChunk;
			lPrevFirstChunk					= ulFirstChunk;

			if (pmaStscAtom -> getSampleToChunkTable (3 * ulEntryIndex,
				&ulFirstChunk) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4STSCATOM_GETSAMPLETOCHUNKTABLE_FAILED,
					1, (long) (3 * ulEntryIndex));
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStscAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaStscAtom -> getSampleToChunkTable (3 * ulEntryIndex + 1,
				&ulSamplesPerChunk) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4STSCATOM_GETSAMPLETOCHUNKTABLE_FAILED,
					1, (long) (3 * ulEntryIndex + 1));
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStscAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (ulPrevSamplesPerChunk == 0)
				ulPrevSamplesPerChunk			= ulSamplesPerChunk;

			if (ulChunkNumber < ulFirstChunk ) // found chunk in group
			{
				// printf("found chunk in group numEntries = %ld this chunk = %ld \n",fNumEntries, chunkNumber);
				lNumSamplesInChunks				=
					(ulChunkNumber - lPrevFirstChunk) * ulPrevSamplesPerChunk;
				ulTotalSamples					+= lNumSamplesInChunks;
				ulPrevSamplesPerChunk			=
					ulTotalSamples - ulPrevSamplesPerChunk;

				break;
			}

			if (ulChunkNumber == ulFirstChunk ) // found chunk
			{
				lNumSamplesInChunks				= ulSamplesPerChunk;
				ulTotalSamples					+= lNumSamplesInChunks;
				ulPrevSamplesPerChunk			=
					ulTotalSamples - ulSamplesPerChunk;

				break;
			}

			lNumChunks					= ulChunkNumber - lPrevFirstChunk;
			lNumSamplesInChunks			= lNumChunks * ulSamplesPerChunk;
			ulTotalSamples				+= lNumSamplesInChunks;
		}
	}

	*pulFirstSample			= ulPrevSamplesPerChunk + 1;
	*pulLastSample			= ulTotalSamples + 1;

	if (pmaStscAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: getChunkOffsetFromChunkNumber (
	MP4StcoAtom_p pmaStcoAtom, unsigned long ulChunkNumber,
	unsigned long *pulChunkOffset, Tracer_p ptTracer)

{

	/*
	MP4StcoAtom_p				pmsaStcoAtom;
	MP4Atom_p					pmaAtom;
	*/
	// unsigned long				ulEntriesNumber;


	if (pmaStcoAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stco:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:minf:0:stbl:0:stco:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmsaStcoAtom		= (MP4StcoAtom_p) pmaAtom;
	*/

	/*
	if (pmaStcoAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStcoAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	*/

	if (pmaStcoAtom -> getChunkOffsetTable (ulChunkNumber - 1,
		pulChunkOffset) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4STCOATOM_GETCHUNKOFFSETTABLE_FAILED,
			1, ulChunkNumber - 1);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStcoAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaStcoAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: updateSampleSizes (MP4StszAtom_p pmaStszAtom,
	unsigned long ulSampleIdentifier, unsigned long ulChunkBufferSize,
	Tracer_p ptTracer)

{

	/*
	MP4StszAtom_p							pmaHintStszAtom;
	MP4Atom_p								pmaAtom;
	*/
	unsigned long							ulCommonSampleSize;
	unsigned long							ulSampleIndex;
	unsigned long							ulSamplesNumber;


	if (pmaStszAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaHintTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stsz:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "mdia:0:minf:0:stbl:0:stsz:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaHintStszAtom		= (MP4StszAtom_p) pmaAtom;
	*/

	// for first sample
	if (ulSampleIdentifier == 1)
	{
		if (ulChunkBufferSize > 0)
		{
			// presume sample size is fixed
			if (pmaStszAtom -> setCommonSampleSize (ulChunkBufferSize) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStszAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
		{
			// special case of first sample is zero bytes in length
			// leave m_pStszFixedSampleSizeProperty at 0
			// start recording variable sample sizes
			if (pmaStszAtom -> getSamplesNumber (&ulSamplesNumber) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStszAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaStszAtom -> setSampleSizeTable (ulSamplesNumber,
				0) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStszAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}
	else
	{
		// ulSampleIdentifier > 1
		if (pmaStszAtom -> getCommonSampleSize (&ulCommonSampleSize) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStszAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (ulCommonSampleSize == 0 || ulChunkBufferSize != ulCommonSampleSize)
		{
			if (pmaStszAtom -> getSamplesNumber (&ulSamplesNumber) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStszAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			// sample size is not fixed
			if (ulCommonSampleSize)
			{
				// need to clear fixed sample size
				if (pmaStszAtom -> setCommonSampleSize (0) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_GETVALUE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaStszAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				// and create sizes for all previous samples
				for (ulSampleIndex = 1;
					ulSampleIndex < ulSampleIdentifier;
					ulSampleIndex++)
				{
					if (pmaStszAtom -> setSampleSizeTable (
						ulSampleIndex - 1, ulCommonSampleSize) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4PROPERTY_GETVALUE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaStszAtom -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}
				}
			}

			// add size value for this sample
			if (pmaStszAtom -> setSampleSizeTable (ulSamplesNumber,
				ulChunkBufferSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStszAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}

	if (pmaStszAtom -> incrementSamplesNumber (1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStszAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaStszAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4Utility:: updateSampleTimes (
		MP4SttsAtom_p pmaSttsAtom,
		__int64 ullHintDuration, Tracer_p ptTracer)
#else
	Error MP4Utility:: updateSampleTimes (
		MP4SttsAtom_p pmaSttsAtom,
		unsigned long long ullHintDuration, Tracer_p ptTracer)
#endif

{

	/*
	MP4SttsAtom_p				pmsaSttsAtom;
	MP4Atom_p					pmaAtom;
	*/
	unsigned long				ulEntriesNumber;
	unsigned long				ulLocalHintDuration;


	if (pmaSttsAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaHintTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stts:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:minf:0:stbl:0:stts:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmsaSttsAtom		= (MP4SttsAtom_p) pmaAtom;
	*/

	if (pmaSttsAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaSttsAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulEntriesNumber > 0)
	{
		if (pmaSttsAtom -> getTimeToSampleTable (2 * ulEntriesNumber - 1,
			&ulLocalHintDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// if duration == duration of last entry
	if (ulEntriesNumber && ullHintDuration == ulLocalHintDuration)
	{
		// increment last entry sampleCount
		if (pmaSttsAtom -> incrementTimeToSampleTable (2 * ulEntriesNumber - 2,
			1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		// add stts entry, sampleCount = 1, sampleDuration = duration
		if (pmaSttsAtom -> setTimeToSampleTable (2 * ulEntriesNumber,
			1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaSttsAtom -> setTimeToSampleTable (2 * ulEntriesNumber + 1,
			(unsigned long) ullHintDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaSttsAtom -> incrementEntriesNumber (1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaSttsAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (pmaSttsAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4Utility:: updateRenderingOffsets (
		MP4StblAtom_p pmaStblAtom,
		unsigned long ulHintSampleIdentifier,
		__int64 ullRenderingOffset, Tracer_p ptTracer)
#else
	Error MP4Utility:: updateRenderingOffsets (
		MP4StblAtom_p pmaStblAtom,
		unsigned long ulHintSampleIdentifier,
		unsigned long long ullRenderingOffset, Tracer_p ptTracer)
#endif

{

	MP4CttsAtom_p				pmaCttsAtom;
	// MP4StblAtom_p				pmaStblAtom;
	Error_t						errSearchAtom;
	unsigned long				ulEntriesNumber;
	unsigned long				ulSampleOffset;
	MP4Atom_p					pmaAtom;


	if (pmaStblAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if ((errSearchAtom = pmaHintTrakAtom -> searchAtom (
		"mdia:0:minf:0:stbl:0", true, &pmaAtom)) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:minf:0:stbl:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaStblAtom		= (MP4StblAtom_p) pmaAtom;
	*/

	if ((errSearchAtom = pmaStblAtom -> searchAtom (
		"ctts:0", false, &pmaAtom)) != errNoError)
	{
		if ((long) errSearchAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "ctts:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStblAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// if ctts atom doesn't exist
		// no rendering offset, so nothing to do
		if (ullRenderingOffset == 0)
		{
			if (pmaStblAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errNoError;
		}

		// else create a ctts atom
		if (pmaStblAtom -> addChild ("ctts", &pmaAtom, -1) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_ADDCHILD_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmaStblAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		else
			pmaCttsAtom		= (MP4CttsAtom_p) pmaAtom;

		// if this is not the first sample
		if (ulHintSampleIdentifier > 1)
		{
			// add a ctts entry for all previous samples
			// with rendering offset equal to zero
			if (pmaCttsAtom -> setEntries (0,
				ulHintSampleIdentifier - 1) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_SETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStblAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaCttsAtom -> setEntries (1, 0) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_SETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStblAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaCttsAtom -> incrementEntriesNumber (2) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStblAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
	}
	else
		pmaCttsAtom		= (MP4CttsAtom_p) pmaAtom;

	// ctts atom exists (now)
	if (pmaCttsAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStblAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaCttsAtom -> getEntries (ulEntriesNumber - 1, &ulSampleOffset) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStblAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// if renderingOffset == renderingOffset of last entry
	if (ulEntriesNumber && ullRenderingOffset == ulSampleOffset)
	{
		// increment last entry sampleCount
		if (pmaCttsAtom -> incrementEntries (ulEntriesNumber - 2, 1) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStblAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		// add ctts entry, sampleCount = 1, sampleOffset = renderingOffset
		if (pmaCttsAtom -> setEntries (ulEntriesNumber, 1) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStblAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaCttsAtom -> setEntries (ulEntriesNumber + 1,
			(unsigned long) ullRenderingOffset) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStblAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaCttsAtom -> incrementEntriesNumber (1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStblAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (pmaStblAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: updateSyncSamples (
	MP4StblAtom_p pmaStblAtom,
	unsigned long ulSampleIdentifier, Boolean_t bIsSyncSample,
	Tracer_p ptTracer)

{

	// MP4StblAtom_p				pmaStblAtom;
	MP4StssAtom_p				pmaStssAtom;
	Error_t						errSearchAtom;
	unsigned long				ulEntriesNumber;
	unsigned long				ulSampleIdentifierIndex;
	Boolean_t					bIsStssExists;
	MP4Atom_p					pmaAtom;


	if (pmaStblAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if ((errSearchAtom = pmaHintTrakAtom -> searchAtom (
		"mdia:0:minf:0:stbl:0", true, &pmaAtom)) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:minf:0:stbl:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaStblAtom		= (MP4StblAtom_p) pmaAtom;
	*/

	if ((errSearchAtom = pmaStblAtom -> searchAtom (
		"stss:0", false, &pmaAtom)) != errNoError)
	{
		if ((long) errSearchAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED,
				1, "stss:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStblAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		bIsStssExists			= false;
	}
	else
	{
		bIsStssExists			= true;
		pmaStssAtom		= (MP4StssAtom_p) pmaAtom;
	}

	if (bIsSyncSample)
	{
		if (bIsStssExists)
		{
			// if stss atom exists, add entry
			if (pmaStssAtom -> getEntriesNumber (&ulEntriesNumber) !=
				errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStblAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaStssAtom -> setSyncSampleTable (ulEntriesNumber,
				ulSampleIdentifier) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_SETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStblAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaStssAtom -> incrementEntriesNumber (1) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaStblAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}	// nothing to do (yet)
	}
	else
	{ // !isSyncSample
		// if stss atom doesn't exist, create one
		if (!bIsStssExists)
		{
			if (pmaStblAtom -> addChild ("stss", &pmaAtom,
				-1) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_ADDCHILD_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
					__FILE__, __LINE__);

				if (pmaStblAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			else
				pmaStssAtom		= (MP4StssAtom_p) pmaAtom;

			ulEntriesNumber				= 0;

			// set values for all samples that came before this one
			for (ulSampleIdentifierIndex = 1;
				ulSampleIdentifierIndex < ulSampleIdentifier;
				ulSampleIdentifierIndex++)
			{
				if (pmaStssAtom -> setSyncSampleTable (ulEntriesNumber++,
					ulSampleIdentifierIndex) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_SETVALUE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaStblAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}

				if (pmaStssAtom -> incrementEntriesNumber (1) != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaStblAtom -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
			}
		} // else nothing to do
	}

	if (pmaStblAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: updateSampleToChunk (
	MP4StscAtom_p pmaStscAtom,
	unsigned long ulHintSampleIdentifier, unsigned long ulChunkIdentifier,
	unsigned long ulSamplesPerChunk, Tracer_p ptTracer)

{

	/*
	MP4StscAtom_p				pmaStscAtom;
	MP4Atom_p					pmaAtom;
	*/
	unsigned long				ulEntriesNumber;
	unsigned long				ulLocalSamplesPerChunk;


	if (pmaStscAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaHintTrakAtom -> searchAtom ("mdia:0:minf:0:stbl:0:stsc:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:minf:0:stbl:0:stsc:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaStscAtom		= (MP4StscAtom_p) pmaAtom;
	*/

	if (pmaStscAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStscAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ulEntriesNumber > 0)
	{
		if (pmaStscAtom -> getSampleToChunkTable (3 * (ulEntriesNumber - 1) + 1,
			&ulLocalSamplesPerChunk) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4STSCATOM_GETSAMPLETOCHUNKTABLE_FAILED,
				1, (long) (3 * (ulEntriesNumber - 1) + 1));
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	// if samplesPerChunk == samplesPerChunk of last entry
	if (ulEntriesNumber && ulSamplesPerChunk == ulLocalSamplesPerChunk)
	{
		// nothing to do
	}
	else
	{
		// add stsc entry
		if (pmaStscAtom -> setSampleToChunkTable (3 * ulEntriesNumber,
			ulChunkIdentifier) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaStscAtom -> setSampleToChunkTable (3 * ulEntriesNumber + 1,
			ulSamplesPerChunk) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaStscAtom -> setSampleToChunkTable (3 * ulEntriesNumber + 2,
			1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		// m_pStscFirstSampleProperty->AddValue(sampleId - samplesPerChunk + 1);

		if (pmaStscAtom -> incrementEntriesNumber (1) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaStscAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (pmaStscAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4Utility:: updateChunkOffsets (MP4StcoAtom_p pmaStcoAtom,
		__int64 ullChunkOffset, Tracer_p ptTracer)
#else
	Error MP4Utility:: updateChunkOffsets (MP4StcoAtom_p pmaStcoAtom,
		unsigned long long ullChunkOffset, Tracer_p ptTracer)
#endif

{

	unsigned long					ulEntriesNumber;


	if (pmaStcoAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (pmaStcoAtom -> getEntriesNumber (&ulEntriesNumber) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStcoAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaStcoAtom -> setChunkOffsetTable (ulEntriesNumber,
		(unsigned long) ullChunkOffset) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStcoAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaStcoAtom -> incrementEntriesNumber (1) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_INCREMENTVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaStcoAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaStcoAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4Utility:: updateDurations (
		MP4TkhdAtom_p pmaTkhdAtom, MP4MdhdAtom_p pmaMdhdAtom,
		MP4MvhdAtom_p pmaMvhdAtom,
		__int64 ullHintDuration, Tracer_p ptTracer)
#else
	Error MP4Utility:: updateDurations (
		MP4TkhdAtom_p pmaTkhdAtom, MP4MdhdAtom_p pmaMdhdAtom,
		MP4MvhdAtom_p pmaMvhdAtom,
		unsigned long long ullHintDuration, Tracer_p ptTracer)
#endif

{

	#ifdef WIN32
		__int64							ullMediaDuration;
		__int64							ullHintTrackDuration;
		__int64							ullMoovDuration;
		__int64							ullMovieDuration;
	#else
		unsigned long long				ullMediaDuration;
		unsigned long long				ullHintTrackDuration;
		unsigned long long				ullMoovDuration;
		unsigned long long				ullMovieDuration;
	#endif
	// MP4MdhdAtom_p					pmaMdhdAtom;
	// MP4MvhdAtom_p					pmaMvhdAtom;
	// MP4TkhdAtom_p					pmaHintTkhdAtom;
	unsigned long					ulTrackTimeScale;
	unsigned long					ulMoovTimeScale;
	// MP4Atom_p						pmaAtom;


	if (pmaTkhdAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	/*
	if (pmaHintTrakAtom -> searchAtom ("tkhd:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED, 1, "tkhd:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaHintTkhdAtom		= (MP4TkhdAtom_p) pmaAtom;
	*/

	/*
	if (pmaHintTrakAtom -> searchAtom ("mdia:0:mdhd:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:mdhd:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaMdhdAtom		= (MP4MdhdAtom_p) pmaAtom;
	*/

	/*
	if (pmaRootAtom -> searchAtom ("moov:0:mvhd:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_SEARCHATOM_FAILED, 1, "moov:0:mvhd:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaMvhdAtom		= (MP4MvhdAtom_p) pmaAtom;
	*/

	if (pmaTkhdAtom -> getDuration (&ullHintTrackDuration) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaTkhdAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaMvhdAtom -> getTimeScale (&ulMoovTimeScale) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaTkhdAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaMdhdAtom -> getTimeScale (&ulTrackTimeScale) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaTkhdAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// update media, track, and movie durations
	if (pmaMdhdAtom -> getDuration (&ullMediaDuration) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaTkhdAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaMdhdAtom -> setDuration (ullMediaDuration + ullHintDuration) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaTkhdAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// calcolated moov duration
	ullMovieDuration			= ulMoovTimeScale * ullHintDuration /
		ulTrackTimeScale;

	if (pmaTkhdAtom -> setDuration (
		ullHintTrackDuration + ullMovieDuration) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaTkhdAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// moov duration from mvhd atom
	if (pmaMvhdAtom -> getDuration (&ullMoovDuration) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaTkhdAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (ullHintTrackDuration + ullMovieDuration > ullMoovDuration)
	{
		if (pmaMvhdAtom -> setDuration (
			ullHintTrackDuration + ullMovieDuration) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaTkhdAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}

	if (pmaTkhdAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: updateModificationTimes (
	MP4MdhdAtom_p pmaMdhdAtom, MP4TkhdAtom_p pmaTkhdAtom,
	Tracer_p ptTracer)

{

	#ifdef WIN32
		__int64							ullNow;
		DWORD							tv;
	#else
		unsigned long long				ullNow;
		struct timeval					tv;
	#endif
	// MP4MdhdAtom_p					pmaMdhdAtom;
	// MP4TkhdAtom_p					pmaTkhdAtom;
	// MP4Atom_p						pmaAtom;


	if (pmaMdhdAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// update media and track modification times
	#ifdef WIN32
		tv			= GetTickCount ();
		ullNow		= tv / 1000 + 209606400;	// MP4 start date is 1/1/1904
	#else
		if (gettimeofday (&tv, NULL) == -1)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_GETTIMEOFDAY_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
				__FILE__, __LINE__);

			if (pmaMdhdAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
		ullNow			= tv.tv_sec + 209606400;	// MP4 start date is 1/1/1904
	#endif

	/*
	if (pmaHintTrakAtom -> searchAtom ("mdia:0:mdhd:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "mdia:0:mdhd:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaMdhdAtom		= (MP4MdhdAtom_p) pmaAtom;
	*/

	if (pmaMdhdAtom -> setModificationTime (ullNow) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaMdhdAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	/*
	if (pmaHintTrakAtom -> searchAtom ("tkhd:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_SEARCHATOM_FAILED,
			1, "tkhd:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaHintTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmaTkhdAtom		= (MP4TkhdAtom_p) pmaAtom;
	*/

	if (pmaTkhdAtom -> setModificationTime (ullNow) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_SETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR, (const char *) err,
			__FILE__, __LINE__);

		if (pmaMdhdAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaMdhdAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


#ifdef WIN32
	Error MP4Utility:: getPointerTrackESConfiguration (
		MP4TrakAtom_p pmaMediaTrakAtom, unsigned char **pucESConfiguration,
		__int64 *pullESConfigurationSize,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#else
	Error MP4Utility:: getPointerTrackESConfiguration (
		MP4TrakAtom_p pmaMediaTrakAtom, unsigned char **pucESConfiguration,
		unsigned long long *pullESConfigurationSize,
		MP4Atom:: Standard_t sStandard, Tracer_p ptTracer)
#endif

{

	MP4DecoderSpecificDescr_p		pmaMediaDecoderSpecificDescr;
	char							pAtomPath [MP4F_MAXPATHNAMELENGTH];
	Error_t							errSearchAtom;
	MP4Atom_p						pmaAtom;


	if (pmaMediaTrakAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:mp4a:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");

	errSearchAtom			= errNoError;

	if ((errSearchAtom = pmaMediaTrakAtom -> searchAtom (pAtomPath, false,
		&pmaAtom)) != errNoError)
	{
		if ((long) errSearchAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
				"mdia:0:minf:0:stbl:0:stsd:0:mp4a:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
		pmaMediaDecoderSpecificDescr		=
			(MP4DecoderSpecificDescr_p) pmaAtom;

	if (errSearchAtom == errNoError)
	{
		if (pmaMediaDecoderSpecificDescr -> getPointerToInfo (
			pucESConfiguration, pullESConfigurationSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:mp4s:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");

	errSearchAtom			= errNoError;

	if ((errSearchAtom = pmaMediaTrakAtom -> searchAtom (pAtomPath, false,
		&pmaAtom)) != errNoError)
	{
		if ((long) errSearchAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
				"mdia:0:minf:0:stbl:0:stsd:0:mp4s:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
		pmaMediaDecoderSpecificDescr		=
			(MP4DecoderSpecificDescr_p) pmaAtom;

	if (errSearchAtom == errNoError)
	{
		if (pmaMediaDecoderSpecificDescr -> getPointerToInfo (
			pucESConfiguration, pullESConfigurationSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:mp4v:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");

	errSearchAtom			= errNoError;

	if ((errSearchAtom = pmaMediaTrakAtom -> searchAtom (pAtomPath, false,
		&pmaAtom)) != errNoError)
	{
		if ((long) errSearchAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
				"mdia:0:minf:0:stbl:0:stsd:0:mp4v:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
		pmaMediaDecoderSpecificDescr		=
			(MP4DecoderSpecificDescr_p) pmaAtom;

	if (errSearchAtom == errNoError)
	{
		if (pmaMediaDecoderSpecificDescr -> getPointerToInfo (
			pucESConfiguration, pullESConfigurationSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	if (sStandard == MP4Atom:: MP4F_3GPP)
	{
		strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:samr:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");

		errSearchAtom			= errNoError;

		if ((errSearchAtom = pmaMediaTrakAtom -> searchAtom (pAtomPath, false,
			&pmaAtom)) != errNoError)
		{
			if ((long) errSearchAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
					"mdia:0:minf:0:stbl:0:stsd:0:samr:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
			pmaMediaDecoderSpecificDescr		=
				(MP4DecoderSpecificDescr_p) pmaAtom;

		if (errSearchAtom == errNoError)
		{
			if (pmaMediaDecoderSpecificDescr -> getPointerToInfo (
				pucESConfiguration, pullESConfigurationSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errNoError;
		}

		strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:sawb:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");

		errSearchAtom			= errNoError;

		if ((errSearchAtom = pmaMediaTrakAtom -> searchAtom (pAtomPath, false,
			&pmaAtom)) != errNoError)
		{
			if ((long) errSearchAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
					"mdia:0:minf:0:stbl:0:stsd:0:sawb:0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else
			pmaMediaDecoderSpecificDescr		=
				(MP4DecoderSpecificDescr_p) pmaAtom;

		if (errSearchAtom == errNoError)
		{
			if (pmaMediaDecoderSpecificDescr -> getPointerToInfo (
				pucESConfiguration, pullESConfigurationSize) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4PROPERTY_GETVALUE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return errNoError;
		}
	}
	else			// MP4Atom:: MP4F_ISMA
		;

	strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:rtp :0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");

	errSearchAtom			= errNoError;

	if ((errSearchAtom = pmaMediaTrakAtom -> searchAtom (pAtomPath, false,
		&pmaAtom)) != errNoError)
	{
		if ((long) errSearchAtom != MP4F_MP4ATOM_ATOMNOTFOUND)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
				"mdia:0:minf:0:stbl:0:stsd:0:rtp :0:esds:0:ESDescr:0:DecoderConfigDescr:0:DecoderSpecificDescr:0");
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
		pmaMediaDecoderSpecificDescr		=
			(MP4DecoderSpecificDescr_p) pmaAtom;

	if (errSearchAtom == errNoError)
	{
		if (pmaMediaDecoderSpecificDescr -> getPointerToInfo (
			pucESConfiguration, pullESConfigurationSize) != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_GETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return errNoError;
	}

	if (pmaMediaTrakAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errSearchAtom;
}


/*
Error MP4Utility:: buildSDPForMediaVideoTrack (
	MP4TrakAtom_p pmaMediaVideoTrack,
	MP4HdlrAtom_p pmaMediaHdlrAtom,
	MP4MdhdAtom_p pmaMediaMdhdAtom,
	MP4StszAtom_p pmaStszAtom,
	MP4Atom:: MP4Codec_t cCodecUsed,
	unsigned long ulObjectTypeIndication,
	unsigned long ulTrackIdentifier,
	unsigned long ulAvgBitRate,
	unsigned long ulPayloadNumber,
	MP4RootAtom_p pmaRootAtom,
	MP4Atom:: Standard_t sStandard,
	Buffer_p pbSDP, Tracer_p ptTracer)

{

	MP4Atom_p					pmaAtom;


	if (pmaMediaVideoTrack -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (cCodecUsed == MP4Atom:: MP4F_CODEC_MPEG4)
	{
		switch (ulObjectTypeIndication)
		{
			case MP4_MPEG4_VIDEO_TYPE:
				{
					// rfc3016

					char						pClockRate [MP4_MAXLONGLENGTH];
					unsigned char				*pucESConfiguration;
					#ifdef WIN32
						__int64						ullESConfigurationSize;
					#else
						unsigned long long			ullESConfigurationSize;
					#endif
					unsigned char				ucSystemsProfileLevel;
					char						*pESConfigToBase16;


					{
						unsigned long				ulHintTimeScale;


						if (MP4Utility:: getHintTimeScale (
							pmaMediaVideoTrack,
							pmaMediaMdhdAtom,
							cCodecUsed,
							ulObjectTypeIndication,
							sStandard, &ulHintTimeScale,
							ptTracer) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4UTILITY_GETHINTTIMESCALE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaVideoTrack ->
								unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						sprintf (pClockRate, "%lu", ulHintTimeScale);
					}

					if (MP4Utility:: getPointerTrackESConfiguration (
						pmaMediaVideoTrack,
						&pucESConfiguration, &ullESConfigurationSize, sStandard,
						ptTracer) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4UTILITY_GETPOINTERTRACKESCONFIGURATION_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (ullESConfigurationSize == 0)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILE_WRONGESCONFIGURATIONFOUND);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaVideoTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					{
						unsigned char				pucVoshStartCode [4];


						// attempt to get a valid profile-level
						pucVoshStartCode [0]			= 0x00;
						pucVoshStartCode [1]			= 0x00;
						pucVoshStartCode [2]			= 0x01;
						pucVoshStartCode [3]			= MP4F_MPEG4_VOSH_START;

						ucSystemsProfileLevel			= 0xFE;

						if (ullESConfigurationSize >= 5 &&
							!memcmp (pucESConfiguration, pucVoshStartCode, 4))
						{
							if (sStandard == MP4Atom:: MP4F_3GPP)
								ucSystemsProfileLevel			= pucESConfiguration [4];
							else			// MP4Atom:: MP4F_ISMA
								ucSystemsProfileLevel			=
									MP4AV_Mpeg4VideoToSystemsProfileLevel (
									pucESConfiguration [4]);
						}

						if (ucSystemsProfileLevel == 0xFE)
						{
							MP4InitialObjectDescr_p			pmaInitialObjectDescr;
							char							pAtomPath [MP4F_MAXPATHNAMELENGTH];
							unsigned char					ucVisualProfileLevelIndication;


							strcpy (pAtomPath, "moov:0:iods:0:InitialObjectDescr:0");

							if (pmaRootAtom -> searchAtom (pAtomPath, true,
								&pmaAtom) != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_SEARCHATOM_FAILED, 1,
									"moov:0:iods:0:InitialObjectDescr:0");
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (pmaMediaVideoTrack ->
									unLockMP4File () != errNoError)
								{
									Error err = MP4FileErrors (__FILE__, __LINE__,
										MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								return err;
							}
							else
								pmaInitialObjectDescr	= (MP4InitialObjectDescr_p) pmaAtom;

							if (pmaInitialObjectDescr -> getVisualProfileLevelIndication (
								&ucVisualProfileLevelIndication) != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4INITIALOBJECTDESCR_GETVISUALPROFILELEVELINDICATION_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);

								if (pmaMediaVideoTrack ->
									unLockMP4File () != errNoError)
								{
									Error err = MP4FileErrors (__FILE__, __LINE__,
										MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
									ptTracer -> trace (Tracer:: TRACER_LERRR,
										(const char *) err, __FILE__, __LINE__);
								}

								return err;
							}

							if (ucVisualProfileLevelIndication > 0 &&
								ucVisualProfileLevelIndication < 0xFE)
							{
								ucSystemsProfileLevel		= ucVisualProfileLevelIndication;
							}
							else
							{
								ucSystemsProfileLevel		= 1;
							}
						}

						if ((pESConfigToBase16 = new char [
							(unsigned int) (ullESConfigurationSize * 2 + 1)]) ==
							(char *) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4UTILITY_NOPAYLOADAVAILABLE);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaVideoTrack ->
								unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						// convert it into ASCII form
						if (binaryToBase16 (pucESConfiguration, (unsigned long) ullESConfigurationSize,
							pESConfigToBase16, (unsigned long) (ullESConfigurationSize * 2 + 1)) !=
							errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								TOOLS_BINARYTOBASE16_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaVideoTrack ->
								unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}

					if (pbSDP -> append ("m=video 0 RTP/AVP ") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (ulPayloadNumber) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append ("b=AS:") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (
						(unsigned long) (ulAvgBitRate / 1000)) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append ("a=rtpmap:") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (ulPayloadNumber) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (" MP4V-ES/") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (pClockRate) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (sStandard == MP4Atom:: MP4F_3GPP)
					{
					}
					else
					{
						if (pbSDP -> append ("a=mpeg4-esid:") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaVideoTrack ->
								unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (ulTrackIdentifier) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaVideoTrack ->
								unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaVideoTrack ->
								unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}

					if (pbSDP -> append ("a=fmtp:") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (ulPayloadNumber) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (" profile-level-id=") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (
						(unsigned long) ucSystemsProfileLevel) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (";config=") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (pESConfigToBase16) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append ("a=control:trackID/") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (
						ulTrackIdentifier) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaVideoTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					delete [] pESConfigToBase16;
					pESConfigToBase16			= (char *) NULL;
				}

				break;
			default:
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_CODECUNKNOWN,
						1, ulTrackIdentifier);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaMediaVideoTrack ->
						unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
		}
	}
	else if (cCodecUsed == MP4Atom:: MP4F_CODEC_H263)
	{
		// rfc2429
		char						pClockRate [MP4_MAXLONGLENGTH];
		unsigned long				ulWidth;
		unsigned long				ulHeight;


		{
			unsigned long				ulHintTimeScale;


			if (MP4Utility:: getHintTimeScale (
				pmaMediaVideoTrack,
				pmaMediaMdhdAtom,
				cCodecUsed,
				ulObjectTypeIndication,
				sStandard, &ulHintTimeScale,
				ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETHINTTIMESCALE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaVideoTrack ->
					unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pClockRate, "%lu", ulHintTimeScale);
		}

		{
			MP4S263Atom_p				pmaS263Atom;
			char						pAtomPath [MP4F_MAXPATHNAMELENGTH];


			strcpy (pAtomPath, "mdia:0:minf:0:stbl:0:stsd:0:s263:0");

			if (pmaMediaVideoTrack -> searchAtom (
				pAtomPath, true, &pmaAtom) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_SEARCHATOM_FAILED,
					1, pAtomPath);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaVideoTrack ->
					unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
			else
				pmaS263Atom			=
					(MP4S263Atom_p) pmaAtom;

			if (pmaS263Atom -> getWidth (&ulWidth) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4S263ATOM_GETWIDTH_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaVideoTrack ->
					unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			if (pmaS263Atom -> getHeight (&ulHeight) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4S263ATOM_GETHEIGHT_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaVideoTrack ->
					unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (pbSDP -> append ("m=video 0 RTP/AVP ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (ulPayloadNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append ("b=AS:") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (
			(unsigned long) (ulAvgBitRate / 1000)) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append ("a=rtpmap:") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (ulPayloadNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (" H263-2000/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (pClockRate) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append ("a=fmtp:") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (ulPayloadNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (" framesize=") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (ulWidth) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append ("-") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (ulHeight) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append ("a=control:trackID/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (
			ulTrackIdentifier) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaVideoTrack ->
				unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_CODECUNKNOWN,
			1, ulTrackIdentifier);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaMediaVideoTrack ->
			unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaMediaVideoTrack ->
		unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: buildSDPForMediaAudioTrack (
	MP4TrakAtom_p pmaMediaAudioTrack,
	MP4HdlrAtom_p pmaMediaHdlrAtom,
	MP4MdhdAtom_p pmaMediaMdhdAtom,
	MP4StszAtom_p pmaStszAtom,
	MP4Atom:: MP4Codec_t cCodecUsed,
	unsigned long ulObjectTypeIndication,
	unsigned long ulTrackIdentifier,
	unsigned long ulAvgBitRate,
	unsigned long ulPayloadNumber,
	MP4RootAtom_p pmaRootAtom,
	MP4Atom:: Standard_t sStandard,
	Buffer_p pbSDP, Tracer_p ptTracer)

{

	if (pmaMediaAudioTrack -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	if (cCodecUsed == MP4Atom:: MP4F_CODEC_AAC)
	{
		// ulObjectTypeIndication is the TrackAudioType
		switch (ulObjectTypeIndication)
		{
			case MP4_MPEG4_AUDIO_TYPE:
			case MP4_MPEG2_AAC_MAIN_AUDIO_TYPE:
			case MP4_MPEG2_AAC_LC_AUDIO_TYPE:
			case MP4_MPEG2_AAC_SSR_AUDIO_TYPE:
				if (sStandard == MP4Atom:: MP4F_3GPP)
				{
					// rfc3016

					char						pClockRate [MP4_MAXLONGLENGTH];
					unsigned char				*pucESConfiguration;
					#ifdef WIN32
						__int64						ullESConfigurationSize;
					#else
						unsigned long long			ullESConfigurationSize;
					#endif
					unsigned char				pucStreamMuxConfig [6];
					unsigned long				ulStreamMuxConfigSize		= 6;
					char						*pESConfigToBase16;


					if (MP4Utility:: getPointerTrackESConfiguration (
						pmaMediaAudioTrack,
						&pucESConfiguration, &ullESConfigurationSize, MP4Atom:: MP4F_3GPP,
						ptTracer) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4UTILITY_GETPOINTERTRACKESCONFIGURATION_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (ullESConfigurationSize != 2)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILE_WRONGESCONFIGURATIONFOUND);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					{
						unsigned long				ulHintTimeScale;


						if (MP4Utility:: getHintTimeScale (
							pmaMediaAudioTrack,
							pmaMediaMdhdAtom,
							cCodecUsed,
							ulObjectTypeIndication,
							sStandard, &ulHintTimeScale,
							ptTracer) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4UTILITY_GETHINTTIMESCALE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaAudioTrack ->
								unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						sprintf (pClockRate, "%lu", ulHintTimeScale);
					}

					// TrackAudioMpeg4Type (AAC, CELP, HXVC, ...) is the first 5 bits
					// of the ES configuration

					if (ulObjectTypeIndication != MP4_MPEG4_AUDIO_TYPE &&
						!MP4_IS_AAC_AUDIO_TYPE (ulObjectTypeIndication))
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILE_NONAACAUDIOTYPE);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (ulObjectTypeIndication == MP4_MPEG4_AUDIO_TYPE)
					{
						if (!MP4_IS_MPEG4_AAC_AUDIO_TYPE (
							((unsigned char) pucESConfiguration [0] >> 3)) &&
							((unsigned char) pucESConfiguration [0] >> 3) !=
							MP4_MPEG4_CELP_AUDIO_TYPE)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILE_NONAACAUDIOTYPE);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}

					if (pbSDP -> append ("m=audio 0 RTP/AVP ") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (ulPayloadNumber) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append ("b=AS:") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (
						(unsigned long) (ulAvgBitRate / 1000)) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append ("a=rtpmap:") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (ulPayloadNumber) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (" MP4A-LATM/") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (pClockRate) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					{
						// see StreamMuxConfig() in ISO document, audio part, sub1
						unsigned short				us;
						#ifdef WIN32
							__int64						ul;
							__int64						ulTemp;
						#else
							unsigned long long			ul;
							unsigned long long			ulTemp;
						#endif


						us						= ntohs (
							*((unsigned short *) pucESConfiguration));
						ul							= us;
						ul							= ul << 17;
						ulTemp						= 0x000040;
						ulTemp						= ulTemp << 40;
						ul							= ul | ulTemp;

						pucStreamMuxConfig [0]		= (unsigned char) ((ul >> (5 * 8)) & 0x00000000000000FF);
						pucStreamMuxConfig [1]		= (unsigned char) ((ul >> (4 * 8)) & 0x00000000000000FF);
						pucStreamMuxConfig [2]		= (unsigned char) ((ul >> (3 * 8)) & 0x00000000000000FF);
						pucStreamMuxConfig [3]		= (unsigned char) ((ul >> (2 * 8)) & 0x00000000000000FF);
						pucStreamMuxConfig [4]		= (unsigned char) ((ul >> (1 * 8)) & 0x00000000000000FF);
						pucStreamMuxConfig [5]		= (unsigned char) ((ul >> (0 * 8)) & 0x00000000000000FF);

						if ((pESConfigToBase16 =
							new char [ulStreamMuxConfigSize * 2 + 1]) == (char *) NULL)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4UTILITY_NOPAYLOADAVAILABLE);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						// convert it into ASCII form
						if (binaryToBase16 (pucStreamMuxConfig, ulStreamMuxConfigSize,
							pESConfigToBase16, ulStreamMuxConfigSize * 2 + 1) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								TOOLS_BINARYTOBASE16_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}

					if (((unsigned char) pucESConfiguration [0] >> 3) ==
						MP4_MPEG4_CELP_AUDIO_TYPE)
					{
						// error
						if (pbSDP -> append ("a=fmtp:") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (ulPayloadNumber) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (" cpresent=0;config=") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (pESConfigToBase16) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}
					else
					{
						// AAC
						if (pbSDP -> append ("a=fmtp:") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (ulPayloadNumber) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (" cpresent=0;config=") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (pESConfigToBase16) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}

					if (pbSDP -> append ("a=control:trackID/") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (
						ulTrackIdentifier) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					delete [] pESConfigToBase16;
					pESConfigToBase16			= (char *) NULL;
				}
				else
				{
					// rfcIsma

					char						pClockRate [MP4_MAXLONGLENGTH];
					unsigned char				*pucESConfiguration;
					#ifdef WIN32
						__int64						ullESConfigurationSize;
					#else
						unsigned long long			ullESConfigurationSize;
					#endif
					char						*pESConfigToBase16;
					unsigned char				ucChannels;
					char						pChannels [MP4_MAXLONGLENGTH];


					if (MP4Utility:: getPointerTrackESConfiguration (
						pmaMediaAudioTrack,
						&pucESConfiguration, &ullESConfigurationSize,
						MP4Atom:: MP4F_ISMA, ptTracer) != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4UTILITY_GETPOINTERTRACKESCONFIGURATION_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (ullESConfigurationSize == 0)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILE_WRONGESCONFIGURATIONFOUND);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (ulObjectTypeIndication != MP4_MPEG4_AUDIO_TYPE &&
						!MP4_IS_AAC_AUDIO_TYPE (ulObjectTypeIndication))
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4FILE_NONAACAUDIOTYPE);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (ulObjectTypeIndication == MP4_MPEG4_AUDIO_TYPE)
					{
						if (!MP4_IS_MPEG4_AAC_AUDIO_TYPE (
							((unsigned char) pucESConfiguration [0] >> 3)) &&
							((unsigned char) pucESConfiguration [0] >> 3) !=
							MP4_MPEG4_CELP_AUDIO_TYPE)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4FILE_NONAACAUDIOTYPE);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}

					if ((pESConfigToBase16 = new char [
						(unsigned int) (ullESConfigurationSize * 2 + 1)]) ==
						(char *) NULL)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4UTILITY_NOPAYLOADAVAILABLE);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						if (pmaMediaAudioTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					// convert it into ASCII form
					if (binaryToBase16 (pucESConfiguration, (unsigned long) ullESConfigurationSize,
						pESConfigToBase16, (unsigned long) (ullESConfigurationSize * 2 + 1)) !=
						errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							TOOLS_BINARYTOBASE16_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					ucChannels					= MP4AV_AacConfigGetChannels (pucESConfiguration);

					if (ucChannels != 1)
					{
						sprintf (pChannels, "%u", ucChannels);
					}

					{
						unsigned long				ulHintTimeScale;


						if (MP4Utility:: getHintTimeScale (
							pmaMediaAudioTrack,
							pmaMediaMdhdAtom,
							cCodecUsed,
							ulObjectTypeIndication,
							sStandard, &ulHintTimeScale,
							ptTracer) != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4UTILITY_GETHINTTIMESCALE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							if (pmaMediaAudioTrack ->
								unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						sprintf (pClockRate, "%lu", ulHintTimeScale);
					}

					if (pbSDP -> append ("m=audio 0 RTP/AVP ") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (ulPayloadNumber) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append ("b=AS:") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (
						(unsigned long) (ulAvgBitRate / 1000)) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append ("a=rtpmap:") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (ulPayloadNumber) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (" mpeg4-generic/") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (pClockRate) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (ucChannels != 1)
					{
						if (pbSDP -> append ("/") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (pChannels) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append ("a=mpeg4-esid:") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (ulTrackIdentifier) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack ->
							unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (((unsigned char) pucESConfiguration [0] >> 3) ==
						MP4_MPEG4_CELP_AUDIO_TYPE)
					{
						if (pbSDP -> append ("a=fmtp:") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (ulPayloadNumber) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (" streamtype=5;profile-level-id=15;mode=CELP-vbr;config=") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (pESConfigToBase16) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (";SizeLength=6;IndexLength=2;IndexDeltaLength=2;Profile=0") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}
					else
					{
						if (pbSDP -> append ("a=fmtp:") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (ulPayloadNumber) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (" streamtype=5;profile-level-id=15;mode=AAC-hbr;config=") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (pESConfigToBase16) != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}

						if (pbSDP -> append (";SizeLength=13;IndexLength=3;IndexDeltaLength=3;Profile=1") != errNoError)
						{
							Error err = ToolsErrors (__FILE__, __LINE__,
								TOOLS_BUFFER_APPEND_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);

							delete [] pESConfigToBase16;
							pESConfigToBase16			= (char *) NULL;

							if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
							{
								Error err = MP4FileErrors (__FILE__, __LINE__,
									MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
								ptTracer -> trace (Tracer:: TRACER_LERRR,
									(const char *) err, __FILE__, __LINE__);
							}

							return err;
						}
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append ("a=control:trackID/") != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (ulTrackIdentifier) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
					{
						Error err = ToolsErrors (__FILE__, __LINE__,
							TOOLS_BUFFER_APPEND_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);

						delete [] pESConfigToBase16;
						pESConfigToBase16			= (char *) NULL;

						if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
						{
							Error err = MP4FileErrors (__FILE__, __LINE__,
								MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
							ptTracer -> trace (Tracer:: TRACER_LERRR,
								(const char *) err, __FILE__, __LINE__);
						}

						return err;
					}

					delete [] pESConfigToBase16;
					pESConfigToBase16			= (char *) NULL;
				}

				break;
			default:
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_CODECUNKNOWN,
						1, ulTrackIdentifier);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);

					if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
					{
						Error err = MP4FileErrors (__FILE__, __LINE__,
							MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
						ptTracer -> trace (Tracer:: TRACER_LERRR,
							(const char *) err, __FILE__, __LINE__);
					}

					return err;
				}
		}
	}
	else if (cCodecUsed == MP4Atom:: MP4F_CODEC_AMRNB ||
		cCodecUsed == MP4Atom:: MP4F_CODEC_AMRWB)
	{
		// rfc3267

		char						pClockRate [MP4_MAXLONGLENGTH];


		{
			unsigned long				ulHintTimeScale;


			if (MP4Utility:: getHintTimeScale (
				pmaMediaAudioTrack,
				pmaMediaMdhdAtom,
				cCodecUsed,
				ulObjectTypeIndication,
				sStandard, &ulHintTimeScale,
				ptTracer) != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4UTILITY_GETHINTTIMESCALE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaAudioTrack ->
					unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}

			sprintf (pClockRate, "%lu", ulHintTimeScale);
		}

		if (pbSDP -> append ("m=audio 0 RTP/AVP ") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (ulPayloadNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append ("b=AS:") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (
			(unsigned long) (ulAvgBitRate / 1000)) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append ("a=rtpmap:") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (ulPayloadNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (cCodecUsed == MP4Atom:: MP4F_CODEC_AMRNB)
		{
			if (pbSDP -> append (" AMR/") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}
		else	// if (cCodecUsed == MP4Atom:: MP4F_CODEC_AMRWB)
		{
			if (pbSDP -> append (" AMR-WB/") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);

				if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
				{
					Error err = MP4FileErrors (__FILE__, __LINE__,
						MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
					ptTracer -> trace (Tracer:: TRACER_LERRR,
						(const char *) err, __FILE__, __LINE__);
				}

				return err;
			}
		}

		if (pbSDP -> append (pClockRate) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append ("a=fmtp:") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (ulPayloadNumber) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (" octet-align=1") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append ("a=control:trackID/") != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (ulTrackIdentifier) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}

		if (pbSDP -> append (MP4F_NEWLINE) != errNoError)
		{
			Error err = ToolsErrors (__FILE__, __LINE__,
				TOOLS_BUFFER_APPEND_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_CODECUNKNOWN,
			1, ulTrackIdentifier);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaMediaAudioTrack -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}
*/


Error MP4Utility:: getNewTrackIdentifier (
	MP4RootAtom_p pmaRootAtom, unsigned long *pulNewTrackIdentifier,
	Tracer_p ptTracer)

{

	MP4MvhdAtom_p				pmhaMoovHeaderAtom;
	MP4Atom_p					pmaAtom;


	if (pmaRootAtom -> lockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_LOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}

	// this algorithm should be reviewed according the MP4IP algorithm
	if (pmaRootAtom -> searchAtom ("moov:0:mvhd:0", true,
		&pmaAtom) != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4FILE_SEARCHATOM_FAILED, 1, "moov:0:mvhd:0");
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}
	else
		pmhaMoovHeaderAtom		= (MP4MvhdAtom_p) pmaAtom;

	if (pmhaMoovHeaderAtom -> getNextTrackID (pulNewTrackIdentifier) !=
		errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4PROPERTY_GETVALUE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	// this algorithm should be reviewed according the MP4IP algorithm
	if (*pulNewTrackIdentifier < 0xFFF)
	{
		if (pmhaMoovHeaderAtom -> setNextTrackID (*pulNewTrackIdentifier + 1) !=
			errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4PROPERTY_SETVALUE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);

			if (pmaRootAtom -> unLockMP4File () != errNoError)
			{
				Error err = MP4FileErrors (__FILE__, __LINE__,
					MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
				ptTracer -> trace (Tracer:: TRACER_LERRR,
					(const char *) err, __FILE__, __LINE__);
			}

			return err;
		}
	}
	else
	{
		// for i = 0 i < 0xFFFF find the first free track id
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_NOTIMPLEMENTEDYET);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		if (pmaRootAtom -> unLockMP4File () != errNoError)
		{
			Error err = MP4FileErrors (__FILE__, __LINE__,
				MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
			ptTracer -> trace (Tracer:: TRACER_LERRR,
				(const char *) err, __FILE__, __LINE__);
		}

		return err;
	}

	if (pmaRootAtom -> unLockMP4File () != errNoError)
	{
		Error err = MP4FileErrors (__FILE__, __LINE__,
			MP4F_MP4ATOM_UNLOCKMP4FILE_FAILED);
		ptTracer -> trace (Tracer:: TRACER_LERRR,
			(const char *) err, __FILE__, __LINE__);

		return err;
	}


	return errNoError;
}


Error MP4Utility:: getCodecName (
	MP4Atom:: MP4Codec_t cCodec,
	char pCodecName [MP4F_MAXCODECUSEDLENGTH])

{

	switch (cCodec)
	{
		case MP4Atom:: MP4F_CODEC_MPEG4:
			strcpy (pCodecName, "MPEG4");

			break;
		case MP4Atom:: MP4F_CODEC_AAC:
			strcpy (pCodecName, "AAC");

			break;
		case MP4Atom:: MP4F_CODEC_H263:
			strcpy (pCodecName, "H.263");

			break;
		case MP4Atom:: MP4F_CODEC_AMRNB:
			strcpy (pCodecName, "AMR-NB");

			break;
		case MP4Atom:: MP4F_CODEC_AMRWB:
			strcpy (pCodecName, "AMR-WB");

			break;
		case MP4Atom:: MP4F_CODEC_UNKNOWN:
		default:
			strcpy (pCodecName, "UNKNOWN");

			break;
	}


	return errNoError;
}

