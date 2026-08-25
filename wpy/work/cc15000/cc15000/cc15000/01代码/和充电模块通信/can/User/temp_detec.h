#ifndef __TEMP_DETEC_H
#define __TEMP_DETEC_H
#include "stdint.h"

// 定义5路CC检测对应的位掩码（从低位到高位）
#define CC1_MASK  (1 << 0) 
#define CC2_MASK  (1 << 1) 
#define CC3_MASK  (1 << 2) 
#define CC4_MASK  (1 << 3) 
#define CC5_MASK  (1 << 4) 
extern volatile uint8_t CC_MASK;

extern float Wire_T1_V;
extern float Wire_T1_R;
extern float Wire_T2_V;
extern float Wire_T2_R;
extern float Seat3_T1_V;
extern float Seat3_T1_R;
extern float Seat2_CC2_V;

extern float Seat1_T1_V;
extern float Seat1_T1_R;
extern float Seat1_T2_V;
extern float Seat1_T2_R;
extern float Seat2_T1_V;
extern float Seat2_T1_R;
extern float Seat2_T2_V;
extern float Seat2_T2_R;
extern float Seat3_T2_V;
extern float Seat3_T2_R;

extern float Seat4_T1_V;
extern float Seat4_T1_R;
extern float Seat4_T2_V;
extern float Seat4_T2_R;
extern float Seat3_CC2_V;
extern float Seat4_CC2_V;

void Temp_ADC1(const uint16_t *adc_data);
void Temp_ADC2(const uint16_t *adc_data);
void Temp_ADC3(const uint16_t *adc_data);
void ADC_SnapshotProcess(void);

#endif
