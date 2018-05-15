/**
 * @file UART2.h
 *
 * @brief This file includes the UART2 functions
 *	      It handles ML Protocol CRC Check and
 *	      general parsing of the MagicLight Protocol.
 *
 * @author Michael Sauer <sauer.uetersen@gmail.com>
 *
 * @date 14.09.2014 - erste Version
 *
 */

//--------------------------------------------------------------
#ifndef _UART2_H
#define _UART2_H

//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include "stm32f10x.h"
#include "general.h"

/** @addtogroup Libraries
  * @{
  */

/** @addtogroup UART2_Library
  * @{
  */

/** @addtogroup UART2_Exported_Functions_Prototypes
  * @{
  */

void USART2_Init(uint32_t baudrate);
void USART2_Put(uint8_t ch);
uint8_t USART2_Get(void);
void USART2_Puts(volatile char *s);
bool USART2_Available(void);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

//--------------------------------------------------------------
#endif // __UART2_H
