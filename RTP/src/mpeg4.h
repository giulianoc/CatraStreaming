
#ifndef mpeg4_h
#define mpeg4_h

/*
#define MP4AV_MPEG4_SYNC		0x000001
#define MP4AV_MPEG4_VOSH_START	0xB0
#define MP4AV_MPEG4_VOL_START	0x20
#define MP4AV_MPEG4_GOV_START	0xB3
#define MP4AV_MPEG4_VO_START	0xB5
#define MP4AV_MPEG4_VOP_START	0xB6

#ifdef __cplusplus
extern "C" {
#endif


bool MP4AV_Mpeg4ParseVosh(
	u_int8_t* pVoshBuf, 
	u_int32_t voshSize,
	u_int8_t* pProfileLevel);

bool MP4AV_Mpeg4CreateVosh(
	u_int8_t** ppBytes,
	u_int32_t* pNumBytes,
	u_int8_t profileLevel);

bool MP4AV_Mpeg4CreateVo(
	u_int8_t** ppBytes,
	u_int32_t* pNumBytes,
	u_int8_t objectId);

bool MP4AV_Mpeg4ParseVol(
	u_int8_t* pVolBuf, 
	u_int32_t volSize,
	u_int8_t* pTimeBits, 
	u_int16_t* pTimeTicks, 
	u_int16_t* pFrameDuration, 
	u_int16_t* pFrameWidth, 
	u_int16_t* pFrameHeight);

bool MP4AV_Mpeg4CreateVol(
	u_int8_t** ppBytes,
	u_int32_t* pNumBytes,
	u_int8_t profile,
	float frameRate,
	bool shortTime,
	bool variableRate,
	u_int16_t width,
	u_int16_t height,
	u_int8_t quantType,
	u_int8_t* pTimeBits DEFAULT_PARM(NULL));

bool MP4AV_Mpeg4ParseGov(
	u_int8_t* pGovBuf, 
	u_int32_t govSize,
	u_int8_t* pHours, 
	u_int8_t* pMinutes, 
	u_int8_t* pSeconds);
*/

bool MP4AV_Mpeg4ParseVop(
	unsigned char* pVopBuf, 
	unsigned long vopSize,
	unsigned char* pVopType, 
	unsigned char timeBits, 
	unsigned long timeTicks, 
	unsigned long* pVopTimeIncrement);


unsigned char MP4AV_Mpeg4VideoToSystemsProfileLevel(
	unsigned char videoProfileLevel);

unsigned char MP4AV_Mpeg4GetVopType(
	unsigned char* pVopBuf, 
	unsigned long vopSize);

/* #ifdef __cplusplus
}
#endif
*/
#endif /* __MP4AV_MPEG4_INCLUDED__ */

