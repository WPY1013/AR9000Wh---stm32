#ifndef __NUMLED_H__
#define __NUMLED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


// --- 1. 保留位域联合体定义 ---
typedef union {
    uint8_t byte;
    struct {
        uint8_t bit0:1;
        uint8_t bit1:1;
        uint8_t bit2:1;
        uint8_t bit3:1;
        uint8_t bit4:1;
        uint8_t bit5:1;
        uint8_t bit6:1;
        uint8_t bit7:1;
    } Bits;
} ByteBit_Union;


// --- 3. 用宏定义映射IO引脚  ---
#define LED_IO_1_PORT  GPIOB
#define LED_IO_1_PIN   GPIO_PIN_5
#define LED_IO_2_PORT  GPIOB
#define LED_IO_2_PIN   GPIO_PIN_6
#define LED_IO_3_PORT  GPIOB
#define LED_IO_3_PIN   GPIO_PIN_7
#define LED_IO_4_PORT  GPIOB
#define LED_IO_4_PIN   GPIO_PIN_8
#define LED_IO_5_PORT  GPIOB
#define LED_IO_5_PIN   GPIO_PIN_9

// --- 4. 核心改进：用HAL函数模拟51的IO方向与电平控制 ---
// 宏定义：将IO设置为输出模式
#define LED_IO_1_OUT()  {GPIO_InitTypeDef init = {LED_IO_1_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}; HAL_GPIO_Init(LED_IO_1_PORT, &init);}
#define LED_IO_2_OUT()  {GPIO_InitTypeDef init = {LED_IO_2_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}; HAL_GPIO_Init(LED_IO_2_PORT, &init);}
#define LED_IO_3_OUT()  {GPIO_InitTypeDef init = {LED_IO_3_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}; HAL_GPIO_Init(LED_IO_3_PORT, &init);}
#define LED_IO_4_OUT()  {GPIO_InitTypeDef init = {LED_IO_4_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}; HAL_GPIO_Init(LED_IO_4_PORT, &init);}
#define LED_IO_5_OUT()  {GPIO_InitTypeDef init = {LED_IO_5_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}; HAL_GPIO_Init(LED_IO_5_PORT, &init);}

// 宏定义：将IO设置为输入模式 
#define LED_IO_1_IN()  {GPIO_InitTypeDef init = {LED_IO_1_PIN, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}; HAL_GPIO_Init(LED_IO_1_PORT, &init);}
#define LED_IO_2_IN()  {GPIO_InitTypeDef init = {LED_IO_2_PIN, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}; HAL_GPIO_Init(LED_IO_2_PORT, &init);}
#define LED_IO_3_IN()  {GPIO_InitTypeDef init = {LED_IO_3_PIN, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}; HAL_GPIO_Init(LED_IO_3_PORT, &init);}
#define LED_IO_4_IN()  {GPIO_InitTypeDef init = {LED_IO_4_PIN, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}; HAL_GPIO_Init(LED_IO_4_PORT, &init);}
#define LED_IO_5_IN()  {GPIO_InitTypeDef init = {LED_IO_5_PIN, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW}; HAL_GPIO_Init(LED_IO_5_PORT, &init);}

// 宏定义：写电平
#define LED_IO_1_WRITE(val) HAL_GPIO_WritePin(LED_IO_1_PORT, LED_IO_1_PIN, (GPIO_PinState)(val))
#define LED_IO_2_WRITE(val) HAL_GPIO_WritePin(LED_IO_2_PORT, LED_IO_2_PIN, (GPIO_PinState)(val))
#define LED_IO_3_WRITE(val) HAL_GPIO_WritePin(LED_IO_3_PORT, LED_IO_3_PIN, (GPIO_PinState)(val))
#define LED_IO_4_WRITE(val) HAL_GPIO_WritePin(LED_IO_4_PORT, LED_IO_4_PIN, (GPIO_PinState)(val))
#define LED_IO_5_WRITE(val) HAL_GPIO_WritePin(LED_IO_5_PORT, LED_IO_5_PIN, (GPIO_PinState)(val))

static uint8_t led_disp_step = 0; // 扫描步骤

#define DG1_A DG1_Flag.Bits.bit0
#define DG1_B DG1_Flag.Bits.bit1
#define DG1_C DG1_Flag.Bits.bit2
#define DG1_D DG1_Flag.Bits.bit3
//#define DG1_E DG1_Flag.Bits.bit4
//#define DG1_F DG1_Flag.Bits.bit5
//#define DG1_G DG1_Flag.Bits.bit6

#define DG2_A DG2_Flag.Bits.bit0
#define DG2_B DG2_Flag.Bits.bit1
#define DG2_C DG2_Flag.Bits.bit2
#define DG2_D DG2_Flag.Bits.bit3
#define DG2_E DG2_Flag.Bits.bit4
#define DG2_F DG2_Flag.Bits.bit5
#define DG2_G DG2_Flag.Bits.bit6

#define DG3_A DG3_Flag.Bits.bit0
#define DG3_B DG3_Flag.Bits.bit1
#define DG3_C DG3_Flag.Bits.bit2
#define DG3_D DG3_Flag.Bits.bit3
#define DG3_E DG3_Flag.Bits.bit4
#define DG3_F DG3_Flag.Bits.bit5
#define DG3_G DG3_Flag.Bits.bit6

void Num_Led_Init(void);
void Led_under_deal(void);
void DG_Display(uint8_t num1, uint8_t num2, uint8_t num3);
void DG_ALL(uint8_t EN_DIS);
void DG_LED(uint8_t EN_DIS);
void Soc_Show(void);

#ifdef __cplusplus
}
#endif

#endif

