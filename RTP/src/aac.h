
#ifndef aac_h
#define aac_h

/*
#define MP4AV_AAC_MAIN_PROFILE	0
#define MP4AV_AAC_LC_PROFILE	1
#define MP4AV_AAC_SSR_PROFILE	2
#define MP4AV_AAC_LTP_PROFILE	3

#ifdef __cplusplus
extern "C" {
#endif
*/

unsigned char MP4AV_AacConfigGetSamplingRateIndex(
	unsigned char* pConfig);

unsigned long MP4AV_AacConfigGetSamplingRate(
	unsigned char *pConfig);

/*
u_int16_t MP4AV_AacConfigGetSamplingWindow(
	u_int8_t* pConfig);
*/

unsigned char MP4AV_AacConfigGetChannels(
	unsigned char * pConfig);

/*
bool MP4AV_AacGetConfigurationFromAdts(
	u_int8_t** ppConfig,
	u_int32_t* pConfigLength,
	u_int8_t* pAdtsHdr);

bool MP4AV_AacGetConfiguration(
	u_int8_t** ppConfig,
	u_int32_t* pConfigLength,
	u_int8_t profile,
	u_int32_t samplingRate,
	u_int8_t channels);

#ifdef __cplusplus
}
#endif
*/

#endif

