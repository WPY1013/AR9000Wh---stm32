#include "temp_detec.h"
#include "adc_snapshot.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>

#define AVG_CNT          30U
#define ADC_FULL_SCALE   4095.0f
#define ADC_VREF         3.3f
#define CC_GAIN                  (45.0f / 11.0f)
#define WIRE_CC1_CAL_GAIN        (11.58f / 11.339f)

typedef struct
{
    float samples[AVG_CNT];
    uint8_t index;
    uint8_t count;
} AverageFilter;

/* ADC1 filters */
static AverageFilter filter_wire_t1;
static AverageFilter filter_wire_t2;
static AverageFilter filter_seat3_t1;
static AverageFilter filter_seat2_cc2;

/* ADC2 filters */
static AverageFilter filter_seat1_t1;
static AverageFilter filter_seat1_t2;
static AverageFilter filter_seat2_t1;
static AverageFilter filter_seat2_t2;
static AverageFilter filter_wire_cc1;
static AverageFilter filter_seat1_cc2;
static AverageFilter filter_seat3_t2;

/* ADC3 filters */
static AverageFilter filter_seat4_t1;
static AverageFilter filter_seat4_t2;
static AverageFilter filter_seat3_cc2;
static AverageFilter filter_seat4_cc2;

uint16_t Wire_T1_value;
uint16_t Wire_T2_value;
uint16_t Seat3_T1_value;
uint16_t Seat2_CC2_value;
float Wire_T1_V;
float Wire_T1_R;
float Wire_T1;
float Wire_T2_V;
float Wire_T2_R;
float Wire_T2;
float Seat3_T1_V;
float Seat3_T1_R;
float Seat3_T1;
float Seat2_CC2_V;

uint16_t Seat1_T1_value;
uint16_t Seat1_T2_value;
uint16_t Seat2_T1_value;
uint16_t Seat2_T2_value;
uint16_t Wire_CC1_value;
uint16_t Seat1_CC2_value;
uint16_t Seat3_T2_value;
float Seat1_T1_V;
float Seat1_T1_R;
float Seat1_T1;
float Seat1_T2_V;
float Seat1_T2_R;
float Seat1_T2;
float Seat2_T1_V;
float Seat2_T1_R;
float Seat2_T1;
float Seat2_T2_V;
float Seat2_T2_R;
float Seat2_T2;
float Wire_CC1_V;
float Seat1_CC2_V;
float Seat3_T2_V;
float Seat3_T2_R;
float Seat3_T2;

uint16_t Seat4_T1_value;
uint16_t Seat4_T2_value;
uint16_t Seat3_CC2_value;
uint16_t Seat4_CC2_value;
float Seat4_T1_V;
float Seat4_T1_R;
float Seat4_T1;
float Seat4_T2_V;
float Seat4_T2_R;
float Seat4_T2;
float Seat3_CC2_V;
float Seat4_CC2_V;

volatile uint8_t CC_MASK;
//多次采集求平均值（30次去掉最大值、最小值）
static float Filter_Update(AverageFilter *filter, uint16_t adc_value)
{
    float voltage;
    float sum = 0.0f;
    float minimum;
    float maximum;
    uint8_t i;

    if (filter == NULL)
    {
        return 0.0f;
    }

    voltage = ADC_VREF * (float)adc_value / ADC_FULL_SCALE;
    filter->samples[filter->index] = voltage;
    filter->index++;

    if (filter->index >= AVG_CNT)
    {
        filter->index = 0U;
    }

    if (filter->count < AVG_CNT)
    {
        filter->count++;
    }

    minimum = filter->samples[0];
    maximum = filter->samples[0];

    for (i = 0U; i < filter->count; i++)
    {
        float sample = filter->samples[i];
        sum += sample;
        if (sample < minimum){minimum = sample;}
        if (sample > maximum){maximum = sample;}
    }

    if (filter->count < 3U)
    {
        return sum / (float)filter->count;
    }

    return (sum - minimum - maximum) / (float)(filter->count - 2U);
}

static float VoltageToTemperature(float voltage, float *resistance)
{
    const float coefficient_a = 0.0039083f;
    const float coefficient_b = -5.775e-7f;
    float denominator;
    float resistance_raw;
    float corrected_resistance;
    float discriminant;

    denominator = 983.1485f - 56.5958f * voltage;
    if (fabsf(denominator) < 0.000001f)
    {
        if (resistance != NULL)
        {
            *resistance = 0.0f;
        }
        return 0.0f;
    }

    resistance_raw =(112.4083f * voltage + 897.449f) / denominator;//电阻阻值
    corrected_resistance = 1.00563f * resistance_raw;//修正后的阻值

    if (resistance != NULL)
    {
        *resistance = corrected_resistance;
    }

    discriminant = coefficient_a * coefficient_a - 4.0f * coefficient_b * (1.0f - corrected_resistance);
    if (discriminant < 0.0f)
    {
        return 0.0f;
    }

    return (-coefficient_a + sqrtf(discriminant)) / (2.0f * coefficient_b);//返回温度
}

static void ProcessTemperatureChannel(AverageFilter *filter,uint16_t adc_value,float *voltage,float *resistance,float *temperature)
{
    float filtered_voltage = Filter_Update(filter, adc_value);

    if (voltage != NULL)
    {
        *voltage = filtered_voltage;
    }
    if (temperature != NULL)
    {
        *temperature = VoltageToTemperature(filtered_voltage, resistance);
    }
}
//connect_threshold CC插上时的电压
//disconnect_threshold CC拔下时的电压
static void ProcessCCChannel(AverageFilter *filter,uint16_t adc_value,float calibration_gain,float connect_threshold,float disconnect_threshold,uint8_t mask,float *output_voltage)
{
    float voltage = CC_GAIN * calibration_gain * Filter_Update(filter, adc_value);

    if (output_voltage != NULL)
    {
        *output_voltage = voltage;
    }

    if (voltage <= connect_threshold)
    {
        CC_MASK |= mask;
    }
    else if (voltage > disconnect_threshold)
    {
        CC_MASK &= (uint8_t)~mask;
    }
}

void Temp_ADC1(const uint16_t *adc_data)
{
    if (adc_data == NULL)
    {
        return;
    }

    Wire_T1_value = adc_data[0];
    Wire_T2_value = adc_data[1];
    Seat3_T1_value = adc_data[2];
    Seat2_CC2_value = adc_data[3];

    ProcessTemperatureChannel(&filter_wire_t1, Wire_T1_value,&Wire_T1_V, &Wire_T1_R, &Wire_T1);
    ProcessTemperatureChannel(&filter_wire_t2, Wire_T2_value,&Wire_T2_V, &Wire_T2_R, &Wire_T2);
    ProcessTemperatureChannel(&filter_seat3_t1, Seat3_T1_value,&Seat3_T1_V, &Seat3_T1_R, &Seat3_T1);
    ProcessCCChannel(&filter_seat2_cc2, Seat2_CC2_value,WIRE_CC1_CAL_GAIN, 6.0f, 6.0f, CC3_MASK, &Seat2_CC2_V);
}

void Temp_ADC2(const uint16_t *adc_data)
{
    if (adc_data == NULL)
    {
        return;
    }

    Seat1_T1_value = adc_data[0];
    Seat1_T2_value = adc_data[1];
    Seat2_T1_value = adc_data[2];
    Seat2_T2_value = adc_data[3];
		Seat3_T2_value = adc_data[4];
    Wire_CC1_value = adc_data[5];
    Seat1_CC2_value = adc_data[6];
    

    ProcessTemperatureChannel(&filter_seat1_t1, Seat1_T1_value,&Seat1_T1_V, &Seat1_T1_R, &Seat1_T1);
    ProcessTemperatureChannel(&filter_seat1_t2, Seat1_T2_value,&Seat1_T2_V, &Seat1_T2_R, &Seat1_T2);
    ProcessTemperatureChannel(&filter_seat2_t1, Seat2_T1_value,&Seat2_T1_V, &Seat2_T1_R, &Seat2_T1);
    ProcessTemperatureChannel(&filter_seat2_t2, Seat2_T2_value,&Seat2_T2_V, &Seat2_T2_R, &Seat2_T2);
    ProcessTemperatureChannel(&filter_seat3_t2, Seat3_T2_value,&Seat3_T2_V, &Seat3_T2_R, &Seat3_T2);
    ProcessCCChannel(&filter_wire_cc1, Wire_CC1_value,WIRE_CC1_CAL_GAIN, 4.0f, 6.0f, CC1_MASK, &Wire_CC1_V);
    ProcessCCChannel(&filter_seat1_cc2, Seat1_CC2_value,WIRE_CC1_CAL_GAIN, 6.0f, 6.0f, CC2_MASK, &Seat1_CC2_V);
}

void Temp_ADC3(const uint16_t *adc_data)
{
    if (adc_data == NULL)
    {
        return;
    }

    Seat4_T1_value = adc_data[0];
    Seat4_T2_value = adc_data[1];
    Seat3_CC2_value = adc_data[2];
    Seat4_CC2_value = adc_data[3];

    ProcessTemperatureChannel(&filter_seat4_t1, Seat4_T1_value,&Seat4_T1_V, &Seat4_T1_R, &Seat4_T1);
    ProcessTemperatureChannel(&filter_seat4_t2, Seat4_T2_value,&Seat4_T2_V, &Seat4_T2_R, &Seat4_T2);
    ProcessCCChannel(&filter_seat3_cc2, Seat3_CC2_value,WIRE_CC1_CAL_GAIN, 6.0f, 6.0f, CC4_MASK, &Seat3_CC2_V);
    ProcessCCChannel(&filter_seat4_cc2, Seat4_CC2_value,WIRE_CC1_CAL_GAIN, 6.0f, 6.0f, CC5_MASK, &Seat4_CC2_V);
}

void ADC_SnapshotProcess(void)
{
    const uint16_t *snapshot;

    snapshot = ADC1_SnapshotAcquire();
    if (snapshot != NULL)
    {
        Temp_ADC1(snapshot);
        ADC1_SnapshotRelease(snapshot);
    }

    snapshot = ADC2_SnapshotAcquire();
    if (snapshot != NULL)
    {
        Temp_ADC2(snapshot);
        ADC2_SnapshotRelease(snapshot);
    }

    snapshot = ADC3_SnapshotAcquire();
    if (snapshot != NULL)
    {
        Temp_ADC3(snapshot);
        ADC3_SnapshotRelease(snapshot);
    }
}
