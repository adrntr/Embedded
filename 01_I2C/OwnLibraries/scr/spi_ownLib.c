#include "spi_ownLib.h"
#include <stdlib.h>
#include <string.h>

void SPI_GpioConfig(){

	/*RCC ENABLES*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

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


}
