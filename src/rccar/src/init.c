/*
 * init.c
 *
 *  Created on: Jun 15, 2014
 *      Author: zoltan
 */


#include "stm32f4xx_usart.h"
#include "rccar/inc/init.h"

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
EXTI_InitTypeDef EXTI_InitStructure;

/*
 * ----------------------------------------------------------------------------------------------------------
 */
void INTTIM_Config(void) {

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = INTTIM_PERIOD;
	TIM_TimeBaseStructure.TIM_Prescaler = INTTIM_PRESCALER;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

/*
 * -------------------------------------------------------------------------------------------------------------
 */
void init_usart(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	/* enable peripheral clock for USART2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	/* GPIOA clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* GPIOA Configuration: USART2 TX, RX on PA2 | PA3 */
	GPIO_InitStructure.GPIO_Pin = USART_TX_PIN | USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*Alternate Functions on GPIOA 2|3*/
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	USART_InitStructure.USART_BaudRate = USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WORDLENGTH;
	USART_InitStructure.USART_StopBits = USART_STOPBITS;
	USART_InitStructure.USART_Parity = USART_PARITY;
	USART_InitStructure.USART_HardwareFlowControl = USART_HAQDWEREFLOWCONTROL;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE); // enable U(S)ART2

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * --------------------------------------------------------------------------------------------------------------------
 */
void TIM3_Config(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* GPIOC and GPIOB clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOB, ENABLE);

	/* GPIOC Configuration: TIM3 CH1 (USART_PWM_PIN1) and TIM3 CH2 (USART_PWM_PIN2) */
	GPIO_InitStructure.GPIO_Pin = PWM_PIN1 | PWM_PIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Connect TIM3 pins to AF2 */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3);
}

void TIM3_Init(void) {

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = TIM3_PRESCALER;
	TIM_TimeBaseStructure.TIM_Period = TIM3_PERIOD;

	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}


void EXTILine0_Config(void) {

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	  /* Enable SYSCFG clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	  /* Configure PA6 pin as input floating */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Pin = EXTI_PIN;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  /* Connect EXTI Line6 to PA6 pin */
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6);

	  /* Configure EXTI Line6 */
	  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);

	  /* Enable and set EXTI Line9-5 Interrupt to the lowest priority */
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);
}
