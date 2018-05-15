/*
 * delay.h
 *
 *  Created on: 08.05.2018
 *      Author: michael
 */

#ifndef LIB_DELAY_H_
#define LIB_DELAY_H_

#include "stm32f10x.h"

void DelayInit(void);
void DelayUs(uint32_t us);
void DelayMs(uint32_t ms);


#endif /* LIB_DELAY_H_ */
