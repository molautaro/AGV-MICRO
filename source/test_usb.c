/*
 * Copyright 2016-2023 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    test_usb.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "Util.h"
#include "usb_device_interface_0_cic_vcom.h"



/* TODO: insert other include files here. */

#define INITESPCMD 0xC0 //Inicializar ESP
#define MOTORSCMD 0xD0 //comando motor
#define TESTCMD 0xD2 // COMANDO ON/OFF MOTOR
#define ALIVECMD 0xF0 // comando alive
#define ENABLE_MOTOR_CMD 0x01 //COMANDO ENABLE MOTOR
#define DISABLE_MOTOR_CMD 0x02 // COMANDO DISABLE MOTOR
#define SPEED_MOTOR_CMD 0x03
#define ID_M_DIREC FLEXCAN_ID_STD(0x601) //ID CAN MOTOR DIRECCION
#define ID_M_VEL FLEXCAN_ID_STD(0x607) //ID CAN MOTOR VELOCIDAD O TRACCION
#define RX_MESSAGE_STD_BUFFER_NUM (0)
#define RX_MESSAGE_EXT_BUFFER_NUM (1)
#define TX_MESSAGE_BUFFER_NUM (2)
#define TX_STD_MESSAGE_BUFFER_NUM (3)
#define BATTERY_RECEIVE_ID 0x18FF50E5
#define BATTERY_SEND_ID 0x1806E5F4
#define ID_RFID_SENSOR FLEXCAN_ID_STD(0x100)
#define ID_MAGNETIC_SENSOR FLEXCAN_ID_STD(0x123)
#define ID_SEND_BATERIA FLEXCAN_ID_EXT(0x1806E5F4)//ID de msjs enviados desde placa a la bateria
#define ID_REC_BATERIA FLEXCAN_ID_EXT(0x18FF50E5)//ID de msjs que recibe la placa desde la bateria

static uint8_t BATTERY_CHARGE_MESSAGE[] = {0x02, 0x48, 0x01, 0x2C, 0x00, 0x00, 0x00, 0x00};

#define MSJ_BAT 0x01
// para funcion ChargetoCANbuf(); whatFormat
#define DATA_STD 0x0A
#define REMOTE_STD 0x1A
#define DATA_EXT 0x2B
#define REMOTE_EXT 0x3B

//para CreateCANMessage(msj);
#define ENABLE_MOTOR_CAN 0x02
#define DISABLE_MOTOR_CAN 0x03


/* TODO: insert other definitions and declarations here. */
void EnviarDatos(uint8_t cmd);
void RecibirDatos(uint8_t head);
void Decode();
void CheckChecksumAndReceiveData();
void UpdateChecksum();
void CheckBytesLeft();
void DecodeMagneticSensor();
static void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData);
void DecodeCANMessage();
void DecodeRFIDSensor();
void CreateCANMessage(uint8_t msj);
void ActionQT();
void ChargeToCANBuf(uint8_t whatFormat, uint8_t payloadCAN[], uint32_t id);

/*******************************************************************************
 * Variables
 ******************************************************************************/

// typedef
typedef union{
	struct{
		uint8_t b0: 1;
		uint8_t b1: 1;
		uint8_t b2: 1;
		uint8_t b3: 1;
		uint8_t b4: 1;
		uint8_t b5: 1;
		uint8_t b6: 1;
		uint8_t b7: 1;
	}bit;
	uint8_t byte;
}_sFlag;


typedef union {
    uint8_t u8[4];
    int8_t i8[4];
    uint16_t u16[2];
    int16_t i16[2];
    int32_t i32;
    uint32_t u32;
    float f;
} _sWork;


volatile uint32_t timerCounter;
volatile _rx ringRx, auxRX;
volatile _tx ringTx, auxTX;
uint16_t timeoutUSB = 200;
uint16_t timeoutBAT = 100;
uint8_t rxBuf[256], txBuf[256], auxbufRX[256],auxbufTX[256], auxlenght;
//uint8_t flag_carga_completa = 0;
//uint8_t msj_CAN_BAT[8] = {0x02, 0x48, 0x01, 0x2C, 0x00, 0x00, 0x00, 0x00};
//uint8_t msj_CAN_BAT2[8] = {0x00, 0x00, 0x00, 0x00, 0x0A, 0x0B, 0x0C, 0x0D};
uint16_t magneticSensorBitStatus;
uint16_t volt_bateria = 0;
volatile _sFlag flag1, SensorsStatus, flagQT;
volatile _sWork RFIDData[2],DestinationStation[2];

#define SENSORSTATUS_0 	SensorsStatus.bit.b0 //Estado Sensor 0
#define SENSORSTATUS_1 	SensorsStatus.bit.b1 //Estado Sensor 1
#define SENSORSTATUS_2 	SensorsStatus.bit.b2 //Estado Sensor 2
#define SENSORSTATUS_3 	SensorsStatus.bit.b3 //Estado Sensor 3
#define SENSORSTATUS_4 	SensorsStatus.bit.b4 //Estado Sensor 4
#define SENSORSTATUS_5 	SensorsStatus.bit.b5 //Estado Sensor 5
#define SENSORSTATUS_6 	SensorsStatus.bit.b6 //Estado Sensor 6
#define SENSORSTATUS_7 	SensorsStatus.bit.b7 //Estado Sensor 7


#define w 	flag1.bit.b0 //
#define RX_CAN_COMPLETE flag1.bit.b1
#define BATT_FULL_CHARGE flag1.bit.b2
#define WWWW 			flag1.bit.b3
#define WWWWW 			flag1.bit.b4
#define WWWWWW 			flag1.bit.b5
#define WWWWWWW 		flag1.bit.b6
#define WWWWWWWW 		flag1.bit.b7

//PARA COMANDOS RECIBIDOS POR QT
#define ALIVE_RECIVE_CMD 	flagQT.bit.b0 //RECIBIO alive
#define ENABLE_RECIVE_CMD 	flagQT.bit.b1
#define DISABLE_RECIVE_CMD	flagQT.bit.b2
#define AAAA 				flagQT.bit.b3
#define AAAAA 				flagQT.bit.b4
#define AAAAAA 				flagQT.bit.b5
#define AAAAAAA 			flagQT.bit.b6
#define AAAAAAAA 			flagQT.bit.b7

flexcan_handle_t rxHandle, txHandle, flexcanHandle;
flexcan_frame_t txFrame, RX_STD_Frame, RX_EXT_Frame;
flexcan_mb_transfer_t TX_CAN_BUF, RX_EXT_CAN_BUF, RX_STD_CAN_BUF;//buffer para enviar,bufer no usado, buffer de recepcion
//flexcan_frame_t frame;
flexcan_rx_mb_config_t mbConfigSTD, mbConfigEXT;

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    FLEXCAN_SetRxMbGlobalMask(CAN0, FLEXCAN_RX_MB_STD_MASK(0, 0, 0));
    FLEXCAN_SetRxMbGlobalMask(CAN0, FLEXCAN_RX_MB_EXT_MASK(0, 0, 0));
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
    //kFLEXCAN_FrameFormatMix;

    //auxRX.buf = auxbufRX;
    //auxTX.buf = auxbufTX;
    ringTx.buf=txBuf;
    ringRx.buf=rxBuf;
    ringTx.iW=0;
    ringTx.iR=0;
    ringRx.iW=0;
    ringRx.iR=0;
    /*auxTX.iW=0;
    auxTX.iR=0;
    auxRX.iW=0;
    auxRX.iR=0;*/
    LED_RED_INIT(1);
    LED_GREEN_INIT(1);
    LED_BLUE_INIT(1);


    //TX_CAN_BUF.frame->format = kFLEXCAN_FrameFormatStandard;
    txFrame.type = kFLEXCAN_FrameTypeData;
    txFrame.length = 8;
    //txframe_STD.type = kFLEXCAN_FrameTypeData;
    //txframe_STD.length = 8;
    //txFrame.format = kFLEXCAN_FrameFormatExtend;
    //txFrame.id = ID_SEND_BATERIA;
    //txFrame.length = 8;
    //TX_CAN_BUF.frame->type = kFLEXCAN_FrameTypeData;
    TX_CAN_BUF.mbIdx = TX_MESSAGE_BUFFER_NUM;
    TX_CAN_BUF.frame = &txFrame;
    /*TX_STD_CAN_BUF.mbIdx = TX_STD_MESSAGE_BUFFER_NUM;
    TX_STD_CAN_BUF.frame = &txframe_STD;*/

    RX_STD_CAN_BUF.mbIdx = RX_MESSAGE_STD_BUFFER_NUM;
    RX_STD_CAN_BUF.frame = &RX_STD_Frame;
    //RX_EXT_CAN_BUF.mbIdx = RX_MESSAGE_STD_BUFFER_NUM;
    //RX_EXT_CAN_BUF.frame = &RX_EXT_Frame;
    mbConfigSTD.format = kFLEXCAN_FrameFormatStandard;
    mbConfigSTD.type   = kFLEXCAN_FrameTypeData;
    mbConfigSTD.id = FLEXCAN_ID_STD(0x0);
    FLEXCAN_SetRxMbConfig(CAN0, RX_MESSAGE_STD_BUFFER_NUM, &mbConfigSTD, true);
    //mbConfigEXT.format = kFLEXCAN_FrameFormatExtend;
    //mbConfigEXT.type   = kFLEXCAN_FrameTypeData;
    //mbConfigEXT.id = FLEXCAN_ID_EXT(0);
    //FLEXCAN_SetRxMbConfig(CAN0, RX_MESSAGE_EXT_BUFFER_NUM, &mbConfigEXT, true);

    FLEXCAN_TransferCreateHandle(CAN0, &flexcanHandle, flexcan_callback, NULL);
    FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_STD_CAN_BUF);
    //FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_EXT_CAN_BUF);

#endif

    PRINTF("Hello World\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    DestinationStation[0].u8[0]= 0x45;
    DestinationStation[0].u8[1]= 0x31;
    while(1) {
    	USB_DeviceInterface0CicVcomTask();
    	//if(FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_STD_CAN_BUF) == kStatus_Success)
    	//LED_BLUE_ON();
    	//if(FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_EXT_CAN_BUF) == kStatus_Success)


    	if(!timeoutUSB){
    	    EnviarDatos(ALIVECMD);
    	    timeoutUSB = 400;
    	}

    	/*if(!timeoutBAT && !BATT_FULL_CHARGE){
    		CreateCANMessage(MSJ_BAT);
    		timeoutBAT = 100;
    	}*/

    	Decode();

    	if(flagQT.byte){ //maquina de estado para comandos recibidos de QT
    		ActionQT();
    	}

    	if(RX_CAN_COMPLETE){//recibe exitosamente un msj por CAN
    		DecodeCANMessage();//llamo a funcion de decodificar mensaje
    		RX_CAN_COMPLETE = 0;
    	}

    	//FLEXCAN_TransferSendNonBlocking(CAN0, &flexcanHandle, & MENSAJE);

        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}

void DecodeCANMessage(){
	switch (RX_STD_CAN_BUF.frame->id) {
		case ID_RFID_SENSOR: //cuando el mensaje que llego es del RFID
			//LED_GREEN_TOGGLE();
			DecodeRFIDSensor();
			break;
		case ID_MAGNETIC_SENSOR: //Cuando el mensaje que llego es del SENSOR MAGNETICO
			//LED_RED_TOGGLE();
			DecodeMagneticSensor();
			break;
		case ID_REC_BATERIA: //cuando la bateria manda algo
			//ESCRIBO COSAS PARA CARGA DE BATERIA
			volt_bateria |= (RX_STD_CAN_BUF.frame->dataByte0) <<8;
			volt_bateria |= RX_STD_CAN_BUF.frame->dataByte1;
			if(volt_bateria == 534){
				BATT_FULL_CHARGE = 1;
				//enviar msj a display para indicar que la bateria esta llena
			}
			break;
		default:
			break;
	}
}

static void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData){
	//LED_BLUE_TOGGLE();
	switch (status){
	/* Process FlexCAN Rx event. */
	case kStatus_FLEXCAN_RxIdle:
		//PRINTF("prueba \n");
		//LED_BLUE_TOGGLE();
		/*if (RX_MESSAGE_EXT_BUFFER_NUM == result)
		{
			//rxComplete = 1;
			LED_GREEN_TOGGLE();
			FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_EXT_CAN_BUF);
		}*/
		if (RX_MESSAGE_STD_BUFFER_NUM == result)
		{
			RX_CAN_COMPLETE = 1;
			//recepcion completada
            FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_STD_CAN_BUF);
			//LED_BLUE_TOGGLE();
		}
		break;
	case kStatus_FLEXCAN_RxOverflow:
		//LED_BLUE_TOGGLE();
		if (RX_MESSAGE_EXT_BUFFER_NUM == result)
		{
			//rxComplete = 1;
		}
		if (RX_MESSAGE_STD_BUFFER_NUM == result)
		{
			RX_CAN_COMPLETE = 1;
			FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_STD_CAN_BUF);
		}
		break;
		/* Process FlexCAN Tx event. */
	case kStatus_FLEXCAN_TxIdle:
		LED_GREEN_TOGGLE();
		if (TX_MESSAGE_BUFFER_NUM == result)
		{
			//txComplete = 1;
		}
		break;
	case kStatus_FLEXCAN_TxBusy:
		//LED_GREEN_TOGGLE();
		if (TX_MESSAGE_BUFFER_NUM == result)
		{
			//retry
		}
		break;

	default:
		//LED_BLUE_ON();
		break;
	}
}

void DecodeRFIDSensor(){
	RFIDData[0].u8[0] = RX_STD_CAN_BUF.frame->dataByte0;
	RFIDData[0].u8[1] = RX_STD_CAN_BUF.frame->dataByte1;
	RFIDData[0].u8[2] = RX_STD_CAN_BUF.frame->dataByte2;
	RFIDData[0].u8[3] = RX_STD_CAN_BUF.frame->dataByte3;
	RFIDData[1].u8[0] = RX_STD_CAN_BUF.frame->dataByte4;
	RFIDData[1].u8[1] = RX_STD_CAN_BUF.frame->dataByte5;
	RFIDData[1].u8[2] = RX_STD_CAN_BUF.frame->dataByte6;
	RFIDData[1].u8[3] = RX_STD_CAN_BUF.frame->dataByte7;

	if (RFIDData[0].u16[0]==DestinationStation[0].u16[0]){
		LED_GREEN_TOGGLE();//LLEGO A DESTINO
	}
	else{
		LED_RED_TOGGLE();//NO LLEGO A DESTINO
	}
}

void DecodeMagneticSensor(){
//cell state byte 1	H		 0000 011 sensores derecha
//cell state byte 2	L		1000 0000 sensores izquierda
// 0000 0110 1000 0000
	magneticSensorBitStatus = ((uint16_t)RX_STD_CAN_BUF.frame->dataByte6 << 8) | RX_STD_CAN_BUF.frame->dataByte7;

	//for (i = 0; i < 8; ++i) {
	//	SensorsStatus.byte =
	//}

	/*sensor0 = magneticSensorBitStatus & (1 << 8);
	sensor1 = magneticSensorBitStatus & (1 << 9);
	sensor2 = magneticSensorBitStatus & (1 << 10);
	sensor3 = magneticSensorBitStatus & (1 << 11);
	// -------------------------- LOW ------------------------- //
	sensor4 = magneticSensorBitStatus & (1 << 0);
	sensor5 = magneticSensorBitStatus & (1 << 1);
	sensor6 = magneticSensorBitStatus & (1 << 2);
	sensor7 = magneticSensorBitStatus & (1 << 3);
	*/
}

void Decode(){
    if(ringRx.iW == ringRx.iR)
        return;

    switch (ringRx.header)
    {
        case 0:
            if (ringRx.buf[ringRx.iR] == 'U')
                ringRx.header++;
            else{
                ringRx.header = 0;
                //ringRx.iR--;
            }
            break;
        case 1:
            if (ringRx.buf[ringRx.iR] == 'N')
                ringRx.header++;
            else{
                ringRx.header = 0;
                //ringRx.iR--;
            }
            break;
        case 2:
            if (ringRx.buf[ringRx.iR] == 'E')
                ringRx.header++;
            else{
                ringRx.header = 0;
                //ringRx.iR--;
            }
            break;
        case 3:
            if (ringRx.buf[ringRx.iR] == 'R'){
                ringRx.header++;

            }
            else{
                ringRx.header = 0;
                //ringRx.iR--;
            }
            break;
        case 4:
            ringRx.nBytes = ringRx.buf[ringRx.iR];
            //auxlenght = ringRx.nBytes;
            ringRx.header++;
            break;
        case 5:
            if (ringRx.buf[ringRx.iR] == 0x00)
                ringRx.header++;
            else{
                ringRx.header = 0;
                //ringRx.iR--;
            }

            break;
        case 6:
            if (ringRx.buf[ringRx.iR] == ':')
            {
                ringRx.cks = 'U'^'N'^'E'^'R'^ringRx.nBytes^0x00^':';
                ringRx.header++;
                ringRx.iData = ringRx.iR+1;
                //LED_RED_TOGGLE();
            }
            else{
                ringRx.header = 0;
                //ringRx.iR--;
            }
            break;

        case 7:
        	if(ringRx.nBytes>1){
				ringRx.cks^=ringRx.buf[ringRx.iR];
			}
			ringRx.nBytes--;
			if(ringRx.nBytes==0){
				ringRx.header=0;
				if(ringRx.cks==ringRx.buf[ringRx.iR]){
					RecibirDatos(ringRx.iData);
				}
			}
            break;
        default:
            ringRx.header = 0;
            break;
    }
    ringRx.iR++;
}

void RecibirDatos(uint8_t head){
	//volatile uint8_t tempARRAY[10];
	LED_RED_TOGGLE();
	switch (ringRx.buf[head++]){
		case 0xD2:
			ALIVE_RECIVE_CMD = 1;
			//algo
		break;
		case ENABLE_MOTOR_CMD:
			ENABLE_RECIVE_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			/*auxbufRX[0]=ringRx.buf[head++];//ID
			auxbufRX[1]=ringRx.buf[head++];
			auxbufRX[2]=ringRx.buf[head++];
			auxbufRX[3]=ringRx.buf[head++];
			auxbufRX[4]=ringRx.buf[head++];
			auxbufRX[5]=ringRx.buf[head++];
			auxbufRX[6]=ringRx.buf[head++];
			auxbufRX[7]=ringRx.buf[head++];
			auxbufRX[8]=ringRx.buf[head++];*/

		break;
		case DISABLE_MOTOR_CMD:
			DISABLE_RECIVE_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			/*auxbufRX[0]=ringRx.buf[head++]; //ID
			auxbufRX[1]=ringRx.buf[head++];
			auxbufRX[2]=ringRx.buf[head++];
			auxbufRX[3]=ringRx.buf[head++];
			auxbufRX[4]=ringRx.buf[head++];
			auxbufRX[5]=ringRx.buf[head++];
			auxbufRX[6]=ringRx.buf[head++];
			auxbufRX[7]=ringRx.buf[head++];
			auxbufRX[8]=ringRx.buf[head++];*/
		break;
	}
}


void UpdateChecksum()
{
    if(ringRx.nBytes > 1)
    {
        ringRx.cks ^= ringRx.buf[ringRx.iR];
    }
}

/*void CheckBytesLeft()
{
    ringRx.nBytes--;
    if(ringRx.nBytes == 0)
    {
        ringRx.header = 0;
    }
}*/

/*void CheckChecksumAndReceiveData()
{
    if(ringRx.cks == ringRx.buf[ringRx.iR])
    {
        RecibirDatos(ringRx.iData);
    }
}*/

void EnviarDatos(uint8_t cmd){
	ringTx.buf[ringTx.iW++]='U';
	ringTx.buf[ringTx.iW++]='N';
	ringTx.buf[ringTx.iW++]='E';
	ringTx.buf[ringTx.iW++]='R';

	switch(cmd){
		case ALIVECMD:
			ringTx.buf[ringTx.iW++] = 0x02;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
		break;
		case TESTCMD:
			ringTx.buf[ringTx.iW++] = 0x02;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
		break;
		case ENABLE_MOTOR_CMD:
			ringTx.buf[ringTx.iW++] = 0x0B;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			for (uint8_t var = 0; var < 9; var++) {
				ringTx.buf[ringTx.iW++] = auxbufRX[var++];
			}
		break;
		case DISABLE_MOTOR_CMD:
			ringTx.buf[ringTx.iW++] = 0x0B;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			for (uint8_t var = 0; var < 9; var++) {
				ringTx.buf[ringTx.iW++] = auxbufRX[var++];
			}
			break;
		default:
		break;
	}

	ringTx.cks=0;
	for(uint8_t i=ringTx.iR; i<ringTx.iW; i++) {
		ringTx.cks^=ringTx.buf[i];
		//pc.printf("%d - %x - %d   v: %d \n",i,cks,cks,tx[i]);
	}
	//if(ringTx.cks>0)
	ringTx.buf[ringTx.iW++]=ringTx.cks;
}

void CreateCANMessage(uint8_t msj){

	switch(msj){
	case 0x01:
		//txFrame.format = kFLEXCAN_FrameFormatExtend;
		//txFrame.id = ID_SEND_BATERIA;

		//uint8_t BATTERY_CHARGE_MESSAGE[] = {0x02, 0x48, 0x01, 0x2C, 0x00, 0x00, 0x00, 0x00};
		ChargeToCANBuf(DATA_EXT, BATTERY_CHARGE_MESSAGE, ID_SEND_BATERIA);

		//memcpy(&txFrame.dataWord0, BATTERY_CHARGE_MESSAGE, 8);
		//memcpy(&txFrame.dataWord1, &BATTERY_CHARGE_MESSAGE2, 8);
		/*txFrame.dataByte0 = 0x02;
		txFrame.dataByte1 = 0x48;
		txFrame.dataByte2 = 0x01;
		txFrame.dataByte3 = 0x2C;
		txFrame.dataByte4 = 0x00;
		txFrame.dataByte5 = 0x00;
		txFrame.dataByte6 = 0x00;
		txFrame.dataByte7 = 0x00;*/
	break;
	case ENABLE_MOTOR_CAN:
		ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
	break;
	case DISABLE_MOTOR_CAN:
		ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
	break;
	}
	FLEXCAN_TransferSendNonBlocking(CAN0, &flexcanHandle, &TX_CAN_BUF);
}

void ChargeToCANBuf(uint8_t whatFormat, uint8_t payloadCAN[], uint32_t id){
	switch(whatFormat){
	case DATA_EXT:
		txFrame.type = kFLEXCAN_FrameTypeData;
		txFrame.format = kFLEXCAN_FrameFormatExtend;
		txFrame.id = id;
		txFrame.dataByte0 = payloadCAN[1];
		txFrame.dataByte1 = payloadCAN[2];
		txFrame.dataByte2 = payloadCAN[3];
		txFrame.dataByte3 = payloadCAN[4];
		txFrame.dataByte4 = payloadCAN[5];
		txFrame.dataByte5 = payloadCAN[6];
		txFrame.dataByte6 = payloadCAN[7];
		txFrame.dataByte7 = payloadCAN[8];
	break;
	case DATA_STD:
	    txFrame.type = kFLEXCAN_FrameTypeData;
		txFrame.format = kFLEXCAN_FrameFormatStandard;
		txFrame.id = id;
		txFrame.dataByte0 = payloadCAN[1];
		txFrame.dataByte1 = payloadCAN[2];
		txFrame.dataByte2 = payloadCAN[3];
		txFrame.dataByte3 = payloadCAN[4];
		txFrame.dataByte4 = payloadCAN[5];
		txFrame.dataByte5 = payloadCAN[6];
		txFrame.dataByte6 = payloadCAN[7];
		txFrame.dataByte7 = payloadCAN[8];
	break;
	case REMOTE_EXT:
	break;
	case REMOTE_STD:
	break;
	}
}

void ActionQT(){
	if(ALIVE_RECIVE_CMD){
		ALIVE_RECIVE_CMD = 0;
	    LED_GREEN_TOGGLE();
	    EnviarDatos(TESTCMD);
	}
	if (ENABLE_RECIVE_CMD) {
		ENABLE_RECIVE_CMD = 0;
		EnviarDatos(ENABLE_MOTOR_CMD);
		CreateCANMessage(ENABLE_MOTOR_CAN);
	}
	if (DISABLE_RECIVE_CMD) {
		DISABLE_RECIVE_CMD = 0;
		EnviarDatos(DISABLE_MOTOR_CMD);
		CreateCANMessage(DISABLE_MOTOR_CAN);
	}
}

/* PIT0_IRQn interrupt handler */
void PIT_CHANNEL_0_IRQHANDLER(void) {
  uint32_t intStatus;
  /* Reading all interrupt flags of status register */
  intStatus = PIT_GetStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_0);
  PIT_ClearStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_0, intStatus);

  /* Place your code here */

  if(timeoutUSB){
	  timeoutUSB--;
      }

  if(timeoutBAT){
	  timeoutBAT--;
  }

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}
