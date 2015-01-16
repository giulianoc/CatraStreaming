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

#ifndef SDPMediaForRTSP_h 
	#define SDPMediaForRTSP_h

	#include "SDPMedia.h"


	#define SDPFORRTSP_MAXCONTROLLENGTH				SDP_ATTRIBUTEVALUELENGTH
	#define SDPFORRTSP_CONTROLATTRIBUTENAME			"control"
	#define SDPFORRTSP_RANGEATTRIBUTENAME			"range"


	typedef class SDPMediaForRTSP: public SDPMedia {

		protected:
			SDPMediaForRTSP (const SDPMediaForRTSP &);

			SDPMediaForRTSP &operator = (const SDPMediaForRTSP &);


			SDPAttribute_p			_psaControlSDPAttribute;

		public:
			SDPMediaForRTSP (void);

			~SDPMediaForRTSP (void);

			virtual Error init (SDPMediaType_t smtSDPMediaType,
				Tracer_p ptTracer);

			virtual Error init (Buffer_p pbSDPMedia, Tracer_p ptTracer);

			virtual Error addAttribute (SDPAttribute_p psaSDPAttribute);

			/**
				This method returns information about the media.
				Every parameter could assume a NULL value and, in this
				case, the method will not use it
			*/
			Error getMediaInfo (
				SDPMediaType_p pmtMediaType,
				char *pMediaControl,
				unsigned long *pulMediaPayloadType,
				unsigned long *pulMediaPort,
				char *pMediaEncodingName,
				char *pMediaClockRate,
				char *pMediaEncodingParameters,
				unsigned long *pulMediaBandwidthInKbps);

		} SDPMediaForRTSP_t, *SDPMediaForRTSP_p;

#endif

