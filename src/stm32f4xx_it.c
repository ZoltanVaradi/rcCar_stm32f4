/**
 ******************************************************************************
 * @file    Project/STM32F4xx_StdPeriph_Template/stm32f4xx_it.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    18-January-2013
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "stm32f4xx_usart.h"
#include "stm32f4_discovery.h"
#include <stdlib.h>
#include "rccar/inc/stm32_ub_hcsr04.h"
#include "rccar/inc/init.h"

#define ECHO_NO_REPLAY_COUNT_MAX 1

char StringLoop[0xFF];

int echoIndex = 0;
int echoNoReplyCount = 0;

int fordulasokSzama = 0;

void EXTI0_IRQHandler(void) {

	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		/* Toggle LED4 */
		STM_EVAL_LEDToggle(LED4);

	//	fordulasokSzama++;

		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);
	}

}

void EXTI9_5_IRQHandler(void) {

	if (EXTI_GetITStatus(EXTI_Line6) != RESET) {
		/* Toggle LED4 */
		STM_EVAL_LEDToggle(LED4);

		fordulasokSzama++;

		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line6);
	}

}

void USART2_IRQHandler(void) {
	static int rx_index = 0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		StringLoop[rx_index++] = USART_ReceiveData(USART2);

		if (echoNoReplyCount <= ECHO_NO_REPLAY_COUNT_MAX) {

			if ((rx_index - 2) > -1) {
				//0x19 = 25b
				if (StringLoop[rx_index - 2] == 0x64
						&& StringLoop[rx_index - 1] < 90) {
					TIM3->CCR2 = (((2000 - 1000) / 90)
							* StringLoop[rx_index - 1]) + 1000;   //PC7
					STM_EVAL_LEDToggle(LED4);
				}

				if (StringLoop[rx_index - 2] == 0x65
						&& StringLoop[rx_index - 1] < 90) {

					TIM3->CCR1 = (((2000 - 1000) / 90)
							* StringLoop[rx_index - 1]) + 1000; //PC6 //  ((rangeMax-rangeMin)/(xMax-xMin))*(x-xMin)+rangeMin
					STM_EVAL_LEDToggle(LED3);
				}

			}
		}

		if (StringLoop[rx_index - 2] == 0x66
				&& StringLoop[rx_index - 1] == 0x02) {
			echoNoReplyCount = 0;
		}

		if (rx_index >= (sizeof(StringLoop) - 1)) //StringLoop
			rx_index = 0;
		STM_EVAL_LEDOff(LED6);
	}
}

void TIM2_IRQHandler(void) {

	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET) {
		//--------------------------------------------------------------
		// ISR von Timer2
		// wird bei Lo-Flanke vom Echo-Signal aufgerufen
		//--------------------------------------------------------------
		uint32_t start, stop;
		// Interrupt Flags loeschen
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

		//-----------------------------------
		// Messwerte auslesen
		//-----------------------------------
		start = TIM_GetCapture1(TIM2);
		stop = TIM_GetCapture2(TIM2);
		HCSR04.delay_us = start - stop;
		HCSR04.t2_akt_time++;
	}
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		int tavolsag = fordulasokSzama * KEREK_ATMERO;
		fordulasokSzama=0;

		if (echoNoReplyCount > ECHO_NO_REPLAY_COUNT_MAX) {
			STM_EVAL_LEDOn(LED6);
			TIM3->CCR1 = 1500;
			TIM3->CCR2 = 1500;
		}
		char msg[256];
		char *echokey = "$echo,";
		char cs = '*';
		char chsum = 14;

		snprintf(msg, sizeof msg, "%s%d%c%c", echokey, echoIndex, cs, chsum);

		sendString(msg);

		++echoIndex;
		++echoNoReplyCount;

		//------------------------------  tavolsg----------------------------------

		char msgT[16];
		char msgPrer[16];
		char *echokeyr = "$dist,";

		snprintf(msgPrer, sizeof msgPrer, "%s%d", echokeyr, tavolsag);

		char chsumt = getCheckSum(msgPrer);

		snprintf(msgT, sizeof msgT, "%s%c%c", msgPrer, cs, chsumt);

		sendString(msgT);
	}
}
/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1) {
		TIM3->CCR1 = 1500;
		TIM3->CCR2 = 1500;
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void) {
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void) {
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void) {
	/*  TimingDelay_Decrement(); */
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
 {
 }*/

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
