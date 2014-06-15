#include "stm32f4_discovery.h"
#include "rccar/inc/init.h"
#include "rccar/inc/stm32_ub_hcsr04.h"
#include <stdlib.h>

void Delay(__IO uint32_t nCount) {
	while (nCount--) {
	}
}



int main(void) {
	int i = 0;

	STM_EVAL_LEDInit(LED3); //g
	STM_EVAL_LEDInit(LED4); //r
	STM_EVAL_LEDInit(LED5); //o
	STM_EVAL_LEDInit(LED6); //b

	init_usart();

	INTTIM_Config();

	TIM3_Config();
	TIM3_Init();

	TIM3->CCR1 = 1500;
	TIM3->CCR2 = 1500;

	UB_HCSR04_Init();

	STM_EVAL_LEDOff(LED3);
	STM_EVAL_LEDOff(LED4);
	STM_EVAL_LEDOff(LED5);
	STM_EVAL_LEDOff(LED6);

	//STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
	EXTILine0_Config();

	/* Infinite loop */
	while (1) {
		i++;

		Delay(0xE00000);

		float disancteCM = UB_HCSR04_Distance_cm();
		//char *hcsMsg = hcsr04DistanceToMassage(disancteCM);

		char floatValue[10];
		gcvt(disancteCM, 6, floatValue);

		char msg[16];
		char msgPre[16];
		char *echokey = "$dcsr,";
		char cs = '*';

		snprintf(msgPre, 16, "%s%s", echokey, floatValue);

		char chsum = getCheckSum(msgPre);

		snprintf(msg, sizeof msg, "%s%c%c", msgPre, cs, chsum);

		sendString(msg);
	}
}


/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size) {
	/* TODO, implement your code here */
	return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void) {
	/* TODO, implement your code here */
	return -1;
}
