/**
 * @file main.h
 *
 * @brief This file includes the main variables, structs, ...
 *
 * @author Michael Sauer <sauer.uetersen@gmail.com>
 *
 * @date 30.08.2014 - First Version (Clone of ml_mainunit.h)
 **/

//--------------------------------------------------------------
#ifndef _MAIN_H
#define _MAIN_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "uart2.h"
#include "dwt.h"
#include "general.h"
#include "wtv020sd.h"
#include "st7735.h"
#include "bitmaps.h"
#include "fingerprint.h"

//--------------------------------------------------------------
// Variables
#define VOICE_ACCESS_GRANTED	13
#define VOICE_ACCESS_DENIED		12
#define VOICE_FP_SCAN			14
#define VOICE_SCAN				16
#define VOICE_OK_BEEP			18
#define VOICE_WRONG_BEEP		24

//--------------------------------------------------------------

void Delay(vu32 nCount);
//--------------------------------------------------------------
#endif // __MAIN_H
