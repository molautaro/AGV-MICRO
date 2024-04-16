#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK64F12.h"
#include "fsl_debug_console.h"
#include "Util.h"
#include "usb_device_interface_0_cic_vcom.h"
#include <math.h>


/* insert other include files here. */

#define INITESPCMD 0xC0 //Inicializar ESP
#define MOTORSCMD 0xD0 //comando motor
#define TESTCMD 0xD2 // COMANDO ON/OFF MOTOR
#define ALIVECMD 0xF0 // comando alive
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
#define ID_REC_MOTOR_SPEED FLEXCAN_ID_STD(0x587)
#define ID_REC_MOTOR_DIRECTION FLEXCAN_ID_STD(0x581)


//Comandos recibidos de QT
#define SPEED_MODE_CMD 0xA3
#define POSITION_MODE_CMD 0xA1
#define ENABLE_MOTOR_CMD 0x01 //COMANDO ENABLE MOTOR
#define DISABLE_MOTOR_CMD 0x02 // COMANDO DISABLE MOTOR
#define INVERTIR_1_CMD 0x03
#define INVERTIR_2_CMD 0x04
#define READY_POSI_CMD 0x05
#define SPEED_MOTOR_CMD 0x06
#define POS_MOTOR_CMD 0x07
#define MANUAL_CMD 0xA5
#define ACC_SPEED_CMD 0xA6
#define DEC_SPEED_CMD 0xA7

const uint8_t CARGA_BAT_MESSAGE[] = {0x02, 0x48, 0x01, 0x2C, 0x00, 0x00, 0x00, 0x00};
const uint8_t SPEED_MODE_MESSAGE[] = {0x07,0x2F,0x60,0x60,0x00,0x03,0x00,0x00,0x00};
const uint8_t POS_MODE_MESSAGE[] = {0x01,0x2F,0x60,0x60,0x00,0x01,0x00,0x00,0x00};
const uint8_t ENABLE_MVEL[] = {0x07,0x2B,0x40,0x60,0x00,0x0F,0x00,0x00,0x00};
const uint8_t ENABLE_MPOS[] = {0x01,0x2B,0x40,0x60,0x00,0x0F,0x00,0x00,0x00};
const uint8_t DISABLE_MVEL[] = {0x07,0x2B,0x40,0x60,0x00,0x06,0x00,0x00,0x00};
const uint8_t DISABLE_MPOS[] = {0x01,0x2B,0x40,0x60,0x00,0x06,0x00,0x00,0x00};
const uint8_t READY_POS[] = {0x01,0x2B,0x40,0x60,0x00,0x3F,0x10,0x00,0x00};
const uint8_t INVERTIR_1[] = {0x07,0x2F,0x60,0x7E,0x00,0x01,0x00,0x00,0x00};
const uint8_t INVERTIR_2[] = {0x07,0x2F,0x60,0x7E,0x00,0x00,0x00,0x00,0x00};

//const uint8_t TARGET_SPEED[] = {0x07,0x23,0xFF,0x60,0x00,X,X,X,X};
//const uint8_t TARGET_POS[] = {0x01,0x23,0x7A,0x60,0x00,X,X,X,X};

const uint8_t ACC_SPEED_10[] = {0x07,0x23,0x83,0x60,0x00,0x66,0x06,0x00,0x00};//10 rpm/s
const uint8_t ACC_SPEED_100[] = {0x07,0x23,0x83,0x60,0x00,0x00,0x40,0x00,0x00};//100 rpm/s
const uint8_t ACC_SPEED_150[] = {0x07,0x23,0x83,0x60,0x00,0x00,0x60,0x00,0x00};//150 rpm/s

const uint8_t DESA_SPEED_10[] = {0x07,0x23,0x84,0x60,0x00,0x66,0x06,0x00,0x00};//10 rpm/s
const uint8_t DESA_SPEED_100[] = {0x07,0x23,0x84,0x60,0x00,0x00,0x40,0x00,0x00};//100 rpm/s
const uint8_t DESA_SPEED_150[] = {0x07,0x23,0x84,0x60,0x00,0x00,0x60,0x00,0x00};//150 rpm/s



#define MSJ_BAT 0x01
// para funcion ChargetoCANbuf(); whatFormat
#define DATA_STD 0x0A
#define REMOTE_STD 0x1A
#define DATA_EXT 0x2B
#define REMOTE_EXT 0x3B

//para CreateCANMessage(msj);
//#define ENABLE_MOTOR_CAN 0x02
//#define DISABLE_MOTOR_CAN 0x03


/* insert other definitions and declarations here. */
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
void workingmode();
float speedControlCalc(float d, int Vmax);
uint32_t speedConvertionRPMtoDEC(float rpmSpeed);
void SpeedMotorControl();
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

uint8_t cmdaux222 = 0;

volatile uint32_t timerCounter;
volatile _rx ringRx, auxRX;
volatile _tx ringTx, auxTX;
uint16_t timeoutUSB = 200;
uint16_t timeoutBAT = 100;
uint8_t rxBuf[256], txBuf[256], auxbufRX[256],auxbufTX[256], auxlenght;
uint8_t operationMode = 0, init_comp = 0, timeoutINIT = 0;
//uint8_t flag_carga_completa = 0;
//uint8_t msj_CAN_BAT[8] = {0x02, 0x48, 0x01, 0x2C, 0x00, 0x00, 0x00, 0x00};
//uint8_t msj_CAN_BAT2[8] = {0x00, 0x00, 0x00, 0x00, 0x0A, 0x0B, 0x0C, 0x0D};
uint16_t magneticSensorBitStatus;
uint16_t volt_bateria = 0;
volatile _sFlag flag1, SensorsStatus, flagQT, flagQT_2;
volatile _sWork RFIDData[2],DestinationStation[2],Distance_Sensor_SIMULATION;
volatile _sWork SpeedMotorCalcRPM, SpeedMotorCalcRPMAUX,SpeedMotorCalcDEC;

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
#define INIT_COMPLETE 			flag1.bit.b3
#define SELECT_MANUAL 			flag1.bit.b4
#define SELECT_AUTO 			flag1.bit.b5
#define FIRST_INIT 		flag1.bit.b6
#define READY_RECIVE 		flag1.bit.b7

//PARA COMANDOS RECIBIDOS POR QT
#define ALIVE_RECIVE_CMD 	 flagQT.bit.b0 //RECIBIO alive
#define ENABLE_RECIVE_CMD 	 flagQT.bit.b1
#define DISABLE_RECIVE_CMD	 flagQT.bit.b2
#define INVERTIR1_RECIVE_CMD flagQT.bit.b3
#define INVERTIR2_RECIVE_CMD flagQT.bit.b4
#define SPEED_MODE_REC_CMD		 flagQT.bit.b5
#define POSITION_MODE_REC_CMD	 flagQT.bit.b6
#define READY_POS_REC_CMD	     flagQT.bit.b7

#define TARGET_SPEED_REC_CMD   	flagQT_2.bit.b0
#define TARGET_POS_REC_CMD     	flagQT_2.bit.b1
#define MANUAL_CMD_REC  		flagQT_2.bit.b2
#define AAAAA   			flagQT_2.bit.b3
#define AAAAAA   			flagQT_2.bit.b4
#define AAAAAAA   			flagQT_2.bit.b5
#define AAAAAAAA  			flagQT_2.bit.b6
#define AAAAAAAAA   		flagQT_2.bit.b7

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
	FIRST_INIT=0;
	READY_RECIVE=1;

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

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();


#endif

    PRINTF("Hello World\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    DestinationStation[0].u8[0]= 0x45;
	DestinationStation[0].u8[1]= 0x31;
	Distance_Sensor_SIMULATION.f = 2.0;
	SpeedMotorCalcRPM.f = 0.0;
	SpeedMotorCalcRPMAUX.f = 0.0;
	//operationMode = 1;
    while(1) {
    	USB_DeviceTasks();
    	//USB_DeviceCdcAcmSend(handle, ep, buffer, length)
    	Decode();
    	workingmode();






    	if(!timeoutUSB){
			EnviarDatos(ALIVECMD);
			timeoutUSB = 400;
		}
    	if(flagQT.byte || flagQT_2.byte){ //maquina de estado para comandos recibidos de QT
			ActionQT();
		}
    	if(RX_CAN_COMPLETE){//recibe exitosamente un msj por CAN
			DecodeCANMessage();//llamo a funcion de decodificar mensaje
			RX_CAN_COMPLETE = 0;
		}
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}

void workingmode(){
	//LED_BLUE_TOGGLE();
	switch(operationMode){
	case 0://MODO INICIALIZACION
		//init_status = 0;
		if(!timeoutINIT || READY_RECIVE){
			switch(init_comp){
			case 0:
				LED_RED_TOGGLE();
				for (uint8_t var = 0; var < 9; var++) {
					auxbufRX[var]=SPEED_MODE_MESSAGE[var];
				}
				CreateCANMessage(SPEED_MODE_CMD);
			break;
			case 1:
				LED_RED_TOGGLE();
				for (uint8_t var = 0; var < 9; var++) {
					auxbufRX[var]=ACC_SPEED_10[var];
				}
				CreateCANMessage(ACC_SPEED_CMD);
			break;
			case 2:
				for (uint8_t var = 0; var < 9; var++) {
					auxbufRX[var]=DESA_SPEED_10[var];
				}
				CreateCANMessage(DEC_SPEED_CMD);
			break;
			}
			timeoutINIT = 50;
		}
//		if(!FIRST_INIT){
//					for (uint8_t var = 0; var < 9; ++var) {
//						auxbufRX[var]=SPEED_MODE_MESSAGE[var];
//					}
//					CreateCANMessage(SPEED_MODE_CMD);
//		}
//		else{
//			for (uint8_t var = 0; var < 9; ++var) {
//					auxbufRX[var]=POS_MODE_MESSAGE[var];
//				}
//				//CreateCANMessage(POSITION_MODE_CMD);
//		}
//		if(){
//
//		}
		//ChargeToCANBuf(DATA_STD, auxbufRX, MOTORS);
		/* poner aqui lo que iria en el buffer*/
	break;
	case 1://MODO MANUAL
		SpeedMotorControl();
		//SteeringMotorControl();
	break;
	case 2:
	break;
	}
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
		case ID_REC_MOTOR_SPEED:
			READY_RECIVE = 1;
			LED_GREEN_TOGGLE();
			if(operationMode==0){
				init_comp++;
				if (init_comp == 3)
					operationMode = 1;
			}
		break;
		case ID_REC_MOTOR_DIRECTION:
			READY_RECIVE = 1;
			if(operationMode==0){
				//FIRST_INIT=0;
				operationMode=1;
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
            if (ringRx.buf[ringRx.iR] == 'U'){
                ringRx.header++;
                ringRx.timeout = 50;
            }
            else{
                ringRx.header = 0;
                ringRx.timeout = 0;
                //ringRx.iR--;
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
            //auxlenght = ringRx.nBytes;
            ringRx.header++;
            break;
        case 5:
            ringRx.header++;

//            if (ringRx.buf[ringRx.iR] == 0x00)
//                ringRx.header++;
//            else{
//                ringRx.header = 0;
//                //ringRx.iR--;
//            }
//
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
                ringRx.iR--;
            }
            break;

        case 7:
        	if(ringRx.nBytes>1){
				ringRx.cks^=ringRx.buf[ringRx.iR];
			}
			ringRx.nBytes--;
			if(ringRx.nBytes==0){
				ringRx.header=0;
				ringRx.timeout = 0;
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
//	cmdaux222 = ringRx.buf[head];
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

		break;
		case DISABLE_MOTOR_CMD:
			DISABLE_RECIVE_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
		break;
		case SPEED_MODE_CMD:
			SPEED_MODE_REC_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
		break;
		case POSITION_MODE_CMD:
			POSITION_MODE_REC_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
		break;
		case READY_POSI_CMD:
			READY_POS_REC_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
		break;
		case SPEED_MOTOR_CMD:
			TARGET_SPEED_REC_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
		break;
		case POS_MOTOR_CMD:
			TARGET_POS_REC_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
		break;
		case INVERTIR_1_CMD:
			INVERTIR1_RECIVE_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
		break;
		case INVERTIR_2_CMD:
			INVERTIR2_RECIVE_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
		break;
		case 0xAF:
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			Distance_Sensor_SIMULATION.u8[0]=auxbufRX[5];
			Distance_Sensor_SIMULATION.u8[1]=auxbufRX[6];
			Distance_Sensor_SIMULATION.u8[2]=auxbufRX[7];
			Distance_Sensor_SIMULATION.u8[3]=auxbufRX[8];
		break;
		case 0xBF:
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			magneticSensorBitStatus = ((uint16_t)auxbufRX[7] << 8) | auxbufRX[8];
		break;
		default:
			//LED_RED_TOGGLE();
		break;
	}
	LED_RED_TOGGLE();
	EnviarDatos(TESTCMD);
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
				ringTx.buf[ringTx.iW++] = auxbufRX[var];
			}
		break;
		case DISABLE_MOTOR_CMD:
			ringTx.buf[ringTx.iW++] = 0x0B;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			for (uint8_t var = 0; var < 9; var++) {
				ringTx.buf[ringTx.iW++] = auxbufRX[var];
			}
		break;
		case SPEED_MODE_CMD:
			ringTx.buf[ringTx.iW++] = 0x0B;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			for (uint8_t var = 0; var < 9; var++) {
				ringTx.buf[ringTx.iW++] = auxbufRX[var];
			}
		break;
		case SPEED_MOTOR_CMD:
			ringTx.buf[ringTx.iW++] = 0x0B;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			for (uint8_t var = 0; var < 9; var++) {
				ringTx.buf[ringTx.iW++] = auxbufRX[var];
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

	if(READY_RECIVE){
	switch(msj){
		case ENABLE_MOTOR_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case DISABLE_MOTOR_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case SPEED_MODE_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case POSITION_MODE_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case READY_POSI_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case SPEED_MOTOR_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case POS_MOTOR_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case INVERTIR_1_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case INVERTIR_2_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case ACC_SPEED_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case DEC_SPEED_CMD:
			ChargeToCANBuf(DATA_STD, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
	}
	FLEXCAN_TransferSendNonBlocking(CAN0, &flexcanHandle, &TX_CAN_BUF);
	READY_RECIVE=0;
	}
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
	    //LED_GREEN_TOGGLE();
	    EnviarDatos(TESTCMD);
	    return;
	}
	if (ENABLE_RECIVE_CMD) {
		ENABLE_RECIVE_CMD = 0;
		LED_BLUE_TOGGLE();
		EnviarDatos(ENABLE_MOTOR_CMD);
		CreateCANMessage(ENABLE_MOTOR_CMD);
		return;
	}
	if (DISABLE_RECIVE_CMD) {
		DISABLE_RECIVE_CMD = 0;
		//LED_RED_TOGGLE();
		EnviarDatos(DISABLE_MOTOR_CMD);
		CreateCANMessage(DISABLE_MOTOR_CMD);
		return;
	}
	if (SPEED_MODE_REC_CMD) {
		SPEED_MODE_REC_CMD = 0;
		LED_GREEN_TOGGLE();
		//EnviarDatos(SPEED_MODE_CMD);
		CreateCANMessage(SPEED_MODE_CMD);
		return;
	}
	if (POSITION_MODE_REC_CMD) {
		POSITION_MODE_REC_CMD = 0;
		LED_BLUE_TOGGLE();
		//EnviarDatos(ENABLE_MOTOR_CMD);
		CreateCANMessage(POSITION_MODE_CMD);
		return;
	}
	if (TARGET_SPEED_REC_CMD) {
		TARGET_SPEED_REC_CMD = 0;
		//LED_BLUE_TOGGLE();
		EnviarDatos(SPEED_MOTOR_CMD);
		CreateCANMessage(SPEED_MOTOR_CMD);
		return;
	}
	if (TARGET_POS_REC_CMD) {
		TARGET_POS_REC_CMD = 0;
		//LED_BLUE_TOGGLE();
		//EnviarDatos(ENABLE_MOTOR_CMD);
		CreateCANMessage(POS_MOTOR_CMD);
		return;
	}
	if (READY_POS_REC_CMD) {
		READY_POS_REC_CMD = 0;
		//LED_BLUE_TOGGLE();
		//EnviarDatos(ENABLE_MOTOR_CMD);
		CreateCANMessage(READY_POSI_CMD);
		return;
	}
	if (INVERTIR1_RECIVE_CMD) {
		INVERTIR1_RECIVE_CMD = 0;
		//LED_BLUE_TOGGLE();
		//EnviarDatos(ENABLE_MOTOR_CMD);
		CreateCANMessage(INVERTIR_1_CMD);
		return;
	}
	if (INVERTIR2_RECIVE_CMD) {
		INVERTIR2_RECIVE_CMD = 0;
		//LED_BLUE_TOGGLE();
		//EnviarDatos(ENABLE_MOTOR_CMD);
		CreateCANMessage(INVERTIR_2_CMD);
		return;
	}
}

void SpeedMotorControl(){
	SpeedMotorCalcRPMAUX.f = speedControlCalc(Distance_Sensor_SIMULATION.f, 3000);
	if(fabs(SpeedMotorCalcRPM.f-SpeedMotorCalcRPMAUX.f) >= 50){
		//cambiar velocidad porque el cambio es de mas de 50rpm
		SpeedMotorCalcRPM.f = SpeedMotorCalcRPMAUX.f;
		SpeedMotorCalcDEC.u32 = speedConvertionRPMtoDEC(SpeedMotorCalcRPM.f);
		auxbufRX[0] = 0x07; //id motor
		auxbufRX[1] = 0x23;
		auxbufRX[2] = 0xFF;
		auxbufRX[3] = 0x60;
		auxbufRX[4] = 0x00;
		auxbufRX[5] = SpeedMotorCalcDEC.u8[0];
		auxbufRX[6] = SpeedMotorCalcDEC.u8[1];
		auxbufRX[7] = SpeedMotorCalcDEC.u8[2];
		auxbufRX[8] = SpeedMotorCalcDEC.u8[3];
		CreateCANMessage(SPEED_MOTOR_CMD);
		//READY_RECIVE=1;
	}
}

uint32_t speedConvertionRPMtoDEC(float rpmSpeed){
	float vel_aux = 0;
	vel_aux = ((rpmSpeed * 512) * (10000.0/1875));
	return (uint32_t)vel_aux;
}

float speedControlCalc(float d, int Vmax){
	LED_BLUE_TOGGLE();
	if (d > 8) {
		return Vmax+0.0;
	} else if (d >= 4 && d <= 8) {
		return (Vmax) - ((Vmax / 2) * ((8 - d) / 4.0));
	} else if (d >= 1.5 && d < 4) {
		return ((Vmax / 2) - ((Vmax/3) * ((4-d)/2.5)));
	} else {
		return 0.0;
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


  if(timeoutINIT){
	  timeoutINIT--;
   }

  if(ringRx.timeout){
	  ringRx.timeout--;
	  if(!ringRx.timeout)
		  ringRx.header = 0;
  }
  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}


