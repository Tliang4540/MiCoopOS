/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __ADC_H__
#define __ADC_H__

typedef struct
{
    void *user_data;
}adc_handle_t;

void adc_handle_init(adc_handle_t *adc_handle, unsigned int adc_id);
void adc_open(adc_handle_t *adc_handle);
unsigned int adc_read(adc_handle_t *adc_handle);
void adc_set_channel(adc_handle_t *adc_handle, unsigned int channel);
void adc_close(adc_handle_t *adc_handle);

#endif
