/**
 * @file uart2.c
 *
 * @brief This file includes the uart2 functions
 *	      It handles ML Protocol CRC Check and
 *	      general parsing of the MagicLight Protocol.
 *
 * @author Michael Sauer <sauer.uetersen@gmail.com>
 *
 * @date 14.09.2014 - erste Version
 *
 * @note Packet Format:
 * STX | LEN | ADR | DATA | BCC | 0x0D (\\r) | 0x0A (\\n)<br>
 * <br>
 * Adress Byte wird wie folgt zusammengesetzt:<br><pre>
 * Bit 7  6  5  4  3  2  1  0
 *                 |--|--|--|--- Typ des Telegramms (1=General / 2=Effektliste / 3= Einstellungen / ...) (one nibble)
 *              |.-------------- Fehler (Bit 4)
 *           |------------------ End condition (Bit 5)
 *        |--------------------- Start Condition (Bit 6)
 *     |------------------------ Antwort auf Anfrage oder Broadcast (1= Antwort / 0= Broadcast) (Bit 7)
 *</pre>
 */

#include "uart2.h"

/** @addtogroup Libraries
 * @{
 */

/** @addtogroup uart2_Library
 * @{
 */

/** @addtogroup uart2_Exported_Variables
 * @{
 */

/**
 * @}
 */

/** @addtogroup uart2_Exported_Functions
 * @{
 */

/**
 * @brief  Initializes USART2
 *
 * @param  baudrate	Baudrate to use
 * @retval None
 */
void USART2_Init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Clock configuration -------------------------------------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0E;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0E;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

	/* Configure the GPIO ports( USART2 Transmit and Receive Lines) */
	/* Configure the USART2_Tx as Alternate function Push-Pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure the USART2_Rx as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART2 configuration ------------------------------------------------------*/
	/* USART2 configured as follow:
	 - BaudRate = like parameter (57600kbps)
	 - Word Length = 9 Bits
	 - One Stop Bit
	 - Even parity
	 - Hardware flow control disabled (RTS and CTS signals)
	 - Receive and transmit enabled
	 */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_DeInit(USART2);

	/* Configure the USART2 */
	USART_Init(USART2, &USART_InitStructure);

	/* Enable USART2 interrupt */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART2, USART_IT_TXE,ENABLE);

	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE);
}

/**
 * @brief  Puts a byte to USART2
 *
 * @param  ch		Byte to send
 * @retval None
 */
void USART2_Put(uint8_t ch)
{
	USART_SendData(USART2, (uint8_t) ch);
	//Loop until the end of transmission
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
	{
	}
}

/**
 * @brief  Gets one byte from USART2
 *
 * @retval None
 */
uint8_t USART2_Get(void)
{
	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
		;
	return (uint8_t) USART_ReceiveData(USART2);
}

bool USART2_Available(void)
{
	if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
		return true;
	return false;
}

/**
 * @brief  Put string to USART2
 *
 * @param  s		String to send
 * @retval None
 */
void USART2_Puts(volatile char *s)
{

	while (*s)
	{
		// wait until data register is empty
		while (!(USART2->SR & 0x00000040))
			;
		USART_SendData(USART2, *s++);
		//*s++;
	}
}
//
/**
 * @brief  This is the interrupt request handler (IRQ) for ALL USART2 interrupts
 *
 * @retval None
 */
void USART2_IRQHandler(void)
{

// check if the USART2 receive interrupt flag was set
	if (USART_GetITStatus(USART2, USART_IT_RXNE))
	{
//static uint8_t cnt = 0; // this counter is used to determine the string length
		char t = USART2->DR; // the character from the USART2 data register is saved in t

		/* check if the received character is not the LF character (used to determine end of string)
		 * or the if the maximum string length has been been reached
		 */
		/*
		 if( (t != '\n') && (cnt < MAX_STRLEN) ){
		 received_string[cnt] = t;
		 cnt++;
		 }
		 else{ // otherwise reset the character counter and print the received string
		 cnt = 0;
		 USART_puts(received_string);
		 }
		 */

//if (g_uart.NewCommand == 1 && g_uart.CommandCount == 0) {
//UB_USB_CDC_SendString((char *)g_uart.Command,NONE);
//	USART_puts((char *)g_uart.Command);
//}
	}
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
