#ifndef __MODBUS_H__
#define __MODBUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

//#define BMS_SLAVE_ID 0x01
//#define BMS_USART_TIMEOUT       200U        /* ms */

/* ================================================================
 * 通信参数
 * ================================================================ */
#define BMS_SLAVE_ID            0x01U
#define BMS_BAUDRATE            115200U
#define BMS_USART_TIMEOUT       200U        /* ms */

/* ================================================================
 * 电芯材料枚举
 * ================================================================ */
typedef enum {
    CELL_MATERIAL_LiFePO4   = 1,            /* 磷酸铁锂 */
    CELL_MATERIAL_NCM       = 2,            /* 三元锂 */
    CELL_MATERIAL_NaIon     = 3             /* 钠电 */
} CellMaterial_t;

/* ================================================================
 * NTC 类型枚举
 * ================================================================ */
typedef enum {
    NTC_B3930 = 1,
    NTC_B3435 = 2
} NTCMaterial_t;

/* ================================================================
 * 故障标志位 (地址 104 — Err_Flag_L)
 * ================================================================ */
typedef union {
    uint16_t raw;
    struct {
        uint16_t CHG             : 1;  /* Bit0  禁止充电(任意原因) */
        uint16_t DSG             : 1;  /* Bit1  禁止放电(任意原因) */
        uint16_t Cell_OV         : 1;  /* Bit2  电芯过压 */
        uint16_t Cell_UV         : 1;  /* Bit3  电芯低压 */
        uint16_t Pack_OV         : 1;  /* Bit4  电池组高压 */
        uint16_t Pack_UV         : 1;  /* Bit5  电池组低压 */
        uint16_t TMP_Max_CHG     : 1;  /* Bit6  充电过温 */
        uint16_t TMP_Max_DSG     : 1;  /* Bit7  放电过温 */
        uint16_t TMP_Min_CHG     : 1;  /* Bit8  充电低温 */
        uint16_t TMP_Min_DSG     : 1;  /* Bit9  放电低温 */
        uint16_t CHG_OC          : 1;  /* Bit10 充电过流 */
        uint16_t DSG_OC          : 1;  /* Bit11 放电过流 */
        uint16_t TMP_Max_MOS     : 1;  /* Bit12 MOS过温 */
        uint16_t CHG_MOS_Damaged : 1;  /* Bit13 充电MOS损坏 */
        uint16_t DSG_MOS_Damaged : 1;  /* Bit14 放电MOS损坏 */
        uint16_t SC              : 1;  /* Bit15 短路 */
    } bits;
} ErrFlag_t;

/* ================================================================
 * 实时数据 (地址 104~113 — SRAM, RO)
 * ================================================================ */
typedef struct {
    ErrFlag_t   err_flag;                   /* 104  故障标志 */
    uint16_t    err_flag_h;                 /* 105  故障标志高位(保留) */
    uint16_t    status;                     /* 106  状态字(保留) */
    int16_t     cell_tmp;                   /* 107  电芯温度   /10,1  ℃ */
    int16_t     mos_tmp;                    /* 108  MOS温度    /10,1  ℃ */
    uint16_t    soc;                        /* 109  SOC        /10,1  % */
    uint16_t    cell_voltage;               /* 110  电芯电压   mV */
    int16_t     cell_current;               /* 111  电芯电流   /10,1  A */
    uint16_t    out_voltage;                /* 112  输出电压   /100,1 V */
    int16_t     out_current;                /* 113  输出电流   /100,1 A */
} BMS_RealtimeData_t;       /* 起始地址: 104,  共 10 个寄存器 */

/* ================================================================
 * 系统信息 (地址 0~4 — EEPROM, RO)
 * ================================================================ */
typedef struct {
    uint16_t    res1;                       /* 0  用户权限寄存器 */
    uint16_t    modbus_cmd_data;            /* 1  Modbus命令携带数据 */
    uint16_t    modbus_cmd;                 /* 2  Modbus命令 */
    uint16_t    software_version;           /* 3  软件版本  /100,2  (如208→V2.08) */
    uint16_t    hardware_version;           /* 4  硬件版本  /100,2  (如302→固件代号3,固件版本2) */
} BMS_SystemInfo_t;         /* 起始地址: 0,  共 5 个寄存器 */

/* ================================================================
 * ID 信息 (地址 5~30 — EEPROM, RO)
 * ================================================================ */
#define CUSTOMER_ID_LEN     20
#define BATTERY_ID_LEN      32

typedef struct {
    uint8_t     customer_id[CUSTOMER_ID_LEN]; /* 5~14  客户名称+型号 (ASCII) */
    uint8_t     battery_id[BATTERY_ID_LEN];   /* 15~30 电池ID (24字符) */
} BMS_IDInfo_t;             /* 起始地址: 5,  共 26 个寄存器 / 52 字节 U8 */

/* ================================================================
 * 事件计数器 (地址 31~45 — EEPROM, RO)
 * ================================================================ */
typedef struct {
    uint16_t    restart_times;              /* 31  重启次数 */
    uint16_t    sleep_times;                /* 32  休眠次数 */
    uint16_t    power_lost_times;           /* 33  掉电次数 */
    uint16_t    total_run_time;             /* 34  累计运行时间 h */
    uint16_t    err_cell_ov_times;          /* 35  电芯过压保护次数 */
    uint16_t    err_cell_uv_times;          /* 36  电芯欠压保护次数 */
    uint16_t    err_pack_ov_times;          /* 37  电池过压保护次数 */
    uint16_t    err_pack_uv_times;          /* 38  电池欠压保护次数 */
    uint16_t    err_chg_oc_times;           /* 39  充电过流保护次数 */
    uint16_t    err_dsg_oc_times;           /* 40  放电过流保护次数 */
    uint16_t    err_chg_ot_times;           /* 41  充电过温保护次数 */
    uint16_t    err_chg_ut_times;           /* 42  充电低温保护次数 */
    uint16_t    err_dsg_ot_times;           /* 43  放电过温保护次数 */
    uint16_t    err_dsg_ut_times;           /* 44  放电低温保护次数 */
    uint16_t    err_sc_times;               /* 45  短路保护次数 */
} BMS_EventCount_t;         /* 起始地址: 31,  共 15 个寄存器 */

/* ================================================================
 * 校准 & 电芯配置 (地址 46~51 — EEPROM, RW)
 * ================================================================ */
typedef struct {
    uint16_t    ntc_material;               /* 46  NTC类型: 1=3930, 2=3435 */
    int16_t     zero_adjust;                /* 47  电流零点校准  -1000~1000 */
    uint16_t    current_adjust;             /* 48  电流斜率校准  0~10000 */
    /* 49 保留 */
    uint16_t    cell_material;              /* 50  电芯材料: 1=磷酸铁锂, 2=三元锂, 3=钠电 */
    uint16_t    factory_ah;                 /* 51  额定容量  /10,1  Ah */
} BMS_CalibConfig_t;        /* 起始地址: 46,  共 6 个有效寄存器 (49是保留) */

/* ================================================================
 * 电压保护阈值 (地址 66~73 — EEPROM, RW)
 * ================================================================ */
typedef struct {
    uint16_t    cell_voltage_max;           /* 66  电芯过压保护    mV */
    uint16_t    cell_voltage_max_recover;   /* 67  电芯过压恢复    mV */
    uint16_t    cell_voltage_min;           /* 68  电芯欠压保护    mV */
    uint16_t    cell_voltage_min_recover;   /* 69  电芯欠压恢复    mV */
    uint16_t    pack_voltage_max;           /* 70  电池组过压保护  /100,2  V */
    uint16_t    pack_voltage_max_recover;   /* 71  电池组过压恢复  /100,2  V */
    uint16_t    pack_voltage_min;           /* 72  电池组欠压保护  /100,2  V */
    uint16_t    pack_voltage_min_recover;   /* 73  电池组欠压恢复  /100,2  V */
} BMS_VoltageProtect_t;     /* 起始地址: 66,  共 8 个寄存器 */

/* ================================================================
 * 温度保护阈值 (地址 74~83 — EEPROM, RW)  /10,1  ℃
 * ================================================================ */
typedef struct {
    int16_t     pack_tmp_max_chg;           /* 74  充电过温保护 */
    int16_t     pack_tmp_max_chg_recover;   /* 75  充电过温恢复 */
    int16_t     pack_tmp_max_dsg;           /* 76  放电过温保护 */
    int16_t     pack_tmp_max_dsg_recover;   /* 77  放电过温恢复 */
    int16_t     pack_tmp_min_chg;           /* 78  充电低温保护 */
    int16_t     pack_tmp_min_chg_recover;   /* 79  充电低温恢复 */
    int16_t     pack_tmp_min_dsg;           /* 80  放电低温保护 */
    int16_t     pack_tmp_min_dsg_recover;   /* 81  放电低温恢复 */
    int16_t     mos_tmp_max;                /* 82  MOS过温保护 */
    int16_t     mos_tmp_max_recover;        /* 83  MOS过温恢复 */
} BMS_TempProtect_t;        /* 起始地址: 74,  共 10 个寄存器 */

/* ================================================================
 * 过流保护阈值 (地址 86~103 — EEPROM, RW)
 * ================================================================ */
typedef struct {
    uint16_t    cur;                        /* 保护电流  /10,1  A */
    uint16_t    time_action;                /* 保护延时  /10,1  s */
    uint16_t    time_recover;               /* 恢复延时  /10,1  s */
} BMS_CurrentProtectStage_t;

typedef struct {
    BMS_CurrentProtectStage_t   dsg[3];     /* 放电过流 3级 */
    BMS_CurrentProtectStage_t   chg[3];     /* 充电过流 3级 */
} BMS_CurrentProtect_t;     /* 起始地址: 86,  共 18 个寄存器 */

/* ================================================================
 * 完整 BMS 设备数据 (所有可读寄存器汇总)
 * ================================================================ */
typedef struct {
    BMS_SystemInfo_t        sys;
    BMS_IDInfo_t            id;
    BMS_EventCount_t        event;
    BMS_CalibConfig_t       calib;
    BMS_VoltageProtect_t    volt;
    BMS_TempProtect_t       temp;
    BMS_CurrentProtect_t    curr;
    BMS_RealtimeData_t      rt;
} BMS_Device_t;

/* ================================================================
 * Modbus 功能函数声明
 * ================================================================ */

/**
 * @brief  初始化 USART (115200, 8N1)
 */
void BMS_USART_Init(UART_HandleTypeDef *huart);
int BMS_WriteRegister1( uint8_t cmd,uint16_t addr, uint16_t value,const uint16_t *p_data, uint16_t *buf);
/**
 * @brief  读多个保持寄存器 (Modbus 0x03)
 * @param  addr    起始地址
 * @param  count   寄存器数量
 * @param  buf     接收缓冲区 (uint16_t 数组)
 * @return 0=成功, 非0=失败
 */
int BMS_ReadRegisters(uint8_t addr, uint8_t count, uint16_t *buf);

/**
 * @brief  写单个寄存器 (Modbus 0x06)
 * @param  addr    寄存器地址
 * @param  value   写入值
 * @return 0=成功, 非0=失败
 */
int BMS_WriteRegister(uint8_t addr, uint16_t value);

/**
 * @brief  写多个寄存器 (Modbus 0x10)
 * @param  addr    起始地址
 * @param  count   寄存器数量
 * @param  values  写入数据
 * @return 0=成功, 非0=失败
 */
int BMS_WriteRegisters(uint8_t addr, uint8_t count, const uint16_t *values);


int BMS_SendCommand(uint16_t cmd, uint16_t data);

/* ---- 批量读取便捷函数 ---- */

int BMS_ReadSystemInfo(BMS_SystemInfo_t *sys);
extern uint16_t BMS_RegisterValues[31];

int BMS_ReadIDInfo(BMS_IDInfo_t *id);
int BMS_ReadEventCount(BMS_EventCount_t *event);
int BMS_ReadCalibConfig(BMS_CalibConfig_t *calib);
int BMS_ReadVoltageProtect(BMS_VoltageProtect_t *volt);
int BMS_ReadTempProtect(BMS_TempProtect_t *temp);
int BMS_ReadCurrentProtect(BMS_CurrentProtect_t *curr);
int BMS_ReadRealtimeData(BMS_RealtimeData_t *rt);
int BMS_ReadAll(BMS_Device_t *dev);

/* ---- 批量写入便捷函数 ---- */

int BMS_WriteCalibConfig(const BMS_CalibConfig_t *calib);
int BMS_WriteVoltageProtect(const BMS_VoltageProtect_t *volt);
int BMS_WriteTempProtect(const BMS_TempProtect_t *temp);
int BMS_WriteCurrentProtect(const BMS_CurrentProtect_t *curr);
int BMS_SendSleepCommand(void);
static void BMS_ClearRxState(void);
/* ---- CRC 工具 (公开以便调试) ---- */
uint16_t BMS_CRC16(const uint8_t *data, uint8_t len);

typedef struct{
		uint16_t CUR;//放电保护电流
		uint16_t Time_Action;//保护延时
		uint16_t Time_Recover;//恢复延时
}Current;
typedef struct{
		uint16_t RES1;//用户权限寄存器
		uint16_t Modbus_CMD_Data;//modbus命令携带参数
		uint16_t Modbus_CMD;//写入功能：modbus命令
		uint16_t Software_Version;//软件版本
		uint16_t Hardware_Version;//硬件版本=固件代号*100+固件版本号
		uint8_t Customer_ID[20];//以ASCII存放协议名称，以及电路板型号
		uint8_t Battery_ID[32];//电池ID
		uint16_t Restart_Times;//重启次数
		uint16_t Sleep_Times;//休眠次数
		uint16_t Power_Lost_Times;//掉电次数
		uint16_t Total_RUN_Time;//总运行时间
		uint16_t Err_Cell_OV_Times;//电芯过电压保护次数
		uint16_t Err_Cell_UV_Times;//电芯欠压保护次数
		uint16_t Err_Pack_OV_Times;//电池过电压保护次数
		uint16_t Err_Pack_UV_Times;//电池欠压保护次数
		uint16_t Err_CHG_OC_Times;//充电过电流保护次数
		uint16_t Err_DSG_OC_Times;//放电过电流保护次数
		uint16_t Err_CHG_OT_Times;//充电过温保护次数
		uint16_t Err_CHG_UT_Times;//充电低温保护次数
		uint16_t Err_DSG_OT_Times;//放电过温保护次数
		uint16_t Err_DSG_UT_Times;//放电低温保护次数
		uint16_t Err_SC_Times;//短路次数
		uint16_t NTC_Material;//NTC材料
		uint16_t Zero_Adjust;//电流0点校准
		uint16_t Current_Adjust;//电流斜率校准
		uint16_t Cell_Material;//电芯材料
		uint16_t Factory_Ah;//出厂额定容量
		uint16_t Cell_Voltage_Max;//电芯过充保护电压
		uint16_t Cell_Voltage_Max_Recover;//电芯过充保护恢复电压
		uint16_t Cell_Voltage_Min;//电芯过放保护电压
		uint16_t Cell_Voltage_Min_Recover;//电芯过放保护恢复电压
		uint16_t Pack_Voltage_Max;//电池过充保护电压
		uint16_t Pack_Voltage_Max_Recover;//电池过充保护恢复电压
		uint16_t Pack_Voltage_Min;//电池过放保护电压
		uint16_t Pack_Voltage_Min_Recover;//电池过放保护恢复电压
		int16_t Pack_TMP_Max_CHG;//充电高温保护温度
		int16_t Pack_TMP_Max_CHG_Recover;//充电高温保护恢复温度
		int16_t Pack_TMP_Max_DSG;//放电高温保护温度
		int16_t Pack_TMP_Max_DSG_Recover;//放电高温保护恢复温度
		int16_t Pack_TMP_Min_CHG;//充电低温保护温度
		int16_t Pack_TMP_Min_CHG_Recover;//充电低温保护恢复温度
		int16_t Pack_TMP_Min_DSG;//放电低温保护温度
		int16_t Pack_TMP_Min_DSG_Recover;//放电低温保护恢复温度
		int16_t MOS_TMP_Max;//MOS过温保护温度
		int16_t MOS_TMP_Max_Recover;//MOS过温保护恢复温度
		Current Current_DSG0;
		Current Current_DSG1;
		Current Current_DSG2;
		Current Current_CHG0;
		Current Current_CHG1;
		Current Current_CHG2;
		uint16_t Err_Flag_L;
		int16_t Cell_TMP;//电芯温度
		int16_t MOS_TMP;//MOS温度
		uint16_t SOC;//SOC
		uint16_t Cell_Voltage;//电芯电压
		uint16_t Cell_Current;//电芯电流
		uint16_t Out_Voltage;//输出电压
		int16_t Out_Current;//电流
}AR9000Wh;

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

