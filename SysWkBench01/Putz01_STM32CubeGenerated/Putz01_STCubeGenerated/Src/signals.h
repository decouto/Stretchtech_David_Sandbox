#include "arm_math.h"
#include "arm_const_structs.h"
#include "i2c_setup.h"
#include "Putz01.h"

#define TEST_LENGTH_SAMPLES 2048

extern void testFFT(float32_t *input1,float32_t *input2, uint16_t length);
extern void formatIncommingData( float32_t *leftBuffer, float32_t *rightBuffer, uint32_t *inputBuffer, uint16_t length);
float32_t complex_running_avg(uint32_t *input, uint16_t length, float32_t startRunAvg);
float32_t complex_variance(uint32_t *input, uint16_t length, float32_t average);
void genCrossCorr_f32(float32_t *inputFreqData1, float32_t *inputFreqData2, float32_t *inputFreqMag1,float32_t *inputFreqMag2, float32_t *output, uint16_t lengthInput);
void spectCentroid_f32(float32_t *inputFreqMag, float32_t *inputFreq, float32_t *outputCentroid, uint16_t lengthInput);
void centerFrequency_f32(float32_t *input, float32_t *output, uint16_t lengthInput, uint32_t Fs);
void fftShift_f32(float32_t *input, uint16_t length);
float32_t uint32TOfloat32(uint32_t input);
