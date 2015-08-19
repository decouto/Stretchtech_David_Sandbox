// ApControl.h -- Audio Processor Control Functions Header
//

#ifndef _APCONTROL_H_
#define _APCONTROL_H_

#include <stdint.h>

//#define ENABLE_DUMPS
//#define ENABLE_LOADS

#define AP_I2C_ADDR				0x40
#define AP_I2C_HANDLE			hi2c1

#define AP_CHANNELS				0x08
#define AP_BQPERCHANNEL		0x0a
#define AP_COEFPERBQ			0x05
#define AP_REGFILESZ			0x90

#define AP_BACOEF_BASE		0x000
#define AP_PRESCALE_BASE	0x190
#define	AP_POSTSCALE_BASE	0x198
#define AP_MIXCOEF_BASE		0x1a0

#ifdef __cplusplus 
extern "C" {
#endif


typedef struct regRange {
	uint8_t Bgn;
	uint8_t End;
} regRange_t;

typedef enum eApRegRange {
	erConfig 				= 0,
	erMasterMute,
	erMasterVolume,
	erChnxVolume,
	erChnxMuteVolBPTrim,
	erChnxyInputMap,
	erAutoMode,
	erPresetEq,
	erEqBandx,
	erChnLoopCtl,
	erBypassCtl,
	erBaseTrebleCtl,
	erChLimitSels,
	erLim12Cfgs,
	erPwmTimings,
	erI2sOutMaps,
	erCoefBlk,
	erExtLimitDrc,
	erFineVol,
	erPcmRecomb,
	erExtLimitTbl,
	erRmsCtlBlk,
	erPllCtlBlk,
	erPllSts,
	erExtBiquadBlk,
	erRmsStatusBlk,
	erDpt,
	erCfr129,
	erPopSuppresTimes,
} eApRegRange_t;

typedef struct rawCoef {
	uint8_t coefHi;
	uint8_t coefMi;
	uint8_t coefLo;
} rawCoef_t;

inline int32_t rawCoefToInt(rawCoef_t *p) {
	return ((p->coefHi<<16)||(p->coefMi<<8)||(p->coefLo<<0));
}

inline rawCoef_t rawCoefFromInt(int32_t coef) {
	rawCoef_t x;
	x.coefHi = (coef>>16)&0xff;
	x.coefMi = (coef>>8)&0xff;
	x.coefLo = coef&0xff;
	return x;
}
typedef struct bqCoefBlk {
	rawCoef_t BqB1;
	rawCoef_t BqB2;
	rawCoef_t BqA1;
	rawCoef_t BqA2;
	rawCoef_t BqB0;
} bqCoefBlk_t;

#ifdef INSTANTIATE_AP_DATA

uint8_t ApRegFile[AP_REGFILESZ] = {
	0
};

regRange_t ApRegRanges[] = {
	{0x00,0x08},		// erConfig
	{0x09,0x09},		// erMasterMute
	{0x0a,0x0a},		// erMasterVolume
	{0x0b,0x12},		// erChanVolume
	{0x13,0x1a},		// erChnxMuteVolBPTrim
	{0x1b,0x1e},		// erChnxyInputMap
	{0x1f,0x21},		// erAutoMode
	{0x22,0x22},		// erPresetEq
	{0x23,0x27},		// erEqBandx
	{0x28,0x29},		// erChnLoopCtl
	{0x2a,0x2b},		// erBypassCtl
	{0x2c,0x2c},		// erBaseTrebleCtl
	{0x2d,0x2e},		// erChLimitSels
	{0x2f,0x32},		// erLim12Cfgs
	{0x33,0x36},		// erPwmTimings
	{0x37,0x3a},		// erI2sOutMaps
	{0x3b,0x4c},		// erCoefBlk
	{0x5a,0x5a},		// erExtLimitDrc
	{0x5b,0x5b},		// erFineVol
	{0x5d,0x6a},		// erPcmRecomb
	{0x6b,0x6e},		// erExtLimitBlk
	{0x6f,0x70},		// erRmsCtlBlk
	{0x71,0x75},		// erPllCtlBlk
	{0x76,0x76},		// erPllSts
	{0x77,0x79},		// erExtBiquadBlk
	{0x7a,0x7d},		// erRmsStatusBlk
	{0x80,0x80},		// erDpt
	{0x81,0x81},		// erCfr129
	{0x82,0x83},		// erPopSuppresTimes
};

bqCoefBlk_t ApBqFile[] = {
	{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
};

rawCoef_t ApPrescalerFile[] = {
	{0,0,0},
};

rawCoef_t ApPostscalerFile[] = {
	{0,0,0},
};
                                             
rawCoef_t ApMixerFile[] = {
	{0,0,0},
};
                                             
#endif //INSTANTIATE_AP_DATA

extern void ApDumpRegFile (void);
extern void ApReadBiquad (uint8_t chNo, uint8_t bqNo, bqCoefBlk_t *pbq);
extern void ApDumpRawCoef (rawCoef_t *pBqCoef, const char* pref, const char* postf);
extern void ApDumpBiquad (uint8_t chNo, uint8_t bqNo);
extern void ApDumpBiquadFile (void);
extern void ApLoadRegRange (uint8_t Bgn, uint8_t End, uint8_t *pRegFileData);
extern void ApLoadBiquadFile (void);
extern void ApReadRawCoefBlk (uint16_t blkAddr, rawCoef_t *buf, uint8_t count);
extern void ApWriteOneRawCoef (uint16_t ramAddr, rawCoef_t *buf);
extern void ApWriteRawCoefBlk (uint16_t blkAddr, rawCoef_t *buf, uint8_t count);
extern void ApDumpPrescaleFile (void);
extern void ApDumpPostscaleFile (void);
extern void ApDumpMixerFile (void);
extern void ApLoadPrescalerFile (void);
extern void ApLoadPostscalerFile (void);
extern void ApLoadMixerFile (void);
extern void ApDumpParameterFiles(void);

extern void ApInitialize (void);
extern void ApI2sOutputEnable (bool bEnable);

#ifdef __cplusplus
}
#endif //

#endif //_APCONTROL_H_
