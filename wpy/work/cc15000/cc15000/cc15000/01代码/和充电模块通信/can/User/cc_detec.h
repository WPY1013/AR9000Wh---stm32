#ifndef __CC_DETEC_H
#define __CC_DETEC_H

#include "stdint.h"

typedef enum {
    STATE_IDLE,
    STATE_ONE_CHARG,
    STATE_TWO_CHARG,
    STATE_CHARGEND,
    STATE_DEFAULT
} State;


#define ERR_NONE                0U// 0 = 无故障，正常
#define ERR_CC_MASK_OVERFLOW    1U// 故障1：CC_MASK掩码越界（大于31，非法硬件采样值）
#define ERR_POWER_MSG_TIMEOUT   2U// 故障2：多枪在位，但长时间未收到上电报文超时
#define ERR_NO_GUN_CONNECT     	3U  // 枪线枪座均未插入
#define ERR_GUN_LINE_DISCONNECT 4U // 仅枪线未插入（枪座在位）
#define ERR_GUN_BASE_DISCONNECT 5U // 仅枪座未插入（枪线在位）


void Set_StateErrCode(uint8_t err);
uint8_t Get_StateErrCode(void);
void Clear_StateErrCode(void);
// 对外提供设置状态函数
void UpdateErrorLed(void);
void Set_State(State new_state);
void STATE_FUN(void);

#endif
