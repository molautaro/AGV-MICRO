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
#define RX_MESSAGE_STD_BUFFER_NUM (0)
#define RX_MESSAGE_EXT_BUFFER_NUM (1)
#define TX_MESSAGE_BUFFER_NUM (2)
#define BATTERY_RECEIVE_ID 0x18FF50E5
#define BATTERY_SEND_ID 0x1806E5F4

/* TODO: insert other definitions and declarations here. */
void EnviarDatos(uint8_t cmd);
void RecibirDatos(uint8_t head);
void Decode();
void CheckChecksumAndReceiveData();
void UpdateChecksum();
void CheckBytesLeft();
void DecodeMagneticSensor();
static void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData);

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
volatile _rx ringRx;
volatile _tx ringTx;
uint8_t timeoutUSB = 4;
uint8_t rxBuf[256], txBuf[256];
uint16_t magneticSensorBitStatus;
volatile _sFlag flag1, SensorsStatus;
volatile _sWork magneticSensorData[8];


#define SENSORSTATUS_0 	SensorsStatus.bit.b0 //Estado Sensor 0
#define SENSORSTATUS_1 	SensorsStatus.bit.b1 //Estado Sensor 1
#define SENSORSTATUS_2 	SensorsStatus.bit.b2 //Estado Sensor 2
#define SENSORSTATUS_3 	SensorsStatus.bit.b3 //Estado Sensor 3
#define SENSORSTATUS_4 	SensorsStatus.bit.b4 //Estado Sensor 4
#define SENSORSTATUS_5 	SensorsStatus.bit.b5 //Estado Sensor 5
#define SENSORSTATUS_6 	SensorsStatus.bit.b6 //Estado Sensor 6
#define SENSORSTATUS_7 	SensorsStatus.bit.b7 //Estado Sensor 7


#define ALIVERECIVE 	flag1.bit.b0 //RECIBIO alive
#define RX_CAN_COMPLETE flag1.bit.b1
#define WWW 			flag1.bit.b2
#define WWWW 			flag1.bit.b3
#define WWWWW 			flag1.bit.b4
#define WWWWWW 			flag1.bit.b5
#define WWWWWWW 		flag1.bit.b6
#define WWWWWWWW 		flag1.bit.b7

flexcan_handle_t rxHandle, txHandle, flexcanHandle;
flexcan_frame_t txFrame, RX_STD_Frame, RX_EXT_Frame;
flexcan_mb_transfer_t TX_CAN_BUF, RX_EXT_CAN_BUF, RX_STD_CAN_BUF;
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

    ringTx.buf=txBuf;
    ringRx.buf=rxBuf;
    ringTx.iW=0;
    ringTx.iR=0;
    ringRx.iW=0;
    ringRx.iR=0;
    LED_RED_INIT(1);
    LED_GREEN_INIT(1);
    LED_BLUE_INIT(1);

    RX_STD_CAN_BUF.mbIdx = RX_MESSAGE_STD_BUFFER_NUM;
    RX_STD_CAN_BUF.frame = &RX_STD_Frame;
    RX_EXT_CAN_BUF.mbIdx = RX_MESSAGE_STD_BUFFER_NUM;
    RX_EXT_CAN_BUF.frame = &RX_EXT_Frame;
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
    while(1) {
    	USB_DeviceInterface0CicVcomTask();
    	//if(FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_STD_CAN_BUF) == kStatus_Success)
    	//LED_BLUE_ON();
    	//if(FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_EXT_CAN_BUF) == kStatus_Success)


    	if(!timeoutUSB){
    	    EnviarDatos(ALIVECMD);
    	    timeoutUSB = 4;
    	}

    	Decode();


    	if(ALIVERECIVE){
    		ALIVERECIVE = 0;
    		LED_GREEN_TOGGLE();
    		EnviarDatos(TESTCMD);
    	}

    	if(RX_CAN_COMPLETE){
    		// iria funcion para decodificar supongo
    		if(RX_STD_CAN_BUF.frame->format == kFLEXCAN_FrameFormatExtend){
    			LED_GREEN_TOGGLE();
    		}
    		if(RX_STD_CAN_BUF.frame->format == kFLEXCAN_FrameFormatStandard){
    			LED_RED_TOGGLE();
    		}
    		//RX_STD_CAN_BUF.frame->id
    		if(RX_STD_CAN_BUF.frame->id == ID_SENSOR_MAGNETICO){
    			//ESCRIBO COSAS PARA SENSOR MAGNETICO
    		}
    		DecodeMagneticSensor();
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

static void flexcan_callback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData){
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
		//LED_GREEN_TOGGLE();
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
                ringRx.iR--;
            }
            break;
        case 1:
            if (ringRx.buf[ringRx.iR] == 'N')
                ringRx.header++;
            else{
                ringRx.header = 0;
                ringRx.iR--;
            }
            break;
        case 2:
            if (ringRx.buf[ringRx.iR] == 'E')
                ringRx.header++;
            else{
                ringRx.header = 0;
                ringRx.iR--;
            }
            break;
        case 3:
            if (ringRx.buf[ringRx.iR] == 'R'){
                ringRx.header++;

            }
            else{
                ringRx.header = 0;
                ringRx.iR--;
            }
            break;
        case 4:
            ringRx.nBytes = ringRx.buf[ringRx.iR];
            ringRx.header++;
            break;
        case 5:
            if (ringRx.buf[ringRx.iR] == 0x00)
                ringRx.header++;
            else{
                ringRx.header = 0;
                ringRx.iR--;
            }
            break;
        case 6:
            if (ringRx.buf[ringRx.iR] == ':')
            {
                ringRx.cks= 'U'^'N'^'E'^'R'^ringRx.nBytes^0x00^':';
                ringRx.header++;
                ringRx.iData = ringRx.iR+1;
                LED_RED_TOGGLE();
            }
            else{
                ringRx.header = 0;
                ringRx.iR--;
            }
            break;

        case 7:
        	UpdateChecksum();
        	CheckBytesLeft();
        	if(ringRx.nBytes == 0)
        	{
        	CheckChecksumAndReceiveData();
        	}
            break;
        default:
            ringRx.header = 0;
            break;
    }
    ringRx.iR++;
}

void RecibirDatos(uint8_t head){
	switch (ringRx.buf[head++]){
		case 0xD2:
			ALIVERECIVE = 1;
			//algo
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

void CheckBytesLeft()
{
    ringRx.nBytes--;
    if(ringRx.nBytes == 0)
    {
        ringRx.header = 0;
    }
}

void CheckChecksumAndReceiveData()
{
    if(ringRx.cks == ringRx.buf[ringRx.iR])
    {
        RecibirDatos(ringRx.iData);
    }
}

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
		default:
		break;
	}

	ringTx.cks=0;
	for(uint8_t i=ringTx.iR; i<ringTx.iW; i++) {
		ringTx.cks^=ringTx.buf[i];
		//pc.printf("%d - %x - %d   v: %d \n",i,cks,cks,tx[i]);
	}
	if(ringTx.cks>0)
		ringTx.buf[ringTx.iW++]=ringTx.cks;
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

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}
