
/*
 * AAC Config in ES:
 *
 * AudioObjectType 			5 bits
 * samplingFrequencyIndex 	4 bits
 * if (samplingFrequencyIndex == 0xF)
 *	samplingFrequency	24 bits 
 * channelConfiguration 	4 bits
 * GA_SpecificConfig
 * 	FrameLengthFlag 		1 bit 1024 or 960
 * 	DependsOnCoreCoder		1 bit (always 0)
 * 	ExtensionFlag 			1 bit (always 0)
 */

// START: AdtsSamplingRates definition take from adts.C

#define NUM_AAC_SAMPLING_RATES	16

static unsigned long AdtsSamplingRates[NUM_AAC_SAMPLING_RATES] = {
	96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 
	16000, 12000, 11025, 8000, 7350, 0, 0, 0
};

// END

unsigned char MP4AV_AacConfigGetSamplingRateIndex(unsigned char * pConfig)
{
	return ((pConfig[0] << 1) | (pConfig[1] >> 7)) & 0xF;
}

unsigned long MP4AV_AacConfigGetSamplingRate(unsigned char *pConfig)
{
	unsigned char index =
		MP4AV_AacConfigGetSamplingRateIndex(pConfig);

	if (index == 0xF) {
		return (pConfig[1] & 0x7F) << 17
			| pConfig[2] << 9
			| pConfig[3] << 1
			| (pConfig[4] >> 7);
	}
	return AdtsSamplingRates[index];
}

/*
extern "C" u_int16_t MP4AV_AacConfigGetSamplingWindow(u_int8_t* pConfig)
{
	u_int8_t adjust = 0;

	if (MP4AV_AacConfigGetSamplingRateIndex(pConfig) == 0xF) {
		adjust = 3;
	}

	if ((pConfig[1 + adjust] >> 2) & 0x1) {
		return 960;
	}
	return 1024;
}
*/

unsigned char MP4AV_AacConfigGetChannels(unsigned char * pConfig)
{
	unsigned char adjust = 0;

	if (MP4AV_AacConfigGetSamplingRateIndex(pConfig) == 0xF) {
		adjust = 3;
	}
	return (pConfig[1 + adjust] >> 3) & 0xF;
}

/*
extern "C" bool MP4AV_AacGetConfigurationFromAdts(
	u_int8_t** ppConfig,
	u_int32_t* pConfigLength,
	u_int8_t* pHdr)
{
	return MP4AV_AacGetConfiguration(
		ppConfig,
		pConfigLength,
		MP4AV_AdtsGetProfile(pHdr),
		MP4AV_AdtsGetSamplingRate(pHdr),
		MP4AV_AdtsGetChannels(pHdr));
}

extern "C" bool MP4AV_AacGetConfiguration(
	u_int8_t** ppConfig,
	u_int32_t* pConfigLength,
	u_int8_t profile,
	u_int32_t samplingRate,
	u_int8_t channels)
{
	// create the appropriate decoder config

	u_int8_t* pConfig = (u_int8_t*)malloc(2);

	if (pConfig == NULL) {
		return false;
	}

	u_int8_t samplingRateIndex = 
		MP4AV_AdtsFindSamplingRateIndex(samplingRate);

	pConfig[0] =
		((profile + 1) << 3) | ((samplingRateIndex & 0xe) >> 1);
	pConfig[1] =
		((samplingRateIndex & 0x1) << 7) | (channels << 3);

	// LATER this option is not currently used in MPEG4IP
	// if (samplesPerFrame == 960) {
	// 	pConfig[1] |= (1 << 2);
	// }

	*ppConfig = pConfig;
	*pConfigLength = 2;

	return true;
}
*/

