/*
 * usart_helper.c
 *
 *  Created on: Jun 15, 2014
 *      Author: zoltan
 */

#include "stm32f4_discovery.h"
#include <stdlib.h>
#include "rccar/inc/usart_helper.h"

int putcharx(uint16_t ch) {
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		;
	USART_SendData(USART2, ch);
	return ch;
}

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

char getCheckSum(char *string) {
	int i;
	char XOR;
	char c;
	int len = strlen(string);
// Calculate checksum ignoring any $'s in the string
	for (XOR = 0, i = 0; i < len; i++) {
		c = string[i];
		if (c == '*' || c == '\0')
			break;
		if (c != '$')
			XOR ^= c;
	}
	return XOR;
}

char *hcsr04DistanceToMassage(double distance) {

	char floatValue[10];
	gcvt(distance, 6, floatValue);

	char msg[16];
	char msgPre[16];
	char *echokey = "$dcsr,";
	char cs = '*';

	snprintf(msgPre, 16, "%s%s", echokey, floatValue);

	char chsum = getCheckSum(msgPre);

	snprintf(msg, sizeof msg, "%s%c%c", msgPre, cs, chsum);

	return msg;
}

