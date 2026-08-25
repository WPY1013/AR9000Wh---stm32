#include "key.h"
#include "gpio.h"
#include "numled.h"
#include "modbus.h"

uint8_t g_KeyFlag = 0;                 // 按键有效标志，0： 按键值无效； 1：按键值有效
volatile KEY_STATE KeyState = KEY_CHECK;
volatile KEY_TYPE g_KeyActionFlag = NULL_KEY;
volatile uint16_t TimeCnt = 0U;

void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
		HAL_GPIO_WritePin(GPIO_Sleep_PORT, GPIO_Sleep_PIN, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = GPIO_KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIO_KEY_PORT, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = GPIO_Sleep_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIO_Sleep_PORT, &GPIO_InitStruct);
    KeyState = KEY_CHECK;
    g_KeyActionFlag = NULL_KEY;
    TimeCnt = 0U;
}

void Key_Scan(void)
{
    static uint16_t TimeCnt = 0;
    static uint8_t lock = 0;
    switch (KeyState)
    {
        //按键未按下状态，此时判断Key的值
        case   KEY_CHECK:    
           if(!Key)   
            {
                KeyState =  KEY_COMFIRM;  //如果按键Key值为0，说明按键开始按下，进入下一个状态 
            }
            TimeCnt = 0;                  //计数复位
            lock = 0;
            break;
            
        case   KEY_COMFIRM:
            if(!Key)                     //查看当前Key是否还是0，再次确认是否按下
            {
                if(!lock)   lock = 1;
               
                TimeCnt++;   
                               
            }   
            else                       
            {
                if(lock)                // 不是第一次进入，  释放按键才执行
                {
                    /*按键时长判断*/
                    if(TimeCnt > 100)            // 长按 1 s
                    {
                        g_KeyActionFlag = LONG_KEY;
                        TimeCnt = 0;  
                    }
                    else                         // Key值变为了1，说明此处动作为短按
                    {
                        g_KeyActionFlag = SHORT_KEY;          // 短按
                    }
                    /*按键时长判断*/
                    
                    KeyState =  KEY_RELEASE;    // 需要进入按键释放状态 
                }
                
                else                          // 当前Key值为1，确认为抖动，则返回上一个状态
                {
                    KeyState =  KEY_CHECK;    // 返回上一个状态
                }
               
            } 
            break;
            
         case  KEY_RELEASE:
             if(Key)                     //当前Key值为1，说明按键已经释放，返回开始状态
             { 
                 KeyState =  KEY_CHECK;    
             }
             break;
             
         default: break;
    }

}
volatile int sleep_result = 0;
volatile uint8_t Sleep_Flag = 1;
volatile uint8_t Soc_Flag = 0;
void Key_Action(void)
{
    KEY_TYPE action = g_KeyActionFlag;
    if (action == SHORT_KEY)
    {
				if(Sleep_Flag){
					HAL_GPIO_WritePin(GPIO_Sleep_PORT, GPIO_Sleep_PIN, GPIO_PIN_RESET);// 拉低 → 复位
					HAL_Delay(200);

					HAL_GPIO_WritePin(GPIO_Sleep_PORT, GPIO_Sleep_PIN, GPIO_PIN_SET);// 拉高 → 释放
				}
				HAL_Delay(200);
				Soc_Flag = 1;
				
        g_KeyActionFlag = NULL_KEY;
    }
    else if (action == LONG_KEY)
    {
				volatile int ret = BMS_SendSleepCommand();
				if(ret == 0){
					DG_ALL(0);
					Sleep_Flag = 1;
					Soc_Flag = 0;
				}g_KeyActionFlag = NULL_KEY;
    }
}
