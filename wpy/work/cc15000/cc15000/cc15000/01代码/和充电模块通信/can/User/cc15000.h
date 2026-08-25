#ifndef __CC15000_H__
#define __CC15000_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
#include "stdbool.h"



typedef enum {
    MODULE_POWER_UNKNOWN = 0,
    MODULE_POWER_ON,
    MODULE_POWER_OFF
} ModulePowerState_t;

typedef struct
{
    uint32_t raw_status;

    uint8_t running;                    // bit0
    uint8_t fault;                      // bit1
    uint8_t alarm;                      // bit2
    uint8_t stop_fault;                 // bit3

    uint8_t output_ovp;                 // bit8 输出过压
    uint8_t output_ocp;                 // bit9 输出过流
    uint8_t output_short;               // bit10 输出短路
    uint8_t over_temperature;           // bit11 散热器过热
    uint8_t fan_fault;                  // bit12 风扇异常
    uint8_t input_or_share_fault;       // bit13 输入/均流异常
    uint8_t phase_loss;                 // bit14 输入缺相
    uint8_t pfc_fault;                  // bit15 PFC 升压异常
    uint8_t discharge_fault;            // bit16 放电异常
    uint8_t address_conflict;           // bit17 地址冲突
    uint8_t fan2_fault;                 // bit18 风扇2异常
    uint8_t communication_fault;        // bit19 通讯异常
    uint8_t battery_reverse;            // bit20 电池反接保护
} ModuleFaultStatus_t;

typedef struct {
    volatile ModulePowerState_t power_state;

    volatile uint32_t set_voltage_mv;
    volatile uint32_t set_current_ma;

    volatile uint32_t output_voltage_mv;
    volatile uint32_t output_current_ma;
		volatile float temp;

    volatile bool power_ack;
    volatile bool set_voltage_ack;
    volatile bool set_current_ack;
    volatile bool voltage_updated;
    volatile bool current_updated;
		volatile bool temp_updated;
	
		volatile ModuleFaultStatus_t fault_status;
} ModuleStatus_t;




void CAN1_Receive(uint32_t can_id, const uint8_t *data, uint8_t len);
uint8_t FDCAN1_Send_ON(void);
uint8_t FDCAN1_Send_OFF(void);
uint8_t FDCAN1_Send_Vol(uint32_t voltage_mv);
uint8_t FDCAN1_Send_Cur(uint32_t current_ma);
uint8_t FDCAN1_Query_Voltage(void);
uint8_t FDCAN1_Query_Current(void);
uint8_t FDCAN1_Query_Temp(void);
uint8_t FDCAN1_Query_State(void);
void CAN2_Receive(uint32_t can_id, const uint8_t *data, uint8_t len);
#ifdef __cplusplus
}
#endif
#endif

