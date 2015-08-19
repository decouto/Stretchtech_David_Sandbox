// ApControl.c -- Audio Processor Control Functions Implementation
//

#define INSTANTIATE_AP_DATA

#define BOOL uint8_t

#include "ApControl.h"
#include "Putz01.h"
#include <string.h>

#if ENABLE_LOADS && VERIFY_LOADS
#define VFYBUFSZ	sizeof(ApRegFile)
static uint8_t VfyBuf[VFYBUFSZ];
#endif //ENABLE_LOADS && VERIFY_LOADS

#if ENABLE_DUMPS

void ApDumpRegFile (void) {
	uint8_t buf[AP_REGFILESZ];
	uint8_t i;
	memset (buf,0,sizeof(buf));
//	PUTZ_DEBUG_WAIT();
	printf ("//I2C Register File:");
	AP_ReadBuffer(0, buf, AP_REGFILESZ);
	for (i = 0; i < AP_REGFILESZ; i++) {
		if (!(i & 15)) {
			printf ("\r\n0x%02x,", buf[i]);
		} else {
			printf ("0x%02x,", buf[i]);
		}
	}
	printf ("\r\n");
}

void ApReadBiquad (uint8_t chNo, uint8_t bqNo, bqCoefBlk_t *pbq) {
	uint16_t addr = chNo*AP_BQPERCHANNEL+bqNo*5;
	uint8_t buf[3];
	buf[0] = 0x3b;
	buf[1] = (addr >> 8) & 3;
	buf[2] = addr & 0xff;
	AP_WriteBuffer (buf, 3);
	AP_ReadBuffer (0x3d, (uint8_t*)pbq, sizeof(*pbq));
}
#endif //ENABLE_DUMPS


void ApWriteBiquad (uint8_t chNo, uint8_t bqNo, bqCoefBlk_t *pbq) {
	uint16_t addr = AP_BQCOEF_BASE+chNo*AP_BQPERCHANNEL+bqNo*AP_COEFPERBQ;
	uint8_t lclbuf[4+sizeof(*pbq)];
	lclbuf[0] = 0x3b;
	lclbuf[1] = (addr >> 8) & 3;
	lclbuf[2] = addr & 0xff;
	memcpy (lclbuf+3, pbq, sizeof(*pbq));
	lclbuf[sizeof(lclbuf)-1] = 2; // Write-All flag
	AP_WriteBuffer (lclbuf, sizeof(lclbuf));
}

#if ENABLE_DUMPS

void ApDumpRawCoef (rawCoef_t *pBqCoef, const char* pref, const char* postf) {
	printf ("%s{0x%02x,0x%02x,0x%02x}%s", pref, pBqCoef->coefHi, pBqCoef->coefMi, pBqCoef->coefLo, postf);
}

void ApDumpBiquad (uint8_t chNo, uint8_t bqNo) {
	bqCoefBlk_t bq;
	ApReadBiquad (chNo, bqNo, &bq);
	ApDumpRawCoef (&bq.BqB1, "{", "");
	ApDumpRawCoef (&bq.BqB2, ",", "");
	ApDumpRawCoef (&bq.BqA1, ",", "");
	ApDumpRawCoef (&bq.BqA2, ",", "");
	ApDumpRawCoef (&bq.BqB0, ",", "},");
	printf ("// Ch%d, Bq%d\r\n", chNo, bqNo);
}

void ApDumpBiquadFile () {
	uint8_t ch, bq;
	printf ("//Biquad filter file:\r\n");
	for (ch = 0; ch < AP_CHANNELS; ch++)
		for (bq = 0; bq < AP_BQPERCHANNEL; bq++)
			ApDumpBiquad (ch, bq);
}

#endif //ENABLE_DUMPS

#if ENABLE_DUMPS || ENABLE_LOADS
void ApReadRawCoefBlk (uint16_t blkAddr, rawCoef_t *buf, uint8_t count) {
	uint8_t rdSiz = 5;
	uint8_t lclbuf[3];
	for (; count; count -= rdSiz, buf += rdSiz, blkAddr+= rdSiz) {
		if (rdSiz > count) rdSiz = count;
		lclbuf[0] = 0x3b;
		lclbuf[1] = (blkAddr >> 8) & 3;
		lclbuf[2] = blkAddr & 0xff;
		AP_WriteBuffer (lclbuf, 3);
		AP_ReadBuffer (0x3d, (uint8_t*)buf, rdSiz*sizeof(*buf));
	}
}

#endif //ENABLE_DUMPS || ENABLE_LOADS

#if ENABLE_LOADS

void ApWriteOneRawCoef (uint16_t ramAddr, rawCoef_t *buf) {
	uint8_t lclbuf[6];
	lclbuf[0] = 0x3b;
	lclbuf[1] = (ramAddr >> 8) & 3;
	lclbuf[2] = ramAddr & 0xff;
	memcpy (lclbuf+3, buf, 3);
	AP_WriteBuffer (lclbuf, 6);
	lclbuf[0] = 0x4c;
	lclbuf[1] = 1;	// Write-One flag
	AP_WriteBuffer (lclbuf, 2);
}

void ApWriteRawCoefBlk (uint16_t blkAddr, rawCoef_t *buf, uint8_t count) {
	uint8_t lclbuf[4+5*sizeof(rawCoef_t)];
	// Use block mode when possible
	for (; count >= 5; count -= 5, buf += 5, blkAddr += 5) {
		lclbuf[0] = 0x3b;
		lclbuf[1] = (blkAddr >> 8) & 3;
		lclbuf[2] = blkAddr & 0xff;
		memcpy (lclbuf+3, buf, 5*sizeof(rawCoef_t));
		lclbuf[sizeof(lclbuf)-1] = 2; // Write-All flag
		AP_WriteBuffer (lclbuf, sizeof(lclbuf));
	}
	// Clean up remainder one coefficient at a time
	for (; count; count--) {
		ApWriteOneRawCoef (blkAddr++, buf++);
	}
}

#endif

#if ENABLE_DUMPS

void ApDumpPrescaleFile (void) {
	uint8_t ch;
	printf ("//Prescaler File\r\n");
	rawCoef_t cbuf[AP_CHANNELS];
	ApReadRawCoefBlk (AP_PRESCALE_BASE, cbuf, AP_CHANNELS);
	for (ch = 0; ch < AP_CHANNELS; ch++) {
		ApDumpRawCoef (cbuf+ch, "", ",");
		printf (" // Ch%d pre-scaler\r\n", ch);
	}
	printf ("\r\n");
}

void ApDumpPostscaleFile (void) {
	uint8_t ch;
	printf ("//Postscaler File\r\n");
	rawCoef_t cbuf[AP_CHANNELS];
	ApReadRawCoefBlk (AP_POSTSCALE_BASE, cbuf, AP_CHANNELS);
	for (ch = 0; ch < AP_CHANNELS; ch++) {
		ApDumpRawCoef (cbuf+ch, "", ",");
		printf (" // Ch%d post-scaler\r\n", ch);
	}
	printf ("\r\n");
}

void ApDumpMixer1File (void) {
	uint8_t cho, chi;
	printf ("//Mixer1 File\r\n");
	rawCoef_t cbuf[AP_CHANNELS];
	for (cho = 0; cho < AP_CHANNELS; cho++) {
		ApReadRawCoefBlk (AP_MIX1COEF_BASE+cho*AP_CHANNELS, cbuf, AP_CHANNELS);
		for (chi = 0; chi < AP_CHANNELS; chi++) {
			ApDumpRawCoef (cbuf+chi, "", ",");
			printf (" // Cho %d Mixer1 Chi %d\r\n", cho, chi);
		}
	}
	printf ("\r\n");
}

void ApDumpMixer2File (void) {
	uint8_t cho, chi;
	printf ("//Mixer2 File\r\n");
	rawCoef_t cbuf[AP_CHANNELS];
	for (cho = 0; cho < AP_CHANNELS; cho++) {
		ApReadRawCoefBlk (AP_MIX2COEF_BASE+cho*AP_CHANNELS, cbuf, AP_CHANNELS);
		for (chi = 0; chi < AP_CHANNELS; chi++) {
			ApDumpRawCoef (cbuf+chi, "", ",");
			printf (" // Cho %d Mixer2 Chi %d\r\n", cho, chi);
		}
	}
	printf ("\r\n");
}

#endif

void ApDumpParameterFiles (void) {
	#if ENABLE_DUMPS
	ApDumpRegFile();
	ApDumpBiquadFile();
	ApDumpPrescaleFile();
	ApDumpPostscaleFile();
	ApDumpMixer1File();
	ApDumpMixer2File();
	#endif //ENABLE_DUMPS
}

#if ENABLE_LOADS

void ApLoadRegRange (uint8_t Bgn, uint8_t End, uint8_t *pRegFileData) {
	uint8_t buf[AP_REGFILESZ+1];
	PUTZ_ASSERT(End - Bgn < AP_REGFILESZ);
	buf[0] = Bgn;
	memcpy (buf+1, pRegFileData+Bgn, End - Bgn + 1);
	AP_WriteBuffer(buf, End - Bgn + 2);
}

void ApLoadBiquadFile (void) {
	uint8_t ch, bq;
	for (ch = 0; ch < AP_CHANNELS; ch++)
		for (bq = 0; bq < AP_BQPERCHANNEL; bq++)
			ApWriteBiquad (ch, bq, &ApBqFile[ch*AP_BQPERCHANNEL+bq]);
}

void ApLoadPrescalerFile (void) {
	ApWriteRawCoefBlk (AP_PRESCALE_BASE, ApPrescalerFile, sizeof(ApPrescalerFile)/sizeof(ApPrescalerFile[0]));
}

void ApLoadPostscalerFile (void) {
	ApWriteRawCoefBlk (AP_POSTSCALE_BASE, ApPostscalerFile, sizeof(ApPostscalerFile)/sizeof(ApPostscalerFile[0]));
}

void ApLoadMixer1File (void) {
	ApWriteRawCoefBlk (AP_MIX1COEF_BASE, ApMixer1File, sizeof(ApMixer1File)/sizeof(ApMixer1File[0]));
}

void ApLoadMixer2File (void) {
	ApWriteRawCoefBlk (AP_MIX2COEF_BASE, ApMixer2File, sizeof(ApMixer2File)/sizeof(ApMixer2File[0]));
}

#if VERIFY_LOADS

uint16_t memvfy (void* p1, void* p2, uint16_t len) {
	uint16_t i;
	for (i = 0; i < len; i++) {
		if (((uint8_t*)p1)[i]!=((uint8_t*)p2)[i]) {
			printf ("memvfy failed at index %u\r\n", i);
			return i+1;
		}
	}
	return 0;
}

BOOL ApVerifyRegRangeLoad (uint8_t Bgn, uint8_t End, uint8_t *pRegFileData) {
	AP_ReadBuffer(Bgn,VfyBuf,End-Bgn+1);
	if (memvfy(VfyBuf, &pRegFileData[Bgn], End-Bgn+1)) {
		PUTZ_ASSERT (FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL ApVerifyBiquadFileLoad (void) {
	uint8_t ch, bq;
	for (ch = 0; ch < AP_CHANNELS; ch++) {
		for (bq = 0; bq < AP_BQPERCHANNEL; bq++) {
			ApReadBiquad (ch, bq, (bqCoefBlk_t *)VfyBuf);
			if (memvfy(VfyBuf,&ApBqFile[ch*AP_BQPERCHANNEL+bq],sizeof(ApBqFile[0]))) {
				PUTZ_ASSERT (FALSE);
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL ApVerifyPrescalerFileLoad (void) {
	ApReadRawCoefBlk (AP_PRESCALE_BASE, (rawCoef_t*)VfyBuf, sizeof(ApPrescalerFile)/sizeof(ApPrescalerFile[0]));
	if (memvfy(VfyBuf, ApPrescalerFile, sizeof(ApPrescalerFile))) {
		PUTZ_ASSERT (FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL ApVerifyPostscalerFileLoad (void) {
	ApReadRawCoefBlk (AP_POSTSCALE_BASE, (rawCoef_t *)VfyBuf, sizeof(ApPostscalerFile)/sizeof(ApPostscalerFile[0]));
	if (memvfy(VfyBuf, ApPostscalerFile, sizeof(ApPostscalerFile))) {
		PUTZ_ASSERT (FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL ApVerifyMixer1FileLoad (void) {
	ApReadRawCoefBlk (AP_MIX1COEF_BASE, (rawCoef_t *)VfyBuf, sizeof(ApMixer1File)/sizeof(ApMixer1File[0]));
	if (memvfy(VfyBuf, ApMixer1File, sizeof(ApMixer1File))) {
		PUTZ_ASSERT (FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL ApVerifyMixer2FileLoad (void) {
	ApReadRawCoefBlk (AP_MIX2COEF_BASE, (rawCoef_t *)VfyBuf, sizeof(ApMixer2File)/sizeof(ApMixer2File[0]));
	if (memvfy(VfyBuf, ApMixer2File, sizeof(ApMixer2File))) {
		PUTZ_ASSERT (FALSE);
		return FALSE;
	}
	return TRUE;
}

#endif //VERIFY_LOADS
#endif //ENABLE_LOADS

void ApInitialize (void) {
#if ENABLE_LOADS
	ApLoadRegRange(ApRegRanges[erConfig].Bgn, ApRegRanges[erI2sOutMaps].End, ApRegFile);
	ApLoadRegRange(ApRegRanges[erExtLimitDrc].Bgn, ApRegRanges[erExtBiquadBlk].End, ApRegFile);
	ApLoadRegRange(ApRegRanges[erDpt].Bgn, ApRegRanges[erPopSuppresTimes].End, ApRegFile);
	ApLoadBiquadFile();
	ApLoadPrescalerFile();
	ApLoadPostscalerFile();
	ApLoadMixer1File();
	ApLoadMixer2File();
#if VERIFY_LOADS
	ApVerifyRegRangeLoad(ApRegRanges[erConfig].Bgn, ApRegRanges[erI2sOutMaps].End, ApRegFile);
	ApVerifyRegRangeLoad(ApRegRanges[erExtLimitDrc].Bgn, ApRegRanges[erExtBiquadBlk].End, ApRegFile);
	ApVerifyRegRangeLoad(ApRegRanges[erDpt].Bgn, ApRegRanges[erPopSuppresTimes].End, ApRegFile);
	ApVerifyBiquadFileLoad();
	ApVerifyPrescalerFileLoad();
	ApVerifyPostscalerFileLoad();
	ApVerifyMixer1FileLoad();
	ApVerifyMixer2FileLoad();
#endif //VERIFY_LOADS
#endif //ENABLE_LOADS
}

void ApI2sOutputEnable (BOOL bEnable) {
	uint8_t buf[2];
	buf[0] = 6;			// Config Reg G
	AP_ReadBuffer(buf[0],&buf[1],1);
	buf[1] &= ~2;
	buf[1] |= bEnable ? 0 : 2;	// Bit 2 is Disable bit!
	AP_WriteBuffer(buf,2);
}

