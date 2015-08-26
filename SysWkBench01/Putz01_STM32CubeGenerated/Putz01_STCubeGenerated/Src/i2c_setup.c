#include "i2c_setup.h"

void I2C2_WriteBuffer(uint8_t I2C_ADDRESS, uint8_t *aTxBuffer, uint16_t TXBUFFERSIZE, I2C_HandleTypeDef hi2c2)
{
    /* -> Start the transmission process */
    /* While the I2C in reception process, user can transmit data through "aTxBuffer" buffer */
    while(HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)I2C_ADDRESS<<1, (uint8_t*)aTxBuffer, (uint16_t)TXBUFFERSIZE, (uint32_t)1000)!= HAL_OK)
    {
        /*
         * Error_Handler() function is called when Timeout error occurs.
         * When Acknowledge failure occurs (Slave don't acknowledge it's address)
         * Master restarts communication
         */

        if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
        {
            //DEBUG(3, "In I2C::WriteBuffer -> error");
            //Error_Handler(3);
        }

    }

    /* -> Wait for the end of the transfer */
    /* Before starting a new communication transfer, you need to check the current
     * state of the peripheral; if it’s busy you need to wait for the end of current
     * transfer before starting a new one.
     * For simplicity reasons, this example is just waiting till the end of the
     * transfer, but application may perform other tasks while transfer operation
     * is ongoing.
     */
      while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
      {
      }
}

void I2C2_ReadBuffer(uint8_t I2C_ADDRESS, uint8_t RegAddr, uint8_t *aRxBuffer, uint16_t RXBUFFERSIZE, I2C_HandleTypeDef hi2c2)
{
    /* -> Lets ask for register's address */
    I2C2_WriteBuffer(I2C_ADDRESS, &RegAddr, 1, hi2c2);

    /* -> Put I2C peripheral in reception process */
    while(HAL_I2C_Master_Receive(&hi2c2, (uint16_t)I2C_ADDRESS<<1, aRxBuffer, (uint16_t)RXBUFFERSIZE, (uint32_t)1000) != HAL_OK)
    {
        /* Error_Handler() function is called when Timeout error occurs.
         * When Acknowledge failure occurs (Slave don't acknowledge it's address)
         * Master restarts communication
         */
        if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
        {
            //DEBUG(3, "In I2C::WriteBuffer -> error");
            //Error_Handler(4);
        }
    }

    /* -> Wait for the end of the transfer */
    /* Before starting a new communication transfer, you need to check the current
     * state of the peripheral; if it’s busy you need to wait for the end of current
     * transfer before starting a new one.
     * For simplicity reasons, this example is just waiting till the end of the
     * transfer, but application may perform other tasks while transfer operation
     * is ongoing.
     **/
    while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY)
    {
    }
}
