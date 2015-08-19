// Putz01.c -- application module definition
//

#include "Putz01.h"
#include "ApControl.h"
#include <stdio.h>
#include <stdarg.h>


#define BOOL uint8_t
#define TRUE 1
#define FALSE 0

#define ENABLE_SYNCWS 0
#define ENABLE_DUMP	1
#define PROC_ALLBUFS 0
#define CMP_S2I2_SAIA	0
#define CMP_S2I2_SAIB 0
#define CMPLEN I2S2BUFSZ

//extern "C" {
#include "Board_Buttons.h"
#include "Board_LED.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_sai.h"
#include "stm32f4xx_hal_i2s.h"
#include <string.h>
//#include "cmsis_os.h"
//}

void Putz01Assert(uint8_t* file, uint32_t line) {
	printf("FAILED ASSERT in: file %s at line %lu\r\n", file, line);
	while(1);
}

#define SWAPHWDS(x)	((((x)&0x0000ffff)<<16)|(((x)&0xffff0000)>>16))
uint32_t CmpI2sBufToSaiBuf(uint32_t *pI2s, uint32_t *pSai, uint32_t len) {
	uint32_t i;
	for (i = 0; i < len; i++)
		if (SWAPHWDS(pI2s[i]) != pSai[i])
			break;
	return i;
}
#undef SWAPHWDS

void DumpHWds (uint16_t *p, uint16_t len, const char *note) {
	uint16_t i;
	if (note) printf (note);
	for (i = 0; i != len; i++) {
		if (i && !(i & 15)) puts ("\r");
		printf ("%04x ", p[i]); 
	}
	if (len) puts ("\r");
}

void DumpBytes (uint8_t *p, uint16_t len, const char *note) {
	uint16_t i;
	if (note) printf (note);
	for (i = 0; i != len; i++) {
		if (i && !(i & 15)) puts ("\r");
		printf ("%02x ", p[i]); 
	}
	if (len) puts ("\r");
}


#define SWAPHWDS(x)	((((x)&0x0000ffff)<<16)|(((x)&0xffff0000)>>16))
void DumpFWds (uint32_t *p, uint16_t len, BOOL bSwap, const char *note) {
	uint16_t i;
	if (note) printf (note);
	for (i = 0; i != len; i++) {
		if (i && !(i & 15)) puts ("\r");
		printf ("%08lx ", bSwap ? SWAPHWDS(p[i]) : p[i]);
	}
	if (len) puts ("\r");
}
#undef SWAPHWDS


#if 1
int PutzPrintf (const char *fmt, ...) {
	va_list ap;
	char buf[128] = {0};
	int len;
	va_start (ap, fmt);
	vsprintf (buf, fmt, ap);
	va_end(ap);
	buf[sizeof(buf)-1] = 0;
	len = strlen(buf);
	HAL_UART_Transmit(&huart3, (void*)buf, len, HAL_MAX_DELAY);
	return len;
}
int PutzPuts (const char *s) {
	int len;
	if (!s) return 0;
	len = strlen(s);
	HAL_UART_Transmit(&huart3, (void*)s, len, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart3, (void*)"\n", 1, HAL_MAX_DELAY);
	return len;
}
#else
#if 0

//volatile int32_t ITM_RxBuffer;

int stdin_getchar(void) {
	return ITM_ReceiveChar();
}

void stdout_putchar(int c) {
	ITM_SendChar(c);
}

#else

int stdin_getchar(void) {
	uint8_t data[2] = {0};
	/*HAL_StatusTypeDef st = */HAL_UART_Receive(&huart3, data, 1, HAL_MAX_DELAY);
	return data[0];
}

void stdout_putchar(int c) {
	HAL_StatusTypeDef st = HAL_UART_Transmit(&huart3, (uint8_t*)&c, 1, HAL_MAX_DELAY);
	PUTZ_ASSERT (st == HAL_OK);
}

#endif
#endif

#define WAITLIM	(1<<18)
BOOL IfBitWiggles(uint32_t* pPortIdr, uint8_t bitPos, char* name) {
	int i;
	// Wait for bit high condition
	for (i = 0; (*pPortIdr & (1<<bitPos)); i++) {
		if (++i > WAITLIM) goto quiet;
	}
	// Wait for bit low condition
	for (i = 0; !(*pPortIdr & (1<<bitPos)); i++) {
		if (++i > WAITLIM) goto quiet;
	}
	// Wait for bit high condition
	for (i = 0; (*pPortIdr & (1<<bitPos)); i++) {
		if (++i > WAITLIM) goto quiet;
	}
	printf ("%s is OK\r\n", name);
	return TRUE;
quiet:
	printf ("%s is SILENT\r\n", name);
	return FALSE;
}

#define SAI_SCK_A	PORTE_IDR,5
#define SAI_FS_A	PORTE_IDR,4
#define SAI_SD_A	PORTE_IDR,6
#define SAI_SD_B	PORTE_IDR,3
#define I2S_2_CK	PORTB_IDR,10
#define I2S_2_WS	PORTB_IDR,12
#define I2S_2_SD	PORTC_IDR,3
#define I2S_3_CK	PORTC_IDR,10
#define I2S_3_WS	PORTA_IDR,4
#define I2S_3_SD	PORTC_IDR,12

BOOL CheckSignalsForLife() {
	BOOL b = TRUE;
	
#if USING_I2S
	puts ("Checking I2S2 signals...\r");
	if (IfBitWiggles(I2S_2_CK," I2S_2_CK")) b = 0;
	if (IfBitWiggles(I2S_2_WS," I2S_2_WS")) b = 0;
	if (IfBitWiggles(I2S_2_SD," I2S_2_SD")) b = 0;
	
	puts ("Checking I2S3 signals...\r");
	if (IfBitWiggles(I2S_3_CK," I2S_3_CK")) b = 0;
	if (IfBitWiggles(I2S_3_WS," I2S_3_WS")) b = 0;
	if (IfBitWiggles(I2S_2_SD," I2S_2_SD")) b = 0;
#endif //USING_I2S

	puts ("Checking SAI signals...\r");
	if (IfBitWiggles(SAI_SCK_A," SAI_SCK_A")) b = 0;
	if (IfBitWiggles(SAI_FS_A," SAI_FS_A")) b = 0;
	if (IfBitWiggles(SAI_SD_A," SAI_SD_A")) b = 0;
	if (IfBitWiggles(SAI_SD_B," SAI_SD_B")) b = 0;
	
	return b;
}

BOOL Putz01Init (void) {
	puts ("\r\n\nPutz01Init() Enter\r");
	Buttons_Initialize();
	LED_Initialize();
	CheckSignalsForLife();
	puts ("Putz01Init() Leave\r");
	return TRUE;
}

#if 0
void DumpApRegs (void) {
	uint8_t buf[AP_REGFILESZ];
	memset (buf,0,sizeof(buf));
//	PUTZ_DEBUG_WAIT();
	AP_ReadBuffer(0, buf, AP_REGFILESZ);
	DumpBytes (buf, sizeof(buf), (const char*)"\r\nAudio Processor Registers:\r\n");
}
#endif

BOOL Putz01Run(void) {
	puts ("Putz01Run() Enter\r");
#if USING_I2S
	memset(&I2S2BufCtl,0,sizeof(I2S2BufCtl)); 
	memset(I2S2Buf,0,sizeof(I2S2Buf)); 
	memset(&I2S3BufCtl,0,sizeof(I2S3BufCtl)); 
	memset(I2S3Buf,0,sizeof(I2S3Buf)); 
#endif //USING_I2S
	memset(&SaiABufCtl,0,sizeof(SaiABufCtl)); 
	memset(SaiABuf,0,sizeof(SaiABuf)); 
	memset(&SaiBBufCtl,0,sizeof(SaiBBufCtl)); 
	memset(SaiBBuf,0,sizeof(SaiBBuf));
	{ BOOL b = 0;	//Change this in the debugger if you want a dump
		if (!b) {
			printf ("Putz01Run bypassing ApDumpParameterFiles\r\n");
			printf ("Change b to TRUE in debugger to get a dump!\r\n");
		} else {
			ApDumpParameterFiles();
		}
	}
	ApInitialize();
	// Stop the I2s audio stream from the Microphone Processor
	ApI2sOutputEnable(FALSE);
#if USING_I2S
	Putz01I2S2Start();
//	Putz01I2S3Start();
//	Putz01SaiAStart();
#endif //USING_I2S
	Putz01SaiAStart();
	Putz01SaiBStart();
	// Star the I2s audio stream from the Microphone Processor
	ApI2sOutputEnable(TRUE);
	while (1) {
//		HAL_Delay(1000);
//		for (int i = 0; i < (1<<20); i++); 
		Putz01HandleButtons();
#if USING_I2S
		Putz01I2S2Proc();
		Putz01I2S3Proc();
#endif //USING_I2S
		Putz01SaiAProc();
		Putz01SaiBProc();
	};
//	return TRUE;
}

#define EVENT_INTERVAL	500
BOOL Putz01HandleButtons (void) {
	int i = 1;
	static uint32_t tickEvent = EVENT_INTERVAL;
	while (Buttons_GetState()) {
		if (i) puts ("Putz01HandleButtons() Looping\r");
		i = 0;
	}
	if (HAL_GetTick() > tickEvent) {
		static BOOL onOff = FALSE;
		LED_SetOut(onOff ? 2 : 1);
		onOff = !onOff;
		tickEvent = HAL_GetTick() + EVENT_INTERVAL;
	}
	return TRUE;
}

//#if USING_I2S

#define PA04I (*((uint32_t*)PORTA_IDR)&(1<<04))
#define PB12I (*((uint32_t*)PORTB_IDR)&(1<<12))
#define BVAL 0
void WaitForI2sWs(I2S_HandleTypeDef *h) {
#if ENABLE_SYNCWS
	int bHigh = BVAL;
	if (h == &hi2s2) {
		printf ("WaitForIs2Ws (PB12) transition to %s\r\n", bHigh ? "HIGH" : "LOW");
		while (!PB12I);					// Wait while low
		while (PB12I);					// Wait while high
		if (bHigh) while (!PB12I);	// Wait while low
	} else if (h == &hi2s3) {
		printf ("WaitForIs2Ws (PA04) transition to %s\r\n", bHigh ? "HIGH" : "LOW");
		while (!PA04I);					// Wait while low
		while (PA04I);					// Wait while high
		if (bHigh) while (!PA04I);	// Wait while low
	} else {
		PUTZ_ASSERT(FALSE);
	}
#endif //ENABLE_SYNCWS
}
#undef PA04I
#undef PB12I
#undef BVAL

#if USING_I2S

I2sBufCtl_t I2S2BufCtl;
I2sData_t I2S2Buf[I2S2BUFSZ];
BOOL Putz01I2S2Start (void) {
	puts ("Putz01I2S2Start() Enter\r");
	HAL_StatusTypeDef sts = HAL_I2S_Receive_DMA(&hi2s2, (uint16_t*)I2S2Buf, sizeof(I2S2Buf)/sizeof(I2S2Buf[0]));//I2S2BUFSZ*sizeof(I2sData_t)/sizeof(uint16_t));
	PUTZ_ASSERT(sts==HAL_OK);
	return TRUE;
}

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
	HAL_I2S_RxCpltCallback(hi2s);
}
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {
	if (hi2s == &hi2s2) {
		I2S2BufCtl.nRecv++;
	} else if (hi2s == &hi2s3) {
			I2S3BufCtl.nRecv++;
	} else {
		PUTZ_ASSERT(FALSE);
	}
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
	if (hi2s == &hi2s2) {
		I2S2BufCtl.nError++;
	} else if (hi2s == &hi2s3) {
			I2S3BufCtl.nRecv++;
	} else {
		PUTZ_ASSERT(FALSE);
	}
}

BOOL Putz01I2S2Proc (void) {
	if (I2S2BufCtl.nRecvProc != I2S2BufCtl.nRecv) {
		if (I2S2BufCtl.nErrorProc != I2S2BufCtl.nError) {
				I2S2BufCtl.nErrorProc++;
		}
		if (PROC_ALLBUFS || !(I2S2BufCtl.nRecvProc & 0x000e)) {
			if (I2S2BufCtl.nRecvProc & 1) {
				#if ENABLE_DUMP
				char strbuf[32];
				sprintf (strbuf, "I2S2 Rx%06lu.5 Er%lu ", I2S2BufCtl.nRecvProc/2, I2S2BufCtl.nErrorProc);
				DumpFWds ((uint32_t*)&I2S2Buf[I2S2BUFSZ/2],4,TRUE,strbuf);
				#endif //ENABLE_DUMP 
			} else {
				#if ENABLE_DUMP
				char strbuf[32];
				sprintf (strbuf, "I2S2 Rx%06lu.0 Er%lu ", I2S2BufCtl.nRecvProc/2, I2S2BufCtl.nErrorProc);
				DumpFWds ((uint32_t*)&I2S2Buf[0],4,TRUE,strbuf);
				#endif //ENABLE_DUMP 
			}
		}
		I2S2BufCtl.nRecvProc++;
	} else if (I2S2BufCtl.nErrorProc != I2S2BufCtl.nError) {
		I2S2BufCtl.nErrorProc++;
		printf ("I2S Er%04lu\r\n", I2S2BufCtl.nErrorProc);
	}
	return TRUE;
}

I2sBufCtl_t I2S3BufCtl;
I2sData_t I2S3Buf[I2S3BUFSZ];
BOOL Putz01I2S3Start (void) {
	puts ("Putz01I2S3Start() Enter\r");
	HAL_StatusTypeDef sts = HAL_I2S_Receive_DMA(&hi2s3, (uint16_t*)I2S3Buf, sizeof(I2S3Buf)/sizeof(I2S3Buf[0]));//I2S3BUFSZ*sizeof(I2sData_t)/sizeof(uint16_t));
	PUTZ_ASSERT(sts==HAL_OK);
	return TRUE;
}
BOOL Putz01I2S3Proc (void) {
	if (I2S3BufCtl.nRecvProc != I2S3BufCtl.nRecv) {
		if (I2S3BufCtl.nErrorProc != I2S3BufCtl.nError) {
				I2S3BufCtl.nErrorProc++;
		}
		if (PROC_ALLBUFS || !(I2S3BufCtl.nRecvProc & 0x000e)) {
			if (I2S3BufCtl.nRecvProc & 1) {
				#if ENABLE_DUMP
				char strbuf[32];
				sprintf (strbuf, "I2S3 Rx%06lu.5 Er%lu ", I2S3BufCtl.nRecvProc/2, I2S3BufCtl.nErrorProc);
				DumpFWds ((uint32_t*)&I2S3Buf[I2S3BUFSZ/2],8,TRUE,strbuf);
				#endif //ENABLE_DUMP 
			} else {
				#if ENABLE_DUMP
				char strbuf[32];
				sprintf (strbuf, "I2S3 Rx%06lu.0 Er%lu ", I2S3BufCtl.nRecvProc/2, I2S3BufCtl.nErrorProc);
				DumpFWds ((uint32_t*)&I2S3Buf[0],8,TRUE,strbuf);
				#endif //ENABLE_DUMP 
			}
		}
		I2S3BufCtl.nRecvProc++;
	} else if (I2S3BufCtl.nErrorProc != I2S3BufCtl.nError) {
		I2S3BufCtl.nErrorProc++;
		printf ("I2S Er%04lu\r\n", I2S3BufCtl.nErrorProc);
	}
	return TRUE;
}
#endif //USING_I2S

#define PE04 ((*PORTE_IDR)&(1<<04))
#define BVAL 0
void WaitForSaiFs(SAI_HandleTypeDef *h) {
#if ENABLE_SYNCWS
	int bHigh = BVAL;
	printf ("WaitForSaiFs (PE04) transition to %s\r\n", bHigh ? "HIGH" : "LOW");
	while (!PE04);						// Wait while low
	while (PE04);							// Wait while high
	if (bHigh) while (!PE04);	// Wait while low
#endif //ENABLE_SYNCWS
}
#undef PE04
#undef BVAL



I2sBufCtl_t SaiABufCtl;
I2sData_t SaiABuf[SAIABUFSZ];
BOOL Putz01SaiAStart (void) {
	puts ("Putz01SaiAStart() Enter\r");
	HAL_StatusTypeDef sts = HAL_SAI_Receive_DMA(&hsai_BlockA1, (uint8_t *)&SaiABuf, sizeof(SaiABuf)/sizeof(SaiABuf[0]));
	PUTZ_ASSERT(sts==HAL_OK);
	return TRUE;
}

BOOL Putz01SaiAProc (void) {
	if (SaiABufCtl.nRecvProc != SaiABufCtl.nRecv) {
		if (SaiABufCtl.nErrorProc != SaiABufCtl.nError) {
				SaiABufCtl.nErrorProc++;
		}
		if (PROC_ALLBUFS || !(SaiABufCtl.nRecvProc & 0x000e)) {
			if (SaiABufCtl.nRecvProc & 1) {
				#if ENABLE_DUMP
				char strbuf[32];
				sprintf (strbuf, "SaiA Rx%06lu.5 Er%lu ", SaiABufCtl.nRecvProc/2, SaiABufCtl.nErrorProc);
				DumpFWds ((uint32_t*)&SaiABuf[SAIABUFSZ/2],4,FALSE,strbuf);
				#endif //ENABLE_DUMP
				#if CMP_S2I2_SAIA
				uint32_t ix = CmpI2sBufToSaiBuf((uint32_t*)&I2S2Buf[I2S2BUFSZ/2], (uint32_t*)&SaiABuf[SAIABUFSZ/2], CMPLEN);
//				printf ("CmpI2sBufToSaiBuf returned %d\r\n", ix);
				if (ix != CMPLEN) {
					printf ("CmpI2sBufToSaiBuf returned %U processing SaiA Rx%06U\r\n", ix, SaiABufCtl.nRecvProc/2);
					PUTZ_ASSERT(ix == CMPLEN);
				}
				#endif //CMP_S2I2_SAIA
			} else {
				#if ENABLE_DUMP
				char strbuf[32];
				sprintf (strbuf, "SaiA Rx%06lu.0 Er%lu ", SaiABufCtl.nRecvProc/2, SaiABufCtl.nErrorProc);
				DumpFWds ((uint32_t*)&SaiABuf[0],4,FALSE,strbuf);
				#endif //ENABLE_DUMP
				#if CMP_S2I2_SAIA
				uint32_t ix = CmpI2sBufToSaiBuf((uint32_t*)I2S2Buf, (uint32_t*)SaiABuf, CMPLEN);
//				printf ("CmpI2sBufToSaiBuf returned %lu\r\n", ix);
				if (ix != CMPLEN) {
					printf ("CmpI2sBufToSaiBuf returned %lu processing SaiA Rx%06lu\r\n", ix, SaiABufCtl.nRecvProc/2);
					PUTZ_ASSERT(ix == CMPLEN);
				}
				#endif //CMP_S2I2_SAIA
			}
		}
		SaiABufCtl.nRecvProc++;
	} else if (SaiABufCtl.nErrorProc != SaiABufCtl.nError) {
		SaiABufCtl.nErrorProc++;
		printf ("SaiA Er%04lu\r\n", SaiABufCtl.nErrorProc);
	}
	return TRUE;
}

I2sBufCtl_t SaiBBufCtl;
I2sData_t SaiBBuf[SAIBBUFSZ];
BOOL Putz01SaiBStart (void) {
	puts ("Putz01SaiBStart() Enter\r");
	HAL_StatusTypeDef sts = HAL_SAI_Receive_DMA(&hsai_BlockB1, (uint8_t *)&SaiBBuf, sizeof(SaiBBuf)/sizeof(SaiBBuf[0]));
	PUTZ_ASSERT(sts==HAL_OK);
	return TRUE;
}

BOOL Putz01SaiBProc (void) {
	if (SaiBBufCtl.nRecvProc != SaiBBufCtl.nRecv) {
		if (SaiBBufCtl.nErrorProc != SaiBBufCtl.nError) {
				SaiBBufCtl.nErrorProc++;
		}
		if (PROC_ALLBUFS || !(SaiBBufCtl.nRecvProc & 0x000e)) {
			if (SaiBBufCtl.nRecvProc & 1) {
				#if ENABLE_DUMP
				char strbuf[32];
				sprintf (strbuf, "SaiB Rx%06lu.5 Er%lu ", SaiBBufCtl.nRecvProc/2, SaiBBufCtl.nErrorProc);
				DumpFWds ((uint32_t*)&SaiBBuf[SAIBBUFSZ/2],4,FALSE,strbuf);
				#endif //ENABLE_DUMP
				#if CMP_S2I2_SAIB
				uint32_t ix = CmpI2sBufToSaiBuf((uint32_t*)&I2S2Buf[I2S2BUFSZ/2], (uint32_t*)&SaiBBuf[SAIBBUFSZ/2], CMPLEN);
//				printf ("CmpI2sBufToSaiBuf returned %lu\r\n", ix);
				if (ix != CMPLEN) {
					printf ("CmpI2sBufToSaiBuf returned %lu processing SaiB Rx%06lu\r\n", ix, SaiBBufCtl.nRecvProc/2);
					PUTZ_ASSERT(ix == CMPLEN);
				}
				#endif //CMP_S2I2_SAIB
			} else {
				#if ENABLE_DUMP
				char strbuf[32];
				sprintf (strbuf, "SaiB Rx%06lu.0 Er%lu ", SaiBBufCtl.nRecvProc/2, SaiBBufCtl.nErrorProc);
				DumpFWds ((uint32_t*)&SaiBBuf[0],4,FALSE,strbuf);
				#endif //ENABLE_DUMP
				#if CMP_S2I2_SAIB
				uint32_t ix = CmpI2sBufToSaiBuf((uint32_t*)I2S2Buf, (uint32_t*)SaiBBuf, CMPLEN);
//				printf ("CmpI2sBufToSaiBuf returned %lu\r\n", ix);
				if (ix != CMPLEN) {
					printf ("CmpI2sBufToSaiBuf returned %lu processing SaiB Rx%06lu\r\n", ix, SaiBBufCtl.nRecvProc/2);
					PUTZ_ASSERT(ix == CMPLEN);
				}
				#endif //CMP_S2I2_SAIB
				}
		}
		SaiBBufCtl.nRecvProc++;
	} else if (SaiBBufCtl.nErrorProc != SaiBBufCtl.nError) {
		SaiBBufCtl.nErrorProc++;
		printf ("SaiB Er%04lu\r\n", SaiBBufCtl.nErrorProc);
	}
	return TRUE;
}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
	HAL_SAI_RxCpltCallback(hsai);
}
void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai) {
	if (hsai == &hsai_BlockA1) {
		SaiABufCtl.nRecv++;
	} else if (hsai == &hsai_BlockB1) {
		SaiBBufCtl.nRecv++;
	} else {
		PUTZ_ASSERT(FALSE);
	}
}

void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai) {
	if (hsai == &hsai_BlockA1) {
		SaiABufCtl.nError++;
	} else if (hsai == &hsai_BlockB1) {
		SaiBBufCtl.nError++;
	} else {
		PUTZ_ASSERT(FALSE);
	}
}

void AP_WriteBuffer(uint8_t *aTxBuffer, uint8_t TXBUFFERSIZE) 
{
    /* -> Start the transmission process */
    /* While the I2C in reception process, user can transmit data through "aTxBuffer" buffer */
    while(HAL_I2C_Master_Transmit(&AP_I2C_HANDLE, AP_I2C_ADDR, (uint8_t*)aTxBuffer, (uint16_t)TXBUFFERSIZE, (uint32_t)1000)!= HAL_OK)
    {
        /*
         * Error_Handler() function is called when Timeout error occurs.
         * When Acknowledge failure occurs (Slave don't acknowledge it's address)
         * Master restarts communication
         */
 
        if (HAL_I2C_GetError(&AP_I2C_HANDLE) != HAL_I2C_ERROR_AF)
        {
            //DEBUG(3, "In I2C::WriteBuffer -> error");
            //Error_Handler(3);
        }
 
    }
 
    /* -> Wait for the end of the transfer */
    /* Before starting a new communication transfer, you need to check the current
     * state of the peripheral; if it�s busy you need to wait for the end of current
     * transfer before starting a new one.
     * For simplicity reasons, this example is just waiting till the end of the
     * transfer, but application may perform other tasks while transfer operation
     * is ongoing.
     */
      while (HAL_I2C_GetState(&AP_I2C_HANDLE) != HAL_I2C_STATE_READY)
      {
      }
}

void AP_ReadBuffer(uint8_t RegAddr, uint8_t *aRxBuffer, uint8_t RXBUFFERSIZE)
{
    /* -> Lets ask for register's address */
    AP_WriteBuffer(&RegAddr, 1);
 
    /* -> Put I2C peripheral in reception process */
    while(HAL_I2C_Master_Receive(&AP_I2C_HANDLE, AP_I2C_ADDR, aRxBuffer, (uint16_t)RXBUFFERSIZE, (uint32_t)1000) != HAL_OK)
    {
        /* Error_Handler() function is called when Timeout error occurs.
         * When Acknowledge failure occurs (Slave don't acknowledge it's address)
         * Master restarts communication
         */
        if (HAL_I2C_GetError(&AP_I2C_HANDLE) != HAL_I2C_ERROR_AF)
        {
            //DEBUG(3, "In I2C::WriteBuffer -> error");
            //Error_Handler(4);
					PUTZ_ASSERT(FALSE);
        }
    }
 
    /* -> Wait for the end of the transfer */
    /* Before starting a new communication transfer, you need to check the current
     * state of the peripheral; if it�s busy you need to wait for the end of current
     * transfer before starting a new one.
     * For simplicity reasons, this example is just waiting till the end of the
     * transfer, but application may perform other tasks while transfer operation
     * is ongoing.
     **/
    while (HAL_I2C_GetState(&AP_I2C_HANDLE) != HAL_I2C_STATE_READY)
    {
    }
}

