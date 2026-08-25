#include "bms.h"
#include "string.h"
Internal_BMS_DATA bmsdata = {0};

// 解析BMS广播报文1
void parse_broadcast1(const uint8_t* data , uint8_t len) {
	
	if(len != 8){// 检查数据长度
		return;
	}
	uint16_t raw_voltage = (data[1] << 8) | data[0];                                     
	uint16_t raw_current = (data[3] << 8) | data[2];                                      
	uint16_t raw_soc = (data[5] << 8) | data[4];
	bmsdata.BCU_Broadcast1_t.total_voltage = raw_voltage * 0.1f;                    // 电池组总电压 (V)
	bmsdata.BCU_Broadcast1_t.total_current = raw_current * 0.1f - 3200.0f;          // 电池组总电流 (A)  
	bmsdata.BCU_Broadcast1_t.soc = raw_soc;                                  // 电池组SOC (%)
	uint8_t status = data[6];                                                                // 风扇状态 (0:断开, 1:闭合)
	bmsdata.BCU_Broadcast1_t.fan_status = (status >> 7) & 0x01;                     // 高压闭合状态 (0:断开, 1:闭合)
	bmsdata.BCU_Broadcast1_t.hv_status = (status >> 6) & 0x01;                      // 允许放电 (0:允许, 1:禁止)
	bmsdata.BCU_Broadcast1_t.discharge_allowed = (status >> 5) & 0x01;              // 允许充电 (0:允许, 1:禁止)
	bmsdata.BCU_Broadcast1_t.charge_allowed = (status >> 4) & 0x01;                 // 工作模式 (0:初始化, 1:待机, 2:充电, 3:放电)
	bmsdata.BCU_Broadcast1_t.operation_mode = status & 0x0F;                        // BCU心跳
	bmsdata.BCU_Broadcast1_t.heartbeat = data[7];
}
// 解析BMS广播报文2
void parse_broadcast2(const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	uint16_t raw_total_cap = (data[1] << 8) | data[0];
	uint16_t raw_remain_cap = (data[3] << 8) | data[2];
	uint16_t raw_rated_energy = (data[5] << 8) | data[4];
	uint16_t raw_remain_energy = (data[7] << 8) | data[6];
	bmsdata.BCU_Broadcast2_t.total_capacity = raw_total_cap * 0.1f;									// 电池组总容量 (Ah)
	bmsdata.BCU_Broadcast2_t.remaining_capacity = raw_remain_cap * 0.1f;             // 电池组剩余容量 (Ah)
	bmsdata.BCU_Broadcast2_t.rated_energy = raw_rated_energy * 0.1f;                 // 电池组额定电量 (kWh)
	bmsdata.BCU_Broadcast2_t.remaining_energy = raw_remain_energy * 0.1f;            // 电池组剩余电量 (kWh)
}
// 解析BMS广播报文3
void parse_broadcast3(const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	bmsdata.BCU_Broadcast3_t.total_cells = (data[1] << 8) | data[0];								// 电芯总数
	bmsdata.BCU_Broadcast3_t.total_temp_sensors = (data[3] << 8) | data[2];        // 温感总数
}
// 解析BMS广播报文4
void parse_broadcast4(const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	uint16_t raw_max_charge_current = (data[1] << 8) | data[0];
	uint16_t raw_max_discharge_current = (data[3] << 8) | data[2];
	uint16_t raw_max_charge_voltage = (data[5] << 8) | data[4];
	uint16_t raw_min_discharge_voltage = (data[7] << 8) | data[6];
	bmsdata.BCU_Broadcast4_t.MAX_Charging_current = raw_max_charge_current * 0.1f;								//最大允许充电电流
	bmsdata.BCU_Broadcast4_t.MAX_DisCharging_current = raw_max_discharge_current * 0.1f;         //最大允许放电电流
	bmsdata.BCU_Broadcast4_t.MAX_Charging_total_voltage = raw_max_charge_voltage * 0.1f;         //最大允许充电总压
	bmsdata.BCU_Broadcast4_t.MIN_DisCharging_total_voltage = raw_min_discharge_voltage * 0.1f;   //最大允许放电总压
}
// 解析BMS广播报文5
void parse_broadcast5(const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	uint16_t raw_max_cell_voltage = (data[1] << 8) | data[0];
	uint16_t raw_min_cell_voltage = (data[3] << 8) | data[2];
	bmsdata.BCU_Broadcast5_t.MAX_MonomerAllow_voltage = raw_max_cell_voltage * 0.001f;		//最高允许单体电压
	bmsdata.BCU_Broadcast5_t.MIN_MonomerAllow_voltage = raw_min_cell_voltage * 0.001f;   // 最低允许单体电压
	bmsdata.BCU_Broadcast5_t.Insulation_resistance = (data[5] << 8) | data[4];           //电池组绝缘电阻
	bmsdata.BCU_Broadcast5_t.Number_cycles = (data[7] << 8) | data[6];                   //电池组循环次数
}
// 解析BMS广播报文6
void parse_broadcast6(const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	uint16_t raw_max_voltage = (data[1] << 8) | data[0];
	uint16_t raw_min_voltage = (data[3] << 8) | data[2];
	uint16_t raw_voltage_diff = (data[5] << 8) | data[4];
	uint16_t raw_avg_voltage = (data[7] << 8) | data[6];
	bmsdata.BCU_Broadcast6_t.MAX_Monomer_voltage = raw_max_voltage * 0.001f;						//最高单体电压
	bmsdata.BCU_Broadcast6_t.MIN_Monomer_voltage = raw_min_voltage * 0.001f;           // 最低单体电压
	bmsdata.BCU_Broadcast6_t.Monomer_pressure_difference = raw_voltage_diff * 0.001f;  //单体压差
	bmsdata.BCU_Broadcast6_t.Average_Monomer_voltage = raw_avg_voltage * 0.001f;       //平均单体电压
}
// 解析BMS广播报文7
void parse_broadcast7(const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	bmsdata.BCU_Broadcast7_t.MAX_Monomer_Temperature = data[0] - 50;				//最高单体温度
	bmsdata.BCU_Broadcast7_t.MIN_Monomer_Temperature = data[1] - 50;       // 最低单体温度
	bmsdata.BCU_Broadcast7_t.Monomer_Temperature_difference = data[2];     //单体温差
	bmsdata.BCU_Broadcast7_t.Average_Monomer_Temperature = data[3] - 50;   //平均单体温度
	bmsdata.BCU_Broadcast7_t.Battery_pack_SOH = data[4];                   //电池组SOH
	bmsdata.BCU_Broadcast7_t.Battery_pack_SOE = data[5];                   //电池组SOE
}
// 解析BMS广播报文8
void parse_broadcast8(const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	bmsdata.BCU_Broadcast8_t.MAX_Copper_busbar_Temperature = data[0] - 50;			//最高铜排温度
	bmsdata.BCU_Broadcast8_t.MIN_Copper_busbar_Temperature = data[1] - 50;     // 最低铜排温度
	bmsdata.BCU_Broadcast8_t.Copper_busbar_Temperature_difference = data[2];   //铜排温差
	bmsdata.BCU_Broadcast8_t.Average_Copper_busbar_Temperature = data[3] - 50; //平均铜排温度
	bmsdata.BCU_Broadcast8_t.MAX_Copper_busbar_Temperature_BMU = data[4];      //最高铜排温度位置 BMU 序号
	bmsdata.BCU_Broadcast8_t.MAX_Copper_busbar_Temperature_TEMPSEN = data[5];  //最高铜排温度位置温感序号
}
// 解析BMS广播报文9
void parse_broadcast9(const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	bmsdata.BCU_Broadcast9_t.Maximum_single_voltage_number= data[0];					//最高单体电压从机序号
	bmsdata.BCU_Broadcast9_t.Highest_individual_voltage_number = data[1];    // 最高单体电压包内序号
	bmsdata.BCU_Broadcast9_t.Minimum_single_voltage_number = data[2];        //最低单体电压从机序号
	bmsdata.BCU_Broadcast9_t.Low_individual_voltage_number = data[3];        //最低单体电压包内序号
	bmsdata.BCU_Broadcast9_t.Maximum_single_Temperature_number = data[4];    //最高单体温度从机序号
	bmsdata.BCU_Broadcast9_t.Highest_individual_Temperature_number = data[5];//最高单体温度包内序号
	bmsdata.BCU_Broadcast9_t.Minimum_single_Temperature_number = data[6];    //最低单体温度从机序号
	bmsdata.BCU_Broadcast9_t.Low_individual_Temperature_number = data[7];    //最低单体温度包内序号
}
// 解析BMS广播报文10-11故障信息
void parse_fault_info(uint32_t can_id, const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	uint8_t base_fault = (can_id == 0x180AFF00) ? 0 : 32;
	for (int byte_idx = 0; byte_idx < 8; byte_idx++) {
		for (int bit_idx = 0; bit_idx < 4; bit_idx++) {
			uint8_t fault_num = base_fault + byte_idx * 4 + bit_idx;
			if (fault_num < 65) {
				bmsdata.fault_levels[fault_num] = (data[byte_idx] >> (bit_idx * 2)) & 0x03;// 故障等级 (0:无故障, 1:轻微, 3:严重)
			}                                                                                     // 故障描述
		}
	}
}
// 解析BMS广播报文12
void parse_broadcast12(const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	bmsdata.BCU_Broadcast12_t.Main_version = data[0];		//主版本
	bmsdata.BCU_Broadcast12_t.Next_version = data[1];    //次版本
	bmsdata.BCU_Broadcast12_t.Revised_version = data[2]; //修订版本
}
// 解析BMS单体电压报文
void parse_cell_voltages(uint32_t can_id, const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	uint8_t frame_idx = ((can_id >> 16) & 0xFF) - 0x30;
	uint16_t start_idx = frame_idx * 4;
	for (int i = 0; i < 4; i++) {
		uint16_t raw_voltage = (data[2*i+1] << 8) | data[2*i];
		if (raw_voltage != 0xFFFF && start_idx + i < 256) {
			bmsdata.cell_voltages[start_idx + i] = raw_voltage * 0.001f;  
		}
	}
}
// 解析BMS单体温度报文
void parse_cell_temperatures(uint32_t can_id, const uint8_t* data , uint8_t len) {
	if (len != 8) {// 检查数据长度
		return;
	}
	uint8_t frame_idx = ((can_id >> 16) & 0xFF) - 0xB0;
	uint16_t start_idx = frame_idx * 8;
	for (int i = 0; i < 8; i++) {
		if (data[i] != 0xFF && start_idx + i < 256) {
			bmsdata.cell_temperatures[start_idx + i] = data[i] - 50;
		}
	}
}
// ===================== 接收函数 =====================
void BCU_CAN_Receive(uint32_t can_id, const uint8_t* data, uint8_t len) {
	if (len != 8) return;
	uint32_t base_id = can_id & 0xFFFFFF00;
	switch (base_id) {
		case 0x1801FF00: parse_broadcast1(data, len); break;
		case 0x1802FF00: parse_broadcast2(data, len); break;
		case 0x1803FF00: parse_broadcast3(data, len); break;
		case 0x1804FF00: parse_broadcast4(data, len); break;
		case 0x1805FF00: parse_broadcast5(data, len); break;
		case 0x1806FF00: parse_broadcast6(data, len); break;
		case 0x1807FF00: parse_broadcast7(data, len); break;
		case 0x1808FF00: parse_broadcast8(data, len); break;
		case 0x1809FF00: parse_broadcast9(data, len); break;
		case 0x180AFF00: parse_fault_info(can_id, data, len); break;
		case 0x180BFF00: parse_fault_info(can_id, data, len); break;
		case 0x180CFF00: parse_broadcast12(data, len); break;
		default:
			if ((can_id & 0xFF00FFFF) == 0x1800FF00) {// 检查单体电压报文 (0x1830FFnn - 0x183FFFnn)
				uint8_t vv = (can_id >> 16) & 0xFF;
				if (vv >= 0x30 && vv <= 0x49) {
					parse_cell_voltages(can_id, data, len);
				}
				uint8_t tt = (can_id >> 16) & 0xFF;
				if (tt >= 0xB0 && tt <= 0xB3) {
					parse_cell_temperatures(can_id, data, len);
				}
			}break;
	}
}
// ===================== 初始化函数 =====================
void BCU_Data_Init(void) {
	memset(&bmsdata, 0, sizeof(bmsdata));
	for(int i = 0; i < 256; i++) {// 初始化数组
		bmsdata.cell_voltages[i] = 0.0f;
		bmsdata.cell_temperatures[i] = -50;
	}
	for (int i = 0; i < 65; i++) {// 初始化故障等级
		bmsdata.fault_levels[i] = 0;
	}
}


