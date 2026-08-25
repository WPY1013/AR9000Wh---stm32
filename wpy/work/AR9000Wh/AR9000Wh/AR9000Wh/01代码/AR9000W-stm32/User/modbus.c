#include "modbus.h"

static uint8_t  tx_buf[256];
static uint8_t  rx_buf[256];
uint16_t BMS_RegisterValues[31];
extern UART_HandleTypeDef huart2;
static UART_HandleTypeDef *bms_huart = &huart2;
//CRC校验
uint16_t BMS_CRC16(const uint8_t *data, uint8_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}
//发送 & 接收底层
static int BMS_ReceivePart(uint8_t *data, uint8_t length)
{
    HAL_StatusTypeDef status;

    status = HAL_UART_Receive(bms_huart, data, length, BMS_USART_TIMEOUT);
    if (status == HAL_BUSY) return -31;
    if (status == HAL_TIMEOUT) return -32;
    if (status != HAL_OK) return -33;
    return 0;
}

static int BMS_SendReceive(uint8_t tx_len, uint8_t expected_rx_len)
{
    uint8_t actual_rx_len;
    uint16_t crc_rx;
    uint16_t crc_calc;
    int ret;

    if (bms_huart == NULL) return -1;
    if (expected_rx_len < 5U) return -6;

//    __HAL_UART_FLUSH_DRREGISTER(bms_huart);
		BMS_ClearRxState();
    if (HAL_UART_Transmit(bms_huart, tx_buf, tx_len,BMS_USART_TIMEOUT) != HAL_OK) return -2;

    //接收的前两个寄存器：从机地址和功能码
    ret = BMS_ReceivePart(rx_buf, 2U);
    if (ret != 0) return ret;
    actual_rx_len = (rx_buf[1] & 0x80U) ? 5U : expected_rx_len;//功能码异常判断
    ret = BMS_ReceivePart(&rx_buf[2], actual_rx_len - 2U);//接收后续寄存器
    if (ret != 0) return ret;
		
    crc_rx = ((uint16_t)rx_buf[actual_rx_len - 1U] << 8) | rx_buf[actual_rx_len - 2U];//接收的CRC
    crc_calc = BMS_CRC16(rx_buf, actual_rx_len - 2U);//接收的数据计算CRC
    if (crc_rx != crc_calc) return -4;//CRC不同，数据有误
    if (rx_buf[0] != BMS_SLAVE_ID) return -5;//从机地址错误

    if (rx_buf[1] & 0x80U) return -(100 + rx_buf[2]);//功能码异常

    return 0;
}

//读写多个寄存器
//cmd 功能码 0x03=读多个寄存器 0x10=写多个寄存器
//addr 寄存器地址
//value 数量
//data 数据
//buf 返回的数据
int BMS_WriteRegister1( uint8_t cmd,uint16_t addr, uint16_t value,const uint16_t *p_data, uint16_t *buf)
{
    if (cmd != 0x03U && cmd != 0x10U) {
    return -7;
    }

    if (cmd == 0x03U) {
        if (value == 0U || value > 125U || buf == NULL) {
            return -8;
        }
    } else {
        if (value == 0U || value > 123U || p_data == NULL) {
            return -8;
        }
    }
    uint8_t idx = 0;
    tx_buf[idx++] = BMS_SLAVE_ID;//从机地址
    tx_buf[idx++] = cmd;//功能码
    tx_buf[idx++] = (uint8_t)(addr >> 8);
    tx_buf[idx++] = (uint8_t)(addr & 0xFFU);
    if(cmd == 0x03){
        if(value == 0U || value > 125U){
            return -8;
        }
        tx_buf[idx++] = (uint8_t)(value >> 8);
        tx_buf[idx++] = (uint8_t)(value & 0xFFU);
    }else if(cmd == 0x10){
        if (value == 0U || value > 123U) {
            return -8;
        }
        if (p_data == NULL) {
            return -8;
        }
        tx_buf[idx++] = (uint8_t)(value >> 8);
        tx_buf[idx++] = (uint8_t)(value & 0xFFU);
        tx_buf[idx++] = value * 2; //字节数
                for (uint16_t i = 0; i < value; i++) {
                    tx_buf[idx++] = p_data[i] >> 8;
                    tx_buf[idx++] = p_data[i] & 0xFF;
                }
    }
    uint16_t crc = BMS_CRC16(tx_buf, idx);//CRC校验
    tx_buf[idx++] = crc & 0xFF;
    tx_buf[idx++] = crc >> 8;
    if(cmd == 0x03){
        if (buf == NULL) return -8;
        uint8_t rx_len = 5 + value * 2;         /* 1地址+1功能码+1字节数+N*2数据+2CRC */
        int ret = BMS_SendReceive(idx, rx_len);
        if (ret != 0) return ret;
        /* 解析数据 */
        for (uint16_t i = 0; i < value; i++) {
            buf[i] = (rx_buf[3 + i * 2] << 8) | rx_buf[4 + i * 2];
        }
        return 0;
    }else if(cmd == 0x10){
        /* 写寄存器响应回显=发送帧 (8字节) */
        return BMS_SendReceive(idx, 8);
    }
    return -7;//功能码错误
}
    
    
/* ================================================================
 * 0x03 — 读多个保持寄存器
 * ================================================================ */
int BMS_ReadRegisters(uint8_t addr, uint8_t count, uint16_t *buf)
{
    uint8_t idx = 0;
    tx_buf[idx++] = BMS_SLAVE_ID;
    tx_buf[idx++] = 0x03;
    tx_buf[idx++] = (uint8_t)(addr >> 8);       /* register address high byte */
    tx_buf[idx++] = (uint8_t)(addr & 0xFFU);    /* register address low byte */
    tx_buf[idx++] = 0x00;       /* register count high byte */
    tx_buf[idx++] = count;      /* register count low byte */
    uint16_t crc = BMS_CRC16(tx_buf, idx);
    tx_buf[idx++] = crc & 0xFF;
    tx_buf[idx++] = crc >> 8;

    uint8_t rx_len = 5 + count * 2;         /* 1地址+1功能码+1字节数+N*2数据+2CRC */
    int ret = BMS_SendReceive(idx, rx_len);
    if (ret != 0) return ret;

    /* 解析数据 */
    for (uint8_t i = 0; i < count; i++) {
        buf[i] = (rx_buf[3 + i * 2] << 8) | rx_buf[4 + i * 2];
    }
    return 0;
}

/* ================================================================
 * 0x06 — 写单个寄存器
 * ================================================================ */
int BMS_WriteRegister(uint8_t addr, uint16_t value)
{
    uint8_t idx = 0;
    tx_buf[idx++] = BMS_SLAVE_ID;
    tx_buf[idx++] = 0x06;
    tx_buf[idx++] = (uint8_t)(addr >> 8);
    tx_buf[idx++] = (uint8_t)(addr & 0xFFU);
    tx_buf[idx++] = value >> 8;
    tx_buf[idx++] = value & 0xFF;

    uint16_t crc = BMS_CRC16(tx_buf, idx);
    tx_buf[idx++] = crc & 0xFF;
    tx_buf[idx++] = crc >> 8;

    /* 写寄存器响应回显=发送帧 (8字节) */
    return BMS_SendReceive(idx, 8);
}

/* ================================================================
 * 0x10 — 写多个寄存器
 * ================================================================ */
int BMS_WriteRegisters(uint8_t addr, uint8_t count, const uint16_t *values)
{
    uint8_t idx = 0;
    tx_buf[idx++] = BMS_SLAVE_ID;
    tx_buf[idx++] = 0x10;
    tx_buf[idx++] = (uint8_t)(addr >> 8);
    tx_buf[idx++] = (uint8_t)(addr & 0xFFU);
    tx_buf[idx++] = 0x00;                   /* quantity high byte */
    tx_buf[idx++] = count;
    tx_buf[idx++] = count * 2;              /* 字节数 */

    for (uint8_t i = 0; i < count; i++) {
        tx_buf[idx++] = values[i] >> 8;
        tx_buf[idx++] = values[i] & 0xFF;
    }

    uint16_t crc = BMS_CRC16(tx_buf, idx);
    tx_buf[idx++] = crc & 0xFF;
    tx_buf[idx++] = crc >> 8;

    return BMS_SendReceive(idx, 8);
}

/* ================================================================
 * Modbus 命令 (地址1,2 配合使用)
 * ================================================================ */
int BMS_SendCommand(uint16_t cmd, uint16_t data)
{
    /* 先写数据，再写命令触发 */
    int ret = BMS_WriteRegister(1, data);
    if (ret != 0) return ret;
    HAL_Delay(10);
    return BMS_WriteRegister(2, cmd);
}

/* ================================================================
 * 批量读取 — 系统信息 (地址 0~4)
 * ================================================================ */
int BMS_ReadSystemInfo(BMS_SystemInfo_t *sys)
{
    uint16_t buf[5];
    int ret = BMS_ReadRegisters(0, 5, buf);
    if (ret != 0) return ret;
    sys->res1             = buf[0];
    sys->modbus_cmd_data  = buf[1];
    sys->modbus_cmd       = buf[2];
    sys->software_version = buf[3];
    sys->hardware_version = buf[4];
    return 0;
}

/* ================================================================
 * 批量读取 — ID 信息 (地址 5~30, U8数组)
 * ================================================================ */
int BMS_ReadIDInfo(BMS_IDInfo_t *id)
{
    uint8_t i;
    int ret;

    /* Keep register 5..30 at the matching array indexes for Keil Watch. */
    ret = BMS_ReadRegisters(5, 26, &BMS_RegisterValues[5]);
    if (ret != 0) return ret;

    for (i = 0; i < 26; i++) {
        ((uint8_t *)id)[i * 2] =
            (uint8_t)(BMS_RegisterValues[i + 5] & 0xFFU);
        ((uint8_t *)id)[i * 2 + 1] =
            (uint8_t)(BMS_RegisterValues[i + 5] >> 8);
    }
    return 0;
}
/* ================================================================
 * 批量读取 — 事件计数器 (地址 31~45)
 * ================================================================ */
int BMS_ReadEventCount(BMS_EventCount_t *event)
{
    uint16_t buf[15];
    int ret = BMS_ReadRegisters(31, 15, buf);
    if (ret != 0) return ret;
    event->restart_times       = buf[0];
    event->sleep_times         = buf[1];
    event->power_lost_times    = buf[2];
    event->total_run_time      = buf[3];
    event->err_cell_ov_times   = buf[4];
    event->err_cell_uv_times   = buf[5];
    event->err_pack_ov_times   = buf[6];
    event->err_pack_uv_times   = buf[7];
    event->err_chg_oc_times    = buf[8];
    event->err_dsg_oc_times    = buf[9];
    event->err_chg_ot_times    = buf[10];
    event->err_chg_ut_times    = buf[11];
    event->err_dsg_ot_times    = buf[12];
    event->err_dsg_ut_times    = buf[13];
    event->err_sc_times        = buf[14];
    return 0;
}

/* ================================================================
 * 批量读取 — 校准 & 电芯配置 (地址 46~48,50~51, 跳过49)
 * ================================================================ */
int BMS_ReadCalibConfig(BMS_CalibConfig_t *calib)
{
    /* 分段读: 46~48(3个), 49跳过, 50~51(2个) */
    uint16_t buf[3];
    int ret = BMS_ReadRegisters(46, 3, buf);
    if (ret != 0) return ret;
    calib->ntc_material    = buf[0];
    calib->zero_adjust     = (int16_t)buf[1];
    calib->current_adjust  = buf[2];

    ret = BMS_ReadRegisters(50, 2, buf);
    if (ret != 0) return ret;
    calib->cell_material   = buf[0];
    calib->factory_ah      = buf[1];
    return 0;
}

/* ================================================================
 * 批量读取 — 电压保护阈值 (地址 66~73)
 * ================================================================ */
int BMS_ReadVoltageProtect(BMS_VoltageProtect_t *volt)
{
    uint16_t buf[8];
    int ret = BMS_ReadRegisters(66, 8, buf);
    if (ret != 0) return ret;
    volt->cell_voltage_max          = buf[0];
    volt->cell_voltage_max_recover  = buf[1];
    volt->cell_voltage_min          = buf[2];
    volt->cell_voltage_min_recover  = buf[3];
    volt->pack_voltage_max          = buf[4];
    volt->pack_voltage_max_recover  = buf[5];
    volt->pack_voltage_min          = buf[6];
    volt->pack_voltage_min_recover  = buf[7];
    return 0;
}

/* ================================================================
 * 批量读取 — 温度保护阈值 (地址 74~83)
 * ================================================================ */
int BMS_ReadTempProtect(BMS_TempProtect_t *temp)
{
    uint16_t buf[10];
    int ret = BMS_ReadRegisters(74, 10, buf);
    if (ret != 0) return ret;
    temp->pack_tmp_max_chg          = (int16_t)buf[0];
    temp->pack_tmp_max_chg_recover  = (int16_t)buf[1];
    temp->pack_tmp_max_dsg          = (int16_t)buf[2];
    temp->pack_tmp_max_dsg_recover  = (int16_t)buf[3];
    temp->pack_tmp_min_chg          = (int16_t)buf[4];
    temp->pack_tmp_min_chg_recover  = (int16_t)buf[5];
    temp->pack_tmp_min_dsg          = (int16_t)buf[6];
    temp->pack_tmp_min_dsg_recover  = (int16_t)buf[7];
    temp->mos_tmp_max               = (int16_t)buf[8];
    temp->mos_tmp_max_recover       = (int16_t)buf[9];
    return 0;
}

/* ================================================================
 * 批量读取 — 过流保护 (地址 86~103)
 * ================================================================ */
int BMS_ReadCurrentProtect(BMS_CurrentProtect_t *curr)
{
    uint16_t buf[18];
    int ret = BMS_ReadRegisters(86, 18, buf);
    if (ret != 0) return ret;

    for (uint8_t i = 0; i < 3; i++) {
        curr->dsg[i].cur          = buf[i * 3 + 0];
        curr->dsg[i].time_action  = buf[i * 3 + 1];
        curr->dsg[i].time_recover = buf[i * 3 + 2];
    }
    for (uint8_t i = 0; i < 3; i++) {
        curr->chg[i].cur          = buf[9 + i * 3 + 0];
        curr->chg[i].time_action  = buf[9 + i * 3 + 1];
        curr->chg[i].time_recover = buf[9 + i * 3 + 2];
    }
    return 0;
}

/* ================================================================
 * 批量读取 — 实时数据 (地址 104~113)
 * ================================================================ */
int BMS_ReadRealtimeData(BMS_RealtimeData_t *rt)
{
    uint16_t buf[10];
    int ret = BMS_ReadRegisters(104, 10, buf);
    if (ret != 0) return ret;
    rt->err_flag.raw   = buf[0];
    rt->err_flag_h     = buf[1];
    rt->status         = buf[2];
    rt->cell_tmp       = (int16_t)buf[3];
    rt->mos_tmp        = (int16_t)buf[4];
    rt->soc            = buf[5];
    rt->cell_voltage   = buf[6];
    rt->cell_current   = (int16_t)buf[7];
    rt->out_voltage    = buf[8];
    rt->out_current    = (int16_t)buf[9];
    return 0;
}
/* ================================================================
 * 批量读取 — 全部数据
 * ================================================================ */
int BMS_ReadAll(BMS_Device_t *dev)
{
    int ret;
    ret = BMS_ReadSystemInfo(&dev->sys);    if (ret != 0) return ret;
    ret = BMS_ReadIDInfo(&dev->id);         if (ret != 0) return ret;
    ret = BMS_ReadEventCount(&dev->event);  if (ret != 0) return ret;
    ret = BMS_ReadCalibConfig(&dev->calib); if (ret != 0) return ret;
    ret = BMS_ReadVoltageProtect(&dev->volt); if (ret != 0) return ret;
    ret = BMS_ReadTempProtect(&dev->temp);  if (ret != 0) return ret;
    ret = BMS_ReadCurrentProtect(&dev->curr); if (ret != 0) return ret;
    ret = BMS_ReadRealtimeData(&dev->rt);   if (ret != 0) return ret;
    return 0;
}
/* ================================================================
 * 批量写入 — 校准 & 电芯配置
 * ================================================================ */
int BMS_WriteCalibConfig(const BMS_CalibConfig_t *calib)
{
    uint16_t buf[3];
    buf[0] = calib->ntc_material;
    buf[1] = (uint16_t)calib->zero_adjust;
    buf[2] = calib->current_adjust;
    int ret = BMS_WriteRegisters(46, 3, buf);
    if (ret != 0) return ret;

    buf[0] = calib->cell_material;
    buf[1] = calib->factory_ah;
    return BMS_WriteRegisters(50, 2, buf);
}

/* ================================================================
 * 批量写入 — 电压保护阈值
 * ================================================================ */
int BMS_WriteVoltageProtect(const BMS_VoltageProtect_t *volt)
{
    uint16_t buf[8];
    buf[0] = volt->cell_voltage_max;
    buf[1] = volt->cell_voltage_max_recover;
    buf[2] = volt->cell_voltage_min;
    buf[3] = volt->cell_voltage_min_recover;
    buf[4] = volt->pack_voltage_max;
    buf[5] = volt->pack_voltage_max_recover;
    buf[6] = volt->pack_voltage_min;
    buf[7] = volt->pack_voltage_min_recover;
    return BMS_WriteRegisters(66, 8, buf);
}

/* ================================================================
 * 批量写入 — 温度保护阈值
 * ================================================================ */
int BMS_WriteTempProtect(const BMS_TempProtect_t *temp)
{
    uint16_t buf[10];
    buf[0] = (uint16_t)temp->pack_tmp_max_chg;
    buf[1] = (uint16_t)temp->pack_tmp_max_chg_recover;
    buf[2] = (uint16_t)temp->pack_tmp_max_dsg;
    buf[3] = (uint16_t)temp->pack_tmp_max_dsg_recover;
    buf[4] = (uint16_t)temp->pack_tmp_min_chg;
    buf[5] = (uint16_t)temp->pack_tmp_min_chg_recover;
    buf[6] = (uint16_t)temp->pack_tmp_min_dsg;
    buf[7] = (uint16_t)temp->pack_tmp_min_dsg_recover;
    buf[8] = (uint16_t)temp->mos_tmp_max;
    buf[9] = (uint16_t)temp->mos_tmp_max_recover;
    return BMS_WriteRegisters(74, 10, buf);
}

/* ================================================================
 * 批量写入 — 过流保护阈值
 * ================================================================ */
int BMS_WriteCurrentProtect(const BMS_CurrentProtect_t *curr)
{
    uint16_t buf[18];
    for (uint8_t i = 0; i < 3; i++) {
        buf[i * 3 + 0] = curr->dsg[i].cur;
        buf[i * 3 + 1] = curr->dsg[i].time_action;
        buf[i * 3 + 2] = curr->dsg[i].time_recover;
    }
    for (uint8_t i = 0; i < 3; i++) {
        buf[9 + i * 3 + 0] = curr->chg[i].cur;
        buf[9 + i * 3 + 1] = curr->chg[i].time_action;
        buf[9 + i * 3 + 2] = curr->chg[i].time_recover;
    }
    return BMS_WriteRegisters(86, 18, buf);
}

int BMS_SendSleepCommand(void)
{
    uint8_t idx = 0U;
    uint16_t crc;
    int ret;

    /* 第一阶段：写命令参数，必须收到BMS回显 */
    ret = BMS_WriteRegister(1U, 0x0000U);
    if (ret != 0) {
        return ret;
    }

    HAL_Delay(10U);

    /* 第二阶段：写休眠命令 */
    tx_buf[idx++] = BMS_SLAVE_ID;
    tx_buf[idx++] = 0x06U;
    tx_buf[idx++] = 0x00U;
    tx_buf[idx++] = 0x02U;
    tx_buf[idx++] = 0x10U;
    tx_buf[idx++] = 0x00U;

    crc = BMS_CRC16(tx_buf, idx);
    tx_buf[idx++] = (uint8_t)(crc & 0xFFU);
    tx_buf[idx++] = (uint8_t)(crc >> 8);

    /*
     * HAL_UART_Transmit会等待最后一个停止位发送完成。
     * BMS可能在收到命令后立即休眠，所以这里不等待回显。
     */
    if (HAL_UART_Transmit(
            bms_huart,
            tx_buf,
            idx,
            BMS_USART_TIMEOUT) != HAL_OK) {
        return -2;
    }

    return 0;
}
static void BMS_ClearRxState(void)
{
    volatile uint32_t dummy;

    /* SR后接DR读取可清除ORE、FE、NE等接收错误 */
    dummy = bms_huart->Instance->SR;
    dummy = bms_huart->Instance->DR;
    (void)dummy;

    while (__HAL_UART_GET_FLAG(bms_huart, UART_FLAG_RXNE) != RESET) {
        dummy = bms_huart->Instance->DR;
        (void)dummy;
    }

    bms_huart->ErrorCode = HAL_UART_ERROR_NONE;
}
