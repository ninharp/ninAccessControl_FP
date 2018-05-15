/*
 * wtv020sd.c
 *
 *  Created on: 08.05.2018
 *      Author: Michael Sauer
 *      Based on Arduino Library from Diego J. Arevalo
 */

#include "wtv020sd.h"
#include "delay.h"

#define _clkH() ({ GPIO_SetBits(GPIOC, GPIO_Pin_9); })
#define _clkL() ({ GPIO_ResetBits(GPIOC, GPIO_Pin_9); })
#define _rstH() ({ GPIO_SetBits(GPIOC, GPIO_Pin_6); })
#define _rstL() ({ GPIO_ResetBits(GPIOC, GPIO_Pin_6); })
#define _dataH() ({ GPIO_SetBits(GPIOC, GPIO_Pin_8); })
#define _dataL() ({ GPIO_ResetBits(GPIOC, GPIO_Pin_8); })
#define _delay(c) ({ for (uint32_t i = c; i != 0; i--); })

const unsigned int PLAY_PAUSE = 0xFFFE;
const unsigned int STOP = 0xFFFF;
const unsigned int VOLUME_MIN = 0xFFF0;
const unsigned int VOLUME_MAX = 0xFFF7;

void WTV020SD_sendCommand(unsigned int command);

void WTV020SD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	// Busy Pin Input C5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// Reset Pin Output C6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// DATA Pin Output C8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// CLK Pin Output C9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	WTV020SD_Reset();
}

void WTV020SD_Reset(void)
{
	_clkL();
	_rstH();
	// Reset
	_rstL();
	DelayMs(5);
	_rstH();
	_clkH();
	DelayMs(300);
}

void WTV020SD_AsyncPlayVoice(unsigned int voiceNumber)
{
	WTV020SD_sendCommand(voiceNumber);
}

void WTV020SD_PlayVoice(unsigned int voiceNumber)
{
	while(WTV020SD_IsPlaying());
	WTV020SD_sendCommand(voiceNumber);
}

bool WTV020SD_IsPlaying(void)
{
	if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)) return true;
	return false;
}

void WTV020SD_StopVoice()
{
	WTV020SD_sendCommand(STOP);
}

void WTV020SD_PauseVoice()
{
	WTV020SD_sendCommand(PLAY_PAUSE);
}

void WTV020SD_Mute()
{
	WTV020SD_sendCommand(VOLUME_MIN);
}

void WTV020SD_Unmute()
{
	WTV020SD_sendCommand(VOLUME_MAX);
}

void WTV020SD_sendCommand(unsigned int command)
{
	  //Start bit Low level pulse.
	  _clkL();
	  DelayMs(2);
	  for (unsigned int mask = 0x8000; mask > 0; mask >>= 1) {
	    //Clock low level pulse.
	    _clkL();
	    DelayUs(50);
	    //Write data setup.
	    if (command & mask) {
	      _dataH();
	    }
	    else {
	      _dataL();
	    }
	    //Write data hold.
	    DelayUs(50);
	    //Clock high level pulse.
	    _clkH();
	    DelayUs(100);
	    if (mask>0x0001){
	    	//Stop bit high level pulse.
	    	DelayMs(2);
	    }
	  }
	  //Busy active high from last data bit latch.
	  DelayMs(20);
	}
