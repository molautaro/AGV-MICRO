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


/* TODO: insert other definitions and declarations here. */
void EnviarDatos(uint8_t cmd);
void RecibirDatos(uint8_t head);
void Decode();
void CheckChecksumAndReceiveData();
void UpdateChecksum();
void CheckBytesLeft();
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
volatile _sFlag flag1;
#define ALIVERECIVE flag1.bit.b0 //RECIBIO alive

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();

    ringTx.buf=txBuf;
    ringRx.buf=rxBuf;
    ringTx.iW=0;
    ringTx.iR=0;
    ringRx.iW=0;
    ringRx.iR=0;
    LED_RED_INIT(1);
#endif

    PRINTF("Hello World\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
    	USB_DeviceInterface0CicVcomTask();

    	if(!timeoutUSB){
    	    EnviarDatos(ALIVECMD);
    	    timeoutUSB = 4;
    	}

    	Decode();


    	if(ALIVERECIVE){
    		//
    		ALIVERECIVE = 0;
    		EnviarDatos(TESTCMD);
    	}

        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
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
