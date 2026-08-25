#include "numled.h"
#include "modbus.h"

volatile ByteBit_Union DG1_Flag = {0};
volatile ByteBit_Union DG2_Flag = {0};
volatile ByteBit_Union DG3_Flag = {0};
BMS_RealtimeData_t rt;

void Num_Led_Init(void)
{
		__HAL_RCC_GPIOB_CLK_ENABLE();
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
		
		GPIO_InitStruct.Pin = LED_IO_1_PIN | LED_IO_2_PIN | LED_IO_3_PIN | LED_IO_4_PIN | LED_IO_5_PIN;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(LED_IO_1_PORT, &GPIO_InitStruct);
		
}

void Led_under_deal(void) {
    // 第一步：所有IO置为输入(高阻)，并软件拉低
    LED_IO_1_IN();
    LED_IO_2_IN();
    LED_IO_3_IN();
    LED_IO_4_IN();
    LED_IO_5_IN();
    LED_IO_1_WRITE(0);
    LED_IO_2_WRITE(0);
    LED_IO_3_WRITE(0);
    LED_IO_4_WRITE(0);
    LED_IO_5_WRITE(0);

    // 第二步：根据步骤，设置对应的IO为输出并置高，公共脚为输出并置低
    if (led_disp_step == 0) {
        if (DG3_B) { LED_IO_2_OUT(); LED_IO_2_WRITE(1); }//B3
        if (DG3_D) { LED_IO_3_OUT(); LED_IO_3_WRITE(1); }//D3
        if (DG3_F) { LED_IO_4_OUT(); LED_IO_4_WRITE(1); }//F3
        if (DG3_G) { LED_IO_5_OUT(); LED_IO_5_WRITE(1); }//G3
        LED_IO_1_OUT();
        LED_IO_1_WRITE(0);//公共脚1
    } 
    else if (led_disp_step == 1) {
        if (DG2_B) { LED_IO_3_OUT(); LED_IO_3_WRITE(1); }//B2
        if (DG2_D) { LED_IO_4_OUT(); LED_IO_4_WRITE(1); }//D2
		if (DG2_E) { LED_IO_5_OUT(); LED_IO_5_WRITE(1); }//E2
        if (DG3_A) { LED_IO_1_OUT(); LED_IO_1_WRITE(1); }//A3
        LED_IO_2_OUT();
        LED_IO_2_WRITE(0);//公共脚2
    } 
    else if (led_disp_step == 2) {
        if (DG2_A) { LED_IO_2_OUT(); LED_IO_2_WRITE(1); }//A2
        if (DG2_C) { LED_IO_4_OUT(); LED_IO_4_WRITE(1); }//C2
		if (DG2_F) { LED_IO_5_OUT(); LED_IO_5_WRITE(1); }//F2
        if (DG3_C) { LED_IO_1_OUT(); LED_IO_1_WRITE(1); }//C3
        LED_IO_3_OUT();
        LED_IO_3_WRITE(0);//公共脚3
    } 
    else if (led_disp_step == 3) {
        if (DG3_E) { LED_IO_1_OUT(); LED_IO_1_WRITE(1); }//E3
		if (DG2_G) { LED_IO_5_OUT(); LED_IO_5_WRITE(1); }//G2
		if (DG1_B) { LED_IO_3_OUT(); LED_IO_3_WRITE(1); }//B1
		if (DG1_C) { LED_IO_2_OUT(); LED_IO_2_WRITE(1); }//C1
        LED_IO_4_OUT();
        LED_IO_4_WRITE(0);//公共脚4
    }
    else if (led_disp_step == 4) {
		if (DG1_A) { LED_IO_3_OUT(); LED_IO_3_WRITE(1); }//充放电符号
		if (DG1_D) { LED_IO_2_OUT(); LED_IO_2_WRITE(1); }//“%”符号
		LED_IO_5_OUT();
		LED_IO_5_WRITE(0);//公共脚5
	}
    led_disp_step++;
    if (led_disp_step >= 5) led_disp_step = 0;
}

// --- 6. 显示函数 ---
void DG_ALL(uint8_t EN_DIS) {
    if (EN_DIS) { // 全开
        DG1_Flag.byte = 0xff;
        DG2_Flag.byte = 0xff;
		DG3_Flag.byte = 0xff;
    } else { // 全关
        DG1_Flag.byte = 0;
        DG2_Flag.byte = 0;
		DG3_Flag.byte = 0;
    }
}
//后面的符号灯：充电全开 -- 1 放电开一个 -- 2 全关 -- 3
void DG_LED(uint8_t EN_DIS) {
    if (EN_DIS == 1) { // 全开
        DG1_A = 1;
				DG1_D = 1;
    } else if (EN_DIS == 2) { // %开、充电灯关
        DG1_A = 1;
				DG1_D = 0;
    }else if (EN_DIS == 3) { // 全关
        DG1_A = 0;
        DG1_D = 0;
    }
}

/*
num1:第一个数码管显示的百位数字（1）
num2:第二个数码管显示的十位数字或故障开头E（0-9、E）
num3:第三个数码管显示的个位数字或故障数字（0-9）
*/
void DG_Display(uint8_t num1, uint8_t num2, uint8_t num3) {
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
		DG_ALL(0); // 先全部清零
		// 设置第一个数码管的段(1) 
    switch(num1) {
        case 1: DG1_B=1;DG1_C=1; break;// 1
        default: break;
    }
    // 设置第二个数码管的段（0-9、E） 
    switch(num2) {
        case 0: DG2_A=1;DG2_B=1;DG2_C=1;DG2_D=1;DG2_E=1;DG2_F=1; break;//0
        case 1: DG2_B=1;DG2_C=1; break;//1
        case 2: DG2_A=1;DG2_B=1;DG2_D=1;DG2_E=1;DG2_G=1; break;//2
        case 3: DG2_A=1;DG2_B=1;DG2_C=1;DG2_D=1;DG2_G=1; break;//3
		case 4: DG2_B=1;DG2_C=1;DG2_F=1;DG2_G=1; break;//4
		case 5: DG2_A=1;DG2_C=1;DG2_D=1;DG2_F=1;DG2_G=1; break;//5
		case 6: DG2_A=1;DG2_C=1;DG2_D=1;DG2_E=1;DG2_F=1;DG2_G=1; break;//6
		case 7: DG2_A=1;DG2_B=1;DG2_C=1; break;//7
		case 8: DG2_A=1;DG2_B=1;DG2_C=1;DG2_D=1;DG2_E=1;DG2_F=1;DG2_G=1; break;//8
        case 9: DG2_A=1;DG2_B=1;DG2_C=1;DG2_D=1;DG2_F=1;DG2_G=1; break;//9
		case 10: DG2_A=1;DG2_D=1;DG2_E=1;DG2_F=1;DG2_G=1; break;//E
        default: break;
    }

    // 设置第三个数码管的段 （0-9）
    switch(num3) {
        case 0: DG3_A=1;DG3_B=1;DG3_C=1;DG3_D=1;DG3_E=1;DG3_F=1; break;//0
        case 1: DG3_B=1;DG3_C=1; break;//1
        case 2: DG3_A=1;DG3_B=1;DG3_D=1;DG3_E=1;DG3_G=1; break;//2
		case 3: DG3_A=1;DG3_B=1;DG3_C=1;DG3_D=1;DG3_G=1; break;//3
		case 4: DG3_B=1;DG3_C=1;DG3_F=1;DG3_G=1; break;//4
		case 5: DG3_A=1;DG3_C=1;DG3_D=1;DG3_F=1;DG3_G=1; break;//5
		case 6: DG3_A=1;DG3_C=1;DG3_D=1;DG3_E=1;DG3_F=1;DG3_G=1; break;//6
		case 7: DG3_A=1;DG3_B=1;DG3_C=1; break;//7
		case 8: DG3_A=1;DG3_B=1;DG3_C=1;DG3_D=1;DG3_E=1;DG3_F=1;DG3_G=1; break;//8
        case 9: DG3_A=1;DG3_B=1;DG3_C=1;DG3_D=1;DG3_F=1;DG3_G=1; break;//9
        case 10: DG3_A=1;DG3_B=1;DG3_C=1;DG3_E=1;DG3_F=1;DG3_G=1; break;//A
		case 11: DG3_C=1;DG3_D=1;DG3_E=1;DG3_F=1;DG3_G=1; break;//b
		case 12: DG3_D=1;DG3_E=1;DG3_G=1; break;//c
		case 13: DG3_B=1;DG3_C=1;DG3_D=1;DG3_E=1;DG3_G=1; break;//d
		//case 13: DG3_A=1;DG3_B=1;DG3_D=1;DG3_E=1;DG3_F=1;DG3_G=1;break;//e
        case 14: DG3_A=1;DG3_D=1;DG3_E=1;DG3_F=1;DG3_G=1; break;//E
		case 15: DG3_A=1;DG3_E=1;DG3_F=1;DG3_G=1;break;//F
				
        default: break;
    }
		if (primask == 0U) {
        __enable_irq();
    }
}
extern volatile uint8_t Sleep_Flag;
volatile int bms_read_result;
void Soc_Show(void)
{
    uint8_t fault_code = 0U;
    uint8_t bit;
		for (uint8_t retry = 0; retry < 30; retry++) {
				HAL_Delay(100);
				bms_read_result = BMS_ReadRealtimeData(&rt);
				if (bms_read_result == 0) {Sleep_Flag = 0;
						break;
				}
				
		}
    ErrFlag_t terr_flag = rt.err_flag;
    if(terr_flag.raw == 0){// 无故障
        int16_t out_current = rt.out_current / 100;// 输出电流单位为A
        if(out_current >= 0){// 放电
            uint16_t soc = rt.soc / 10;
            uint8_t soc_bai = soc / 100;  // 百位部分
            uint8_t soc_shi = (soc / 10) % 10;  // 十位部分
            uint8_t soc_ge = soc % 10;  // 个位部分
            DG_Display(soc_bai, soc_shi, soc_ge);
            DG_LED(2);  // 打开放电指示灯
        }
        else if(out_current < 0){// 充电
            uint16_t soc = rt.soc / 10;
            uint8_t soc_bai = soc / 100;  // 百位部分
            uint8_t soc_shi = (soc / 10) % 10;  // 十位部分
            uint8_t soc_ge = soc % 10;  // 个位部分
            DG_Display(soc_bai, soc_shi, soc_ge);
            DG_LED(1);  // 打开充电指示灯
        }
    }else{// 有故障
        //多个故障同时存在时显示最高位故障
        for (bit = 0U; bit < 16U; bit++) {
            if ((terr_flag.raw & ((uint16_t)1U << bit)) != 0U) {
                fault_code = bit;
            }
        }
        DG_Display(0, 10, fault_code);
        DG_LED(3);  // 关闭充放电指示灯
    }
}
