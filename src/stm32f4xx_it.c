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

char StringLoop[0xFF];

int putcharx(uint16_t ch) {
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USART2, ch);
	return ch;
}

int echoIndex = 0;
int echoNoReplyCount = 0;

int sendString(char *str) {
	int i = 0;
	char c;
	int len = strlen(str);
	for (i = 0; i < len; i++) {
		c = str[i];
		if (c == '\0') {
			break;
		}
		putcharx(c);
	}

	return 1;
}

void EXTI0_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		/* Sending a single character */
		uint16_t sendchar = 0x12;
		USART_SendData(USART2, 'r'); //in: stm32f4xx_usart.h

		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);

		USART_SendData(USART2, 't'); //in: stm32f4xx_usart.h

	}

}

void USART2_IRQHandler(void) {
	static int rx_index = 0;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		StringLoop[rx_index++] = USART_ReceiveData(USART2);

		if ((rx_index - 2) > -1) {
			//0x19 = 25b
			if (StringLoop[rx_index - 2] == 0x64
					&& StringLoop[rx_index - 1] < 0x33) {
				TIM3->CCR2 = 2000 / 50 * StringLoop[rx_index - 1];  //PC7
				STM_EVAL_LEDToggle(LED4);
			}

			if (StringLoop[rx_index - 2] == 0x65
					&& StringLoop[rx_index - 1] < 0x33) {

				TIM3->CCR1 = 2000 / 50 * StringLoop[rx_index - 1]; //PC6 //  ((rangeMax-rangeMin)/(xMax-xMin))*(x-xMin)+rangeMin
				STM_EVAL_LEDToggle(LED3);
			}

			if (StringLoop[rx_index - 2] == 0x66
					&& StringLoop[rx_index - 1] == 0x02) {
				echoNoReplyCount = 0;
			}
		}
		if (rx_index >= (sizeof(StringLoop) - 1)) //StringLoop
			rx_index = 0;
		STM_EVAL_LEDOff(LED6);
	}
}

void TIM2_IRQHandler(void) {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		if (echoNoReplyCount > 1) {
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
