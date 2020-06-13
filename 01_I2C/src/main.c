/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "I2C_Drivers.h"
#include <stdio.h>
#include <string.h>

#define SDA_PIN GPIO_Pin_9
#define SDA_GPIO GPIOB
#define SDA_PinSource GPIO_PinSource9

#define SCL_PIN GPIO_Pin_8
#define SCL_GPIO GPIOB
#define SCL_PinSource GPIO_PinSource8






RCC_ClocksTypeDef RCC_Clock;
USART_InitTypeDef USART_InitStruct;
GPIO_InitTypeDef GPIO_InitStruct;
I2C_InitTypeDef I2C_InitStruct;
EXTI_InitTypeDef EXTI_initButton;


uint32_t iDelay = 0;

I2C_HandlerIT i2cHandler;



/***********************************************************************************************
								FUNCTIONS DECLARATIONS
***********************************************************************************************/

void Error_Handler();
void RCC_Config();
void Uart_Config();
void printmsg(char *msg);
void GPIOS_Config();
void I2C_Config();
void I2C_IRQConfig();
void GPIOS_IRQConfig();
void delay(uint32_t delayms);



/***********************************************************************************************
										MAIN
***********************************************************************************************/

int main(void){
	//RCC_Config();

	/*

	RCC_PLLCmd(DISABLE);

	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);

	RCC_GetClocksFreq(&RCC_Clock);

	if(SysTick_Config(RCC_Clock.HCLK_Frequency/1000)==1){
		Error_Handler();
	}

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
*/
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);


	Uart_Config();


	GPIOS_Config();

	GPIOS_IRQConfig();



	I2C_Config();

	I2C_IRQConfig();

	while(1){

	}

}


/***********************************************************************************************
								I2C_EVEN CALL BACK
***********************************************************************************************/

void I2C_EventCallBack(uint8_t event){

	switch(event){
	case STOP:
		printmsg("Transfer finished. Data = ");
		printmsg(i2cHandler.dataReceived);

		memset(i2cHandler.dataReceived,0,sizeof(i2cHandler.dataReceived));
		i2cHandler.bytesReceived = 0;
		break;

	}

}


/***********************************************************************************************
										DELAY
***********************************************************************************************/

void delay(uint32_t delayms){

	iDelay=1;
	while(iDelay<delayms);
	iDelay = 0;


}

/***********************************************************************************************
								GPIO CONFIG
***********************************************************************************************/

void GPIOS_Config(){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/*LED*/
	//reset values
	GPIO_StructInit(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA,&GPIO_InitStruct);

	/*BUTTON*/
	GPIO_StructInit(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOC,&GPIO_InitStruct);

	/*ALL GPIOC AS OUTPUT*/
	GPIO_StructInit(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All&~GPIO_Pin_13;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOC,&GPIO_InitStruct);

}

/***********************************************************************************************
								GPIO CONFIG
***********************************************************************************************/

void GPIOS_IRQConfig(){

	EXTI_initButton.EXTI_Line = EXTI_Line13;
	EXTI_initButton.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_initButton.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_initButton.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_initButton);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Connect EXTI4 Line to PB4 pin */

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
	/* Configure EXTI4 line */


	NVIC_InitTypeDef NVICinit;
	NVICinit.NVIC_IRQChannel =  EXTI15_10_IRQn;
	NVICinit.NVIC_IRQChannelCmd = ENABLE;
	NVICinit.NVIC_IRQChannelPreemptionPriority = 15;
	NVICinit.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVICinit);

}

/***********************************************************************************************
								UART CONFIG
***********************************************************************************************/
void Uart_Config(){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	//reset values
	GPIO_StructInit(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_Init(GPIOA,&GPIO_InitStruct );



	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2, &USART_InitStruct );


	USART_Cmd(USART2, ENABLE);

	printmsg("UART CONFIGURED \r\n");
}

/***********************************************************************************************
								RCC CONFIG
***********************************************************************************************/
void RCC_Config(){



	RCC_HSEConfig(RCC_HSE_Bypass);

	if(RCC_WaitForHSEStartUp()==ERROR){
		Error_Handler();
	}

	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);

	RCC_HSICmd(DISABLE);

	RCC_GetClocksFreq(&RCC_Clock);

	if(SysTick_Config(RCC_Clock.HCLK_Frequency/1000)==1){
		Error_Handler();
	}


}

/***********************************************************************************************
								I2C CONFIG
***********************************************************************************************/
void I2C_Config(){

	/*RCC ENABLES*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);

	/*GPIOS ALTERNATE FUNC CONFIG - SCL*/
	GPIO_StructInit(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = SCL_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;

	GPIO_Init(SCL_GPIO,&GPIO_InitStruct);

	/*GPIOS ALTERNATE FUNC CONFIG - SDA*/
	GPIO_StructInit(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = SDA_PIN; //c12 -> SDA
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;

	GPIO_Init(SDA_GPIO,&GPIO_InitStruct);


	GPIO_PinAFConfig(SCL_GPIO, SCL_PinSource, GPIO_AF_I2C1);
	GPIO_PinAFConfig(SDA_GPIO, SDA_PinSource, GPIO_AF_I2C1);


	/*I2C CONFIG*/
	I2C_StructInit(&I2C_InitStruct);
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_ClockSpeed = 100000;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_OwnAddress1 = ownAddress<<1;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_Init(I2C1, &I2C_InitStruct);

	/*I2C ENABLE*/
	//I2C_Cmd(I2C1, ENABLE);


}

/***********************************************************************************************
								I2C IRQ CONFIG
***********************************************************************************************/

void I2C_IRQConfig(){


	NVIC_InitTypeDef NVICinit;
	NVICinit.NVIC_IRQChannel = I2C1_EV_IRQn;
	NVICinit.NVIC_IRQChannelCmd = ENABLE;
	NVICinit.NVIC_IRQChannelPreemptionPriority = 15;
	NVICinit.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVICinit);

	NVICinit.NVIC_IRQChannel = I2C1_ER_IRQn;
	NVIC_Init(&NVICinit);

	I2C_ITConfig(I2C1, I2C_IT_BUF|I2C_IT_EVT|I2C_IT_ERR, ENABLE);
}
/***********************************************************************************************
								PRINT MESSAGE
***********************************************************************************************/

void printmsg(char* msg){

	for(uint32_t i = 0; i<strlen(msg);i++){
		//Don't send msg if the txe flag is not set (transmission buffer empty)
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
		//send data
		USART_SendData(USART2, msg[i]);
	}

}

/***********************************************************************************************
								ERROR HANDLER
***********************************************************************************************/
void Error_Handler(){
	while(1);
}

/***********************************************************************************************
								SYSTICK HANDLER
***********************************************************************************************/
//IRQ each 1 ms --> if we don't call this function, there is an error and the program finish
void SysTick_Handler(){
	printmsg("´");
	if(iDelay>0){
		iDelay ++;
		GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
	}
}

/***********************************************************************************************
								I2C1 EVENT IRQ HANDLER
***********************************************************************************************/

void I2C1_EV_IRQHandler (){
	I2C_EV_IRQHandler(&i2cHandler);
}

void I2C1_ER_IRQHandler(){
	printmsg("Error, Cortaste toda la lux");
}

/***********************************************************************************************
								BUTTON IRQ HANDLER
***********************************************************************************************/

void EXTI15_10_IRQHandler(){
	GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
	EXTI_ClearITPendingBit(EXTI_Line13);
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)==0){
		printmsg("Sending");
		I2C_Send1char("hola caraculo puto\r\n");
	}




}
/***********************************************************************************************
								NVIC PRIORITYGROUP CONFIG
***********************************************************************************************/

