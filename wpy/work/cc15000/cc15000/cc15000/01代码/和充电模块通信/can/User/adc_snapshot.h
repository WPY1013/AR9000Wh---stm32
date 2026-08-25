#ifndef __ADC_SNAPSHOT_H__
#define __ADC_SNAPSHOT_H__

#include "main.h"
#include <stdint.h>

#define ADC1_CHANNEL_COUNT  4U
#define ADC2_CHANNEL_COUNT  7U
#define ADC3_CHANNEL_COUNT  4U

typedef enum
{
    ADC_SNAPSHOT_FREE = 0,
    ADC_SNAPSHOT_WRITING,
    ADC_SNAPSHOT_READY,
    ADC_SNAPSHOT_READING
} ADC_SnapshotState;

/* DMA 完成回调调用 */
void ADC1_SnapshotPublish(const volatile uint16_t *dma_buffer);
void ADC2_SnapshotPublish(const volatile uint16_t *dma_buffer);
void ADC3_SnapshotPublish(const volatile uint16_t *dma_buffer);

/* 主循环获取快照 */
const uint16_t *ADC1_SnapshotAcquire(void);
const uint16_t *ADC2_SnapshotAcquire(void);
const uint16_t *ADC3_SnapshotAcquire(void);

/* 主循环处理完成后释放 */
void ADC1_SnapshotRelease(const uint16_t *snapshot);
void ADC2_SnapshotRelease(const uint16_t *snapshot);
void ADC3_SnapshotRelease(const uint16_t *snapshot);

/* 两个缓冲都忙时的丢帧计数 */
extern volatile uint32_t adc1_snapshot_drop_count;
extern volatile uint32_t adc2_snapshot_drop_count;
extern volatile uint32_t adc3_snapshot_drop_count;

#endif
