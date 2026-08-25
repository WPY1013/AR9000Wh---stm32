#ifndef __SPI_H
#define __SPI_H

#include "stm32g4xx_hal.h"

/* SPI2 硬件定义 */
extern SPI_HandleTypeDef hspi2;
void MX_SPI2_Init(void);
uint8_t SPI2_WriteReadByte(uint8_t tx);
void SPI2_WriteBuf(uint8_t *tx_buf, uint16_t len);
void SPI2_ReadWriteBuf(uint8_t *tx, uint8_t *rx, uint16_t len);

/* SPI3 硬件定义 */
extern SPI_HandleTypeDef hspi3;
#define SPI3_CS3_PIN     GPIO_PIN_15
#define SPI3_CS3_PORT    GPIOA
void MX_SPI3_Init(void);
void SPI3_CS_Low(void);
void SPI3_CS_High(void);
uint8_t SPI3_WriteReadByte(uint8_t tx);
void SPI3_WriteBuf(uint8_t *tx_buf, uint16_t len);
void SPI3_ReadWriteBuf(uint8_t *tx, uint8_t *rx, uint16_t len);


#endif
