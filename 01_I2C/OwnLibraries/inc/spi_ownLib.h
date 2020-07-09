#include "stm32f4xx.h"


GPIO_InitTypeDef GPIO_InitStruct;

#define SPI_CR1_CPHA_OWN 		0
#define SPI_CR1_CPOL_OWN 		1
#define SPI_CR1_MSTR_OWM		2
#define SPI_CR1_BR_OWN			3
#define SPI_CR1_SPE_OWN 		6
#define SPI_CR1_LSBFIRST_OWN	7
#define SPI_CR1_SSI_OWN			8
#define SPI_CR1_SSM_OWN			9
#define SPI_CR1_RXONLY_OWN		10
#define SPI_CR1_DFF_OWN			11
#define SPI_CR1_BIDIMODE_OWN	15


#define SPI_CR2_SSOE_OWN		2

#define SPI_SR_RXNE_OWN			0
#define SPI_SR_TXE_OWN			1
#define SPI_SR_BSY_OWN			7


/***********************************************************************************************
								FUNCTIONS DECLARATIONS
***********************************************************************************************/

void SPI_GpioConfig();
void SPI_Config();
void SPI_MasterSendData();
