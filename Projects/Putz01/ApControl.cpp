// ApControl.cpp -- Audio Processor Control Functions Implementation
//

#define INSTANTIATE_AP_DATA

#include "ApControl.h"
#include "Putz01.h"
#include <string.h>

#ifdef ENABLE_DUMPS

void ApDumpRegFile (void) {
	uint8_t buf[AP_REGFILESZ];
	memset (buf,0,sizeof(buf));
//	PUTZ_DEBUG_WAIT();
	printf ("I2C Register File:\r\n");
	AP_ReadBuffer(0, buf, AP_REGFILESZ);
	for (uint8_t i = 0; i < AP_REGFILESZ; i++) {
		if (i && !(i & 15)) {
			printf ("\r\n%02x ", buf[i]);
		} else {
			printf (",%02x", buf[i]);
		}
	}
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
	uint16_t addr = chNo*AP_BQPERCHANNEL+bqNo*AP_COEFPERBQ;
	uint8_t lclbuf[4+sizeof(*pbq)];
	lclbuf[0] = 0x3b;
	lclbuf[1] = (addr >> 8) & 3;
	lclbuf[2] = addr & 0xff;
	memcpy (lclbuf+3, pbq, sizeof(*pbq));
	lclbuf[sizeof(lclbuf)-1] = 2; // Write-All flag
	AP_WriteBuffer (lclbuf, sizeof(lclbuf));
}

#ifdef ENABLE_DUMPS

void ApDumpRawCoef (rawCoef_t *pBqCoef, const char* pref, const char* postf) {
	printf ("%s{%02x,%02x,%02x}%s", pref, pBqCoef->coefHi, pBqCoef->coefMi, pBqCoef->coefLo, postf);
}

void ApDumpBiquad (uint8_t chNo, uint8_t bqNo) {
	bqCoefBlk_t bq;
	ApReadBiquad (chNo, bqNo, &bq);
	ApDumpRawCoef (&bq.BqB1, "{", "");
	ApDumpRawCoef (&bq.BqB2, ",", "");
	ApDumpRawCoef (&bq.BqA1, ",", "");
	ApDumpRawCoef (&bq.BqA2, ",", "");
	ApDumpRawCoef (&bq.BqB0, "", "},");
	printf ("// Ch%d, Bq%d\r\n", chNo, bqNo);
}

void ApDumpBiquadFile () {
	printf ("Biquad filter file:\r\n");
	for (uint8_t ch = 0; ch < AP_CHANNELS; ch++)
		for (uint8_t bq = 0; bq < AP_BQPERCHANNEL; bq++)
			ApDumpBiquad (ch, bq);
}

void ApReadRawCoefBlk (uint16_t blkAddr, rawCoef_t *buf, uint8_t count) {
	uint8_t rdSiz = 5;
	uint8_t lclbuf[3];
	for (; count; count -= rdSiz, buf += rdSiz) {
		if (rdSiz > count) rdSiz = count;
		lclbuf[0] = 0x3b;
		lclbuf[1] = (blkAddr >> 8) & 3;
		lclbuf[2] = blkAddr & 0xff;
		AP_WriteBuffer (lclbuf, 3);
		AP_ReadBuffer (0x3d, (uint8_t*)buf, rdSiz*sizeof(*buf));
	}
}
#endif //ENABLE_DUMPS


#ifdef ENABLE_LOADS

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

#ifdef ENABLE_DUMPS

void ApDumpPrescaleFile (void) {
	printf ("Prescaler File\r\n");
	rawCoef_t cbuf[AP_CHANNELS];
	ApReadRawCoefBlk (AP_PRESCALE_BASE, cbuf, AP_CHANNELS);
	for (uint8_t ch = 0; ch < AP_CHANNELS; ch++) {
		ApDumpRawCoef (cbuf+ch, "", ",");
		printf (" // Ch%d pre-scaler\r\n", ch);
	}
	printf ("\r\n");
}

void ApDumpPostscaleFile (void) {
	printf ("Postscaler File\r\n");
	rawCoef_t cbuf[AP_CHANNELS];
	ApReadRawCoefBlk (AP_POSTSCALE_BASE, cbuf, AP_CHANNELS);
	for (uint8_t ch = 0; ch < AP_CHANNELS; ch++) {
		ApDumpRawCoef (cbuf+ch, "", ",");
		printf (" // Ch%d post-scaler\r\n", ch);
	}
	printf ("\r\n");
}

void ApDumpMixerFile (void) {
	printf ("Mixer File\r\n");
	rawCoef_t cbuf[AP_CHANNELS];
	for (uint8_t cho = 0; cho < AP_CHANNELS; cho++) {
		ApReadRawCoefBlk (AP_MIXCOEF_BASE+cho*AP_CHANNELS, cbuf, AP_CHANNELS);
		for (uint8_t chi = 0; chi < AP_CHANNELS; chi++) {
			ApDumpRawCoef (cbuf+chi, "", ",");
			printf (" // Cho%d mixer chi %d\r\n", cho, chi);
		}
	}
	printf ("\r\n");
}

#endif

void ApDumpParameterFiles (void) {
	#ifdef ENABLE_DUMPS
	ApDumpRegFile();
	ApDumpBiquadFile();
	ApDumpPrescaleFile();
	ApDumpPostscaleFile();
	ApDumpMixerFile();
	#endif //ENABLE_DUMPS
}

#ifdef ENABLE_LOADS

void ApLoadRegRange (uint8_t Bgn, uint8_t End, uint8_t *pRegFileData) {
	uint8_t buf[AP_REGFILESZ+1];
	PUTZ_ASSERT(End - Bgn < AP_REGFILESZ);
	buf[0] = Bgn;
	memcpy (buf+1, pRegFileData+Bgn, End - Bgn + 1);
	AP_WriteBuffer(buf, End - Bgn + 2);
}

void ApLoadBiquadFile (void) {
	for (uint8_t ch = 0; ch < AP_CHANNELS; ch++)
		for (uint8_t bq = 0; bq < AP_BQPERCHANNEL; bq++)
			ApWriteBiquad (ch, bq, &ApBqFile[ch*AP_BQPERCHANNEL+bq]);
}

void ApLoadPrescalerFile (void) {
	ApWriteRawCoefBlk (AP_PRESCALE_BASE, ApPrescalerFile, sizeof(ApPrescalerFile)/sizeof(ApPrescalerFile[0]));
}

void ApLoadPostscalerFile (void) {
	ApWriteRawCoefBlk (AP_POSTSCALE_BASE, ApPostscalerFile, sizeof(ApPostscalerFile)/sizeof(ApPostscalerFile[0]));
}

void ApLoadMixerFile (void) {
	ApWriteRawCoefBlk (AP_MIXCOEF_BASE, ApMixerFile, sizeof(ApMixerFile)/sizeof(ApMixerFile[0]));
}

#endif //ENABLE_LOADS

void ApInitialize (void) {
	#ifdef ENABLE_LOADS
	ApLoadRegRange(ApRegRanges[erConfig].Bgn, ApRegRanges[erCoefBlk].End, ApRegFile);
	ApLoadRegRange(ApRegRanges[erExtLimitDrc].Bgn, ApRegRanges[erRmsStatusBlk].End, ApRegFile);
	ApLoadRegRange(ApRegRanges[erDpt].Bgn, ApRegRanges[erPopSuppresTimes].End, ApRegFile);
	ApLoadBiquadFile();
	ApLoadPrescalerFile();
	ApLoadPostscalerFile();
	ApLoadMixerFile();
	#endif //ENABLE_LOADS
}

void ApI2sOutputEnable (bool bEnable) {
	uint8_t buf[2];
	buf[0] = 6;			// Config Reg G
	AP_ReadBuffer(buf[0],&buf[1],1);
	buf[1] &= ~2;
	buf[1] |= bEnable ? 2 : 0;
	AP_WriteBuffer(buf,2);
}
