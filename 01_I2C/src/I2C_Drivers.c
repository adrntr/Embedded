
#include "I2C_Drivers.h"

#ifdef RECEIVER
uint8_t ownAddress = 0x68;
uint8_t device2Address = 0x61;
#endif

#ifdef SENDER
uint8_t ownAddress=0x61;
uint8_t device2Address = 0x68;
#endif



/***********************************************************************************************
								SEND I2C 1 CHAR
***********************************************************************************************/

void I2C_Send1char(char *msg){

	uint8_t address = 0;

	I2C_GenerateSTART(I2C1,ENABLE);

	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_SB)!=SET);

	address = device2Address << 1;

	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Transmitter);

	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)!=SUCCESS);


	for(int i=0; i < strlen(msg);i++){

		while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE)!=SET);
		I2C_SendData(I2C1, msg[i]);

	}

	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE)!=SET);

	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BTF)!=SET);

	I2C_GenerateSTOP(I2C1, ENABLE);


}


/***********************************************************************************************
								SEND I2C 1 BYTE
***********************************************************************************************/

void I2C_Send1byte(){

	uint8_t address = 0;

	I2C_GenerateSTART(I2C1,ENABLE);

	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_SB)!=SET);

	address = device2Address << 1;

	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Transmitter);

	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)!=SUCCESS);


	I2C_SendData(I2C1, '4');

	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE)!=SET);

	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BTF)!=SET);

	I2C_GenerateSTOP(I2C1, ENABLE);

}


/***********************************************************************************************
								I2C1_EV_IRQHANDLER
***********************************************************************************************/

void I2C_EV_IRQHandler(I2C_HandlerIT *i2cHandlerIT){

	uint32_t sr1=0,sr2=0,sr;

	sr1 = I2C1->SR1;
	sr2 = I2C1->SR2;
	sr=sr1|(sr2<<16);

	/*Start bit on*/
	if(I2C_IT_SB & sr){
		uint32_t dummyRead = I2C1->SR2;
		(void)dummyRead;

		uint8_t AddressAndType=((i2cHandlerIT->receiverAddress<<1)|(i2cHandlerIT->writeRead));
		i2cHandlerIT->I2Cx->DR = AddressAndType;
	}



	/*Addres matched*/
	if(I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED & sr){
		uint32_t dummyRead = I2C1->SR2;
		(void)dummyRead;

		if(i2cHandlerIT->mode == MODE_MASTER){
			i2cHandlerIT->I2Cx->DR = i2cHandlerIT->dataToSend[0];
			i2cHandlerIT->msgSize -= 1;
		}
	}

	/*TxE is set*/
	if(I2C_IT_TXE & sr){
		i2cHandlerIT->I2Cx->DR = i2cHandlerIT->dataToSend[0];
		i2cHandlerIT->msgSize -= 1;
		/*Si ultimo byte enviado --> STOP Condition*/
		if(i2cHandlerIT->msgSize <= 0){
			i2cHandlerIT->I2Cx->CR1 |= I2C_CR1_STOP;
		}
	}


	/*CHECK IF RxNE and BUSY are SET*/
	if(I2C_IT_RXNE & sr){
		i2cHandlerIT->dataReceived[i2cHandlerIT->bytesReceived] = I2C1->DR;
		i2cHandlerIT->bytesReceived ++;
	}

	/*STOP detected*/
	if(I2C_EVENT_SLAVE_STOP_DETECTED & sr){
		uint32_t dummyread = I2C1->SR1;
		(void)dummyread;
		I2C1->CR1 |= 0x0;

		I2C_EventCallBack(STOP);
	}


}
