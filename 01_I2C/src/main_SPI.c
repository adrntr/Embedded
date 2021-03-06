/*
 * main_SPI.c
 *
 *  Created on: 30 jun. 2020
 *      Author: adria
 */


#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "spi_ownLib.h"

#define SUM 1
#define SUB 2
#define MUL 3


USART_InitTypeDef USART_InitStruct;
GPIO_InitTypeDef GPIO_InitStruct;

SPI_HandlerDef SPI_Handler;

void Uart_Config();
void printmsg(char *msg);
void ButtonLed_Config();

char data[200];
char handlerdata[32];

typedef struct operation{
	char num_one;
	char num_two;
	char operation;
}OperationStruct;

void printmsg(char* msg);

void delay(void){
	for ( uint32_t i=0;i<200000;i++);
}

/***********************************************************************************************
										MAIN
***********************************************************************************************/

int main(){

	Uart_Config();

	ButtonLed_Config();

	SPI_GpioConfig();

	SPI_Config();

#ifdef Master
	OperationStruct oper;


	printmsg("/********/\nMASTER\n/********/ \n\r");
	printmsg("Select operation:\n\r"
			"1. sum\n\r"
			"2. substract\n\r"
			"3. Multiply\n\r");
	oper.operation = getchar();
	printmsg("Insert number 1: ");
	oper.num_one = getchar();
	printmsg("Insert number 2: ");
	scanf("%c",&oper.num_two);

	memcpy(SPI_Handler.data,"Hello world",strlen("Hello world"));

	SPI_Handler.length = strlen(SPI_Handler.data);

	while(1){
		while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13));

		delay();

		SPI_MasterSendData(&SPI_Handler);

		delay();
	}

#else

	printmsg("SOY EL ESCLAVO Y LA TENGO RE CHICA \n\r");

	while(1){

		SPI_SlaveReceiveData(&SPI_Handler);

		printmsg(SPI_Handler.data);
		printmsg("\n\r");

	}

#endif

	return 0;
}

/***********************************************************************************************
								BUTTON AND LED CONFIG
***********************************************************************************************/

void ButtonLed_Config(){

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		/*BUTTON*/
		GPIO_StructInit(&GPIO_InitStruct);

		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

		GPIO_Init(GPIOC,&GPIO_InitStruct);

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

	//printmsg("UART CONFIGURED \r\n");
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

