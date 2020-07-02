/*
 * main_SPI.c
 *
 *  Created on: 30 jun. 2020
 *      Author: adria
 */


#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"

USART_InitTypeDef USART_InitStruct;
GPIO_InitTypeDef GPIO_InitStruct;

void Uart_Config();
void printmsg(char *msg);

/***********************************************************************************************
										MAIN
***********************************************************************************************/

int main(){

	Uart_Config();

	return 0;
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

