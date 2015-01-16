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

#ifndef SDPAttribute_h 
	#define SDPAttribute_h


	#define SDP_ATTRIBUTENAMELENGTH			128 + 1
	#define SDP_ATTRIBUTEVALUELENGTH		1024 + 1

	#define SDP_PAYLOADTYPELENGTH			128 + 1
	#define SDP_ENCODINGNAMELENGTH			128 + 1
	#define SDP_CLOCKRATELENGTH				128 + 1
	#define SDP_ENCODINGPARAMETERSLENGTH	128 + 1

	#define SDP_FORMATLENGTH				128 + 1
	#define SDP_FORMATPARAMETERSLENGTH		256 + 1

	#define SDP_RTPMAPATTRIBUTENAME			"rtpmap"
	#define SDP_FMTPATTRIBUTENAME			"fmtp"
	#define SDP_CONTROLATTRIBUTENAME		"control"

	#define SDP_VERSION						'v'
	#define SDP_ORIGIN						'o'
	#define SDP_SESSIONNAME					's'
	#define SDP_INFORMATION					'i'
	#define SDP_URI							'u'
	#define SDP_EMAILADDRESS				'e'
	#define SDP_PHONENUMBER					'p'
	#define SDP_CONNECTIONDATA				'c'
	#define SDP_BANDWIDTH					'b'
	#define SDP_TIMES						't'
	#define SDP_ATTRIBUTE					'a'
	#define SDP_MEDIA						'm'

	#define SDP_NEWLINE						"\r\n"


	typedef struct SDPAttribute {
		char			_pName [SDP_ATTRIBUTENAMELENGTH];
		char			_pValue [SDP_ATTRIBUTEVALUELENGTH];

		virtual Error appendToBuffer (Buffer_p pbSDP)
		{
			if (pbSDP -> append (SDP_ATTRIBUTE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append ("=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_pName) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (":") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_pValue) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (SDP_NEWLINE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}


			return errNoError;
		} ;

	} SDPAttribute_t, *SDPAttribute_p;

	typedef struct SDPRtpMapAttribute: public SDPAttribute {
		// a=rtpmap:<payload type> <encoding name>/<clock rate>[/<encoding parameters>]
		char			_pPayloadType [SDP_PAYLOADTYPELENGTH];
		char			_pEncodingName [SDP_ENCODINGNAMELENGTH];
		char			_pClockRate [SDP_CLOCKRATELENGTH];
		char			_pEncodingParameters [SDP_ENCODINGPARAMETERSLENGTH];

		virtual Error appendToBuffer (Buffer_p pbSDP)
		{
			if (pbSDP -> append (SDP_ATTRIBUTE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append ("=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_pName) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (":") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_pPayloadType) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (" ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_pEncodingName) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append ("/") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_pClockRate) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (strcmp (_pEncodingParameters, ""))
			{
				if (pbSDP -> append ("/") != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);

					return err;
				}

				if (pbSDP -> append (_pEncodingParameters) != errNoError)
				{
					Error err = ToolsErrors (__FILE__, __LINE__,
						TOOLS_BUFFER_APPEND_FAILED);

					return err;
				}
			}

			if (pbSDP -> append (SDP_NEWLINE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}


			return errNoError;
		} ;

	} SDPRtpMapAttribute_t, *SDPRtpMapAttribute_p;

	typedef struct SDPFmtpAttribute: public SDPAttribute {
		// a=fmtp:<format> <format specific parameters>
		char			_pFormat [SDP_FORMATLENGTH];
		char			_pFormatParameters [SDP_FORMATPARAMETERSLENGTH];

		virtual Error appendToBuffer (Buffer_p pbSDP)
		{
			if (pbSDP -> append (SDP_ATTRIBUTE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append ("=") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_pName) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (":") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_pFormat) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (" ") != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (_pFormatParameters) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}

			if (pbSDP -> append (SDP_NEWLINE) != errNoError)
			{
				Error err = ToolsErrors (__FILE__, __LINE__,
					TOOLS_BUFFER_APPEND_FAILED);

				return err;
			}


			return errNoError;
		} ;

	} SDPFmtpAttribute_t, *SDPFmtpAttribute_p;

#endif

