#include "fdcan.h"
#include "stdbool.h"
#include "bms.h"
#include "cc15000.h"
// FDCAN句柄实体定义（CubeMX原本写在h，移到c）
FDCAN_HandleTypeDef hfdcan1;
FDCAN_HandleTypeDef hfdcan2;
FDCAN_HandleTypeDef hfdcan3;

volatile uint8_t FDCAN1_Rx_Flag = 0U;
volatile uint8_t FDCAN2_Rx_Flag = 0U;
volatile uint8_t FDCAN3_Rx_Flag = 0U;
// 用户全局变量实体定义，仅此处初始化
FDCAN_TxHeaderTypeDef TxHeader;

static FDCAN_RxHeaderTypeDef RxHeader1, RxHeader2;
static uint8_t RxData1[64], RxData2[64];

void MX_FDCAN1_Init(void)
{


  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 40;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 5;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 1;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	 if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
 /* 所有标准帧、扩展帧均进入 RX FIFO0，拒绝远程帧 */
		if (HAL_FDCAN_ConfigGlobalFilter(
						&hfdcan1,
						FDCAN_ACCEPT_IN_RX_FIFO0,
						FDCAN_ACCEPT_IN_RX_FIFO0,
						FDCAN_REJECT_REMOTE,
						FDCAN_REJECT_REMOTE) != HAL_OK)
		{
				Error_Handler();
		}

}

void MX_FDCAN2_Init(void)
{


  hfdcan2.Instance = FDCAN2;
  hfdcan2.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan2.Init.AutoRetransmission = DISABLE;
  hfdcan2.Init.TransmitPause = DISABLE;
  hfdcan2.Init.ProtocolException = DISABLE;
  hfdcan2.Init.NominalPrescaler = 40;
  hfdcan2.Init.NominalSyncJumpWidth = 1;
  hfdcan2.Init.NominalTimeSeg1 = 5;
  hfdcan2.Init.NominalTimeSeg2 = 2;
  hfdcan2.Init.DataPrescaler = 1;
  hfdcan2.Init.DataSyncJumpWidth = 1;
  hfdcan2.Init.DataTimeSeg1 = 1;
  hfdcan2.Init.DataTimeSeg2 = 1;
  hfdcan2.Init.StdFiltersNbr = 0;
  hfdcan2.Init.ExtFiltersNbr = 0;
  hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FDCAN_ConfigGlobalFilter(
        &hfdcan2,
        FDCAN_ACCEPT_IN_RX_FIFO0,
        FDCAN_ACCEPT_IN_RX_FIFO0,
        FDCAN_REJECT_REMOTE,
        FDCAN_REJECT_REMOTE) != HAL_OK)
	{
    Error_Handler();
	}

}

void MX_FDCAN3_Init(void)
{


  hfdcan3.Instance = FDCAN3;
  hfdcan3.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan3.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan3.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan3.Init.AutoRetransmission = DISABLE;
  hfdcan3.Init.TransmitPause = DISABLE;
  hfdcan3.Init.ProtocolException = DISABLE;
  hfdcan3.Init.NominalPrescaler = 40;
  hfdcan3.Init.NominalSyncJumpWidth = 1;
  hfdcan3.Init.NominalTimeSeg1 = 5;
  hfdcan3.Init.NominalTimeSeg2 = 2;
  hfdcan3.Init.DataPrescaler = 1;
  hfdcan3.Init.DataSyncJumpWidth = 1;
  hfdcan3.Init.DataTimeSeg1 = 1;
  hfdcan3.Init.DataTimeSeg2 = 1;
  hfdcan3.Init.StdFiltersNbr = 0;
  hfdcan3.Init.ExtFiltersNbr = 0;
  hfdcan3.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	if (HAL_FDCAN_Init(&hfdcan3) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_FDCAN_ConfigGlobalFilter(
        &hfdcan3,
        FDCAN_ACCEPT_IN_RX_FIFO0,
        FDCAN_ACCEPT_IN_RX_FIFO0,
        FDCAN_REJECT_REMOTE,
        FDCAN_REJECT_REMOTE) != HAL_OK)
	{
    Error_Handler();
	}
	
}

static uint32_t HAL_RCC_FDCAN_CLK_ENABLED=0;

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }


    HAL_RCC_FDCAN_CLK_ENABLED++;
    if(HAL_RCC_FDCAN_CLK_ENABLED==1){
      __HAL_RCC_FDCAN_CLK_ENABLE();
    }

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
		CANH3 、CANL3
    PD0     ------> FDCAN1_RX
    PD1     ------> FDCAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);

  }
  else if(fdcanHandle->Instance==FDCAN2)
  {

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* FDCAN2 clock enable */
    HAL_RCC_FDCAN_CLK_ENABLED++;
    if(HAL_RCC_FDCAN_CLK_ENABLED==1){
      __HAL_RCC_FDCAN_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**FDCAN2 GPIO Configuration
		CANH1 、CANL1
    PB12     ------> FDCAN2_RX
    PB13     ------> FDCAN2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* FDCAN2 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);

  }
  else if(fdcanHandle->Instance==FDCAN3)
  {

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* FDCAN3 clock enable */
    HAL_RCC_FDCAN_CLK_ENABLED++;
    if(HAL_RCC_FDCAN_CLK_ENABLED==1){
      __HAL_RCC_FDCAN_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**FDCAN3 GPIO Configuration
		CANH2 、CANL2
    PB3     ------> FDCAN3_RX
    PB4     ------> FDCAN3_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_FDCAN3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* FDCAN3 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN3_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN3_IT0_IRQn);

  }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {

    HAL_RCC_FDCAN_CLK_ENABLED--;
    if(HAL_RCC_FDCAN_CLK_ENABLED==0){
      __HAL_RCC_FDCAN_CLK_DISABLE();
    }

    /**FDCAN1 GPIO Configuration
		CANL3\CANH3
    PD0     ------> FDCAN1_RX
    PD1     ------> FDCAN1_TX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);

  }
  else if(fdcanHandle->Instance==FDCAN2)
  {

    HAL_RCC_FDCAN_CLK_ENABLED--;
    if(HAL_RCC_FDCAN_CLK_ENABLED==0){
      __HAL_RCC_FDCAN_CLK_DISABLE();
    }

    /**FDCAN2 GPIO Configuration
		CANL1/CANH1
    PB12     ------> FDCAN2_RX
    PB13     ------> FDCAN2_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);


    HAL_NVIC_DisableIRQ(FDCAN2_IT0_IRQn);

  }
  else if(fdcanHandle->Instance==FDCAN3)
  {

    HAL_RCC_FDCAN_CLK_ENABLED--;
    if(HAL_RCC_FDCAN_CLK_ENABLED==0){
      __HAL_RCC_FDCAN_CLK_DISABLE();
    }

    /**FDCAN3 GPIO Configuration
		CANL2/CANH2
    PB3     ------> FDCAN3_RX
    PB4     ------> FDCAN3_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4);

    /* FDCAN3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN3_IT0_IRQn);

  }
}

uint8_t FDCAN1_SendTestMsg(uint16_t std_id, const uint8_t* data, uint8_t len)
{
    // 标准CAN2.0数据帧配置
    TxHeader.Identifier = std_id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;    // 关闭CAN FD波特率切换，兼容普通CAN
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;     // 使用经典CAN2.0，不用FD帧
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;
		// 限制长度0~8，自动匹配DLC
    switch(len)
    {
        case 0: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
        case 1: TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2: TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3: TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4: TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5: TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6: TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7: TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8:
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
    }
    if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, data) == HAL_OK)
    {
        return 0;// 发送入队成功
    }
		return 1;
}
/**
 * @brief FDCAN1 通用发送函数，经典CAN2.0标准帧
 * @param std_id 11位标准CAN ID 0~0x7FF
 * @param data 待发送数据缓冲区指针
 * @param len 数据长度 0~8
 * @retval uint8_t 0=发送入队成功  1=失败
 */
uint8_t FDCAN1_SendStdCanMsg(uint16_t std_id, const uint8_t* data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    // 基础固定配置：经典CAN2.0，关闭FD/BRS
    TxHeader.Identifier = std_id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    // 限制长度0~8，自动匹配DLC
    switch(len)
    {
        case 0: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
        case 1: TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2: TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3: TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4: TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5: TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6: TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7: TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8:
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
    }

    // 压入发送FIFO
    if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, data) == HAL_OK)
    {
        return 0; // 成功
    }
    return 1; // FIFO满/外设未启动，发送失败
}
/**
 * @brief FDCAN1 29位扩展CAN2.0报文发送
 * @param ext_id 29位扩展ID
 * @param data 数据指针
 * @param len 长度0~8
 * @retval 0成功 1失败
 */
uint8_t FDCAN1_SendExtCanMsg(uint32_t ext_id, const uint8_t* data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;
    TxHeader.Identifier = ext_id;
    TxHeader.IdType = FDCAN_EXTENDED_ID; // 扩展ID
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    switch(len)
    {
        case 0: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
        case 1: TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2: TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3: TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4: TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5: TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6: TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7: TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8:
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
    }

    if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, data) == HAL_OK)
        return 0;
    return 1;
}
uint8_t FDCAN2_SendTestMsg(uint16_t std_id, const uint8_t* data, uint8_t len)
{
    // 标准CAN2.0数据帧配置
    TxHeader.Identifier = std_id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;    // 关闭CAN FD波特率切换，兼容普通CAN
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;     // 使用经典CAN2.0，不用FD帧
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;
	// 限制长度0~8，自动匹配DLC
    switch(len)
    {
        case 0: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
        case 1: TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2: TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3: TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4: TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5: TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6: TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7: TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8:
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
    }
    if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, data) == HAL_OK)
    {
        return 0; // 发送入队成功
    }
		return 1;
}
uint8_t FDCAN2_SendStdCanMsg(uint16_t std_id, const uint8_t* data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    // 基础固定配置：经典CAN2.0，关闭FD/BRS
    TxHeader.Identifier = std_id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    // 限制长度0~8，自动匹配DLC
    switch(len)
    {
        case 0: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
        case 1: TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2: TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3: TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4: TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5: TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6: TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7: TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8:
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
    }

    // 压入发送FIFO
    if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, data) == HAL_OK)
    {
        return 0; // 成功
    }
    return 1; // FIFO满/外设未启动，发送失败
}
uint8_t FDCAN2_SendExtCanMsg(uint32_t ext_id, const uint8_t* data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;
    TxHeader.Identifier = ext_id;
    TxHeader.IdType = FDCAN_EXTENDED_ID; // 扩展ID
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    switch(len)
    {
        case 0: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
        case 1: TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2: TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3: TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4: TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5: TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6: TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7: TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8:
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
    }

    if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, data) == HAL_OK)
        return 0;
    return 1;
}
uint8_t FDCAN3_SendTestMsg(uint16_t std_id, const uint8_t* data, uint8_t len)
{
    // 标准CAN2.0数据帧配置
    TxHeader.Identifier = std_id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;    // 关闭CAN FD波特率切换，兼容普通CAN
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;     // 使用经典CAN2.0，不用FD帧
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;
		// 限制长度0~8，自动匹配DLC
    switch(len)
    {
        case 0: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
        case 1: TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2: TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3: TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4: TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5: TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6: TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7: TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8:
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
    }
    if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &TxHeader, data) == HAL_OK)
    {
        return 0; // 成功
    }
		return 1;
}
uint8_t FDCAN3_SendStdCanMsg(uint16_t std_id, const uint8_t* data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    // 基础固定配置：经典CAN2.0，关闭FD/BRS
    TxHeader.Identifier = std_id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    // 限制长度0~8，自动匹配DLC
    switch(len)
    {
        case 0: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
        case 1: TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2: TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3: TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4: TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5: TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6: TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7: TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8:
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
    }

    // 压入发送FIFO
    if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &TxHeader, data) == HAL_OK)
    {
        return 0; // 成功
    }
    return 1; // FIFO满/外设未启动，发送失败
}
uint8_t FDCAN3_SendExtCanMsg(uint32_t ext_id, const uint8_t* data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;
    TxHeader.Identifier = ext_id;
    TxHeader.IdType = FDCAN_EXTENDED_ID; // 扩展ID
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    switch(len)
    {
        case 0: TxHeader.DataLength = FDCAN_DLC_BYTES_0; break;
        case 1: TxHeader.DataLength = FDCAN_DLC_BYTES_1; break;
        case 2: TxHeader.DataLength = FDCAN_DLC_BYTES_2; break;
        case 3: TxHeader.DataLength = FDCAN_DLC_BYTES_3; break;
        case 4: TxHeader.DataLength = FDCAN_DLC_BYTES_4; break;
        case 5: TxHeader.DataLength = FDCAN_DLC_BYTES_5; break;
        case 6: TxHeader.DataLength = FDCAN_DLC_BYTES_6; break;
        case 7: TxHeader.DataLength = FDCAN_DLC_BYTES_7; break;
        case 8:
        default: TxHeader.DataLength = FDCAN_DLC_BYTES_8; break;
    }

    if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &TxHeader, data) == HAL_OK)
        return 0;
    return 1;
}
HAL_StatusTypeDef FDCAN_StartAll(void)
{
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
        return HAL_ERROR;

    if (HAL_FDCAN_ActivateNotification(&hfdcan1,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0U) != HAL_OK)
        return HAL_ERROR;

//    if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK)
//        return HAL_ERROR;

//    if (HAL_FDCAN_ActivateNotification(&hfdcan2,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0U) != HAL_OK)
//        return HAL_ERROR;

//    if (HAL_FDCAN_Start(&hfdcan3) != HAL_OK)
//        return HAL_ERROR;

//    if (HAL_FDCAN_ActivateNotification(&hfdcan3,FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0U) != HAL_OK)
//        return HAL_ERROR;

    return HAL_OK;
}
void FDCAN_StopAll(void)
{
	HAL_FDCAN_Stop(&hfdcan1);
	HAL_FDCAN_Stop(&hfdcan2);
	HAL_FDCAN_Stop(&hfdcan3);
}

/**
 * @brief CAN2发送帧3固定持续报文 ID=0x18FE57F4
 * @retval TRUE成功 / FALSE失败
 */
uint8_t CAN2_SendFrame3_Fixed(void)
{
    uint8_t tx_buf[8] = {0xE3,0x02,0xDF,0x00,0xFF,0xFF,0xFF,0xFF};
    if(FDCAN2_SendExtCanMsg(0x18FE57F4, tx_buf, 8) == 0)
    {
        return true;
    }
    return false;
}
uint8_t CAN3_SendFrame3_Fixed(void)
{
    uint8_t tx_buf[8] = {0xE3,0x02,0xDF,0x00,0xFF,0xFF,0xFF,0xFF};
    if(FDCAN3_SendExtCanMsg(0x18FE57F4, tx_buf, 8) == 0)
    {
        return true;
    }
    return false;
}
/**
 * @brief CAN2发送帧4 未进入放电状态报文 ID=0x18FF57F4
 * @retval TRUE成功 / FALSE失败
 */
uint8_t CAN2_SendFrame4_UnDischarge(void)
{
    uint8_t tx_buf[8] = {0x86,0x96,0xA0,0x0F,0x00,0x00,0xF0,0xF1};
    if(FDCAN2_SendExtCanMsg(0x18FF57F4, tx_buf, 8) == 0)
    {
        return true;
    }
    return false;
}
uint8_t CAN3_SendFrame4_UnDischarge(void)
{
    uint8_t tx_buf[8] = {0x86,0x96,0xA0,0x0F,0x00,0x00,0xF0,0xF1};
    if(FDCAN3_SendExtCanMsg(0x18FF57F4, tx_buf, 8) == 0)
    {
        return true;
    }
    return false;
}
/**
 * @brief CAN2发送帧5 放电过程报文 ID=0x18FF57F4
 * @retval TRUE成功 / FALSE失败
 */
uint8_t CAN2_SendFrame5_Discharging(void)
{
    uint8_t tx_buf[8] = {0x85,0xB6,0x07,0x12,0x3B,0x2D,0xF7,0xF0};
    if(FDCAN2_SendExtCanMsg(0x18FF57F4, tx_buf, 8) == 0)
    {
        return true;
    }
    return false;
}
uint8_t CAN3_SendFrame5_Discharging(void)
{
    uint8_t tx_buf[8] = {0x85,0xB6,0x07,0x12,0x3B,0x2D,0xF7,0xF0};
    if(FDCAN3_SendExtCanMsg(0x18FF57F4, tx_buf, 8) == 0)
    {
        return true;
    }
    return false;
}
BCUHigh_voltage_power BCU_highVpower;//放电指令结构体

//发送高压上下电指令到内部BMS(按下屏幕放电按钮之后再发送,放电或者停止放电)
uint8_t CAN_SendBCU_DisCharge(uint8_t High_voltage_power_onorOFF)
{
	uint8_t txData[8] = {0};
	// 第0字节存放上下电指令，其余字节默认0
	txData[0] = High_voltage_power_onorOFF;
	/*
	txData[1]~txData[7] 初始化已经为0，无需重复赋值
	0：不操作  1：高压上电  2：高压下电
	*/

	// 调用FDCAN1扩展帧发送函数，ID=0x1800FFF4，长度8字节
	if (FDCAN1_SendExtCanMsg(0x1800FFF4, txData, 8) == 0)
	{
		return true;  // 返回0代表发送入队成功
	}
	else
	{
		return false; // 返回1代表发送失败
	}
}

static uint8_t FDCAN_DlcToByteLen(uint32_t dlc_code)
{
    switch(dlc_code)
    {
        case FDCAN_DLC_BYTES_0:  return 0;
        case FDCAN_DLC_BYTES_1:  return 1;
        case FDCAN_DLC_BYTES_2:  return 2;
        case FDCAN_DLC_BYTES_3:  return 3;
        case FDCAN_DLC_BYTES_4:  return 4;
        case FDCAN_DLC_BYTES_5:  return 5;
        case FDCAN_DLC_BYTES_6:  return 6;
        case FDCAN_DLC_BYTES_7:  return 7;
        case FDCAN_DLC_BYTES_8:  return 8;
        case FDCAN_DLC_BYTES_12: return 12;
        case FDCAN_DLC_BYTES_16: return 16;
        case FDCAN_DLC_BYTES_20: return 20;
        case FDCAN_DLC_BYTES_24: return 24;
        case FDCAN_DLC_BYTES_32: return 32;
        case FDCAN_DLC_BYTES_48: return 48;
        case FDCAN_DLC_BYTES_64: return 64;
        default: return 0;
    }
}

void FDCAN1_ProcessRx(void)
{
    uint32_t processed = 0U;
    FDCAN1_Rx_Flag = 0U;//先清零。如果处理期间又收到报文，中断会重新将它置为 1。
    while ((HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1,FDCAN_RX_FIFO0) > 0U) &&(processed < FDCAN_MAX_RX_PER_CALL))
    {
        if (HAL_FDCAN_GetRxMessage(&hfdcan1,FDCAN_RX_FIFO0,&RxHeader1,RxData1) != HAL_OK)
        {
            break;
        }
        CAN1_Receive(RxHeader1.Identifier,RxData1,FDCAN_DlcToByteLen(RxHeader1.DataLength));
        processed++;
    }
    if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1,FDCAN_RX_FIFO0) > 0U)//本次限制处理8帧。如果FIFO还有数据，保持标志有效，让下一轮主循环继续处理。
    {
        FDCAN1_Rx_Flag = 1U;
    }
}
void FDCAN2_ProcessRx(void)
{
    uint32_t processed = 0U;
    FDCAN2_Rx_Flag = 0U;//先清零。如果处理期间又收到报文，中断会重新将它置为 1。
    while ((HAL_FDCAN_GetRxFifoFillLevel(&hfdcan2,FDCAN_RX_FIFO0) > 0U) &&(processed < FDCAN_MAX_RX_PER_CALL))
    {
        if (HAL_FDCAN_GetRxMessage(&hfdcan2,FDCAN_RX_FIFO0,&RxHeader2,RxData2) != HAL_OK)
        {
            break;
        }
        CAN2_Receive(RxHeader2.Identifier,RxData2,FDCAN_DlcToByteLen(RxHeader2.DataLength));
        processed++;
    }
    if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan2,FDCAN_RX_FIFO0) > 0U)//本次限制处理8帧。如果FIFO还有数据，保持标志有效，让下一轮主循环继续处理。
    {
        FDCAN2_Rx_Flag = 1U;
    }
}
//void FDCAN3_ProcessRx(void)
//{
//    uint32_t processed = 0U;
//    FDCAN3_Rx_Flag = 0U;//先清零。如果处理期间又收到报文，中断会重新将它置为 1。
//    while ((HAL_FDCAN_GetRxFifoFillLevel(&hfdcan3,FDCAN_RX_FIFO0) > 0U) &&(processed < FDCAN_MAX_RX_PER_CALL))
//    {
//        if (HAL_FDCAN_GetRxMessage(&hfdcan3,FDCAN_RX_FIFO0,&RxHeader3,RxData3) != HAL_OK)
//        {
//            break;
//        }
//        Seat2_CC15000_CAN_Receive(RxHeader3.Identifier,RxData3,FDCAN_DlcToByteLen(RxHeader3.DataLength));
//        processed++;
//    }
//    if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan3,FDCAN_RX_FIFO0) > 0U)//本次限制处理8帧。如果FIFO还有数据，保持标志有效，让下一轮主循环继续处理。
//    {
//        FDCAN3_Rx_Flag = 1U;
//    }
//}
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
    {
        if(hfdcan->Instance == FDCAN1)
        {
            FDCAN1_Rx_Flag = 1U;
        }
				if(hfdcan->Instance == FDCAN2)
        {
            FDCAN2_Rx_Flag = 1U;
        }
//				if(hfdcan->Instance == FDCAN3)
//        {
//            FDCAN3_Rx_Flag = 1U;
//        }
    }
}
