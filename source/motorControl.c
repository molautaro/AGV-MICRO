#include "motorControl.h"
#include "qtComms.h"
#include "canComms.h"
#include "board.h"
#include <math.h>

const uint8_t CARGA_BAT_MESSAGE[] =     {0x00,0x02,0x48,0x01,0x2C,0x00,0x00,0x00,0x00};
const uint8_t SPEED_MODE_MESSAGE[] =    {0x07,0x2F,0x60,0x60,0x00,0x03,0x00,0x00,0x00};
const uint8_t POS_MODE_MESSAGE[] =      {0x01,0x2F,0x60,0x60,0x00,0x01,0x00,0x00,0x00};
const uint8_t ENABLE_MVEL[] =           {0x07,0x2B,0x40,0x60,0x00,0x0F,0x00,0x00,0x00};
const uint8_t ENABLE_MPOS[] =           {0x01,0x2B,0x40,0x60,0x00,0x0F,0x00,0x00,0x00};
const uint8_t DISABLE_MVEL[] =          {0x07,0x2B,0x40,0x60,0x00,0x06,0x00,0x00,0x00};
const uint8_t DISABLE_MPOS[] =          {0x01,0x2B,0x40,0x60,0x00,0x06,0x00,0x00,0x00};
const uint8_t READY_POS[] =             {0x01,0x2B,0x40,0x60,0x00,0x3F,0x10,0x00,0x00};
const uint8_t INVERTIR_1[] =            {0x07,0x2F,0x60,0x7E,0x00,0x01,0x00,0x00,0x00};
const uint8_t INVERTIR_2[] =            {0x07,0x2F,0x60,0x7E,0x00,0x00,0x00,0x00,0x00};

const uint8_t START_PDO_SPEED_MESSAGE[] = {0x00,0x01,0x07};
const uint8_t START_PDO_DIR_MESSAGE[] =   {0x00,0x01,0x01};

const uint8_t ACC_SPEED_10[] = {0x07,0x23,0x83,0x60,0x00,0x66,0x06,0x00,0x00};//10 rpm/s
const uint8_t ACC_SPEED_100[] = {0x07,0x23,0x83,0x60,0x00,0x00,0x40,0x00,0x00};//100 rpm/s
const uint8_t ACC_SPEED_150[] = {0x07,0x23,0x83,0x60,0x00,0x00,0x60,0x00,0x00};//150 rpm/s

const uint8_t DESA_SPEED_10[] = {0x07,0x23,0x84,0x60,0x00,0x66,0x06,0x00,0x00};//10 rpm/s
const uint8_t DESA_SPEED_100[] = {0x07,0x23,0x84,0x60,0x00,0x00,0x40,0x00,0x00};//100 rpm/s
const uint8_t DESA_SPEED_150[] = {0x07,0x23,0x84,0x60,0x00,0x00,0x60,0x00,0x00};//150 rpm/s

const uint8_t POS_SPEED[] = {0x01,0x23,0x81,0x60,0x00,0xBC,0xBB,0xBB,0x03};//3000 rpm

uint8_t operationMode = 0;
uint8_t init_comp = 0;
uint8_t brakestatus = 0;

uint16_t timeoutINIT = 0;
uint16_t timeoutBRAKE = 0;
uint16_t timeoutMOTORS = 2;
uint16_t timeoutBAT = 100;
uint16_t timeoutDIREC = 250;

_sWork volt_bateria, cor_bateria;
_sWork Distance_Sensor_REAL, Distance_Sensor_SIMULATION;
_sWork Kp_SteeringMotor, Kd_SteeringMotor, Ki_SteeringMotor;
_sWork DestinationStation[2], RFIDData[2];
_sWork RealSpeedVEL, StatusWordVEL, RealCurrentVEL;
_sWork RealPositionDIR, StatusWordDIR, RealCurrentDIR;

volatile _sWork SpeedMotorCalcRPM, SpeedMotorCalcRPMAUX, SpeedMotorCalcDEC;
volatile _sWork PosSend;

uint16_t magneticSensorBitStatus;
uint16_t COORD_SENSORES[8];
const int SENS_MODEL_EXAC[] = {-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7};
uint8_t pos_lin = 0;
float error = 0, lastError = 0, integral = 0, derivativo = 0, turn = 0;
int32_t direccion = 0;
uint8_t cont_perd = 0;


void workingMode(void){
    switch(operationMode){
        case INIT_MODE:
            if(!timeoutINIT || READY_RECIVE){
                switch(init_comp){
                    case 0:
                        LED_RED_TOGGLE();
                        for (uint8_t var = 0; var < 9; var++) {
                            auxbufTX[var] = SPEED_MODE_MESSAGE[var];
                        }
                        createCanMessage(SPEED_MODE_CMD);
                        break;
                    case 1:
                        LED_RED_TOGGLE();
                        for (uint8_t var = 0; var < 9; var++) {
                            auxbufTX[var] = ACC_SPEED_10[var];
                        }
                        createCanMessage(ACC_SPEED_CMD);
                        break;
                    case 2:
                        for (uint8_t var = 0; var < 9; var++) {
                            auxbufTX[var] = DESA_SPEED_100[var];
                        }
                        createCanMessage(DEC_SPEED_CMD);
                        break;
                    case 3:
                        for (uint8_t var = 0; var < 9; var++) {
                            auxbufTX[var] = POS_MODE_MESSAGE[var];
                        }
                        createCanMessage(POSITION_MODE_CMD);
                        break;
                    case 4:
                        for (uint8_t var = 0; var < 9; var++) {
                            auxbufTX[var] = POS_SPEED[var];
                        }
                        createCanMessage(SPEED_POS_CMD);
                        break;
                }
                timeoutINIT = 50;
            }
            break;
        case MANUAL_MODE:
            break;
        case AUTOMATIC_MODE:
            if(!timeoutMOTORS){
                if(CAN_MESSAGE_TEST){
                    speedMotorControl();
                    CAN_MESSAGE_TEST = 0;
                } else {
                    steeringMotorControl();
                    CAN_MESSAGE_TEST = 1;
                }
                timeoutMOTORS = 2;
            }
            break;
        case BRAKE_MODE:
            if(!timeoutBRAKE || READY_RECIVE){
                brakeControl();
            }
            break;
        case CHARGE_BATTERY_MODE:
            if(!timeoutBAT && BATT_FULL_CHARGE != 1){
                chargeToCanBuf(DATA_EXT, &TX_CAN_BUF, (uint8_t *)&CARGA_BAT_MESSAGE, ID_SEND_BATERIA);
                sendData(BATTERY_LEVEL_CMD);
                timeoutBAT=100;
            }
            if(!timeoutCONNETIONBAT){
                CHARGER_CONNECTED = 0;
                timeoutCONNETIONBAT=300;
                sendData(BATTERY_CONNECTION_CMD);
            }
            break;
    }
}

void speedMotorControl(void){
    SpeedMotorCalcRPMAUX.f = speedControlCalc(Distance_Sensor_REAL.f, 2500);
    SpeedMotorCalcRPM.f = SpeedMotorCalcRPMAUX.f;
    SpeedMotorCalcDEC.i32 = speedConvertionRPMtoDEC(SpeedMotorCalcRPM.f);
    auxbufTX[0] = 0x07;
    auxbufTX[1] = 0x23;
    auxbufTX[2] = 0xFF;
    auxbufTX[3] = 0x60;
    auxbufTX[4] = 0x00;
    auxbufTX[5] = SpeedMotorCalcDEC.u8[0];
    auxbufTX[6] = SpeedMotorCalcDEC.u8[1];
    auxbufTX[7] = SpeedMotorCalcDEC.u8[2];
    auxbufTX[8] = SpeedMotorCalcDEC.u8[3];
    createCanMessage(SPEED_MOTOR_CMD);
}

uint32_t speedConvertionRPMtoDEC(float rpmSpeed){
    float vel_aux = ((rpmSpeed * 512) * (10000.0/1875));
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

void steeringMotorControl(void){
    int32_t direc_base = 0;
    pos_lin = 0;

    for(uint8_t i = 0; i < 8; i++){
        if(COORD_SENSORES[i] == 1 && COORD_SENSORES[i+1] == 1){
            pos_lin = i + (i+1);
            break;
        }else{
            if(COORD_SENSORES[i] == 1){
                pos_lin = i * 2;
            }
        }
    }

    if(pos_lin > 14) pos_lin = 14;

    error = SENS_MODEL_EXAC[pos_lin];
    integral += error;

    if(integral > 1000) integral = 0;

    derivativo = error - lastError;
    turn = (Kp_SteeringMotor.u32 * error) + (Kd_SteeringMotor.u32 * derivativo) + (Ki_SteeringMotor.u32 * integral);

    direccion = (direc_base + turn);
    if(direccion > 21000) direccion = 21000;
    else if(direccion < -21000) direccion = -21000;

    lastError = error;
    positionMotorControl();
}

int32_t positionConvert(int32_t pos_degree){
    int32_t pos_aux = ((pos_degree/5.49316)*1000);
    return (int32_t)pos_aux;
}

void positionMotorControl(void){
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
        createCanMessage(POS_MOTOR_CMD);
        timeoutDIREC=50;
    }
}

void brakeControl(void){
    switch(brakestatus){
        case 0:
            SpeedMotorCalcDEC.i32 = 0;
            auxbufTX[0] = 0x07;
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
            createCanMessage(SPEED_MOTOR_CMD);
            break;
    }
}

void checkDeviceConnection(void){
    if (MOTOR_VEL_CONNECTED && MOTOR_DIR_CONNECTED && MAG_SENSOR_CONNECTED && RFID_SENSOR_CONNECTED && HMI_CONNECTED){
        ALL_DEVICES_CONNECTED = 1;
    } else {
        ALL_DEVICES_CONNECTED = 0;
        COMMUNICATION_FAULT = 1;
    }
}

void checkFaults(void){
    // To be implemented
}

void decodeRfidSensor(flexcan_frame_t *rxFrame){
    RFIDData[0].u8[0] = rxFrame->dataByte0;
    RFIDData[0].u8[1] = rxFrame->dataByte1;
    RFIDData[0].u8[2] = rxFrame->dataByte2;
    RFIDData[0].u8[3] = rxFrame->dataByte3;
    RFIDData[1].u8[0] = rxFrame->dataByte4;
    RFIDData[1].u8[1] = rxFrame->dataByte5;
    RFIDData[1].u8[2] = rxFrame->dataByte6;
    RFIDData[1].u8[3] = rxFrame->dataByte7;

    if (RFIDData[0].u16[0] == DestinationStation[0].u16[0]){
        operationMode = BRAKE_MODE;
        brakestatus = 0;
        if(DestinationStation[0].u16[0] == 0x3045){
            sendData(ORIGENALCANZADO_CMD);
        } else {
            sendData(DESTINOALCANZADO_CMD);
        }
    } else {
        LED_RED_TOGGLE();
    }
}

void decodeMagneticSensor(flexcan_mb_transfer_t *rxBuf){
    magneticSensorBitStatus = ((uint16_t)rxBuf->frame->dataByte6 << 8) | rxBuf->frame->dataByte7;

    if(magneticSensorBitStatus == 0x000 && operationMode == AUTOMATIC_MODE){
        cont_perd++;
        if(cont_perd >= 10){
            operationMode = BRAKE_MODE;
            sendData(OUT_OF_LINE_CMD);
        }
    } else {
        cont_perd = 0;
        COORD_SENSORES[0] = (magneticSensorBitStatus & (1 << 11))>>11;
        COORD_SENSORES[1] = (magneticSensorBitStatus & (1 << 10))>>10;
        COORD_SENSORES[2] = (magneticSensorBitStatus & (1 << 9))>>9;
        COORD_SENSORES[3] = (magneticSensorBitStatus & (1 << 8))>>8;
        COORD_SENSORES[4] = (magneticSensorBitStatus & (1 << 3))>>3;
        COORD_SENSORES[5] = (magneticSensorBitStatus & (1 << 2))>>2;
        COORD_SENSORES[6] = (magneticSensorBitStatus & (1 << 1))>>1;
        COORD_SENSORES[7] = (magneticSensorBitStatus & (1 << 0));
    }
}
