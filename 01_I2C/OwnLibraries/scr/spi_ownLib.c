#include "spi_ownLib.h"
#include <stdlib.h>
#include <string.h>

void SPI_GpioConfig(){

	/*RCC ENABLES*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	/*GPIOS ALTERNATE FUNC CONFIG - SCL*/
	GPIO_StructInit(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;

	GPIO_Init(GPIOA,&GPIO_InitStruct);		//SCK


	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6; //MISO
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7; //MOSI
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4; // NSS
	GPIO_Init(GPIOA,&GPIO_InitStruct);


	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI1);	//NSS
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);	//SCK
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);	//MISO
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);	//MOSI

}
//857
void SPI_Config(){

	//CR1
	uint16_t SPI_CR1_Aux = 0;
	//1. Configure the serial clock baud rate using BR[2:0]
	SPI_CR1_Aux &= ~((0x7) << SPI_CR1_BR_OWN); 		//BR = 000 --> fclk/2

	//2. Configure the CPOL and CPHA bits combination to define one of the four
	//relationships between the data transfer and the serial clock
	SPI_CR1_Aux &= ~((0x3) << SPI_CR1_CPHA_OWN); 		//CPOL = 0 , CPHA = 0

	//Select simplex or half-duplex mode by configuring RXONLY or BIDIMODE and
	//BIDIOE (RXONLY and BIDIMODE can't be set at the same time)
	SPI_CR1_Aux &= ~ ((0x1) << SPI_CR1_RXONLY_OWN); 	//RXONLY = 0
	SPI_CR1_Aux &= ~ ((0x1) << SPI_CR1_BIDIMODE_OWN);	//BIDIMODE = 0

	//Configure the LSBFIRST bit to define the frame format
	SPI_CR1_Aux &= ~ ((0x1) << SPI_CR1_LSBFIRST_OWN); //LSBFIRST = 0 -> MSB transmitted first

	//Configure the CRCEN and CRCEN bits if CRC is needed (while SCK clock signal
	//is at idle state). --> STAY IN 0. DON'T DO NOTHING.

	//Configure SSM and SSI
	SPI_CR1_Aux |= ((0x3) << SPI_CR1_SSI_OWN); 		//When the SSM bit is set, the NSS pin input is replaced with the value from the SSI bit.
													//SSM = 1 , SSI = 1

	//Configure MSTR: Master selection (0-> SLAVE , 1->MASTER)
	SPI_CR1_Aux |= ((0x1) << SPI_CR1_MSTR_OWM); 		//MSTR = 1 -> MASTER

	//Set the DFF bit to configure the data frame format (8 or 16 bits). --> STAY IN 0. DON'T DO NOTHING.

	//COPY AUX INTO CR1 REGISTER

	SPI1->CR1 = SPI_CR1_Aux;

	//CR2
	//1. SSOE: SS output enable
	SPI1->CR2 |= (0x1 << SPI_CR2_SSOE_OWN);

}

void SPI_MasterSendData(){
	//1. Enable SPI
	SPI1->CR1 |= (0x1 << SPI_CR1_SPE_OWN);

	//2. Check TXE flag
	while(!((SPI1->SR) & (1<<SPI_SR_TXE_OWN)));

	//3. Write data into DR register
	SPI1->DR = 0x44;

	//4. Disable SPI
	//4.1 Wait until RXNE = 1 to receive the last data
	while(!((SPI1->SR) & (1<<SPI_SR_RXNE_OWN)));
	//4.2 Wait until TXE = 1
	while(!((SPI1->SR) & (1<<SPI_SR_TXE_OWN)));
	//4.3 Wait until BSY = 0
	while((SPI1->SR) & (1<<SPI_SR_BSY_OWN));
	//4.4 Disable SPI
	SPI1->CR1 &= ~ (0x1 << SPI_CR1_SPE_OWN);

}
