/* Includes */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "usb_bsp.h"
#include "usbh_core.h"
#include "usbh_usr.h"
#include "usbh_adk_core.h"
#include "uart_debug.h"
/* Private macro */
/* Private variables */__IO uint32_t TimingDelay;

/* Private function prototypes */
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);

void TIM4_Config(void);
void TIM4_Init(void);
void TIM3_Config(void);
void TIM3_Init(void);

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
uint16_t PrescalerValue = 0;

/** @defgroup USBH_USR_MAIN_Private_Variables
 * @{
 */
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_Core_dev __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST USB_Host __ALIGN_END;
/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 */
int main(void) {
	int i = 0;
	uint8_t msg[2];
	uint16_t len;
	RCC_ClocksTypeDef RCC_Clocks;

	TIM4_Config();
	TIM4_Init();

	TIM3_Config();
	TIM3_Init();

	TIM4 ->CCR1 = 0;
	TIM4 ->CCR2 = 0;
	TIM4 ->CCR3 = 0;
	TIM4 ->CCR4 = 0;

	TIM3 ->CCR1 = 0;
	TIM3 ->CCR2 = 0;
	TIM3 ->CCR3 = 0;
	TIM3 ->CCR4 = 0;

	//STM_EVAL_LEDInit(LED4);
	//STM_EVAL_LEDOn(LED4);
	/* SysTick end of count event each 1ms */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

#ifdef DEBUG
	/* Init Debug out setting(UART2) */
	uart_debug_init();
#endif

	USBH_Init(&USB_OTG_Core_dev, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_ADK_cb,
			&USR_Callbacks);

	USBH_ADK_Init("Zoltan_Varadi", "rcCar_MC", "rcCar STM32F4 Discovery", "0.1",
			"https://github.com/ZoltanVaradi", "1234568");

	TIM4 ->CCR1 = 20000;
	while (1) {
		USBH_Process(&USB_OTG_Core_dev, &USB_Host);

		if (USBH_ADK_getStatus() == ADK_IDLE) {

			len = USBH_ADK_read(&USB_OTG_Core_dev, msg, sizeof(msg));
			if (len > 0) {
				if (i++ == 100) {
					if (TIM4 ->CCR1 == 0)
						TIM4 ->CCR1 = 10000;
					else
						TIM4 ->CCR1 = 0;

					i = 0;
				}

				if (msg[0] == 0x2) {
					if (msg[1] >= 0 && msg[1] <= 50) {
						TIM4 ->CCR2 = 20000 / 50 * msg[1];
					}
				} else if (msg[0] == 0x3) {
					if (msg[1] >= 0 && msg[1] <= 50) {
						TIM4 ->CCR4 = 20000 / 50 * msg[1]; //  ((rangeMax-rangeMin)/(xMax-xMin))*(x-xMin)+rangeMin
						TIM3 ->CCR1 = 2000 / 50 * msg[1];  //PC6
					}
				}

			}
			msg[0] = 0;
			msg[1] = 0;
			USBH_ADK_write(&USB_OTG_Core_dev, msg, sizeof(msg));
		}
		Delay(1);

	}
}

void TIM4_Config(void) {

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //TIM4 enable

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/*GPIOD config*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14
			| GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //Alternate function
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //Push-Pull
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //Felhúzó ellenállat
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/*Lábanként engedélyezzük az Alt. Func.-t*/
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4 );
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4 );
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4 );
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4 );

}

void TIM4_Init(void) {

	uint32_t pulse = 3200;
//PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 28000000) - 1;

//TIM_TimeBaseStructure.TIM_Period=665; //minden 666-ik órajelre ad megszakítást
//TIM_TimeBaseStructure.TIM_Prescaler=PrescalerValue;
	TIM_TimeBaseStructure.TIM_Prescaler = 84; // pers 33.6 -> 0,2us-enkét jön ide egy órajel
	TIM_TimeBaseStructure.TIM_Period = 20000; // 20ms for servo period  //

	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //0-tól számlál fölfele

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	/*CH1*/
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable );

	/*CH2*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable );

	/*CH3*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable );

	/*CH4*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable );

	TIM_ARRPreloadConfig(TIM4, ENABLE);

	TIM_Cmd(TIM4, ENABLE);

}

void TIM3_Config(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* GPIOC and GPIOB clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOB, ENABLE);

	/* GPIOC Configuration: TIM3 CH1 (PC6) and TIM3 CH2 (PC7) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* GPIOB Configuration:  TIM3 CH3 (PB0) and TIM3 CH4 (PB1) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect TIM3 pins to AF2 */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3 );
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3 );
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_TIM3 );
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_TIM3 );
}

void TIM3_Init(void) {
	uint16_t PrescalerValue = 0;
	uint32_t pulse = 2000;
	PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 2800000) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 2000;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable );

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable );

	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable );

	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable );
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  assert_failed
 *         Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  File: pointer to the source file name
 * @param  Line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line
	 number,ex: printf("Wrong parameters value: file %s on line %d\r\n",
	 file, line) */

	/* Infinite loop */
	while (1)
	{}
}

#endif

/**
 * @brief  Inserts a delay time.
 * @param  nTime: specifies the delay time length, in 1 ms.
 * @retval None
 */
void Delay(__IO uint32_t nTime) {
	TimingDelay = nTime;

	while (TimingDelay != 0)
		;
}

/**
 * @brief  Decrements the TimingDelay variable.
 * @param  None
 * @retval None
 */
void TimingDelay_Decrement(void) {
	if (TimingDelay != 0x00) {
		TimingDelay--;
	}
}

