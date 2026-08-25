#ifndef __FDCAN_H__
#define __FDCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#define FDCAN_MAX_RX_PER_CALL  8U
/* USER CODE END Includes */


/* USER CODE BEGIN Private defines */
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3;
extern volatile uint8_t FDCAN1_Rx_Flag;
extern volatile uint8_t FDCAN2_Rx_Flag;
extern volatile uint8_t FDCAN3_Rx_Flag;
/* USER CODE END Private defines */

void MX_FDCAN1_Init(void);
void MX_FDCAN2_Init(void);
void MX_FDCAN3_Init(void);
uint8_t FDCAN1_SendTestMsg(uint16_t std_id, const uint8_t* data, uint8_t len);
uint8_t FDCAN1_SendStdCanMsg(uint16_t std_id, const uint8_t* data, uint8_t len);
uint8_t FDCAN1_SendExtCanMsg(uint32_t ext_id, const uint8_t* data, uint8_t len);
uint8_t FDCAN2_SendTestMsg(uint16_t std_id, const uint8_t* data, uint8_t len);
uint8_t FDCAN2_SendStdCanMsg(uint16_t std_id, const uint8_t* data, uint8_t len);
uint8_t FDCAN2_SendExtCanMsg(uint32_t ext_id, const uint8_t* data, uint8_t len);
uint8_t FDCAN3_SendTestMsg(uint16_t std_id, const uint8_t* data, uint8_t len);
uint8_t FDCAN3_SendStdCanMsg(uint16_t std_id, const uint8_t* data, uint8_t len);
uint8_t FDCAN3_SendExtCanMsg(uint32_t ext_id, const uint8_t* data, uint8_t len);
HAL_StatusTypeDef FDCAN_StartAll(void);
void FDCAN_StopAll(void);
//uint8_t CAN_SendFrame2_Up(void);
//uint8_t CAN_SendFrame2_On(void);
//uint8_t CAN_SendFrame2_Off(void);
uint8_t CAN2_SendFrame3_Fixed(void);
uint8_t CAN3_SendFrame3_Fixed(void);
uint8_t CAN2_SendFrame4_UnDischarge(void);
uint8_t CAN3_SendFrame4_UnDischarge(void);
uint8_t CAN2_SendFrame5_Discharging(void);
uint8_t CAN3_SendFrame5_Discharging(void);
uint8_t CAN_SendBCU_DisCharge(uint8_t High_voltage_power_onorOFF);
void FDCAN1_ProcessRx(void);
void FDCAN2_ProcessRx(void);
void FDCAN3_ProcessRx(void);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __FDCAN_H__ */

