#include "spi.h"
#include "main.h"
#include "stm32g4xx_hal_spi.h"

SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

/**
  * @brief SPI2初始化
  * SCK=PF9 AF5
  * MISO=PB14 AF5
  * MOSI=PB15 AF5
  */
void MX_SPI2_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_SPI2_CLK_ENABLE();

  /* PF9 SCK */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* PB14 MISO PB15 MOSI */
  GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI3初始化
  * SCK=PC10 AF6
  * MISO=PC11 AF6
  * MOSI=PC12 AF6
  * CS=PA15 软件片选
  */
void MX_SPI3_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_SPI3_CLK_ENABLE();

  /* PC10/11/12 SCK/MISO/MOSI */
  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* PA15 CS推挽输出 */
  GPIO_InitStruct.Pin = SPI3_CS3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI3_CS3_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(SPI3_CS3_PORT, SPI3_CS3_PIN, GPIO_PIN_SET);

  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* ---------------------- SPI2 读写接口 ---------------------- */
uint8_t SPI2_WriteReadByte(uint8_t tx)
{
  uint8_t rx = 0;
  HAL_SPI_TransmitReceive(&hspi2, &tx, &rx, 1, 100);
  return rx;
}

void SPI2_WriteBuf(uint8_t *tx_buf, uint16_t len)
{
  HAL_SPI_Transmit(&hspi2, tx_buf, len, 100);
}

void SPI2_ReadWriteBuf(uint8_t *tx, uint8_t *rx, uint16_t len)
{
  HAL_SPI_TransmitReceive(&hspi2, tx, rx, len, 100);
}

/* ---------------------- SPI3 CS控制与读写 ---------------------- */
void SPI3_CS_Low(void)
{
  HAL_GPIO_WritePin(SPI3_CS3_PORT, SPI3_CS3_PIN, GPIO_PIN_RESET);
}

void SPI3_CS_High(void)
{
  HAL_GPIO_WritePin(SPI3_CS3_PORT, SPI3_CS3_PIN, GPIO_PIN_SET);
}

uint8_t SPI3_WriteReadByte(uint8_t tx)
{
  uint8_t rx = 0;
  HAL_SPI_TransmitReceive(&hspi3, &tx, &rx, 1, 100);
  return rx;
}

void SPI3_WriteBuf(uint8_t *tx_buf, uint16_t len)
{
  HAL_SPI_Transmit(&hspi3, tx_buf, len, 100);
}

void SPI3_ReadWriteBuf(uint8_t *tx, uint8_t *rx, uint16_t len)
{
  HAL_SPI_TransmitReceive(&hspi3, tx, rx, len, 100);
}

