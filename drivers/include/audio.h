/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __AUDIO_H__
#define __AUDIO_H__

typedef void (*audio_callback_t)(void *buffer, unsigned int size);

void audio_init(unsigned int dac_id, unsigned int channel);
void audio_play_start(audio_callback_t callback);
void audio_play_stop(void);

#endif
