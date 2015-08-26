#include "stm32f4xx_hal.h"

void I2C2_WriteBuffer(uint8_t I2C_ADDRESS, uint8_t *aTxBuffer, uint16_t TXBUFFERSIZE, I2C_HandleTypeDef hi2c2);
void I2C2_ReadBuffer(uint8_t I2C_ADDRESS, uint8_t RegAddr, uint8_t *aRxBuffer, uint16_t RXBUFFERSIZE, I2C_HandleTypeDef hi2c2);
