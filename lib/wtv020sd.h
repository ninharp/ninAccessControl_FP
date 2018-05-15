/*
 * wtv020sd.h
 *
 *  Created on: 08.05.2018
 *      Author: Michael Sauer
 *      Based on Arduino Library from Diego J. Arevalo
 */

#ifndef LIB_WTV020SD_H_
#define LIB_WTV020SD_H_

#include <stdint.h>
#include <stdbool.h>
#include <stm32f10x.h>

void WTV020SD_Init(void);
void WTV020SD_Reset(void);
void WTV020SD_AsyncPlayVoice(unsigned int voiceNumber);
void WTV020SD_PlayVoice(unsigned int voiceNumber);
bool WTV020SD_IsPlaying(void);
void WTV020SD_Unmute(void);
void WTV020SD_Mute(void);
void WTV020SD_PauseVoice(void);
void WTV020SD_StopVoice(void);

#endif /* LIB_WTV020SD_H_ */
