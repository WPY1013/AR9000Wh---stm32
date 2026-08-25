#include "cc15000.h"
#include "temp_detec.h"
#include "cc_detec.h"
#include "tim.h"
#include "fdcan.h"

#define MODULE_SEND_CAN_ID  0x06080F83U

#include "stdbool.h"

#define CAN1_POWER_ON_REPLY_ID  0x060F800BU
/* 接收中断和主循环共享时使用 volatile */
volatile uint32_t g_output_voltage_mv = 0;
volatile uint32_t g_output_current_ma = 0;
volatile uint32_t g_temp = 0;

volatile bool g_voltage_updated = false;
volatile bool g_current_updated = false;
volatile bool g_temp_updated = false;

static void Uint32ToBigEndian(uint32_t value, uint8_t *buf)
{
    buf[0] = (uint8_t)(value >> 24);
    buf[1] = (uint8_t)(value >> 16);
    buf[2] = (uint8_t)(value >> 8);
    buf[3] = (uint8_t)value;
}
static uint32_t BigEndianToUint32(const uint8_t *buf)
{
    return ((uint32_t)buf[0] << 24) |
           ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8)  |
           ((uint32_t)buf[3]);
}


ModuleStatus_t g_module_status = {
    .power_state = MODULE_POWER_UNKNOWN
};
//故障解析
static void DecodeModuleFaultStatus(uint32_t status)
{
    g_module_status.fault_status.raw_status = status;

    g_module_status.fault_status.running      = (status >> 0) & 1U;
    g_module_status.fault_status.fault        = (status >> 1) & 1U;
    g_module_status.fault_status.alarm        = (status >> 2) & 1U;
    g_module_status.fault_status.stop_fault   = (status >> 3) & 1U;

    g_module_status.fault_status.output_ovp   = (status >> 8)  & 1U;
    g_module_status.fault_status.output_ocp   = (status >> 9)  & 1U;
    g_module_status.fault_status.output_short = (status >> 10) & 1U;
    g_module_status.fault_status.over_temperature = (status >> 11) & 1U;
    g_module_status.fault_status.fan_fault    = (status >> 12) & 1U;
    g_module_status.fault_status.input_or_share_fault = (status >> 13) & 1U;
    g_module_status.fault_status.phase_loss   = (status >> 14) & 1U;
    g_module_status.fault_status.pfc_fault    = (status >> 15) & 1U;
    g_module_status.fault_status.discharge_fault = (status >> 16) & 1U;
    g_module_status.fault_status.address_conflict = (status >> 17) & 1U;
    g_module_status.fault_status.fan2_fault  = (status >> 18) & 1U;
    g_module_status.fault_status.communication_fault = (status >> 19) & 1U;
    g_module_status.fault_status.battery_reverse = (status >> 20) & 1U;
}

void CAN1_Receive(uint32_t can_id, const uint8_t *data, uint8_t len)
{
    uint32_t value;
		int32_t temp_raw;
    if ((data == NULL) || (len != 8U)) {
        return;
    }

    if (can_id != CAN1_POWER_ON_REPLY_ID) {
        return;
    }

    /* 回复帧固定帧头 */
    if ((data[0] != 0x41U) || (data[1] != 0xF0U)) {
        return;
    }

    /*
     * 开关机、设置电压、设置电流回复
     * data[2] == 0x00
     */
    if (data[2] == 0x00U) {
        switch (data[3]) {
        case 0x01U:
            if ((data[4] == 0x00U) &&
                (data[5] == 0x00U) &&
                (data[6] == 0xAAU) &&
                (data[7] == 0xAAU)) {
                g_module_status.power_state = MODULE_POWER_ON;
                g_module_status.power_ack = true;
            } else if ((data[4] == 0x00U) &&
                       (data[5] == 0x00U) &&
                       (data[6] == 0x55U) &&
                       (data[7] == 0x55U)) {
                g_module_status.power_state = MODULE_POWER_OFF;
                g_module_status.power_ack = true;
            }
            break;

        case 0x2CU:
            value = BigEndianToUint32(&data[4]);
            g_module_status.set_voltage_mv = value;
            g_module_status.set_voltage_ack = true;
            break;

        case 0x2DU:
            value = BigEndianToUint32(&data[4]);
            g_module_status.set_current_ma = value;
            g_module_status.set_current_ack = true;
            break;

        default:
            break;
        }

        return;
    }

    /*
     * 查询电压、电流回复
     * data[2] == 0x02
     */
    if (data[2] == 0x02U) {
        value = BigEndianToUint32(&data[4]);

        switch (data[3]) {
						case 0x18U:       // 0x0218，读取模块运行状态
								DecodeModuleFaultStatus(value);
						break;
						case 0x0BU:     
								temp_raw = (int32_t)value;							
								g_module_status.temp = (float)temp_raw * 0.1f;;
								g_module_status.temp_updated = true;
						break;
						case 0x31U:
								g_module_status.output_voltage_mv = value;
								g_module_status.voltage_updated = true;
								break;

						case 0x32U:
								g_module_status.output_current_ma = value;
								g_module_status.current_updated = true;
								break;

						default:
								break;
        }
    }
}


//开机
uint8_t FDCAN1_Send_ON(void)
{
    uint8_t tx_buf[8] = {0x01,0xF0,0x00,0x01,0x00,0x00,0x00,0xAA};
    if(FDCAN1_SendExtCanMsg(0x06080F83, tx_buf, 8) == 0)
    {
        return true;
    }
    return false;
}
//关机
uint8_t FDCAN1_Send_OFF(void)
{
    uint8_t tx_buf[8] = {0x01,0xF0,0x00,0x01,0x00,0x00,0x00,0x55};
    if(FDCAN1_SendExtCanMsg(0x06080F83, tx_buf, 8) == 0)
    {
        return true;
    }
    return false;
}

//设置输出电压/mV
uint8_t FDCAN1_Send_Vol(uint32_t voltage_mv)
{
    uint8_t tx_buf[8] = {0x01, 0xF0, 0x00, 0x2C,0x00, 0x00, 0x00, 0x00};

    Uint32ToBigEndian(voltage_mv, &tx_buf[4]);

    return FDCAN1_SendExtCanMsg(MODULE_SEND_CAN_ID,tx_buf,sizeof(tx_buf)) == 0;
}
//设置输出电流/mA
uint8_t FDCAN1_Send_Cur(uint32_t current_ma)
{
    uint8_t tx_buf[8] = {0x01, 0xF0, 0x00, 0x2D,0x00, 0x00, 0x00, 0x00};

    Uint32ToBigEndian(current_ma, &tx_buf[4]);

    return FDCAN1_SendExtCanMsg(MODULE_SEND_CAN_ID,tx_buf,sizeof(tx_buf)) == 0;
}
//查询电压
uint8_t FDCAN1_Query_Voltage(void)
{
    uint8_t tx_buf[8] = {0x01, 0xF0, 0x02, 0x31,0x00, 0x00, 0x00, 0x00};

    return FDCAN1_SendExtCanMsg(MODULE_SEND_CAN_ID,tx_buf,sizeof(tx_buf)) == 0;
}
//查询电流
uint8_t FDCAN1_Query_Current(void)
{
    uint8_t tx_buf[8] = {0x01, 0xF0, 0x02, 0x32,0x00, 0x00, 0x00, 0x00};

    return FDCAN1_SendExtCanMsg(MODULE_SEND_CAN_ID,tx_buf,sizeof(tx_buf)) == 0;
}
//查询温度
uint8_t FDCAN1_Query_Temp(void)
{
    uint8_t tx_buf[8] = {0x01, 0xF0, 0x02, 0x0B,0x00, 0x00, 0x00, 0x00};

    return FDCAN1_SendExtCanMsg(MODULE_SEND_CAN_ID,tx_buf,sizeof(tx_buf)) == 0;
}
//查询运行状态
uint8_t FDCAN1_Query_State(void)
{
    uint8_t tx_buf[8] = {0x01, 0xF0, 0x02, 0x18,0x00, 0x00, 0x00, 0x00};

    return FDCAN1_SendExtCanMsg(MODULE_SEND_CAN_ID,tx_buf,sizeof(tx_buf)) == 0;
}
void CAN2_Receive(uint32_t can_id, const uint8_t *data, uint8_t len)
{
//    uint32_t value;

    if ((data == NULL) || (len != 8U)) {
        return;
    }

    if (can_id != CAN1_POWER_ON_REPLY_ID) {
        return;
    }

   
}
