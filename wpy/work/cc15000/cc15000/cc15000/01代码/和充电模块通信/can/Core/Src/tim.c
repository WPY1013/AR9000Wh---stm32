#include "tim.h"

TIM_HandleTypeDef htim2;

/* 全局计时结构体初始化清零 */
volatile TimeFlag_t g_stTimeFlag = {0};

/**
  * @brief TIM2 Initialization Function 1ms中断，160MHz定时器时钟
  * @param None
  * @retval None
  */
void MX_TIM2_Init(void)
{
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 159;                // PSC=159，160M/160=1MHz
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;                   // ARR=999，1MHz/1000=1ms溢出
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  启动非阻塞计时，单次调用清零所有计数与标志
  */
void Time_NonBlock_Start(void)
{
    g_stTimeFlag.u32TickCnt = 0;
    g_stTimeFlag.bFlag100ms = 0;
		g_stTimeFlag.bFlag120ms = 0;
		g_stTimeFlag.bFlag180ms = 0;
    g_stTimeFlag.bFlag500ms = 0;
    g_stTimeFlag.bFlag1s    = 0;
    g_stTimeFlag.bFlag3s    = 0;
    g_stTimeFlag.bRunEn = 1;
}

/**
  * @brief  停止计时，冻结计数
  */
void Time_NonBlock_Stop(void)
{
    g_stTimeFlag.bRunEn = 0;
}

/**
  * @brief 定时器底层初始化回调（CubeMX自动生成框架）
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if(tim_baseHandle->Instance==TIM2)
  {
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if(tim_baseHandle->Instance==TIM2)
  {
    __HAL_RCC_TIM2_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  }
}

/**
  * @brief 定时器周期溢出回调，1ms进一次中断
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
			
        if(g_stTimeFlag.bRunEn == 1)
        {
            g_stTimeFlag.u32TickCnt += TIM2_BASE_MS;

            // 到达阈值仅置1一次，主循环处理后清零
            if((g_stTimeFlag.u32TickCnt >= TIME_100MS) && (g_stTimeFlag.bFlag100ms == 0))
            {
                g_stTimeFlag.bFlag100ms = 1;
            }
						if((g_stTimeFlag.u32TickCnt >= TIME_120MS) && (g_stTimeFlag.bFlag120ms == 0))
            {
                g_stTimeFlag.bFlag120ms = 1;
            }
						if((g_stTimeFlag.u32TickCnt >= TIME_180MS) && (g_stTimeFlag.bFlag180ms == 0))
            {
                g_stTimeFlag.bFlag180ms = 1;
            }
            if((g_stTimeFlag.u32TickCnt >= TIME_500MS) && (g_stTimeFlag.bFlag500ms == 0))
            {
                g_stTimeFlag.bFlag500ms = 1;
            }
            if((g_stTimeFlag.u32TickCnt >= TIME_1S) && (g_stTimeFlag.bFlag1s == 0))
            {
                g_stTimeFlag.bFlag1s = 1;
            }
            if((g_stTimeFlag.u32TickCnt >= TIME_3S) && (g_stTimeFlag.bFlag3s == 0))
            {
                g_stTimeFlag.bFlag3s = 1;
                g_stTimeFlag.bRunEn = 0; // 3s计时结束自动停止
            }
        }
    }
}
