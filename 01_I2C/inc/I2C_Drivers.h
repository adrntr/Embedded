#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#define SENDER
//#define RECEIVER


/*@I2C_MODE*/
#define MODE_MASTER 0
#define MODE_SLAVE 	1

/*@I2C_W/R*/
#define I2C_TXMODE_WRITE 	0
#define I2C_TXMODE_READ 	1

/*@I2C_STATUS*/
#define FREE	0
#define BUSY 	1

/*Callbacks*/
#define STOP 1
#define TRANSFER_FINISHED 2

uint8_t ownAddress;
uint8_t device2Address;


typedef struct
{
	I2C_TypeDef* I2Cx;
	uint8_t receiverAddress;
	uint8_t writeRead;				/*@I2C_W/R*/
	char dataToSend[32];
	uint32_t bytesSent;
	uint32_t msgSize;
	char dataReceived[32];          /*!< Where to save the I2C data */
	uint32_t bytesReceived; 		/*!< Number of bytes received at the moment */

	uint8_t mode; 					/*@I2C_MODE*/
	uint8_t status;					/*@I2C_STATUS*/

}I2C_HandlerIT;



void I2C_Send1byte();
void I2C_Receive1byte();
void I2C_Send1char(char *msg);
void I2C_EnableIT();

void I2C_EV_IRQHandler(I2C_HandlerIT *i2cHandlerIT);

void I2C_EventCallBack(uint8_t event);





