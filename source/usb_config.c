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

#define ID_M_DIREC 								FLEXCAN_ID_STD(0x601) //ID CAN MOTOR DIRECCION
#define ID_M_VEL 								FLEXCAN_ID_STD(0x607) //ID CAN MOTOR VELOCIDAD O TRACCION
#define RX_MESSAGE_STD_BUFFER_NUM 				(0)
#define RX_MESSAGE_EXT_BUFFER_NUM 				(1)
#define TX_MESSAGE_BUFFER_NUM					(2)
#define NEW_TX_MESSAGE_BUFFER_NUM 				(3)
//#define NEW_TX_MESSAGE_BUFFER_NUM 				(4)
#define ID_RFID_SENSOR 							FLEXCAN_ID_STD(0x100) //ID SENSOR RFID
#define ID_MAGNETIC_SENSOR 						FLEXCAN_ID_STD(0x123) //ID SENSOR MAGNETICO
#define ID_SEND_BATERIA 						FLEXCAN_ID_EXT(0x1806E5F4)//ID de msjs enviados desde placa a la bateria
#define ID_REC_BATERIA 							FLEXCAN_ID_EXT(0x18FF50E5)//ID de msjs que recibe la placa desde la bateria
#define ID_REC_MOTOR_SPEED 						FLEXCAN_ID_STD(0x587) //ID RECEPCION SDO MOTOR VELOCIDAD
#define ID_REC_MOTOR_DIRECTION 					FLEXCAN_ID_STD(0x581) //ID RECEPCION SDO MOTOR DIRECCION
#define ID_MOTOR_SPEED_ALIVE 					FLEXCAN_ID_STD(0x707) //ID ALIVE MOTOR VELOCIDAD
#define ID_MOTOR_DIRECTION_ALIVE 				FLEXCAN_ID_STD(0x701) //ID ALIVE MOTOR DIRECCION
#define ID_MOTOR_SPEED_TPDO1					FLEXCAN_ID_STD(0x187) //ID TPDO1 MOTOR VELOCIDAD
#define ID_MOTOR_DIR_TPDO1						FLEXCAN_ID_STD(0x181) //ID TPDO1 MOTOR DIRECCION
#define ID_MOTOR_SPEED_RPDO1					FLEXCAN_ID_STD(0x207) //ID RPDO1 MOTOR VELOCIDAD
#define ID_MOTOR_DIR_RPDO1						FLEXCAN_ID_STD(0x201) //ID RPDO1 MOTOR DIRECCION
#define ID_MOTOR_SPEED_RPDO2					FLEXCAN_ID_STD(0x307) //ID RPDO2 MOTOR VELOCIDAD



//Comandos recibidos de QT

//#define INITESPCMD 								0xC0 //Inicializar ESP
#define MOTORSCMD 								0xD0 //comando motor
#define TESTCMD 								0xD2 // COMANDO ON/OFF MOTOR
#define ALIVECMD 								0x0F // comando alive

#define ENABLE_MOTOR_CMD 						0x01 //COMANDO ENABLE MOTOR
#define DISABLE_MOTOR_CMD 						0x02 // COMANDO DISABLE MOTOR
#define INVERTIR_1_CMD 							0x03
#define INVERTIR_2_CMD 							0x04
#define READY_POSI_CMD 							0x05
#define SPEED_MOTOR_CMD 						0x06
#define POS_MOTOR_CMD 							0x07
#define POSITION_MODE_CMD 						0xA1
#define SPEED_MODE_CMD 							0xA3
#define MANUAL_CMD 								0xA5
#define ACC_SPEED_CMD 							0xA6
#define DEC_SPEED_CMD 							0xA7
#define SPEED_POS_CMD 							0xA8
#define DISTANCE_SENSOR_CMD						0xA9
#define DISTANCE_SENSOR_SIM_CMD					0xAF
#define FAULT_CMD								0xF0
#define RESET_PROGRAM							0xA9
#define MOTOR_SPEED_DATA1_CMD					0xB0
#define MOTOR_SPEED_DATA2_CMD					0xB1
#define MOTOR_DIR_DATA1_CMD						0xB2
#define MOTOR_DIR_DATA2_CMD						0xB3
//#define FAULT_CMD								0xB4
#define PID_PARAMETERS_CMD						0xC0
#define MAG_SENSOR_SIM_CMD						0xC1
#define START_PDO_SPEED_CMD						0xD0
#define START_PDO_DIR_CMD						0xD1
#define HMI_ALIVE_CMD							0xD2
#define RESET_PROGRAM_CMD						0xD3
#define DESTINATIONSTATION_CMD					0xD4
#define DESTINOALCANZADO_CMD					0xD5
#define ORIGENALCANZADO_CMD						0xD6
#define CHANGE_CONTROL_CMD						0xD8
#define OUT_OF_LINE_CMD							0xD9
#define BATTERY_MODE_CMD						0xDA
#define BATTERY_LEVEL_CMD						0xDB
#define BATTERY_CONNECTION_CMD					0xDC

#define INIT_MODE 								0x00
#define MANUAL_MODE								0x01
#define AUTOMATIC_MODE							0x02
#define BRAKE_MODE								0x03
#define CHARGE_BATTERY_MODE						0x04

const uint8_t CARGA_BAT_MESSAGE[] = 	{0x00,0x02,0x48,0x01,0x2C,0x00,0x00,0x00,0x00};
const uint8_t SPEED_MODE_MESSAGE[] = 	{0x07,0x2F,0x60,0x60,0x00,0x03,0x00,0x00,0x00};
const uint8_t POS_MODE_MESSAGE[] = 		{0x01,0x2F,0x60,0x60,0x00,0x01,0x00,0x00,0x00};
const uint8_t ENABLE_MVEL[] = 			{0x07,0x2B,0x40,0x60,0x00,0x0F,0x00,0x00,0x00};
const uint8_t ENABLE_MPOS[] = 			{0x01,0x2B,0x40,0x60,0x00,0x0F,0x00,0x00,0x00};
const uint8_t DISABLE_MVEL[] = 			{0x07,0x2B,0x40,0x60,0x00,0x06,0x00,0x00,0x00};
const uint8_t DISABLE_MPOS[] = 			{0x01,0x2B,0x40,0x60,0x00,0x06,0x00,0x00,0x00};
const uint8_t READY_POS[] = 			{0x01,0x2B,0x40,0x60,0x00,0x3F,0x10,0x00,0x00};
const uint8_t INVERTIR_1[] = 			{0x07,0x2F,0x60,0x7E,0x00,0x01,0x00,0x00,0x00};
const uint8_t INVERTIR_2[] = 			{0x07,0x2F,0x60,0x7E,0x00,0x00,0x00,0x00,0x00};

const uint8_t START_PDO_SPEED_MESSAGE[] = 			{0x00,0x01,0x07};
const uint8_t START_PDO_DIR_MESSAGE[] = 			{0x00,0x01,0x01};

//const uint8_t TARGET_SPEED[] = {0x07,0x23,0xFF,0x60,0x00,X,X,X,X};
//const uint8_t TARGET_POS[] = {0x01,0x23,0x7A,0x60,0x00,X,X,X,X};

const uint8_t ACC_SPEED_10[] = {0x07,0x23,0x83,0x60,0x00,0x66,0x06,0x00,0x00};//10 rpm/s
const uint8_t ACC_SPEED_100[] = {0x07,0x23,0x83,0x60,0x00,0x00,0x40,0x00,0x00};//100 rpm/s
const uint8_t ACC_SPEED_150[] = {0x07,0x23,0x83,0x60,0x00,0x00,0x60,0x00,0x00};//150 rpm/s

const uint8_t DESA_SPEED_10[] = {0x07,0x23,0x84,0x60,0x00,0x66,0x06,0x00,0x00};//10 rpm/s
const uint8_t DESA_SPEED_100[] = {0x07,0x23,0x84,0x60,0x00,0x00,0x40,0x00,0x00};//100 rpm/s
const uint8_t DESA_SPEED_150[] = {0x07,0x23,0x84,0x60,0x00,0x00,0x60,0x00,0x00};//150 rpm/s

const uint8_t POS_SPEED[] = {0x01,0x23,0x81,0x60,0x00,0xBC,0xBB,0xBB,0x03};//3000 rpm

#define MSJ_BAT 0x01
// para funcion ChargetoCANbuf(); whatFormat
#define DATA_STD 0x0A
#define REMOTE_STD 0x1A
#define DATA_EXT 0x2B
#define REMOTE_EXT 0x3B
#define PDO 0x4B

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
//void ChargeToCANBuf(uint8_t whatFormat, uint8_t payloadCAN[], uint32_t id);
void ChargeToCanBUF(uint8_t format, flexcan_mb_transfer_t *CANTransfer, uint8_t *data, uint32_t id);
void workingmode();
float speedControlCalc(float d, int Vmax);
uint32_t speedConvertionRPMtoDEC(float rpmSpeed);
void SpeedMotorControl();
void SteeringMotorControl();
int32_t positionConvert(int32_t pos_degree);
void PositionMotorControl();
void BrakeControl();
void checkDeviceConnection();
void CheckFaults();
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

uint16_t timeoutMotorVel = 200, timeoutMotorDir = 200, timeoutMagSensor = 200, timeoutRFIDSensor = 200,timeoutHMI = 200;
uint16_t timeoutUSB = 200, timeoutMOTOR_DATA_QT = 500, timeoutCAN_MESSAGE=5, timeoutPDO=0, timeoutMOTORS = 2;
uint16_t timeoutBAT = 100, timeoutDIREC = 250, timeoutCONNETIONBAT=3000;
uint16_t magneticSensorBitStatus;
//uint16_t volt_bateria = 0;

uint8_t rxBuf[256], txBuf[256], auxbufRX[256],auxbufTX[256], auxlenght, cont_perd = 0;
uint8_t operationMode = 0, init_comp = 0, timeoutINIT = 0,timeoutBRAKE=0, brakestatus=0;

uint16_t COORD_SENSORES[8];
const int SENS_MODEL_EXAC[] = {-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7};
uint8_t pos_lin = 0;
float error=0, lastError=0, integral=0, derivativo=0, turn=0;
int32_t direccion=0;

//uint8_t flag_carga_completa = 0;
//uint8_t msj_CAN_BAT[8] = {0x02, 0x48, 0x01, 0x2C, 0x00, 0x00, 0x00, 0x00};
//uint8_t msj_CAN_BAT2[8] = {0x00, 0x00, 0x00, 0x00, 0x0A, 0x0B, 0x0C, 0x0D};

volatile _sFlag flag1, flag2, flag3,flagFaults,SensorsStatus, flagQT, flagQT_2;
volatile _sWork RFIDData[2],DestinationStation[2],Distance_Sensor_SIMULATION,Distance_Sensor_REAL;
volatile _sWork SpeedMotorCalcRPM, SpeedMotorCalcRPMAUX,SpeedMotorCalcDEC;
volatile _sWork PosSend;
volatile _sWork volt_bateria, cor_bateria;
_sWork Kp_SteeringMotor, Kd_SteeringMotor, Ki_SteeringMotor;
_sWork RealSpeedVEL,StatusWordVEL,RealCurrentVEL; //Variables para almacenar datos enviados del motor velocidad por TPDO1
_sWork RealPositionDIR,StatusWordDIR,RealCurrentDIR; //Variables para almacenar datos enviados del motor direccion por TPDO1


#define SENSORSTATUS_0 						SensorsStatus.bit.b0 //Estado Sensor 0
#define SENSORSTATUS_1 						SensorsStatus.bit.b1 //Estado Sensor 1
#define SENSORSTATUS_2 						SensorsStatus.bit.b2 //Estado Sensor 2
#define SENSORSTATUS_3 						SensorsStatus.bit.b3 //Estado Sensor 3
#define SENSORSTATUS_4 						SensorsStatus.bit.b4 //Estado Sensor 4
#define SENSORSTATUS_5 						SensorsStatus.bit.b5 //Estado Sensor 5
#define SENSORSTATUS_6 						SensorsStatus.bit.b6 //Estado Sensor 6
#define SENSORSTATUS_7 						SensorsStatus.bit.b7 //Estado Sensor 7


#define w 									flag1.bit.b0 //
#define RX_CAN_COMPLETE 					flag1.bit.b1
#define BATT_FULL_CHARGE 					flag1.bit.b2
#define INIT_COMPLETE 						flag1.bit.b3
#define SELECT_MANUAL 						flag1.bit.b4
#define SELECT_AUTO 						flag1.bit.b5
#define FIRST_INIT 							flag1.bit.b6
#define READY_RECIVE 						flag1.bit.b7

#define MOTOR_VEL_CONNECTED 				flag2.bit.b0 //Motor velocidad conectado
#define MOTOR_DIR_CONNECTED					flag2.bit.b1 //Motor direccion conectado
#define MAG_SENSOR_CONNECTED				flag2.bit.b2 //Sensor magnetico conectado
#define RFID_SENSOR_CONNECTED				flag2.bit.b3 //Sensor RFID Conectado
#define HMI_CONNECTED						flag2.bit.b4 //HMI Conectado
#define datos_PDO_SPEED 					flag2.bit.b5 //PDO motor velocidad activado
#define datos_PDO_DIR						flag2.bit.b6 //PDO motor direccion activado
#define ALL_DEVICES_CONNECTED				flag2.bit.b7 //Todos los dispositivos conectados

#define CHARGER_CONNECTED	 				flag3.bit.b0
#define CAN_MESSAGE_TEST					flag3.bit.b1
#define DD									flag3.bit.b2
#define DDD									flag3.bit.b3
#define DDDD								flag3.bit.b4
#define DDDDD 								flag3.bit.b5
#define DDDDDD								flag3.bit.b6
#define DDDDDDD								flag3.bit.b7

#define GLOBAL_FAULT						flagFaults.bit.b0
#define COMMUNICATION_FAULT					flagFaults.bit.b1
#define sss									flagFaults.bit.b2
#define ssss								flagFaults.bit.b3
#define sssss								flagFaults.bit.b4
#define ssssss								flagFaults.bit.b5
#define sssssss								flagFaults.bit.b6
#define ssssssss							flagFaults.bit.b7

#define DISTANCE1 20 //distancia maxima de medida
#define DISTANCE2 10 //distancia media
#define DISTANCE3 5 //distancia minima



//PARA COMANDOS RECIBIDOS POR QT
#define ALIVE_RECIVE_CMD 	 				flagQT.bit.b0 //RECIBIO alive
#define ENABLE_RECIVE_CMD 	 				flagQT.bit.b1
#define DISABLE_RECIVE_CMD	 				flagQT.bit.b2
#define INVERTIR1_RECIVE_CMD 				flagQT.bit.b3
#define INVERTIR2_RECIVE_CMD 				flagQT.bit.b4
#define SPEED_MODE_REC_CMD		 			flagQT.bit.b5
#define POSITION_MODE_REC_CMD	 			flagQT.bit.b6
#define READY_POS_REC_CMD	     			flagQT.bit.b7

#define TARGET_SPEED_REC_CMD   				flagQT_2.bit.b0
#define TARGET_POS_REC_CMD     				flagQT_2.bit.b1
#define MANUAL_CMD_REC  					flagQT_2.bit.b2
#define AAAAA   							flagQT_2.bit.b3
#define AAAAAA   							flagQT_2.bit.b4
#define AAAAAAA   							flagQT_2.bit.b5
#define AAAAAAAA  							flagQT_2.bit.b6
#define AAAAAAAAA   						flagQT_2.bit.b7

flexcan_handle_t rxHandle, txHandle, flexcanHandle;
flexcan_frame_t txFrame, RX_STD_Frame, RX_EXT_Frame;
flexcan_mb_transfer_t TX_CAN_BUF, RX_EXT_CAN_BUF, RX_STD_CAN_BUF;//buffer para enviar,bufer no usado, buffer de recepcion
//flexcan_frame_t frame;
flexcan_rx_mb_config_t mbConfigSTD, mbConfigEXT;

flexcan_frame_t newTxFrame;
flexcan_mb_transfer_t TX_CAN_M_BUF2;
flexcan_rx_mb_config_t newMbConfigSTD;

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

	// Configure new mailbox for transmission
	newTxFrame.type = kFLEXCAN_FrameTypeData;
	newTxFrame.length = 8;
	TX_CAN_M_BUF2.mbIdx = NEW_TX_MESSAGE_BUFFER_NUM;
	TX_CAN_M_BUF2.frame = &newTxFrame;

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
    //DestinationStation[0].u8[0]= 0x45;
	//DestinationStation[0].u8[1]= 0x31;
    DestinationStation[0].u32 = 0;
	Distance_Sensor_SIMULATION.f = 2.0;
	SpeedMotorCalcRPM.f = 0.0;
	SpeedMotorCalcRPMAUX.f = 0.0;
	operationMode = INIT_MODE;
	init_comp = 0;

	Kp_SteeringMotor.u32 = 1000;
	Kd_SteeringMotor.u32 = 0;
	Ki_SteeringMotor.u32 = 0;

    while(1) {
    	USB_DeviceTasks();
    	//USB_DeviceCdcAcmSend(handle, ep, buffer, length)
    	Decode();
    	workingmode();
    	checkDeviceConnection();
    	//checkFaults();

    	if(!timeoutUSB){
			//EnviarDatos(MOTOR_SPEED_DATA1_CMD);
			//EnviarDatos(MOTOR_SPEED_DATA2_CMD);
			//EnviarDatos(MOTOR_DIR_DATA1_CMD);
			//EnviarDatos(MOTOR_DIR_DATA2_CMD);
    		EnviarDatos(ALIVECMD);
			timeoutUSB = 40;
		}

    	if(!timeoutPDO){
    		if(!datos_PDO_SPEED){
    			for (uint8_t var = 0; var < 3; var++) {
    				auxbufTX[var]=START_PDO_SPEED_MESSAGE[var];
    			}
    			CreateCANMessage(START_PDO_SPEED_CMD);
    		}
    		if(!datos_PDO_DIR){
    			for (uint8_t var = 0; var < 3; var++) {
    				auxbufTX[var]=START_PDO_DIR_MESSAGE[var];
    			}
    			CreateCANMessage(START_PDO_DIR_CMD);
    		}
    		timeoutPDO = 100;
    	}

    	if(!timeoutMOTOR_DATA_QT){
			EnviarDatos(MOTOR_SPEED_DATA1_CMD);
			//EnviarDatos(MOTOR_SPEED_DATA2_CMD);
			EnviarDatos(MOTOR_DIR_DATA1_CMD);
			//EnviarDatos(MOTOR_DIR_DATA2_CMD);
			timeoutMOTOR_DATA_QT = 50;
		}
    	//PONER TIMEOUT PARA ENVIAR DATOS A PANTALLA
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
	case INIT_MODE://MODO INICIALIZACION
		//init_status = 0;
		if(!timeoutINIT || READY_RECIVE){
			switch(init_comp){
			case 0:
				LED_RED_TOGGLE();
				for (uint8_t var = 0; var < 9; var++) {
					auxbufTX[var]=SPEED_MODE_MESSAGE[var];
				}
				CreateCANMessage(SPEED_MODE_CMD);
			break;
			case 1:
				LED_RED_TOGGLE();
				for (uint8_t var = 0; var < 9; var++) {
					auxbufTX[var]=ACC_SPEED_10[var];
				}
				CreateCANMessage(ACC_SPEED_CMD);
			break;
			case 2:
				for (uint8_t var = 0; var < 9; var++) {
					auxbufTX[var]=DESA_SPEED_100[var];
				}
				CreateCANMessage(DEC_SPEED_CMD);
			break;
			case 3:
				for (uint8_t var = 0; var < 9; var++) {
					auxbufTX[var]=POS_MODE_MESSAGE[var];
				}
				CreateCANMessage(POSITION_MODE_CMD);
			break;
			case 4:
				for (uint8_t var = 0; var < 9; var++) {
					auxbufTX[var]=POS_SPEED[var];
				}
				CreateCANMessage(SPEED_POS_CMD);
			break;
			}
			timeoutINIT = 50;
		}
		//ChargeToCANBuf(DATA_STD, auxbufRX, MOTORS);
		/* poner aqui lo que iria en el buffer*/
	break;
	case MANUAL_MODE:
	break;
	case AUTOMATIC_MODE://MODO AUTOMATICO
		if(!timeoutMOTORS){
			if(CAN_MESSAGE_TEST){
				SpeedMotorControl(); //funcion control velocidad
				CAN_MESSAGE_TEST = 0;
			}
			else {
				SteeringMotorControl(); //funcion control direccion
				CAN_MESSAGE_TEST = 1;
			}
			timeoutMOTORS = 2;
		}

	break;
	case BRAKE_MODE:
		if(!timeoutBRAKE || READY_RECIVE){
			BrakeControl();//funcion de frenado.
		}
	break;
	case CHARGE_BATTERY_MODE:
		if(!timeoutBAT && BATT_FULL_CHARGE != 1){
			ChargeToCanBUF(DATA_EXT, &TX_CAN_BUF, (uint8_t *)&CARGA_BAT_MESSAGE, ID_SEND_BATERIA);
			EnviarDatos(BATTERY_LEVEL_CMD);
			timeoutBAT=100;
		}
		if(!timeoutCONNETIONBAT){
			CHARGER_CONNECTED = 0;
			timeoutCONNETIONBAT=300;
			EnviarDatos(BATTERY_CONNECTION_CMD);
		}
	break;
	}
}

void DecodeCANMessage(){
	switch (RX_STD_Frame.id) {
		case ID_RFID_SENSOR: //cuando el mensaje que llego es del RFID
			//LED_GREEN_TOGGLE();
			DecodeRFIDSensor();
			timeoutRFIDSensor = 200;
			RFID_SENSOR_CONNECTED = 1;
			break;
		case ID_MAGNETIC_SENSOR: //Cuando el mensaje que llego es del SENSOR MAGNETICO
			//LED_RED_TOGGLE();
			DecodeMagneticSensor();
			timeoutMagSensor = 200;
			MAG_SENSOR_CONNECTED = 1;
			break;
		case ID_REC_BATERIA: //cuando la bateria manda algo
			//ESCRIBO COSAS PARA CARGA DE BATERIA
			volt_bateria.u8[1] = (RX_STD_Frame.dataByte0);
			volt_bateria.u8[0] = RX_STD_Frame.dataByte1;
			cor_bateria.u8[1] = RX_STD_Frame.dataByte2;
			cor_bateria.u8[0] = RX_STD_Frame.dataByte3;
			timeoutCONNETIONBAT = 3000;
			CHARGER_CONNECTED = 1;
			if(volt_bateria.u16[0] == 534){
				BATT_FULL_CHARGE = 1;
				EnviarDatos(BATTERY_LEVEL_CMD);
				//enviar msj a display para indicar que la bateria esta llena
			}
			else{
				BATT_FULL_CHARGE = 0;
			}
			break;
		case ID_REC_MOTOR_SPEED:
			READY_RECIVE = 1;
			//LED_GREEN_TOGGLE();
			if(operationMode==INIT_MODE){
				init_comp++;
//				if (init_comp == 3)
//					operationMode = 1;
			}
			if(operationMode==BRAKE_MODE){
				//brakestatus++;
			}
		break;
		case ID_REC_MOTOR_DIRECTION:
			READY_RECIVE = 1;
			LED_GREEN_TOGGLE();
			if(operationMode==INIT_MODE){
				init_comp++;
				 if(init_comp == 5){
					 LED_GREEN_TOGGLE();
					 operationMode = BRAKE_MODE;
				 }
				//FIRST_INIT=0;
//				if (init_comp == 5)
//					operationMode = 1;
			}
		break;
		case ID_MOTOR_SPEED_ALIVE:
			timeoutMotorVel = 200;
			MOTOR_VEL_CONNECTED = 1;
		break;
		case ID_MOTOR_DIRECTION_ALIVE:
			timeoutMotorDir = 200;
			MOTOR_DIR_CONNECTED = 1;
		break;
		case ID_MOTOR_SPEED_TPDO1: //datos tpdo1 del motor de velocidad
			RealSpeedVEL.i8[0] = RX_STD_Frame.dataByte0;
			RealSpeedVEL.i8[1] = RX_STD_Frame.dataByte1;
			RealSpeedVEL.i8[2] = RX_STD_Frame.dataByte2;
			RealSpeedVEL.i8[3] = RX_STD_Frame.dataByte3;
			StatusWordVEL.u8[0] = RX_STD_Frame.dataByte4;
			StatusWordVEL.u8[1] = RX_STD_Frame.dataByte5;
			RealCurrentVEL.i8[0] = RX_STD_Frame.dataByte6;
			RealCurrentVEL.i8[1] = RX_STD_Frame.dataByte7;
			READY_RECIVE = 1;
			datos_PDO_SPEED = 1;
		break;
		case ID_MOTOR_DIR_TPDO1: //datos tpdo1 del motor de direccion
			RealPositionDIR.i8[0] = RX_STD_Frame.dataByte0;
			RealPositionDIR.i8[1] = RX_STD_Frame.dataByte1;
			RealPositionDIR.i8[2] = RX_STD_Frame.dataByte2;
			RealPositionDIR.i8[3] = RX_STD_Frame.dataByte3;
			StatusWordDIR.u8[0] = RX_STD_Frame.dataByte4;
			StatusWordDIR.u8[1] = RX_STD_Frame.dataByte5;
			RealCurrentDIR.i8[0] = RX_STD_Frame.dataByte6;
			RealCurrentDIR.i8[1] = RX_STD_Frame.dataByte7;
			READY_RECIVE = 1;
			datos_PDO_DIR = 1;
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
	RFIDData[0].u8[0] = RX_STD_Frame.dataByte0;
	RFIDData[0].u8[1] = RX_STD_Frame.dataByte1;
	RFIDData[0].u8[2] = RX_STD_Frame.dataByte2;
	RFIDData[0].u8[3] = RX_STD_Frame.dataByte3;
	RFIDData[1].u8[0] = RX_STD_Frame.dataByte4;
	RFIDData[1].u8[1] = RX_STD_Frame.dataByte5;
	RFIDData[1].u8[2] = RX_STD_Frame.dataByte6;
	RFIDData[1].u8[3] = RX_STD_Frame.dataByte7;

	if (RFIDData[0].u16[0]==DestinationStation[0].u16[0]){
		//LED_GREEN_TOGGLE();//LLEGO A DESTINO
		operationMode = BRAKE_MODE; //ENTRA EN MODO FRENADO
		brakestatus = 0;
		if(DestinationStation[0].u16[0] == 0x3045){
			EnviarDatos(ORIGENALCANZADO_CMD);
		}
		else{
			EnviarDatos(DESTINOALCANZADO_CMD);
		}

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
	if(magneticSensorBitStatus == 0x000 && operationMode == AUTOMATIC_MODE){
		cont_perd++;
		if(cont_perd >= 10){
			operationMode = BRAKE_MODE;
			EnviarDatos(OUT_OF_LINE_CMD);
		}
	}else{
		cont_perd = 0;
		COORD_SENSORES[0] = (magneticSensorBitStatus & (1 << 11))>>11;
		COORD_SENSORES[1] = (magneticSensorBitStatus & (1 << 10))>>10;
		COORD_SENSORES[2] = (magneticSensorBitStatus & (1 << 9))>>9;
		COORD_SENSORES[3] = (magneticSensorBitStatus & (1 << 8))>>8;
		// -------------------------- LOW ------------------------- //
		COORD_SENSORES[4] = (magneticSensorBitStatus & (1 << 3))>>3;
		COORD_SENSORES[5] = (magneticSensorBitStatus & (1 << 2))>>2;
		COORD_SENSORES[6] = (magneticSensorBitStatus & (1 << 1))>>1;
		COORD_SENSORES[7] = (magneticSensorBitStatus & (1 << 0));
	}

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
//		case 0xD2:
//			ALIVE_RECIVE_CMD = 1;
//			//algo
//		break;
		case ENABLE_MOTOR_CMD:
			ENABLE_RECIVE_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case DISABLE_MOTOR_CMD:
			DISABLE_RECIVE_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
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
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case SPEED_MOTOR_CMD:
			TARGET_SPEED_REC_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case POS_MOTOR_CMD:
			TARGET_POS_REC_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case INVERTIR_1_CMD:
			INVERTIR1_RECIVE_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case INVERTIR_2_CMD:
			INVERTIR2_RECIVE_CMD = 1;
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
		break;
		case DISTANCE_SENSOR_CMD:
			head+=5;
			Distance_Sensor_REAL.u8[0]=ringRx.buf[head++];
			Distance_Sensor_REAL.u8[1]=ringRx.buf[head++];
			Distance_Sensor_REAL.u8[2]=ringRx.buf[head++];
			Distance_Sensor_REAL.u8[3]=ringRx.buf[head++];
			//LED_BLUE_TOGGLE();
			Distance_Sensor_REAL.f;
		break;
		case DISTANCE_SENSOR_SIM_CMD:
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			Distance_Sensor_SIMULATION.u8[0]=auxbufRX[5];
			Distance_Sensor_SIMULATION.u8[1]=auxbufRX[6];
			Distance_Sensor_SIMULATION.u8[2]=auxbufRX[7];
			Distance_Sensor_SIMULATION.u8[3]=auxbufRX[8];
		break;
		case MAG_SENSOR_SIM_CMD:
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			//magneticSensorBitStatus = ((uint16_t)auxbufRX[7] << 8) | auxbufRX[8];
			DecodeMagneticSensor();
		break;

		case PID_PARAMETERS_CMD:
			for (uint8_t var = 0; var < 9; var++) {
				auxbufRX[var]=ringRx.buf[head++];
			}
			Kp_SteeringMotor.u8[0]=auxbufRX[1];
			Kp_SteeringMotor.u8[1]=auxbufRX[2];
			Kp_SteeringMotor.u8[2]=auxbufRX[3];
			Kp_SteeringMotor.u8[3]=auxbufRX[4];
			Kd_SteeringMotor.u8[0]=auxbufRX[5];
			Kd_SteeringMotor.u8[1]=auxbufRX[6];
			Kd_SteeringMotor.u8[2]=auxbufRX[7];
			Kd_SteeringMotor.u8[3]=auxbufRX[8];
		break;
		case RESET_PROGRAM_CMD: //reset programa
			operationMode = 0;
			init_comp = 0;
			READY_RECIVE = 1;
		break;
		case CHANGE_CONTROL_CMD:
			head++;
			LED_RED_TOGGLE();
			uint8_t operationModeAux = ringRx.buf[head++];
			switch(operationModeAux){
				case MANUAL_MODE:
					operationMode = MANUAL_MODE;
				break;
				case AUTOMATIC_MODE:
					operationMode = AUTOMATIC_MODE;
				break;
				case BRAKE_MODE:
					operationMode = BRAKE_MODE;
				break;
				case INIT_MODE:
					operationMode = INIT_MODE;
				break;
				default:
				break;
			}
			head+=7;
		break;
		case HMI_ALIVE_CMD:
			head+=9;
			ALIVE_RECIVE_CMD = 1;
			timeoutHMI = 50;
			HMI_CONNECTED = 1;
		break;
		case DESTINATIONSTATION_CMD:
			head+=7;
			DestinationStation[0].u8[0]=ringRx.buf[head++];
			DestinationStation[0].u8[1]=ringRx.buf[head++];
			//LED_BLUE_TOGGLE();
			cont_perd = 0;
			operationMode = AUTOMATIC_MODE;
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, (uint8_t *)&ENABLE_MVEL, ID_M_VEL);
			//SpeedMotorCalcRPM.f = 0;
			ChargeToCanBUF(DATA_STD, &TX_CAN_M_BUF2, (uint8_t *)&READY_POS, ID_M_DIREC);
		break;
		case OUT_OF_LINE_CMD:
			head+=9;
			operationMode = AUTOMATIC_MODE;
		break;
		case BATTERY_MODE_CMD:
			head+=9;
			if (operationMode != CHARGE_BATTERY_MODE){
				operationMode = CHARGE_BATTERY_MODE;
				timeoutCONNETIONBAT = 3000;
				ChargeToCanBUF(DATA_EXT, &TX_CAN_BUF, (uint8_t *)&CARGA_BAT_MESSAGE, ID_SEND_BATERIA);
				timeoutBAT=100;
			}
			else{
				CHARGER_CONNECTED = 0;
				BATT_FULL_CHARGE = 0;
				//operationMode = BRAKE_MODE;
			}
		break;
		default:
			//LED_RED_TOGGLE();
		break;
	}
	//LED_RED_TOGGLE();
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
			ringTx.buf[ringTx.iW++] = 0x05;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			ringTx.buf[ringTx.iW++] = operationMode;
			_sWork temp_mag_status;
			temp_mag_status.u16[0] = magneticSensorBitStatus;
			ringTx.buf[ringTx.iW++] = temp_mag_status.u8[0];
			ringTx.buf[ringTx.iW++] = temp_mag_status.u8[1];
		break;
		case TESTCMD: //actualmode
			ringTx.buf[ringTx.iW++] = 0x02;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			//ringTx.buf[ringTx.iW++] = operationMode;
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
		case FAULT_CMD:
			ringTx.buf[ringTx.iW++] = 0x03;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			ringTx.buf[ringTx.iW++] = flagFaults.byte;
		break;
		case MOTOR_SPEED_DATA1_CMD:
			ringTx.buf[ringTx.iW++] = 0x0A;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			ringTx.buf[ringTx.iW++] = RealSpeedVEL.u8[0];
			ringTx.buf[ringTx.iW++] = RealSpeedVEL.u8[1];
			ringTx.buf[ringTx.iW++] = RealSpeedVEL.u8[2];
			ringTx.buf[ringTx.iW++] = RealSpeedVEL.u8[3];
			ringTx.buf[ringTx.iW++] = StatusWordVEL.u8[0];
			ringTx.buf[ringTx.iW++] = StatusWordVEL.u8[1];
			ringTx.buf[ringTx.iW++] = RealCurrentVEL.u8[0];
			ringTx.buf[ringTx.iW++] = RealCurrentVEL.u8[1];
		break;
		case MOTOR_SPEED_DATA2_CMD:
		break;
		case MOTOR_DIR_DATA1_CMD:
			ringTx.buf[ringTx.iW++] = 0x0A;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			ringTx.buf[ringTx.iW++] = RealPositionDIR.u8[0];
			ringTx.buf[ringTx.iW++] = RealPositionDIR.u8[1];
			ringTx.buf[ringTx.iW++] = RealPositionDIR.u8[2];
			ringTx.buf[ringTx.iW++] = RealPositionDIR.u8[3];
			ringTx.buf[ringTx.iW++] = StatusWordDIR.u8[0];
			ringTx.buf[ringTx.iW++] = StatusWordDIR.u8[1];
			ringTx.buf[ringTx.iW++] = RealCurrentDIR.u8[0];
			ringTx.buf[ringTx.iW++] = RealCurrentDIR.u8[1];
		break;
		case DESTINOALCANZADO_CMD:
			ringTx.buf[ringTx.iW++] = 0x02;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
		break;
		case ORIGENALCANZADO_CMD:
			ringTx.buf[ringTx.iW++] = 0x02;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
		break;
		case OUT_OF_LINE_CMD:
			ringTx.buf[ringTx.iW++] = 0x02;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
		break;
		case BATTERY_LEVEL_CMD:
			ringTx.buf[ringTx.iW++] = 0x07;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			ringTx.buf[ringTx.iW++] = volt_bateria.u8[0];
			ringTx.buf[ringTx.iW++] = volt_bateria.u8[1];
			ringTx.buf[ringTx.iW++] = cor_bateria.u8[0];
			ringTx.buf[ringTx.iW++] = cor_bateria.u8[1];
			ringTx.buf[ringTx.iW++] = BATT_FULL_CHARGE;
		break;
		case BATTERY_CONNECTION_CMD:
			ringTx.buf[ringTx.iW++] = 0x03;
			ringTx.buf[ringTx.iW++] = 0x00;
			ringTx.buf[ringTx.iW++] = ':';
			ringTx.buf[ringTx.iW++] = cmd;
			ringTx.buf[ringTx.iW++] = CHARGER_CONNECTED;
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

//void CreateCANMessage(uint8_t msj){
//
//	if(READY_RECIVE){
//	switch(msj){
//		case ENABLE_MOTOR_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case DISABLE_MOTOR_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case SPEED_MODE_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case POSITION_MODE_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case READY_POSI_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case SPEED_MOTOR_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case POS_MOTOR_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case INVERTIR_1_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case INVERTIR_2_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case ACC_SPEED_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case DEC_SPEED_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case SPEED_POS_CMD:
//			ChargeToCANBuf(DATA_STD, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
//		break;
//		case START_PDO_SPEED_CMD:
//			ChargeToCANBuf(PDO, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0]));
//		break;
//		case START_PDO_DIR_CMD:
//			ChargeToCANBuf(PDO, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0]));
//		break;
//	}
//	FLEXCAN_TransferSendNonBlocking(CAN0, &flexcanHandle, &TX_CAN_BUF);
//	READY_RECIVE=0;
//	timeoutCAN_MESSAGE=5;
//	}
//}

void CreateCANMessage(uint8_t msj){

	if(READY_RECIVE){
	switch(msj){
		case ENABLE_MOTOR_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case DISABLE_MOTOR_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case SPEED_MODE_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case POSITION_MODE_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_M_BUF2, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case READY_POSI_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_M_BUF2, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case SPEED_MOTOR_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case POS_MOTOR_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_M_BUF2, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case INVERTIR_1_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case INVERTIR_2_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case ACC_SPEED_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case DEC_SPEED_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case SPEED_POS_CMD:
			ChargeToCanBUF(DATA_STD, &TX_CAN_M_BUF2, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
		break;
		case START_PDO_SPEED_CMD:
			ChargeToCanBUF(PDO, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0]));
		break;
		case START_PDO_DIR_CMD:
			ChargeToCanBUF(PDO, &TX_CAN_M_BUF2, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0]));
		break;
	}
	READY_RECIVE=0;
	timeoutCAN_MESSAGE=5;
	}
}

void ChargeToCanBUF(uint8_t format, flexcan_mb_transfer_t *CANTransfer, uint8_t *data, uint32_t id){

	flexcan_frame_t *CANFrame = CANTransfer->frame;
	CANFrame->dataByte0 = 0;
	CANFrame->dataByte1 = 0;
	CANFrame->dataByte2 = 0;
	CANFrame->dataByte3 = 0;
	CANFrame->dataByte4 = 0;
	CANFrame->dataByte5 = 0;
	CANFrame->dataByte6 = 0;
	CANFrame->dataByte7 = 0;

	switch(format){
		case DATA_EXT:
			CANFrame->type = kFLEXCAN_FrameTypeData;
			CANFrame->format = kFLEXCAN_FrameFormatExtend;
			CANFrame->id = id;
			CANFrame->dataByte0 = data[1];
			CANFrame->dataByte1 = data[2];
			CANFrame->dataByte2 = data[3];
			CANFrame->dataByte3 = data[4];
			CANFrame->dataByte4 = data[5];
			CANFrame->dataByte5 = data[6];
			CANFrame->dataByte6 = data[7];
			CANFrame->dataByte7 = data[8];
		break;
		case DATA_STD:
			CANFrame->type = kFLEXCAN_FrameTypeData;
			CANFrame->format = kFLEXCAN_FrameFormatStandard;
			CANFrame->id = id;
			CANFrame->dataByte0 = data[1];
			CANFrame->dataByte1 = data[2];
			CANFrame->dataByte2 = data[3];
			CANFrame->dataByte3 = data[4];
			CANFrame->dataByte4 = data[5];
			CANFrame->dataByte5 = data[6];
			CANFrame->dataByte6 = data[7];
			CANFrame->dataByte7 = data[8];
		break;
		case REMOTE_EXT:
		break;
		case REMOTE_STD:
		break;
		case PDO:
			CANFrame->type = kFLEXCAN_FrameTypeData;
			CANFrame->format = kFLEXCAN_FrameFormatStandard;
			CANFrame->id = id;
			CANFrame->dataByte0 = data[1];
			CANFrame->dataByte1 = data[2];
		break;
		}
    FLEXCAN_TransferSendNonBlocking(CAN0, &flexcanHandle, CANTransfer);
}

//void ChargeToCANBuf(uint8_t whatFormat, uint8_t payloadCAN[], uint32_t id){
//
//	txFrame.dataByte0 = 0;
//	txFrame.dataByte1 = 0;
//	txFrame.dataByte2 = 0;
//	txFrame.dataByte3 = 0;
//	txFrame.dataByte4 = 0;
//	txFrame.dataByte5 = 0;
//	txFrame.dataByte6 = 0;
//	txFrame.dataByte7 = 0;
//
//	switch(whatFormat){
//	case DATA_EXT:
//		txFrame.type = kFLEXCAN_FrameTypeData;
//		txFrame.format = kFLEXCAN_FrameFormatExtend;
//		txFrame.id = id;
//		txFrame.dataByte0 = payloadCAN[1];
//		txFrame.dataByte1 = payloadCAN[2];
//		txFrame.dataByte2 = payloadCAN[3];
//		txFrame.dataByte3 = payloadCAN[4];
//		txFrame.dataByte4 = payloadCAN[5];
//		txFrame.dataByte5 = payloadCAN[6];
//		txFrame.dataByte6 = payloadCAN[7];
//		txFrame.dataByte7 = payloadCAN[8];
//	break;
//	case DATA_STD:
//	    txFrame.type = kFLEXCAN_FrameTypeData;
//		txFrame.format = kFLEXCAN_FrameFormatStandard;
//		txFrame.id = id;
//		txFrame.dataByte0 = payloadCAN[1];
//		txFrame.dataByte1 = payloadCAN[2];
//		txFrame.dataByte2 = payloadCAN[3];
//		txFrame.dataByte3 = payloadCAN[4];
//		txFrame.dataByte4 = payloadCAN[5];
//		txFrame.dataByte5 = payloadCAN[6];
//		txFrame.dataByte6 = payloadCAN[7];
//		txFrame.dataByte7 = payloadCAN[8];
//	break;
//	case REMOTE_EXT:
//	break;
//	case REMOTE_STD:
//	break;
//	case PDO:
//	    txFrame.type = kFLEXCAN_FrameTypeData;
//		txFrame.format = kFLEXCAN_FrameFormatStandard;
//		txFrame.id = id;
//		txFrame.dataByte0 = payloadCAN[1];
//		txFrame.dataByte1 = payloadCAN[2];
//	break;
//	}
//}

void ActionQT(){
	if(ALIVE_RECIVE_CMD){
		ALIVE_RECIVE_CMD = 0;
	    //LED_GREEN_TOGGLE();
	    EnviarDatos(TESTCMD);
	    return;
	}
	if (ENABLE_RECIVE_CMD) {
		ENABLE_RECIVE_CMD = 0;
		//LED_BLUE_TOGGLE();
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

void BrakeControl(){
	switch(brakestatus){
	case 0:
		//if (RealSpeedVEL.i32 >= 2 || RealSpeedVEL.i32 <= -2) {
			SpeedMotorCalcDEC.i32 = 0;
			auxbufTX[0] = 0x07; //id motor velocidad
			auxbufTX[1] = 0x23;
			auxbufTX[2] = 0xFF;
			auxbufTX[3] = 0x60;
			auxbufTX[4] = 0x00;
			auxbufTX[5] = SpeedMotorCalcDEC.i8[0];
			auxbufTX[6] = SpeedMotorCalcDEC.i8[1];
			auxbufTX[7] = SpeedMotorCalcDEC.i8[2];
			auxbufTX[8] = SpeedMotorCalcDEC.i8[3];
			timeoutBRAKE = 5;
			READY_RECIVE = 1;
			CreateCANMessage(SPEED_MOTOR_CMD);
		//}

	break;
	}
}

void SpeedMotorControl(){
	SpeedMotorCalcRPMAUX.f = speedControlCalc(Distance_Sensor_REAL.f, 2500);
	//if(fabs(RealSpeedVEL.f-SpeedMotorCalcRPMAUX.f) >= 50){
		//cambiar velocidad porque el cambio es de mas de 50rpm
		SpeedMotorCalcRPM.f = SpeedMotorCalcRPMAUX.f;
		SpeedMotorCalcDEC.i32 = speedConvertionRPMtoDEC(SpeedMotorCalcRPM.f);
		auxbufTX[0] = 0x07; //id motor
		auxbufTX[1] = 0x23;
		auxbufTX[2] = 0xFF;
		auxbufTX[3] = 0x60;
		auxbufTX[4] = 0x00;
		auxbufTX[5] = SpeedMotorCalcDEC.u8[0];
		auxbufTX[6] = SpeedMotorCalcDEC.u8[1];
		auxbufTX[7] = SpeedMotorCalcDEC.u8[2];
		auxbufTX[8] = SpeedMotorCalcDEC.u8[3];
		CreateCANMessage(SPEED_MOTOR_CMD);
		//READY_RECIVE=1;
	//}
// ESTO ES PARA SIMULACION
//	SpeedMotorCalcRPMAUX.f = speedControlCalc(Distance_Sensor_SIMULATION.f, 3000);
//	if(fabs(SpeedMotorCalcRPM.f-SpeedMotorCalcRPMAUX.f) >= 50){
//		//cambiar velocidad porque el cambio es de mas de 50rpm
//		SpeedMotorCalcRPM.f = SpeedMotorCalcRPMAUX.f;
//		SpeedMotorCalcDEC.i32 = speedConvertionRPMtoDEC(SpeedMotorCalcRPM.f);
//		auxbufRX[0] = 0x07; //id motor
//		auxbufRX[1] = 0x23;
//		auxbufRX[2] = 0xFF;
//		auxbufRX[3] = 0x60;
//		auxbufRX[4] = 0x00;
//		auxbufRX[5] = SpeedMotorCalcDEC.u8[0];
//		auxbufRX[6] = SpeedMotorCalcDEC.u8[1];
//		auxbufRX[7] = SpeedMotorCalcDEC.u8[2];
//		auxbufRX[8] = SpeedMotorCalcDEC.u8[3];
//		CreateCANMessage(SPEED_MOTOR_CMD);
//		//READY_RECIVE=1;
//	}
}

uint32_t speedConvertionRPMtoDEC(float rpmSpeed){
	float vel_aux = 0;
	vel_aux = ((rpmSpeed * 512) * (10000.0/1875));
	return (int32_t)vel_aux;
}

float speedControlCalc(float d, int Vmax){
	LED_BLUE_TOGGLE();
	if (d > DISTANCE1) {
		return Vmax+0.0;
	} else if (d >= DISTANCE2 && d <= DISTANCE1) {
		return (Vmax) - ((Vmax / 2) * ((DISTANCE1 - d) / 4.0));
	} else if (d >= DISTANCE3 && d < DISTANCE2) {
		return ((Vmax / 2) - ((Vmax/3) * ((DISTANCE2-d)/2.5)));
	} else {
		return 0.0;
	}
}

void SteeringMotorControl(int32_t direc_base){

	direc_base = 0;
	pos_lin = 0;

	for(uint8_t i = 0; i < 8; i++){
		if(COORD_SENSORES[i] == 1 && COORD_SENSORES[i+1] == 1){
			pos_lin = i + (i+1);
			//pos_lin2 = i+1;
			break;
		}else{
			if(COORD_SENSORES[i] == 1){
				pos_lin = i * 2;
			}
		}
	}

	if(pos_lin > 14){
		pos_lin = 14;
	}

	error = SENS_MODEL_EXAC[pos_lin];

	integral += error;

	if(integral > 1000){
		integral = 0;
	}

	derivativo = error - lastError;

	turn = (Kp_SteeringMotor.u32 * error) + (Kd_SteeringMotor.u32 * derivativo) + (Ki_SteeringMotor.u32 * integral);

	direccion = (direc_base + turn);//el *30 es pq la correcion era muy chica, se revisa dsp

	if(direccion > 21000){
		direccion = 21000;
	}else{
		if(direccion < -21000){
			direccion = -21000;
		}
	}

	lastError = error;

	PositionMotorControl();

}

int32_t positionConvert(int32_t pos_degree){
	int32_t pos_aux = 0;
	pos_aux = ((pos_degree/5.49316)*1000);
	return (int32_t)pos_aux;
}

void PositionMotorControl(){

	PosSend.i32 = positionConvert(direccion);
	auxbufTX[0] = 0x01;
	auxbufTX[1] = 0x23;
	auxbufTX[2] = 0x7A;
	auxbufTX[3] = 0x60;
	auxbufTX[4] = 0x00;
	auxbufTX[5] = PosSend.i8[0];
	auxbufTX[6] = PosSend.i8[1];
	auxbufTX[7] = PosSend.i8[2];
	auxbufTX[8] = PosSend.i8[3];
	if(!timeoutDIREC){
		CreateCANMessage(POS_MOTOR_CMD);
		timeoutDIREC=50;
	}
}

void checkDeviceConnection(){
	if (MOTOR_VEL_CONNECTED & MOTOR_DIR_CONNECTED & MAG_SENSOR_CONNECTED & RFID_SENSOR_CONNECTED & HMI_CONNECTED){
		ALL_DEVICES_CONNECTED = 1;
	}
	else{
		ALL_DEVICES_CONNECTED = 0;
		COMMUNICATION_FAULT = 1;
		//ENVIAR COMANDO PARA ALERTA O FALLA EN PANTALLA
	}
}

void CheckFaults(){
	//algo
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

  if(timeoutCONNETIONBAT){
	  timeoutCONNETIONBAT--;
    }

  if(timeoutPDO){
  	  timeoutPDO--;
    }
  if(timeoutMOTORS){
	  timeoutMOTORS--;
  }


  if(timeoutINIT){
	  timeoutINIT--;
	  if(!timeoutINIT){
		  READY_RECIVE=1;
	  }
   }

  if(timeoutDIREC){
 	  timeoutDIREC--;
    }

  if(timeoutBRAKE){
	  timeoutBRAKE--;
  }

  if(ringRx.timeout){
	  ringRx.timeout--;
	  if(!ringRx.timeout)
		  ringRx.header = 0;
  }

  if(timeoutMotorDir){
	  timeoutMotorDir--;
	  if(!timeoutMotorDir)
		  MOTOR_DIR_CONNECTED = 0;
  }

  if(timeoutMotorVel){
	  timeoutMotorVel--;
	  if(!timeoutMotorVel)
		  MOTOR_VEL_CONNECTED = 0;
  }

  if(timeoutMagSensor){
  	  timeoutMagSensor--;
  	  if(!timeoutMagSensor)
  		  MAG_SENSOR_CONNECTED = 0;
    }

  if(timeoutRFIDSensor){
	  timeoutRFIDSensor--;
	  if(!timeoutRFIDSensor)
		  RFID_SENSOR_CONNECTED = 0;
  }

  if(timeoutHMI){
	  timeoutHMI--;
	  if(!timeoutHMI)
		  HMI_CONNECTED = 0;
  }

  if(timeoutMOTOR_DATA_QT){
	  timeoutMOTOR_DATA_QT--;
    }
  if(timeoutCAN_MESSAGE){
	  timeoutCAN_MESSAGE--;
	  if(!timeoutCAN_MESSAGE){
		  READY_RECIVE = 1;
	  }
  }
  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}


