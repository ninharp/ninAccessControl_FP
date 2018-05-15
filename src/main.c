/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

/* Private typedef -----------------------------------------------------------*//* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADCConvertedValue;
uint32_t SystemCoreClock;
ErrorStatus HSEStartUpStatus;
/* Private function prototypes -----------------------------------------------*/
void IO_Init(void);
void ADC1_Init(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
void Delay(vu32 nCount);
uint8_t getFingerprintEnroll(uint8_t id);
void Voice_Effect(void);

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = Fingerprint_GetImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = Fingerprint_Image2Tz(1);
  if (p != FINGERPRINT_OK)  return -2;

  p = Fingerprint_FingerFastSearch();
  if (p != FINGERPRINT_OK)  return -2;

  return fingerID;
}

int main(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	SystemInit();
	/* System Clock init */
	RCC_Configuration();

	/* NVIC Configuration init */
	NVIC_Configuration();

	/* GPIO init */
	IO_Init();

	/* Delay init */
	DelayInit();

	/* USART init @ 115200 baud 8n1 */
	USART2_Init(115200);

	//Delay(500000);
	//DelayMs(100);
	printf("run\r\nSelf Test Started\r\n");

	/* Fingerprint sensor init */
	printf("Fingerprint Sensor... ");
	Fingerprint_Init(0x00000000);
	DelayMs(100);
	if (Fingerprint_CheckPassword() == FINGERPRINT_OK) {
		printf("OK!\r\n");
		/*
		printf("Emptying Fingerprint Database\r\n");
		uint8_t p = Fingerprint_EmptyDatabase();
		if (p == FINGERPRINT_DBCLEARFAIL) {
			printf("Failed to clear the Database\r\n");
		} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
			printf("Communication error\r\n");
		} else if (p == FINGERPRINT_OK) {
			printf("Empty success\r\n");
		}
		Fingerprint_GetTemplateCount();
		printf("%d Fingerprints stored!\r\n", templateCount);
		*/

		/*
		uint8_t m = 1;
		printf("Deleting Model %d\r\n", m);
		uint8_t p = Fingerprint_DeleteModel(m);
		if (p == FINGERPRINT_BADLOCATION) {
			printf("Bad Location %d\r\n", m);
		} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
			printf("Communication error\r\n");
		} else if (p == FINGERPRINT_DELETEFAIL) {
			printf("Failed to delete Model %d\r\n", m);
		} else if (p == FINGERPRINT_OK) {
			printf("Deleted!\r\n");
		}
		*/

		printf("Reading Fingerprint System Parameter\r\n");
		if (Fingerprint_ReadSystemParameter() == FINGERPRINT_OK) {
			printf("System Parameters read successfully\r\n");
			printf("Status Register: %02x (", statusRegister);
			for (uint16_t z = 32768; z > 0; z >>= 1) {
				printf("%s", (statusRegister & z) ? "1" : "0");
			}
			printf(")\r\n");
			printf("System Identifier: %02x\r\n", sysIdentifier);
			printf("Library Size: %d\r\n", librarySize);
			printf("Security Level: %d\r\n", securityLevel);
			printf("Device Address: %04x\r\n", devAddress);
			printf("Data Packet Size: %d\r\n", packetSize);
			printf("Baud Settings: %d (%ld kbps)\r\n", baudSettings, (uint16_t) baudSettings * 9600);
		} else {
			printf("Failed to read System Parameter\r\n");
		}

		Fingerprint_GetTemplateCount();
		printf("%3d/%d Fingerprints stored!\r\n", templateCount, librarySize);
	} else {
		printf("failed!\r\n");
		printf("System halted!\r\n");
		while(1) {
			GPIO_SetBits(GPIOA, GPIO_Pin_5);
			DelayMs(500);
			GPIO_ResetBits(GPIOA, GPIO_Pin_5);
			DelayMs(500);
		}
	}

	/* ADC1 init */
	ADC1_Init();

	/* Data Watch init */
	//DWT_Init();

	/* Init WTV020SD MP3 Voice Module */
	WTV020SD_Init();
	//Delay(5000000);
	//DelayMs(300);
	//WTV020SD_AsyncPlayVoice(VOICE_ACCESS_DENIED);

	/* Display ST7735 init */
	ST7735_Init();
	ST7735_AddrSet(0,0,127,127);
	ST7735_Clear(COLOR565_BLACK);
	//ST7735_Orientation(scr_CCW);

	/*uint16_t c = 0;
	for (uint8_t y = 0; y < 100; y++) {
		for (uint8_t x = 0; x < 100; x++) {
			if (Fingerprint_bits[c] > 0)
				ST7735_Pixel(x+20, y+20, ST7735_WHITE);
			else
				ST7735_Pixel(x+20, y+20, ST7735_BLACK);
			c++;
		}
	}*/
	ST7735_drawXBitmap(15,15,Fingerprint_bits, Fingerprint_width, Fingerprint_height, 0xFFFF);

	//WTV020SD_PlayVoice(15);
	DelayMs(500);

	/*
	WTV020SD_AsyncPlayVoice(VOICE_FP_SCAN);
	while(WTV020SD_IsPlaying()) { Voice_Effect(); }

	DelayMs(3000);
	WTV020SD_AsyncPlayVoice(VOICE_SCAN);
	//while(WTV020SD_IsPlaying()) { Voice_Effect(); }
	DelayMs(2000);

	WTV020SD_AsyncPlayVoice(VOICE_WRONG_BEEP);
	DelayMs(1500);
	WTV020SD_AsyncPlayVoice(VOICE_ACCESS_DENIED);
	while(WTV020SD_IsPlaying()) { Voice_Effect(); }

	ST7735_drawXBitmap(15,15,Fingerprint_bits, 100, 100, 0x001F);
	*/

	/*
	getFingerprintEnroll(9);
	DelayMs(1000);
	getFingerprintEnroll(10);
	DelayMs(1000);
	getFingerprintEnroll(11);
	DelayMs(1000);
	getFingerprintEnroll(12);
	*/

	/* Main Loop */
	while (1)
	{
		int id = getFingerprintIDez();
		if (id > -1) {
			// found a match!
			WTV020SD_AsyncPlayVoice(VOICE_OK_BEEP);
			printf("Found ID #%d\r\n", fingerID);
			printf(" with confidence of %d\r\n", confidence);
			ST7735_drawXBitmap(15,15,Fingerprint_bits, Fingerprint_width, Fingerprint_height, ST7735_GREEN);
			ST7735_PutStr7x11(35, 54," ACCESS", ST7735_WHITE, ST7735_BLACK);
			ST7735_PutStr7x11(35, 67,"GRANTED", ST7735_WHITE, ST7735_BLACK);
			//DelayMs(1000);
			WTV020SD_AsyncPlayVoice(VOICE_ACCESS_GRANTED);
			GPIO_ResetBits(GPIOA, GPIO_Pin_6);
			DelayMs(5000);
			GPIO_SetBits(GPIOA, GPIO_Pin_6);
			DelayMs(250);
			ST7735_Clear(COLOR565_BLACK);
			ST7735_drawXBitmap(15,15,Fingerprint_bits, Fingerprint_width, Fingerprint_height, ST7735_WHITE);
		} else if (id == -2) {
			// finger but no match
			WTV020SD_AsyncPlayVoice(VOICE_WRONG_BEEP);
			printf("no Match\r\n");
			//DelayMs(500);
			ST7735_drawXBitmap(15,15,Fingerprint_bits, Fingerprint_width, Fingerprint_height, ST7735_BLUE);
			ST7735_PutStr7x11(38, 54,"ACCESS", ST7735_WHITE, ST7735_BLUE);
			ST7735_PutStr7x11(38, 67,"DENIED", ST7735_WHITE, ST7735_BLUE);
			WTV020SD_PlayVoice(VOICE_ACCESS_DENIED);
			DelayMs(2000);
			ST7735_Clear(COLOR565_BLACK);
			ST7735_drawXBitmap(15,15,Fingerprint_bits, Fingerprint_width, Fingerprint_height, ST7735_WHITE);
		}
		DelayMs(250);
		//DelayMs(3000);
		//if (!WTV020SD_IsPlaying()) i++;
		//if (i > 25) i = 9;

		/*char buffer[20];
		__itoa(ADCConvertedValue, buffer, 10);
		USART1_Puts(buffer);
		USART1_Puts("\r\n");*/

		//DelayMs(200);

		/*if (USART1_Available()) {
			switch(USART1_Get()) {
			case '1': WTV020SD_AsyncPlayVoice(1); break;
			}
		}*/
		/*
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		//DWT_Delay_ms(500);
		//Delay(500000);
		DelayMs(500);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		//Delay(500000);
		//DWT_Delay_ms(500);
		DelayMs(500);
		*/
	}
}

uint8_t getFingerprintEnroll(uint8_t id)
{
	printf("Waiting for fingerprint #%d... \r\n", id);
	int p = -1;
	while (p != FINGERPRINT_OK) {
		p = Fingerprint_GetImage();
		switch (p) {
			case FINGERPRINT_OK:
			  printf("Image taken\r\n");
			  break;
			case FINGERPRINT_NOFINGER:
				printf(".");
			  break;
			case FINGERPRINT_PACKETRECIEVEERR:
				printf("Communication error\r\n");
			  break;
			case FINGERPRINT_IMAGEFAIL:
				printf("Imaging error\r\n");
			  break;
			default:
				printf("Unknown error\r\n");
			  break;
		}
	}

	p = Fingerprint_Image2Tz(1);
	switch (p) {
		case FINGERPRINT_OK:
			printf("Image converted\r\n");
		  break;
		case FINGERPRINT_IMAGEMESS:
			printf("Image too messy\r\n");
		  return p;
		case FINGERPRINT_PACKETRECIEVEERR:
			printf("Communication error\r\n");
		  return p;
		case FINGERPRINT_FEATUREFAIL:
			printf("Could not find fingerprint features\r\n");
		  return p;
		case FINGERPRINT_INVALIDIMAGE:
			printf("Could not find fingerprint features\r\n");
		  return p;
		default:
			printf("Unknown error\r\n");
		  return p;
	}

	printf("Remove finger\r\n");
	DelayMs(2000);
	p = 0;
	while (p != FINGERPRINT_NOFINGER) {
		p = Fingerprint_GetImage();
	}
	printf("ID %d\r\n", id);
	p = -1;
	printf("Place same finger again\r\n");
	while (p != FINGERPRINT_OK) {
		p = Fingerprint_GetImage();
		switch (p) {
			case FINGERPRINT_OK:
				printf("Image taken\r\n");
			  break;
			case FINGERPRINT_NOFINGER:
				printf(".");
			  break;
			case FINGERPRINT_PACKETRECIEVEERR:
				printf("Communication error\r\n");
			  break;
			case FINGERPRINT_IMAGEFAIL:
				printf("Imaging error\r\n");
			  break;
			default:
				printf("Unknown error\r\n");
			  break;
		}
	}

	p = Fingerprint_Image2Tz(2);
	switch (p) {
		case FINGERPRINT_OK:
			printf("Image converted\r\n");
			break;
		case FINGERPRINT_IMAGEMESS:
			printf("Image too messy\r\n");
			return p;
		case FINGERPRINT_PACKETRECIEVEERR:
			printf("Communication error\r\n");
			return p;
		case FINGERPRINT_FEATUREFAIL:
			printf("Could not find fingerprint features\r\n");
			return p;
		case FINGERPRINT_INVALIDIMAGE:
			printf("Could not find fingerprint features\r\n");
			return p;
		default:
			printf("Unknown error\r\n");
			return p;
	}

	printf("Creating model for #%d\r\n", id);

	p = Fingerprint_CreateModel();
	if (p == FINGERPRINT_OK) {
		printf("Prints matched!\r\n");
	} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
		printf("Communication error\r\n");
		return p;
	} else if (p == FINGERPRINT_ENROLLMISMATCH) {
		printf("Fingerprints did not match\r\n");
		return p;
	} else {
		printf("Unknown error\r\n");
		return p;
	}

	printf("ID %d\r\n", id);
	p = Fingerprint_StoreModel(id);
	if (p == FINGERPRINT_OK) {
		printf("Stored!\r\n");
		return p;
	} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
		printf("Communication error\r\n");
		return p;
	} else if (p == FINGERPRINT_BADLOCATION) {
		printf("Could not store in that location\r\n");
		return p;
	} else if (p == FINGERPRINT_FLASHERR) {
		printf("Error writing to flash\r\n");
		return p;
	} else {
		printf("Unknown error\r\n");
		return p;
	}
}

/* Quickndirty experimental voice effect, could be better */
void Voice_Effect(void)
{
	uint8_t xpos = 0;
	uint8_t ypos = 63;
	for (xpos = 0; xpos < 128; xpos++) {

		/*
		if (xpos > 0) ST7735_Pixel(xpos-1, ypos, ST7735_BLUE);
		ST7735_Pixel(xpos, ypos, ST7735_WHITE);
		if (xpos < 127) ST7735_Pixel(xpos-1, ypos, ST7735_BLUE);
		*/
		ST7735_Line(xpos, ypos, xpos, ypos+(ADCConvertedValue-2100), ST7735_WHITE);
		//ypos += (ADCConvertedValue-2100);
	}
	ST7735_Clear(COLOR565_BLACK);
}

void IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Init LED on STM32F103C8B6 Board */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_6);
}

void ADC1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

		  /* Configure PC.04 (ADC Channel14) as analog input -------------------------*/
		  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		  GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* Enable peripheral clocks ------------------------------------------------*/
		  /* Enable DMA1 clock */
		  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

		  /* Enable ADC1 and GPIOC clock */
		  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);

		ADC_InitTypeDef ADC_InitStructure;
		DMA_InitTypeDef DMA_InitStructure;
		/* DMA1 channel1 configuration ----------------------------------------------*/
		  DMA_DeInit(DMA1_Channel1);
		  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
		  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
		  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		  DMA_InitStructure.DMA_BufferSize = 1;
		  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
		  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
		  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

		  /* Enable DMA1 channel1 */
		  DMA_Cmd(DMA1_Channel1, ENABLE);

		  /* ADC1 configuration ------------------------------------------------------*/
		  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
		  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
		  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		  ADC_InitStructure.ADC_NbrOfChannel = 1;
		  ADC_Init(ADC1, &ADC_InitStructure);

		  /* ADC1 regular channel14 configuration */
		  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_55Cycles5);

		  /* Enable ADC1 DMA */
		  ADC_DMACmd(ADC1, ENABLE);

		  /* Enable ADC1 */
		  ADC_Cmd(ADC1, ENABLE);

		  /* Enable ADC1 reset calibration register */
		  ADC_ResetCalibration(ADC1);
		  /* Check the end of ADC1 reset calibration register */
		  while(ADC_GetResetCalibrationStatus(ADC1));

		  /* Start ADC1 calibration */
		  ADC_StartCalibration(ADC1);
		  /* Check the end of ADC1 calibration */
		  while(ADC_GetCalibrationStatus(ADC1));

		  /* Start ADC1 Software Conversion */
		  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
 * Function Name  : RCC_Configuration
 * Description    : Configures the different system clocks.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RCC_Configuration(void)
{
// RCC system reset(for debug purpose)
	RCC_DeInit();

// Enable HSE
	RCC_HSEConfig(RCC_HSE_ON);

// Wait till HSE is ready
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		// HCLK = SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		// PCLK2 = HCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);

		// PCLK1 = HCLK/2
		RCC_PCLK1Config(RCC_HCLK_Div2);

		// PLLCLK = 8MHz * 9 = 72 MHz
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

		// Enable PLL
		RCC_PLLCmd(ENABLE);

		// Wait till PLL is ready
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}

		// Select PLL as system clock source
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		// Wait till PLL is used as system clock source
		while (RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}
	/* Output HSE clock on MCO pin ---------------------------------------------*/
	/*
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	 RCC_MCOConfig(RCC_MCO_HSE);
	 */
}

/*******************************************************************************
 * Function Name  : NVIC_Configuration
 * Description    : Configures Vector Table base location.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM  
	/* Set the Vector Table base location at 0x20000000 */
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
}

/*******************************************************************************
 * Function Name  : Delay
 * Description    : Inserts a delay time.
 * Input          : nCount: specifies the delay time length.
 * Output         : None
 * Return         : None
 *******************************************************************************/
void Delay(vu32 nCount)
{
	for (; nCount != 0; nCount--)
		;
}

#ifdef  DEBUG
/*******************************************************************************
 * Function Name  : assert_failed
 * Description    : Reports the name of the source file and the source line number
 *                  where the assert_param error has occurred.
 * Input          : - file: pointer to the source file name
 *                  - line: assert_param error line source number
 * Output         : None
 * Return         : None
 *******************************************************************************/
void assert_failed(u8* file, u32 line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	printf("Wrong parameters value: file %s on line %d\r\n", (char*) file,
			(int) line);
	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/******************* (C) COPYRIGHT 2014 Michael Sauer *****END OF FILE****/
