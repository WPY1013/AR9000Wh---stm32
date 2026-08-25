#include "cc_detec.h"
#include "main.h"
#include "temp_detec.h"
#include "gpio.h"
#include "tim.h"
#include "cc15000.h"

static State sm = STATE_IDLE;
extern volatile uint8_t Upele_MASK;
volatile uint8_t Gun_lock_flag = 0;
static volatile uint8_t timer_run_flag_1seat = 0;  // 单枪计时锁
static volatile uint8_t timer_run_flag_2seat = 0;  // 双枪计时锁

static volatile uint8_t g_StateErrCode = ERR_NONE;
uint32_t Lock_ms;
void Set_StateErrCode(uint8_t err)
{
    g_StateErrCode = err;
}
uint8_t Get_StateErrCode(void)
{
    return g_StateErrCode;
}
void Clear_StateErrCode(void)
{
    g_StateErrCode = ERR_NONE;
}

// 对外设置状态接口
void Set_State(State new_state)
{
    // 可增加合法性校验
    if(new_state <= STATE_DEFAULT)
    {
        sm = new_state;
    }
}
void One_Seat(void)
{
	if(CC_MASK){
		if(CC_MASK > 31){
				Set_State(STATE_DEFAULT);
				Set_StateErrCode(ERR_CC_MASK_OVERFLOW);//更新故障码
			return ;
		}
		//枪线插入 && 枪座插入
		if((CC_MASK&CC1_MASK) && (CC_MASK&~CC1_MASK) ){
			//启动计时
			if(!timer_run_flag_1seat){Time_NonBlock_Start();timer_run_flag_1seat = 1;}
			
			// 长时间未收到上电报文
			if(g_stTimeFlag.bFlag3s && Get_StateErrCode() != ERR_POWER_MSG_TIMEOUT){
				Set_State(STATE_DEFAULT);
				Set_StateErrCode(ERR_POWER_MSG_TIMEOUT);//更新故障码
				g_stTimeFlag.bFlag3s = 0;
				timer_run_flag_1seat = 0;
				return ;
			}
		}else timer_run_flag_1seat = 0;
		if((CC_MASK&CC1_MASK) && (CC_MASK&~CC1_MASK)){
				if(Upele_MASK == Upele1_MASK || Upele_MASK == Upele2_MASK){
					Set_State(STATE_ONE_CHARG);
		//			Time_NonBlock_Start();
					return ;
				}
				if(Upele_MASK == (Upele1_MASK | Upele2_MASK)){
					Set_State(STATE_TWO_CHARG);
		//			Time_NonBlock_Start();
					return ;
				}
			}
	}
}
void Two_Seat(void)
{
	if(CC_MASK){
		if(CC_MASK > 31){
			Set_State(STATE_DEFAULT);
			Set_StateErrCode(ERR_CC_MASK_OVERFLOW);//更新故障码
			return ;
		}
		//枪线插入 && 枪座插入
		if((CC_MASK&CC1_MASK) && (CC_MASK&~CC1_MASK) ){
			//启动计时
			if(!timer_run_flag_2seat){Time_NonBlock_Start();timer_run_flag_2seat = 1;}
			// 长时间未收到上电报文
			if(g_stTimeFlag.bFlag3s && Get_StateErrCode() != ERR_POWER_MSG_TIMEOUT){
				Set_State(STATE_DEFAULT);
				Set_StateErrCode(ERR_POWER_MSG_TIMEOUT);//更新故障码
				g_stTimeFlag.bFlag3s = 0;
				timer_run_flag_2seat = 0;
				return ;
			}
		}else timer_run_flag_2seat = 0;
		if(Upele_MASK == (Upele1_MASK | Upele2_MASK)){
			Set_State(STATE_TWO_CHARG);
//			Time_NonBlock_Start();
			return ;
		}
	}
}

void One_Seat_Charg(void)
{
		//枪锁闭合
    if(Gun_lock_flag == 0){
			Gun_lock_flag = 1;
			Lock_ms = HAL_GetTick();
			Enable_Gun_lock();
		}
//		if(g_stTimeFlag.bFlag120ms)
//			{
		if(HAL_GetTick() - Lock_ms >= 120 && Gun_lock_flag == 1){
					Stop_Gun_lock();
					Gun_lock_flag = 2;
		}
					// 120ms业务
//					g_stTimeFlag.bFlag120ms = 0;
//			}
			
		//枪线+两个枪座
		if(Upele_MASK == (Upele1_MASK | Upele2_MASK)){Set_State(STATE_TWO_CHARG);return ;}
		//充电完成
//		if(!Upele_MASK) {Set_State(STATE_CHARGEND);Time_NonBlock_Start();return ;}
		if(!Upele_MASK && Gun_lock_flag == 2) {Set_State(STATE_CHARGEND);return ;}
	
}
void Two_Seat_Charg(void)
{
		//枪锁闭合
		if(Gun_lock_flag == 0){
			Gun_lock_flag = 1;
			Lock_ms = HAL_GetTick();
			Enable_Gun_lock();
		}
//		if(g_stTimeFlag.bFlag120ms)
//			{
			if(HAL_GetTick() - Lock_ms >= 120 && Gun_lock_flag == 1){
					Stop_Gun_lock();
				Gun_lock_flag = 2;
			}
					// 120ms业务
//					g_stTimeFlag.bFlag120ms = 0;
//			}
		if(Upele_MASK == Upele1_MASK || Upele_MASK == Upele2_MASK) {Set_State(STATE_ONE_CHARG);return ;}
		//充电完成
//		if(!Upele_MASK) {Set_State(STATE_CHARGEND);Time_NonBlock_Start();return ;}
		if(!Upele_MASK && Gun_lock_flag == 2) {Set_State(STATE_CHARGEND);return ;}
}
void State_Charg_End(void)
{
		//两个枪座均充电完成
    if(!Upele_MASK){
			//两个枪座均拔出
			if(CC_MASK == 1 && Gun_lock_flag == 2){
				Lock_ms = HAL_GetTick();
				//枪锁打开
//				Gun_lock_flag = 0;
				Gun_lock_flag = 3;
				Disable_Gun_lock();
			}
//			if(g_stTimeFlag.bFlag180ms)
//			{
			if(HAL_GetTick() - Lock_ms >= 180 && Gun_lock_flag == 3){
					Stop_Gun_lock();
					Gun_lock_flag = 0;
			}
					// 180ms业务
//					g_stTimeFlag.bFlag180ms = 0;
//			}
			//枪线枪座均拔出
			if(CC_MASK == 0 && Gun_lock_flag == 0) Set_State(STATE_IDLE);
		}
}

void UpdateErrorLed(void)
{
    if (Get_StateErrCode() != ERR_NONE)
    {
        Led_ON;
    }
    else
    {
        Led_OFF;
    }
}

void ErrCode(void)
{
	uint8_t cur_err = Get_StateErrCode();
	switch(cur_err)
	{
			case ERR_CC_MASK_OVERFLOW://CC_MASK掩码越界
//					Led_ON;
					break;
			case ERR_POWER_MSG_TIMEOUT://多枪在位，但长时间未收到上电报文超时
//					Led_ON;
					break;
			case ERR_NO_GUN_CONNECT:// 枪线枪座均未插入
//					Led_ON;
					break;
			case ERR_GUN_LINE_DISCONNECT:// 仅枪线未插入（枪座在位）
//					Led_ON;
					break;
			case ERR_GUN_BASE_DISCONNECT:// 仅枪座未插入（枪线在位）
//					Led_ON;
					break;
			default: break;
	}
}
// ===================== 状态机函数 =====================
void STATE_FUN(void) {
	switch (sm){
		case STATE_IDLE://CC检测
				One_Seat();//枪线+单个枪座
//				Two_Seat();//枪线+两个枪座
				break;
		case STATE_ONE_CHARG:One_Seat_Charg();break;
		case STATE_TWO_CHARG:Two_Seat_Charg();break;
		case STATE_CHARGEND:State_Charg_End();break;
		case STATE_DEFAULT:sm = STATE_IDLE;ErrCode();break;
	}
}
