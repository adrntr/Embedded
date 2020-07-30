#include "spi_ownLib.h"
#include <stdlib.h>
#include <string.h>



void SPI_GpioConfig(){

	/*RCC ENABLES*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);


	/*GPIOS ALTERNATE FUNC CONFIG - SCL*/
	GPIO_StructInit(&GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Fast_Speed;

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15; //MOSI
	GPIO_Init(GPIOB,&GPIO_InitStruct);


	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB,&GPIO_InitStruct);		//SCK


	//GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14; //MISO
	//GPIO_Init(GPIOB,&GPIO_InitStruct);


	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12; // NSS
	GPIO_Init(GPIOB,&GPIO_InitStruct);


	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);	//NSS
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);	//SCK
	//GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);	//MISO
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);	//MOSI



}
//857
void SPI_Config(){

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	//CR1
	uint16_t SPI_CR1_Aux = 0;
	//1. Configure the serial clock baud rate using BR[2:0]
	SPI_CR1_Aux |= ((0x2) << SPI_CR1_BR_OWN); 		//BR = 010 --> fclk/8

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
	SPI_CR1_Aux &= ~ ((0x1) << SPI_CR1_SSM_OWN); 	//When the SSM bit is set, the NSS pin input is replaced with the value from the SSI bit.
													//SSM = 1 --> for hardware management make SSM to 0

#ifdef Master
	//Configure MSTR: Master selection (0-> SLAVE , 1->MASTER)
	SPI_CR1_Aux |= ((0x1) << SPI_CR1_MSTR_OWM); 		//MSTR = 1 -> MASTER
#else
	SPI_CR1_Aux &= ~((0x1) << SPI_CR1_MSTR_OWM); 		//MSTR = 0 -> SLAVE
#endif

	//Set the DFF bit to configure the data frame format (8 or 16 bits). --> STAY IN 0. DON'T DO NOTHING.

	//COPY AUX INTO CR1 REGISTER

	SPI2->CR1 = SPI_CR1_Aux;

#ifdef Master
	//CR2
	//1. SSOE: SS output enable
	SPI2->CR2 |= (0x1 << SPI_CR2_SSOE_OWN);

#endif
}

void SPI_MasterSendData(SPI_HandlerDef *SPI2_Handler){


	//1. Enable SPI
	SPI2->CR1 |= (0x1 << SPI_CR1_SPE_OWN);

	//2. Check TXE flag
	while(!((SPI2->SR) & (1<<SPI_SR_TXE_OWN)));

	//3. Write data into DR register --> LENGHT
	SPI2->DR = SPI2_Handler->length;

	//3.2 Wait until TXE = 1
	while(!((SPI2->SR) & (1<<SPI_SR_TXE_OWN)));

	//3.3 Write the hole data in SPI
	for(int i=0;i<SPI2_Handler->length;i++){
		SPI2->DR = SPI2_Handler->data[i];
		while(!((SPI2->SR) & (1<<SPI_SR_TXE_OWN)));
	}



	//4. Disable SPI
	//4.1 Wait until RXNE = 1 to receive the last data
	//while(!((SPI2->SR) & (1<<SPI_SR_RXNE_OWN)));
	//4.2 Wait until TXE = 1
	while(!((SPI2->SR) & (1<<SPI_SR_TXE_OWN)));
	//4.3 Wait until BSY = 0
	while((SPI2->SR) & (1<<SPI_SR_BSY_OWN));
	//4.4 Disable SPI
	SPI2->CR1 &= ~ (0x1 << SPI_CR1_SPE_OWN);

}

void SPI_SlaveReceiveData(SPI_HandlerDef *SPI2_Handler){


	//1. Enable SPI
	SPI2->CR1 |= (0x1 << SPI_CR1_SPE_OWN);

	//2. Check RNXE flag
	while(!((SPI2->SR) & (1<<SPI_SR_RXNE_OWN)));

	//3. Read data from DR register --> LENGHT
	SPI2_Handler->length = SPI2->DR;



	//3.3 Read the hole data in SPI
	for(int i=0;i<SPI2_Handler->length;i++){
		while(!((SPI2->SR) & (1<<SPI_SR_RXNE_OWN)));
		SPI2_Handler->data[i] = SPI2->DR;
	}



	//4. Disable SPI

	//4.2 Wait until TXE = 1
	while(!((SPI2->SR) & (1<<SPI_SR_TXE_OWN)));
	//4.3 Wait until BSY = 0
	while((SPI2->SR) & (1<<SPI_SR_BSY_OWN));
	//4.4 Disable SPI
	SPI2->CR1 &= ~ (0x1 << SPI_CR1_SPE_OWN);

}
