#include "adc_snapshot.h"

#define SNAPSHOT_BUFFER_COUNT  2U

static uint16_t adc1_snapshot[SNAPSHOT_BUFFER_COUNT][ADC1_CHANNEL_COUNT];
static uint16_t adc2_snapshot[SNAPSHOT_BUFFER_COUNT][ADC2_CHANNEL_COUNT];
static uint16_t adc3_snapshot[SNAPSHOT_BUFFER_COUNT][ADC3_CHANNEL_COUNT];

static volatile ADC_SnapshotState adc1_state[SNAPSHOT_BUFFER_COUNT] =
{
    ADC_SNAPSHOT_FREE,
    ADC_SNAPSHOT_FREE
};

static volatile ADC_SnapshotState adc2_state[SNAPSHOT_BUFFER_COUNT] =
{
    ADC_SNAPSHOT_FREE,
    ADC_SNAPSHOT_FREE
};

static volatile ADC_SnapshotState adc3_state[SNAPSHOT_BUFFER_COUNT] =
{
    ADC_SNAPSHOT_FREE,
    ADC_SNAPSHOT_FREE
};

volatile uint32_t adc1_snapshot_drop_count = 0;
volatile uint32_t adc2_snapshot_drop_count = 0;
volatile uint32_t adc3_snapshot_drop_count = 0;

void ADC1_SnapshotPublish(const volatile uint16_t *dma_buffer)
{
    uint8_t buffer_index;
    uint8_t channel_index;

    for (buffer_index = 0; buffer_index < SNAPSHOT_BUFFER_COUNT; buffer_index++)
    {
        if (adc1_state[buffer_index] == ADC_SNAPSHOT_FREE)
        {
            adc1_state[buffer_index] = ADC_SNAPSHOT_WRITING;

            for (channel_index = 0;
                 channel_index < ADC1_CHANNEL_COUNT;
                 channel_index++)
            {
                adc1_snapshot[buffer_index][channel_index] =
                    dma_buffer[channel_index];
            }

            adc1_state[buffer_index] = ADC_SNAPSHOT_READY;
            return;
        }
    }

    adc1_snapshot_drop_count++;
}

void ADC2_SnapshotPublish(const volatile uint16_t *dma_buffer)
{
    uint8_t buffer_index;
    uint8_t channel_index;

    for (buffer_index = 0; buffer_index < SNAPSHOT_BUFFER_COUNT; buffer_index++)
    {
        if (adc2_state[buffer_index] == ADC_SNAPSHOT_FREE)
        {
            adc2_state[buffer_index] = ADC_SNAPSHOT_WRITING;

            for (channel_index = 0;
                 channel_index < ADC2_CHANNEL_COUNT;
                 channel_index++)
            {
                adc2_snapshot[buffer_index][channel_index] =
                    dma_buffer[channel_index];
            }

            adc2_state[buffer_index] = ADC_SNAPSHOT_READY;
            return;
        }
    }

    adc2_snapshot_drop_count++;
}

void ADC3_SnapshotPublish(const volatile uint16_t *dma_buffer)
{
    uint8_t buffer_index;
    uint8_t channel_index;

    for (buffer_index = 0; buffer_index < SNAPSHOT_BUFFER_COUNT; buffer_index++)
    {
        if (adc3_state[buffer_index] == ADC_SNAPSHOT_FREE)
        {
            adc3_state[buffer_index] = ADC_SNAPSHOT_WRITING;

            for (channel_index = 0;
                 channel_index < ADC3_CHANNEL_COUNT;
                 channel_index++)
            {
                adc3_snapshot[buffer_index][channel_index] =
                    dma_buffer[channel_index];
            }

            adc3_state[buffer_index] = ADC_SNAPSHOT_READY;
            return;
        }
    }

    adc3_snapshot_drop_count++;
}

const uint16_t *ADC1_SnapshotAcquire(void)
{
    uint8_t i;

    for (i = 0; i < SNAPSHOT_BUFFER_COUNT; i++)
    {
        if (adc1_state[i] == ADC_SNAPSHOT_READY)
        {
            adc1_state[i] = ADC_SNAPSHOT_READING;
            return adc1_snapshot[i];
        }
    }

    return NULL;
}

const uint16_t *ADC2_SnapshotAcquire(void)
{
    uint8_t i;

    for (i = 0; i < SNAPSHOT_BUFFER_COUNT; i++)
    {
        if (adc2_state[i] == ADC_SNAPSHOT_READY)
        {
            adc2_state[i] = ADC_SNAPSHOT_READING;
            return adc2_snapshot[i];
        }
    }

    return NULL;
}

const uint16_t *ADC3_SnapshotAcquire(void)
{
    uint8_t i;

    for (i = 0; i < SNAPSHOT_BUFFER_COUNT; i++)
    {
        if (adc3_state[i] == ADC_SNAPSHOT_READY)
        {
            adc3_state[i] = ADC_SNAPSHOT_READING;
            return adc3_snapshot[i];
        }
    }

    return NULL;
}

void ADC1_SnapshotRelease(const uint16_t *snapshot)
{
    uint8_t i;

    for (i = 0; i < SNAPSHOT_BUFFER_COUNT; i++)
    {
        if (snapshot == adc1_snapshot[i])
        {
            adc1_state[i] = ADC_SNAPSHOT_FREE;
            return;
        }
    }
}

void ADC2_SnapshotRelease(const uint16_t *snapshot)
{
    uint8_t i;

    for (i = 0; i < SNAPSHOT_BUFFER_COUNT; i++)
    {
        if (snapshot == adc2_snapshot[i])
        {
            adc2_state[i] = ADC_SNAPSHOT_FREE;
            return;
        }
    }
}

void ADC3_SnapshotRelease(const uint16_t *snapshot)
{
    uint8_t i;

    for (i = 0; i < SNAPSHOT_BUFFER_COUNT; i++)
    {
        if (snapshot == adc3_snapshot[i])
        {
            adc3_state[i] = ADC_SNAPSHOT_FREE;
            return;
        }
    }
}
