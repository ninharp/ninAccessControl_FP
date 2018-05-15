/**
 * @file dwt.c
 *
 * @brief This file includes the DWT functions
 *		  The DataWatch counts how much CPU cycles are gone
 *		  so we can use it for a simple delay
 *
 * @author Michael Sauer <sauer.uetersen@gmail.com>
 *
 * @date 14.09.2014 - First Version
 */
#include "dwt.h"

/** @addtogroup Libraries
 * @{
 */

/** @addtogroup DWT_Library
 * @{
 */

/** @addtogroup DWT_Exported_Functions
 * @{
 */

/**
 * @brief  Initializes the DWT
 *
 * @retval None
 */
void DWT_Init(void)
{
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
	{
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
}

/**
 * @brief  Returns the current ticks
 *
 * @retval Current CPU Ticks
 */
uint32_t DWT_Get(void)
{
	return DWT->CYCCNT;
}

/**
 * @brief  Compares parameter with current ticks
 *
 *	@param	tp	Ticks to compare
 * @retval true if comparison ok
 */
uint8_t DWT_Compare(int32_t tp)
{
	return (((int32_t) DWT_Get() - tp) < 0);
}

/**
 * @brief  Delays n microseconds
 *	@param	us	Microseconds to delay
 * @retval None
 */
void DWT_Delay(uint32_t us) // microseconds
{
	// 1 Tick = 13.88 ns
	// 1 Tick = 0.01388 us
	// 1 Tick = 0.00001388 ms
	int32_t tp = DWT_Get() + (us / (1 / (SystemCoreClock / 1000000)));
	while (DWT_Compare(tp))
		;
}

/**
 * @brief  Delays n milliseconds
 *
 *	@param	ms	Milliseconds to delay
 * @retval None
 */
void DWT_Delay_ms(uint16_t ms) // milliseconds
{
	DWT_Delay(ms * 1000);
}

/**
 * @brief  Returns the actual count of milliseconds
 *
 * @retval Actual count of milliseconds
 */
uint32_t millis()
{
	return DWT_Get() / (SystemCoreClock / 1000);
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */
