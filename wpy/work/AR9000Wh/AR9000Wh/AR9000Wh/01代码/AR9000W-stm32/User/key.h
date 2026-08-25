#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define GPIO_KEY_PORT GPIOB
#define GPIO_KEY_PIN  GPIO_PIN_0
#define GPIO_Sleep_PORT GPIOB
#define GPIO_Sleep_PIN  GPIO_PIN_1
#define Key    HAL_GPIO_ReadPin(GPIO_KEY_PORT, GPIO_KEY_PIN)


typedef enum
{
    KEY_CHECK = 0,
    KEY_COMFIRM = 1,
    KEY_RELEASE = 2 
}KEY_STATE;



typedef enum
{
    NULL_KEY = 0,
    SHORT_KEY,
    LONG_KEY
} KEY_TYPE;

extern volatile KEY_STATE KeyState;
extern volatile KEY_TYPE g_KeyActionFlag;
extern volatile uint16_t TimeCnt;

void Key_Init(void);
void Key_Scan(void);
void Key_Action(void);

#ifdef __cplusplus
}
#endif

#endif /* __KEY_H__ */
