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


#ifndef MP4Atom_h
	#define MP4Atom_h

	#include "Tracer.h"
	#include "FileReader.h"
	#include "MP4UInteger8BitsProperty.h"
	#include "MP4UInteger16BitsProperty.h"
	#include "MP4UInteger24BitsProperty.h"
	#include "MP4UInteger32BitsProperty.h"
	#include "MP4UInteger64BitsProperty.h"
	#include "MP4Float16BitsProperty.h"
	#include "MP4Float32BitsProperty.h"
	#include "MP4StringNullTerminatedProperty.h"
	#include "MP4StringFixedProperty.h"
	#include "MP4StringLastProperty.h"
	#include "MP4BytesProperty.h"
	#include "MP4FileErrors.h"
	#include <vector>


	#define MP4_MAXATOMTYPELENGTH				4 + 1

	// We consider the descriptor property like an atom. In this case a string
	// representing the type of an atom could contain also a description
	// property and in this case 4 chars is not enougth 
	#define MP4_MAXTYPELENGTH					256 + 1

	// MP4 Known track type names
	// #define MP4_OD_TRACK_TYPE					"odsm"
	// #define MP4_SCENE_TRACK_TYPE				"sdsm"
	#define MP4_AUDIO_TRACK_TYPE				"soun"
	#define MP4_VIDEO_TRACK_TYPE				"vide"
	#define MP4_HINT_TRACK_TYPE					"hint"

	// MP4 Systems Track
	// #define MP4_CLOCK_TRACK_TYPE				"crsm"
	// #define MP4_MPEG7_TRACK_TYPE				"m7sm"
	// #define MP4_OCI_TRACK_TYPE					"ocsm"
	// #define MP4_IPMP_TRACK_TYPE					"ipsm"
	// #define MP4_MPEGJ_TRACK_TYPE				"mjsm"

	// MP4 Audio track types
	// #define MP4_INVALID_AUDIO_TYPE				0x00
	// #define MP4_MPEG1_AUDIO_TYPE				0x6B
	// #define MP4_MPEG2_AUDIO_TYPE				0x69
	// #define MP4_MP3_AUDIO_TYPE					MP4_MPEG2_AUDIO_TYPE
	#define MP4_MPEG2_AAC_MAIN_AUDIO_TYPE		0x66
	#define MP4_MPEG2_AAC_LC_AUDIO_TYPE			0x67
	#define MP4_MPEG2_AAC_SSR_AUDIO_TYPE		0x68
	// #define MP4_MPEG2_AAC_AUDIO_TYPE			MP4_MPEG2_AAC_MAIN_AUDIO_TYPE
	#define MP4_MPEG4_AUDIO_TYPE				0x40
	// #define MP4_PRIVATE_AUDIO_TYPE				0xC0
	// #define MP4_PCM16_AUDIO_TYPE				0xE0
	// #define MP4_VORBIS_AUDIO_TYPE			0xE1
	// #define MP4_AC3_AUDIO_TYPE				0xE2

	/* MP4 MPEG-4 Audio types from 14496-3 Table 1.5.1 */
	// #define MP4_MPEG4_INVALID_AUDIO_TYPE		0
	#define MP4_MPEG4_AAC_MAIN_AUDIO_TYPE		1
	#define MP4_MPEG4_AAC_LC_AUDIO_TYPE			2
	#define MP4_MPEG4_AAC_SSR_AUDIO_TYPE		3
	#define MP4_MPEG4_AAC_LTP_AUDIO_TYPE		4
	#define MP4_MPEG4_AAC_SCALABLE_AUDIO_TYPE	6
	#define MP4_MPEG4_CELP_AUDIO_TYPE			8
	// #define MP4_MPEG4_HVXC_AUDIO_TYPE			9
	// #define MP4_MPEG4_TTSI_AUDIO_TYPE			12
	// #define MP4_MPEG4_MAIN_SYNTHETIC_AUDIO_TYPE	13
	// #define MP4_MPEG4_WAVETABLE_AUDIO_TYPE		14
	// #define MP4_MPEG4_MIDI_AUDIO_TYPE			15
	// #define MP4_MPEG4_ALGORITHMIC_FX_AUDIO_TYPE	16

	/* MP4 Audio type utilities following common usage */
	// #define MP4_IS_MP3_AUDIO_TYPE(type) \
		// ((type) == MP4_MPEG1_AUDIO_TYPE || (type) == MP4_MPEG2_AUDIO_TYPE) 

	#define MP4_IS_MPEG2_AAC_AUDIO_TYPE(type) \
		(((type) >= MP4_MPEG2_AAC_MAIN_AUDIO_TYPE \
		&& (type) <= MP4_MPEG2_AAC_SSR_AUDIO_TYPE))

	#define MP4_IS_MPEG4_AAC_AUDIO_TYPE(mpeg4Type) \
		(((mpeg4Type) >= MP4_MPEG4_AAC_MAIN_AUDIO_TYPE \
		&& (mpeg4Type) <= MP4_MPEG4_AAC_LTP_AUDIO_TYPE) \
		|| (mpeg4Type) == MP4_MPEG4_AAC_SCALABLE_AUDIO_TYPE)

	#define MP4_IS_AAC_AUDIO_TYPE(type) \
		(MP4_IS_MPEG2_AAC_AUDIO_TYPE(type) \
		|| (type) == MP4_MPEG4_AUDIO_TYPE)

	// MP4 Video track types
	// #define MP4_INVALID_VIDEO_TYPE				0x00
	// #define MP4_MPEG1_VIDEO_TYPE				0x6A
	// #define MP4_MPEG2_SIMPLE_VIDEO_TYPE			0x60
	// #define MP4_MPEG2_MAIN_VIDEO_TYPE			0x61
	// #define MP4_MPEG2_SNR_VIDEO_TYPE			0x62
	// #define MP4_MPEG2_SPATIAL_VIDEO_TYPE		0x63
	// #define MP4_MPEG2_HIGH_VIDEO_TYPE			0x64
	// #define MP4_MPEG2_442_VIDEO_TYPE			0x65
	// #define MP4_MPEG2_VIDEO_TYPE				MP4_MPEG2_MAIN_VIDEO_TYPE
	#define MP4_MPEG4_VIDEO_TYPE				0x20
	// #define MP4_JPEG_VIDEO_TYPE					0x6C
	// #define MP4_PRIVATE_VIDEO_TYPE				0xC1

	/* MP4 Video type utilities */
	// #define MP4_IS_MPEG1_VIDEO_TYPE(type) \
	// 	((type) == MP4_MPEG1_VIDEO_TYPE)

	// #define MP4_IS_MPEG2_VIDEO_TYPE(type) \
	// 	(((type) >= MP4_MPEG2_SIMPLE_VIDEO_TYPE \
	// 	&& (type) <= MP4_MPEG2_442_VIDEO_TYPE) \
	// 	|| MP4_IS_MPEG1_VIDEO_TYPE(type))

	// #define MP4_IS_MPEG4_VIDEO_TYPE(type) \
	// 	((type) == MP4_MPEG4_VIDEO_TYPE)


	// TAGs
	#define MP4_OBJECTDESCR_TAG						0x01
	#define MP4_INITIALOBJECTDESCR_TAG				0x02
	#define MP4_ESDESCR_TAG							0x03
	#define MP4_DECODERCONFIGDESCR_TAG				0x04
	#define MP4_DECODERSPECIFICDESCR_TAG			0x05
	#define MP4_SYNCLAYERCONFIGDESCR_TAG			0x06
	#define MP4_CONTENTIDENTIFICATIONDESCR_TAG		0x07
	#define MP4_MORECONTENTIDENTIFICATIONDESCR_TAG 	0x08
	#define MP4_IPIDESCRPOINTER_TAG			 		0x09
	#define MP4_IPMPDESCRPOINTER_TAG		 		0x0A
	#define MP4_IPMPDESCR_TAG			 			0x0B
	#define MP4_QOSDESCR_TAG			 			0x0C
	#define MP4_REGISTRATIONDESCR_TAG	 			0x0D
	#define MP4_ESIDINCDESCR_TAG					0x0E
	#define MP4_ESIDREFDESCR_TAG					0x0F
	#define MP4_IODESCR_TAG							0x10
	#define MP4_ODESCR_TAG							0x11
	#define MP4_EXTENDEDPROFILELEVELDESCR_TAG	 	0x13

	// START OCI DESCR TAG
	#define MP4_OCISTARTDESCR_TAG					0x40
	#define MP4_CONTENTCLASSIFICATIONDESCR_TAG		0x40
	#define MP4_KEYWORDDESCR_TAG					0x41
	#define MP4_RATINGDESCR_TAG						0x42
	#define MP4_LANGUAGEDESCR_TAG					0x43
	#define MP4_SHORTTEXTUALDESCR_TAG				0x44
	#define MP4_EXPANDEDTEXTUALDESCR_TAG			0x45
	#define MP4_CONTENTCREATORNAMEDESCR_TAG			0x46
	#define MP4_CONTENTCREATIONNAMEDESCR_TAG		0x47
	#define MP4_OCICREATORNAMEDESCR_TAG				0x48
	#define MP4_OCICREATIONDATEDESCR_TAG			0x49
	#define MP4_SMPTECAMERAPOSITIONDESCR_TAG		0x4A
	#define MP4_OCIENDDESCR_TAG						0x5F
	// End OCI DESCR TAG

	#define MP4_EXTENDEDSTARTDESCR_TAGS				0x80
	#define MP4_EXTENDEDENDDESCR_TAGS				0xFE


	// TYPEs
	#define MP4_OBJECTDESCR_TYPE					"ObjectDescr"
	#define MP4_INITIALOBJECTDESCR_TYPE				"InitialObjectDescr"
	#define MP4_ESDESCR_TYPE						"ESDescr"
	#define MP4_DECODERCONFIGDESCR_TYPE				"DecoderConfigDescr"
	#define MP4_DECODERSPECIFICDESCR_TYPE			"DecoderSpecificDescr"
	#define MP4_SYNCLAYERCONFIGDESCR_TYPE			"SyncLayerConfigDescr"
	#define MP4_CONTENTIDENTIFICATIONDESCR_TYPE		"ContentIdentificationDescr"
	#define MP4_MORECONTENTIDENTIFICATIONDESCR_TYPE	"MoreContentIdentificationDescr"
	#define MP4_IPIDESCRPOINTER_TYPE		 		"IPIPointerDescr"
	#define MP4_IPMPDESCRPOINTER_TYPE		 		"IPMPPointerDescr"
	#define MP4_IPMPDESCR_TYPE			 			"IPMPDescr"
	#define MP4_QOSDESCR_TYPE			 			"QoSDescr"
	#define MP4_REGISTRATIONDESCR_TYPE	 			"RegistrationDescr"
	#define MP4_ESIDINCDESCR_TYPE					"EsIdIncDescr"
	#define MP4_ESIDREFDESCR_TYPE					"ESIDRefDescr"
	#define MP4_IODESCR_TYPE						"InitialObjectDescr"
	#define MP4_ODESCR_TYPE							"MP4ODDescr"
	#define MP4_EXTENDEDPROFILELEVELDESCR_TYPE	 	"ExtendedProfileLevelDescr"

	#define MP4_UNKNOWNDESCR_TYPE					"UnknownDescr"

	// START OCI DESCR TYPE
	#define MP4_CONTENTCLASSIFICATIONDESCR_TYPE		"ContentClassificationDescr"
	#define MP4_KEYWORDDESCR_TYPE					"KeyWordDescr"
	#define MP4_RATINGDESCR_TYPE					"RatingDescr"
	#define MP4_LANGUAGEDESCR_TYPE					"LanguageDescr"
	#define MP4_SHORTTEXTUALDESCR_TYPE				"ShortTextualDescr"
	#define MP4_EXPANDEDTEXTUALDESCR_TYPE			"ExpandedTextualDescr"
	#define MP4_CONTENTCREATORNAMEDESCR_TYPE		"ContentCreatorNameDescr"
	#define MP4_CONTENTCREATIONNAMEDESCR_TYPE		"ContentCreationNameDescr"
	#define MP4_OCICREATORNAMEDESCR_TYPE			"OCICreatorNameDescr"
	#define MP4_OCICREATIONDATEDESCR_TYPE			"OCICreationDateDescr"
	#define MP4_SMPTECAMERAPOSITIONDESCR_TYPE		"SmpteCameraPositionDescr"
	// End OCI DESCR TYPE

	#define MP4_UNKNOWNOCIDESCR_TYPE				"UnknownOCIDescr"

	#define MP4_EXTENDEDDESCR_TYPE					"ExtensionDescr"



	typedef class MP4Atom {

		public:
			typedef enum Standard {
				MP4F_ISMA,
				MP4F_3GPP
			} Standard_t, *Standard_p;

			typedef enum MP4Codec
			{
				MP4F_CODEC_MPEG4,
				MP4F_CODEC_AAC,
				MP4F_CODEC_H263,
				MP4F_CODEC_AMRNB,
				MP4F_CODEC_AMRWB,
				MP4F_CODEC_UNKNOWN
			} MP4Codec_t, *MP4Codec_p;

		protected:
			typedef struct ChildrenAtomInfo {
				char				_pType [MP4_MAXTYPELENGTH];
				Boolean_t			_bMandatory;
				Boolean_t			_bOnlyOne;
			} ChildrenAtomInfo_t, *ChildrenAtomInfo_p;

		protected:
			FileReader_p						_pfFile;
			#ifdef WIN32
				__int64							_ullAtomStartOffset;
				__int64							_ullAtomSize;
			#else
				unsigned long long				_ullAtomStartOffset;
				unsigned long long				_ullAtomSize;
			#endif

			// if _bUse64Bits is true, the atom header should be:
			//		size		32 bits		(1)
			//		size		64 bits
			//		type		4 bytes
			//			===> header size = 16 bytes
			// if _bUse64Bits is false, the atom header should be:
			//		size		32 bits
			//		type		4 bytes
			//			===> header size = 8 bytes
			unsigned long					_ulHeaderSize;
			long							_lAtomLevel;
			Boolean_t						_bUse64Bits;
			Boolean_t						_bUseMP4ConsistencyCheck;
			PMutex_p						_pmtMP4File;
			Tracer_p						_ptTracer;
			MP4Atom							*_pmaParentAtom;
			MP4Atom:: Standard_t			_sStandard;


			std:: vector<ChildrenAtomInfo_t>	_vChildrensAtomsInfo;
			std:: vector<MP4Atom *>			_vChildrenAtoms;

			MP4Atom (const MP4Atom &);

			MP4Atom &operator = (const MP4Atom &);

			virtual Error prepareChildrensAtomsInfo (void)	= 0;

			virtual Error createProperties (
				unsigned long *pulPropertiesSize) = 0;

			virtual Error readProperties (
				unsigned long *pulPropertiesSize) = 0;

			virtual Error writeProperties (int iFileDescriptor) = 0;

			virtual Error getPropertiesSize (
				unsigned long *pulPropertiesSize) = 0;

			#ifdef WIN32
				virtual Error createChildAtoms (
					__int64 *pullChildrenSize);
			#else
				virtual Error createChildAtoms (
					unsigned long long *pullChildrenSize);
			#endif

			virtual Error readChildAtoms (void);

			Error consistencyCheck (const char *pTypeToInsert);

			Error consistencyCheck (void);

			/**
				lIncrement - could be also negative
			*/
			#ifdef WIN32
				Error changeSizeManagement (__int64 llIncrement);
			#else
				Error changeSizeManagement (long long llIncrement);
			#endif

			virtual Error appendPropertiesDump (Buffer_p pbBuffer) = 0;

			virtual Error printPropertiesOnStdOutput (void) = 0;

			static Error buildAtom (const char *pType, MP4Atom **pmaMP4Atom,
				Boolean_t bUseMP4ConsistencyCheck, Tracer_p ptTracer);

			static Error buildDescriptor (unsigned long ulTag,
				MP4Atom **pmaMP4Descriptor, Boolean_t bUseMP4ConsistencyCheck,
				Tracer_p ptTracer);

			static Error getDescriptorTag (const char *pType,
				unsigned long *pulTag, Tracer_p ptTracer);

			static Error isDescriptor (const char *pType,
				Boolean_p pbIsDescriptor);

		public:
			/**
				Costruttore.
			*/
			MP4Atom ();

			/**
				Distruttore.
			*/
			virtual ~MP4Atom ();

			/**
				The pointer to the file in pfFile parameter shall be initialized
				to the first property of the atom
			*/
			#ifdef WIN32
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					__int64 ullAtomStartOffset, __int64 ullAtomSize,
					unsigned long ulHeaderSize,
					PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
					Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#else
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					unsigned long long ullAtomStartOffset,
					unsigned long long ullAtomSize,
					unsigned long ulHeaderSize,
					PMutex_p pmtMP4File, long lAtomLevel, Boolean_t bUse64Bits,
					Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer);
			#endif

			#ifdef WIN32
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					__int64 ullAtomStartOffset, PMutex_p pmtMP4File,
					long lAtomLevel,
					Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer,
					__int64 *pullAtomSize);
			#else
				virtual Error init (MP4Atom *pmaParentAtom,
					FileReader_p pfFile,
					unsigned long long ullAtomStartOffset, PMutex_p pmtMP4File,
					long lAtomLevel,
					Boolean_t bUse64Bits, Boolean_t bUseMP4ConsistencyCheck,
					MP4Atom:: Standard_t sStandard, Tracer_p ptTracer,
					unsigned long long *pullAtomSize);
			#endif

			virtual Error finish (void);

			virtual Error write (int iFileDescriptor);

			virtual Error getType (char *pType) = 0;

			Error getParent (MP4Atom **pParent);

			Error getChild (unsigned long ulChildIndex, MP4Atom **pParent);

			Error getChildIndex (MP4Atom *pmaMP4Atom, long *plChildIndex);

			Error getChildrenNumber (long *plChildrenNumber);

			#ifdef WIN32
				Error getStartOffset (__int64 *pullStartOffset);
			#else
				Error getStartOffset (unsigned long long *pullStartOffset);
			#endif

			/**
				lIncrementStartOffset - could be also negative
			*/
			#ifdef WIN32
				Error incrementStartOffset (__int64 llIncrementStartOffset);
			#else
				Error incrementStartOffset (long long llIncrementStartOffset);
			#endif

			#ifdef WIN32
				Error getSize (__int64 *pullSize,
					unsigned long *pulHeaderSize);
			#else
				Error getSize (unsigned long long *pullSize,
					unsigned long *pulHeaderSize);
			#endif

			Error getStandard (Standard_p psStandard);

			Error addChild (const char *pType, MP4Atom **pmaMp4Atom,
				long lIndexToInsert);

			Error deleteChild (MP4Atom *pmaMp4AtomToDelete);

			Error addMdatChild (MP4Atom **pmaMp4Atom);

			/**
				pAtomPath can be: <ChildType>:<InstanceIndex>:...
					...:<SearchedChildType>:<InstanceIndex>
				InstanceIndex may start from 0
			*/
			Error searchAtom (const char *pAtomPath,
				Boolean_t bIsSearchErrorTraceEnabled, MP4Atom **pmp4Atom);

			Error lockMP4File (void);

			Error unLockMP4File (void);

			virtual Error appendDump (Buffer_p pbBuffer);

			virtual Error printOnStdOutput (void);

	} MP4Atom_t, *MP4Atom_p;

#endif

