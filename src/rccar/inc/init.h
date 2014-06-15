#define USART_TX_PIN				GPIO_Pin_2  // PA2
#define USART_RX_PIN				GPIO_Pin_3  // PA3
#define USART_BAUDRATE				57600
#define USART_WORDLENGTH			USART_WordLength_8b
#define USART_STOPBITS				USART_Parity_No
#define USART_PARITY				USART_StopBits_1
#define USART_HAQDWEREFLOWCONTROL	USART_HardwareFlowControl_None


#define PWM_PIN1					GPIO_Pin_6  // PC6
#define PWM_PIN2					GPIO_Pin_7  // PC7
#define TIM3_PRESCALER				84 			// pers 33.6 -> 0,2us-enkét jön ide egy órajel
#define TIM3_PERIOD					20000 		// 20ms for servo period  //

#define INTTIM_PERIOD				1000000 - 1	 // 1 MHz down to 1 KHz (1 s)
#define INTTIM_PRESCALER			84 - 1		 // 24 MHz Clock down to 1 MHz (adjust per your clock)l

#define EXTI_PIN					GPIO_Pin_6  // PA6

#define KEREK_ATMERO			    30//cm

/*
 	STM_EVAL_LEDInit(LED3); //g
	STM_EVAL_LEDInit(LED4); //r
	STM_EVAL_LEDInit(LED5); //o
	STM_EVAL_LEDInit(LED6); //b
 */
void INTTIM_Config(void);

void init_usart(void);

void TIM3_Config(void);

void TIM3_Init(void);

void EXTILine0_Config(void);
