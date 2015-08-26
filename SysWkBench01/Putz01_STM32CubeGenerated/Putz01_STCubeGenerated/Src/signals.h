#include "arm_math.h"
#include "arm_const_structs.h"
#include "i2c_setup.h"
#include "Putz01.h"

#define TEST_LENGTH_SAMPLES 2048

extern void testFFT(q31_t *input1,q31_t *input2, uint16_t length);
extern void formatIncommingData( float32_t *leftBuffer, float32_t *rightBuffer, uint32_t *inputBuffer, uint16_t length);
float32_t complex_running_avg(uint32_t *input, uint16_t length, float32_t startRunAvg);
float32_t complex_variance(uint32_t *input, uint16_t length, float32_t average);
void genCrossCorr_q31(q31_t *inputFreqData1, q31_t *inputFreqData2, q31_t *inputFreqMag1,q31_t *inputFreqMag2, q31_t *output, uint16_t lengthInput);
void spectCentroid_q31(q31_t *inputFreqMag, q31_t *inputFreq, q31_t *outputCentroid, uint16_t lengthInput);
void centerFrequency_q31(q31_t *input, q31_t *output, uint16_t lengthInput, uint32_t Fs);
void fftShift_q31(q31_t *input, uint16_t length);
float32_t uint32TOfloat32(uint32_t input);
