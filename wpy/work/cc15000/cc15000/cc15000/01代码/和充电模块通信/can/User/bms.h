#ifndef __BMS_H__
#define __BMS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <stdbool.h>
typedef struct {//当 BMS 收到下电指令执行下电后, 需要收到上电指令后才会重新上电
    uint8_t High_voltage_power_on;//0：不操作1：高压上电2：高压下电
} BCUHigh_voltage_power;
typedef struct {//内部BMS放电广播数据
	struct {//BCU 广播报文1  ID:0x1801FFnn
    float total_voltage;      // 电池组总电压 (V)
    float total_current;      // 电池组总电流 (A)
    float soc;                // 电池组SOC (%)
    uint8_t fan_status;       // 风扇状态 (0:断开, 1:闭合)
    uint8_t hv_status;        // 高压闭合状态 (0:断开, 1:闭合)
    uint8_t discharge_allowed;// 允许放电 (0:允许, 1:禁止)
    uint8_t charge_allowed;   // 允许充电 (0:允许, 1:禁止)
    uint8_t operation_mode;   // 工作模式 (0:初始化, 1:待机, 2:充电, 3:放电)
    uint8_t heartbeat;        // BCU心跳
	}BCU_Broadcast1_t;
	struct {//BCU 广播报文2 ID:0x1802FFnn
    float total_capacity;     // 电池组总容量 (Ah)
    float remaining_capacity; // 电池组剩余容量 (Ah)
    float rated_energy;       // 电池组额定电量 (kWh)
    float remaining_energy;   // 电池组剩余电量 (kWh)
	} BCU_Broadcast2_t;
	struct {//BCU 广播报文3 ID:0x1803FFnn
    uint16_t total_cells;     		// 电芯总数
    uint16_t total_temp_sensors; 	// 温感总数
	} BCU_Broadcast3_t;
	struct {//BCU 广播报文4 ID:0x1804FFnn
    float MAX_Charging_current;     		//最大允许充电电流
    float MAX_DisCharging_current; 			// 最大允许放电电流
		float MAX_Charging_total_voltage;		//最大允许充电总压
		float MIN_DisCharging_total_voltage;//最大允许放电总压
	} BCU_Broadcast4_t;
	struct {//BCU 广播报文5 ID:0x1805FFnn
    float MAX_MonomerAllow_voltage;     			//最高允许单体电压
    float MIN_MonomerAllow_voltage; 		 			// 最低允许单体电压
		uint16_t Insulation_resistance;						//电池组绝缘电阻
		uint16_t Number_cycles;										//电池组循环次数
	} BCU_Broadcast5_t;
	struct {//BCU 广播报文6 ID:0x1806FFnn
    float MAX_Monomer_voltage;     			//最高单体电压
    float MIN_Monomer_voltage; 		 			// 最低单体电压
		float Monomer_pressure_difference;	//单体压差
		float Average_Monomer_voltage;			//平均单体电压
	} BCU_Broadcast6_t;
	struct {//BCU 广播报文7 ID:0x1807FFnn
    int8_t MAX_Monomer_Temperature;     			//最高单体温度
    int8_t MIN_Monomer_Temperature; 		 			// 最低单体温度
		uint8_t Monomer_Temperature_difference;		//单体温差
		int8_t Average_Monomer_Temperature;				//平均单体温度
		float Battery_pack_SOH;										//电池组SOH
		float Battery_pack_SOE;										//电池组SOE
	} BCU_Broadcast7_t;
	struct {//BCU 广播报文8 ID:0x1808FFnn
    int8_t MAX_Copper_busbar_Temperature;     			//最高铜排温度
    int8_t MIN_Copper_busbar_Temperature; 		 			// 最低铜排温度
		uint8_t Copper_busbar_Temperature_difference;		//铜排温差
		int8_t Average_Copper_busbar_Temperature;				//平均铜排温度
		uint8_t MAX_Copper_busbar_Temperature_BMU;			//最高铜排温度位置 BMU 序号
		uint8_t MAX_Copper_busbar_Temperature_TEMPSEN;	//最高铜排温度位置温感序号
	} BCU_Broadcast8_t;
	struct {//BCU 广播报文9 ID:0x1809FFnn
    uint16_t Maximum_single_voltage_number;     		//最高单体电压从机序号
    uint16_t Highest_individual_voltage_number; 		// 最高单体电压包内序号
		uint16_t Minimum_single_voltage_number;					//最低单体电压从机序号
		uint16_t Low_individual_voltage_number;					//最低单体电压包内序号
		uint16_t Maximum_single_Temperature_number;			//最高单体温度从机序号
		uint16_t Highest_individual_Temperature_number;	//最高单体温度包内序号
		uint16_t Minimum_single_Temperature_number;			//最低单体温度从机序号
		uint16_t Low_individual_Temperature_number;			//最低单体温度包内序号
	} BCU_Broadcast9_t;
	struct {//BCU广播报文10和11  ID:0x180AFFnn和0x180BFFnn
    uint8_t level;            // 故障等级 (0:无故障, 1:轻微, 3:严重)// 故障信息结构体
    const char* description;  // 故障描述
	} BCU_FaultInfo_t;
	struct {//BCU广播报文12  ID:0x180CFFnn
    uint8_t Main_version;       //主版本
    uint8_t Next_version; 			//次版本
		uint8_t Revised_version;  	//修订版本
	} BCU_Broadcast12_t;
	struct {// 单体电压帧结构体
    uint32_t frame_id;        // CAN帧ID
    uint8_t bcu_address;      // BCU地址 (nn)
    uint16_t frame_index;     // 帧索引 (vv - 0x30)
    float cell_voltages[4];   // 4个单体电压值 (V)
    uint8_t valid_cells;      // 有效电芯数量 (1-4)
    bool is_valid;            // 数据是否有效
	} CellVoltageFrame_t;
	struct {// 单体温度帧结构体
    uint32_t frame_id;        // CAN帧ID
    uint8_t bcu_address;      // BCU地址 (nn)
    uint16_t frame_index;     // 帧索引 (tt - 0xB0)
    float cell_temperatures[8]; // 8个单体温度值 (°C)
    uint8_t valid_temps;      // 有效温度数量 (1-8)
    bool is_valid;            // 数据是否有效
	} CellTemperatureFrame_t;
	// 单体电压数据
	float cell_voltages[256];
	uint16_t cell_voltage_count;
	// 单体温度数据
	int8_t cell_temperatures[256];
	uint16_t cell_temp_count;
	// 广播报文10-11故障信息
  uint8_t fault_levels[65];        // 故障0-64的等级
}Internal_BMS_DATA;

void BCU_Data_Init(void);
void BCU_CAN_Receive(uint32_t can_id, const uint8_t* data, uint8_t len);
#ifdef __cplusplus
}
#endif
#endif

