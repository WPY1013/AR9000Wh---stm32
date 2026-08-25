#ifndef __TIM_H
#define __TIM_H

#include "stm32g4xx_hal.h"

/* 定时器句柄 */
extern TIM_HandleTypeDef htim2;

/* 定时基础参数 */
#define TIM2_BASE_MS        1U        // 定时器中断周期1ms
#define TIME_100MS          100U
#define TIME_120MS          120U
#define TIME_180MS          180U
#define TIME_500MS          500U
#define TIME_1S             1000U
#define TIME_3S             3000U


/* 多时间标志结构体 */
typedef struct
{
    uint32_t u32TickCnt;       // 累计1ms节拍
    uint8_t  bRunEn;           // 计时总使能
    uint8_t  bFlag100ms;
	  uint8_t  bFlag120ms;
	  uint8_t  bFlag180ms;
    uint8_t  bFlag500ms;
    uint8_t  bFlag1s;
    uint8_t  bFlag3s;
} TimeFlag_t;

/* 全局计时实例 */
extern volatile TimeFlag_t g_stTimeFlag;

/* 函数声明 */
void MX_TIM2_Init(void);
void Time_NonBlock_Start(void);
void Time_NonBlock_Stop(void);
void Error_Handler(void);

#endif
