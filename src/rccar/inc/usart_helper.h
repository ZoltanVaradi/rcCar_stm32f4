/*
 * usart_helper.h
 *
 *  Created on: Jun 15, 2014
 *      Author: zoltan
 */

int putcharx(uint16_t ch);

/*
 char msg[256];
 char *echokey = "$echo,";
 char cs = '*';
 char chsum = 14;

 snprintf(msg, sizeof msg, "%s%d%c%c", echokey, echoIndex, cs, chsum);

 sendString(msg);
 */
int sendString(char *str);

char getCheckSum(char *string);

char *hcsr04DistanceToMassage(double distance);
