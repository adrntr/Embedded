#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#define SENDER
//#define RECEIVER


/*@I2C_MODE*/
#define MODE_MASTER 0
#define MODE_SLAVE 	1

#define STOP 1

uint8_t ownAddress;
uint8_t device2Address;


typedef struct
{
	I2C_TypeDef* I2Cx;
	uint8_t receiverAddress;
	uint8_t writeRead;
	char dataToSend[32];
	uint32_t msgSize;
	char dataReceived[32];          /*!< Where to save the I2C data */
	uint32_t bytesReceived; 		/*!< Number of bytes received at the moment */

	uint8_t mode; 					/*@I2C_MODE*/

}I2C_HandlerIT;



void I2C_Send1byte();
void I2C_Receive1byte();
void I2C_Send1char(char *msg);
void I2C_EnableIT();

void I2C_EV_IRQHandler(I2C_HandlerIT *i2cHandlerIT);

void I2C_EventCallBack(uint8_t event);




