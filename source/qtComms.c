#include "qtComms.h"
#include "motorControl.h"
#include "canComms.h"
#include "board.h"
#include "fsl_debug_console.h"

volatile _sFlag flagQT;
volatile _sFlag flagQT_2;

volatile _rx ringRx, auxRX;
volatile _tx ringTx, auxTX;

uint8_t rxBuf[256];
uint8_t txBuf[256];
uint8_t auxbufRX[256];

void decodeQt(void) {
    if(ringRx.iW == ringRx.iR) return;

    switch (ringRx.header) {
        case 0:
            if (ringRx.buf[ringRx.iR] == 'U'){
                ringRx.header++;
                ringRx.timeout = 50;
            } else {
                ringRx.header = 0;
                ringRx.timeout = 0;
            }
            break;
        case 1:
            if (ringRx.buf[ringRx.iR] == 'N') ringRx.header++;
            else { ringRx.header = 0; ringRx.iR--; }
            break;
        case 2:
            if (ringRx.buf[ringRx.iR] == 'E') ringRx.header++;
            else { ringRx.header = 0; ringRx.iR--; }
            break;
        case 3:
            if (ringRx.buf[ringRx.iR] == 'R') ringRx.header++;
            else { ringRx.header = 0; ringRx.iR--; }
            break;
        case 4:
            ringRx.nBytes = ringRx.buf[ringRx.iR];
            ringRx.header++;
            break;
        case 5:
            ringRx.header++;
            break;
        case 6:
            if (ringRx.buf[ringRx.iR] == ':') {
                ringRx.cks = 'U'^'N'^'E'^'R'^ringRx.nBytes^0x00^':';
                ringRx.header++;
                ringRx.iData = ringRx.iR+1;
            } else {
                ringRx.header = 0;
                ringRx.iR--;
            }
            break;
        case 7:
            if(ringRx.nBytes > 1) {
                ringRx.cks ^= ringRx.buf[ringRx.iR];
            }
            ringRx.nBytes--;
            if(ringRx.nBytes == 0){
                ringRx.header = 0;
                ringRx.timeout = 0;
                if(ringRx.cks == ringRx.buf[ringRx.iR]){
                    receiveData(ringRx.iData);
                }
            }
            break;
        default:
            ringRx.header = 0;
            break;
    }
    ringRx.iR++;
}

void receiveData(uint8_t head) {
    switch (ringRx.buf[head++]){
        case ENABLE_MOTOR_CMD:
            ENABLE_RECIVE_CMD = 1;
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
            break;
        case DISABLE_MOTOR_CMD:
            DISABLE_RECIVE_CMD = 1;
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
            break;
        case SPEED_MODE_CMD:
            SPEED_MODE_REC_CMD = 1;
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            break;
        case POSITION_MODE_CMD:
            POSITION_MODE_REC_CMD = 1;
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            break;
        case READY_POSI_CMD:
            READY_POS_REC_CMD = 1;
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
            break;
        case SPEED_MOTOR_CMD:
            TARGET_SPEED_REC_CMD = 1;
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
            break;
        case POS_MOTOR_CMD:
            TARGET_POS_REC_CMD = 1;
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
            break;
        case INVERTIR_1_CMD:
            INVERTIR1_RECIVE_CMD = 1;
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
            break;
        case INVERTIR_2_CMD:
            INVERTIR2_RECIVE_CMD = 1;
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufRX, FLEXCAN_ID_STD(auxbufRX[0] + 0x600));
            break;
        case DISTANCE_SENSOR_CMD:
            head+=5;
            Distance_Sensor_REAL.u8[0]=ringRx.buf[head++];
            Distance_Sensor_REAL.u8[1]=ringRx.buf[head++];
            Distance_Sensor_REAL.u8[2]=ringRx.buf[head++];
            Distance_Sensor_REAL.u8[3]=ringRx.buf[head++];
            Distance_Sensor_REAL.f;
            break;
        case DISTANCE_SENSOR_SIM_CMD:
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            Distance_Sensor_SIMULATION.u8[0]=auxbufRX[5];
            Distance_Sensor_SIMULATION.u8[1]=auxbufRX[6];
            Distance_Sensor_SIMULATION.u8[2]=auxbufRX[7];
            Distance_Sensor_SIMULATION.u8[3]=auxbufRX[8];
            break;
        case MAG_SENSOR_SIM_CMD:
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            decodeMagneticSensor(&RX_STD_CAN_BUF);
            break;
        case PID_PARAMETERS_CMD:
            for (uint8_t var = 0; var < 9; var++) auxbufRX[var]=ringRx.buf[head++];
            Kp_SteeringMotor.u8[0]=auxbufRX[1];
            Kp_SteeringMotor.u8[1]=auxbufRX[2];
            Kp_SteeringMotor.u8[2]=auxbufRX[3];
            Kp_SteeringMotor.u8[3]=auxbufRX[4];
            Kd_SteeringMotor.u8[0]=auxbufRX[5];
            Kd_SteeringMotor.u8[1]=auxbufRX[6];
            Kd_SteeringMotor.u8[2]=auxbufRX[7];
            Kd_SteeringMotor.u8[3]=auxbufRX[8];
            break;
        case RESET_PROGRAM_CMD:
            operationMode = 0;
            init_comp = 0;
            READY_RECIVE = 1;
            break;
        case CHANGE_CONTROL_CMD:
            head++;
            LED_RED_TOGGLE();
            uint8_t operationModeAux = ringRx.buf[head++];
            switch(operationModeAux){
                case MANUAL_MODE: operationMode = MANUAL_MODE; break;
                case AUTOMATIC_MODE: operationMode = AUTOMATIC_MODE; break;
                case BRAKE_MODE: operationMode = BRAKE_MODE; break;
                case INIT_MODE: operationMode = INIT_MODE; break;
                default: break;
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
            cont_perd = 0;
            operationMode = AUTOMATIC_MODE;
            chargeToCanBuf(DATA_STD, &TX_CAN_BUF, (uint8_t *)&ENABLE_MVEL, ID_M_VEL);
            chargeToCanBuf(DATA_STD, &TX_CAN_M_BUF2, (uint8_t *)&READY_POS, ID_M_DIREC);
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
                chargeToCanBuf(DATA_EXT, &TX_CAN_BUF, (uint8_t *)&CARGA_BAT_MESSAGE, ID_SEND_BATERIA);
                timeoutBAT=100;
            } else {
                CHARGER_CONNECTED = 0;
                BATT_FULL_CHARGE = 0;
            }
            break;
        default:
            break;
    }
    sendData(TESTCMD);
}

void sendData(uint8_t cmd){
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
        case TESTCMD:
            ringTx.buf[ringTx.iW++] = 0x02;
            ringTx.buf[ringTx.iW++] = 0x00;
            ringTx.buf[ringTx.iW++] = ':';
            ringTx.buf[ringTx.iW++] = cmd;
            break;
        case ENABLE_MOTOR_CMD:
        case DISABLE_MOTOR_CMD:
        case SPEED_MODE_CMD:
        case SPEED_MOTOR_CMD:
            ringTx.buf[ringTx.iW++] = 0x0B;
            ringTx.buf[ringTx.iW++] = 0x00;
            ringTx.buf[ringTx.iW++] = ':';
            ringTx.buf[ringTx.iW++] = cmd;
            for (uint8_t var = 0; var < 9; var++) ringTx.buf[ringTx.iW++] = auxbufRX[var];
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
        case ORIGENALCANZADO_CMD:
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
    }
    ringTx.buf[ringTx.iW++]=ringTx.cks;
}

void actionQt(void){
    if(ALIVE_RECIVE_CMD){
        ALIVE_RECIVE_CMD = 0;
        sendData(TESTCMD);
        return;
    }
    if (ENABLE_RECIVE_CMD) {
        ENABLE_RECIVE_CMD = 0;
        sendData(ENABLE_MOTOR_CMD);
        createCanMessage(ENABLE_MOTOR_CMD);
        return;
    }
    if (DISABLE_RECIVE_CMD) {
        DISABLE_RECIVE_CMD = 0;
        sendData(DISABLE_MOTOR_CMD);
        createCanMessage(DISABLE_MOTOR_CMD);
        return;
    }
    if (SPEED_MODE_REC_CMD) {
        SPEED_MODE_REC_CMD = 0;
        LED_GREEN_TOGGLE();
        createCanMessage(SPEED_MODE_CMD);
        return;
    }
    if (POSITION_MODE_REC_CMD) {
        POSITION_MODE_REC_CMD = 0;
        LED_BLUE_TOGGLE();
        createCanMessage(POSITION_MODE_CMD);
        return;
    }
    if (TARGET_SPEED_REC_CMD) {
        TARGET_SPEED_REC_CMD = 0;
        sendData(SPEED_MOTOR_CMD);
        createCanMessage(SPEED_MOTOR_CMD);
        return;
    }
    if (TARGET_POS_REC_CMD) {
        TARGET_POS_REC_CMD = 0;
        createCanMessage(POS_MOTOR_CMD);
        return;
    }
    if (READY_POS_REC_CMD) {
        READY_POS_REC_CMD = 0;
        createCanMessage(READY_POSI_CMD);
        return;
    }
    if (INVERTIR1_RECIVE_CMD) {
        INVERTIR1_RECIVE_CMD = 0;
        createCanMessage(INVERTIR_1_CMD);
        return;
    }
    if (INVERTIR2_RECIVE_CMD) {
        INVERTIR2_RECIVE_CMD = 0;
        createCanMessage(INVERTIR_2_CMD);
        return;
    }
}
