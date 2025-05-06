/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __AUDIO_H__
#define __AUDIO_H__

typedef struct 
{
    void *user_data;
}audio_handle_t;

typedef void (*audio_callback_t)(void *buffer, unsigned int size);

void audio_handle_init(audio_handle_t *audio_handle, unsigned int dac_id, unsigned int channel);
void audio_open(audio_handle_t *audio_handle);
void audio_play_start(audio_handle_t *audio_handle, audio_callback_t callback);
void audio_play_stop(audio_handle_t *audio_handle);

#endif
