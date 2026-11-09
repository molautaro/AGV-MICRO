#include "canComms.h"
#include "motorControl.h"
#include "qtComms.h"
#include "fsl_debug_console.h"
#include "board.h"

// FlexCAN objects
flexcan_handle_t flexcanHandle;
flexcan_frame_t txFrame, RX_STD_Frame, RX_EXT_Frame;
flexcan_mb_transfer_t TX_CAN_BUF, RX_EXT_CAN_BUF, RX_STD_CAN_BUF;
flexcan_rx_mb_config_t mbConfigSTD, mbConfigEXT;
flexcan_frame_t newTxFrame;
flexcan_mb_transfer_t TX_CAN_M_BUF2;
flexcan_rx_mb_config_t newMbConfigSTD;

volatile _sFlag flag1, flag2, flag3, flagFaults;

uint16_t timeoutMotorVel = 200;
uint16_t timeoutMotorDir = 200;
uint16_t timeoutMagSensor = 200;
uint16_t timeoutRFIDSensor = 200;
uint16_t timeoutHMI = 200;
uint16_t timeoutCAN_MESSAGE = 5;
uint16_t timeoutCONNETIONBAT = 3000;
uint8_t auxbufTX[256];

void initCanComms(void) {
    FLEXCAN_SetRxMbGlobalMask(CAN0, FLEXCAN_RX_MB_STD_MASK(0, 0, 0));
    FLEXCAN_SetRxMbGlobalMask(CAN0, FLEXCAN_RX_MB_EXT_MASK(0, 0, 0));

    txFrame.type = kFLEXCAN_FrameTypeData;
    txFrame.length = 8;
    TX_CAN_BUF.mbIdx = TX_MESSAGE_BUFFER_NUM;
    TX_CAN_BUF.frame = &txFrame;

    newTxFrame.type = kFLEXCAN_FrameTypeData;
    newTxFrame.length = 8;
    TX_CAN_M_BUF2.mbIdx = NEW_TX_MESSAGE_BUFFER_NUM;
    TX_CAN_M_BUF2.frame = &newTxFrame;

    RX_STD_CAN_BUF.mbIdx = RX_MESSAGE_STD_BUFFER_NUM;
    RX_STD_CAN_BUF.frame = &RX_STD_Frame;

    mbConfigSTD.format = kFLEXCAN_FrameFormatStandard;
    mbConfigSTD.type   = kFLEXCAN_FrameTypeData;
    mbConfigSTD.id = FLEXCAN_ID_STD(0x0);
    FLEXCAN_SetRxMbConfig(CAN0, RX_MESSAGE_STD_BUFFER_NUM, &mbConfigSTD, true);

    FLEXCAN_TransferCreateHandle(CAN0, &flexcanHandle, flexcanCallback, NULL);
    FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_STD_CAN_BUF);
}

void decodeCanMessage(void) {
    switch (RX_STD_Frame.id) {
        case ID_RFID_SENSOR:
            decodeRfidSensor(&RX_STD_Frame);
            timeoutRFIDSensor = 200;
            RFID_SENSOR_CONNECTED = 1;
            break;
        case ID_MAGNETIC_SENSOR:
            decodeMagneticSensor(&RX_STD_CAN_BUF);
            timeoutMagSensor = 200;
            MAG_SENSOR_CONNECTED = 1;
            break;
        case ID_REC_BATERIA:
            volt_bateria.u8[1] = RX_STD_Frame.dataByte0;
            volt_bateria.u8[0] = RX_STD_Frame.dataByte1;
            cor_bateria.u8[1] = RX_STD_Frame.dataByte2;
            cor_bateria.u8[0] = RX_STD_Frame.dataByte3;
            timeoutCONNETIONBAT = 3000;
            CHARGER_CONNECTED = 1;
            if(volt_bateria.u16[0] == 534){
                BATT_FULL_CHARGE = 1;
                sendData(BATTERY_LEVEL_CMD);
            } else {
                BATT_FULL_CHARGE = 0;
            }
            break;
        case ID_REC_MOTOR_SPEED:
            READY_RECIVE = 1;
            if(operationMode == INIT_MODE){
                init_comp++;
            }
            break;
        case ID_REC_MOTOR_DIRECTION:
            READY_RECIVE = 1;
            LED_GREEN_TOGGLE();
            if(operationMode == INIT_MODE){
                init_comp++;
                if(init_comp == 5){
                    LED_GREEN_TOGGLE();
                    operationMode = BRAKE_MODE;
                }
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
        case ID_MOTOR_SPEED_TPDO1:
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
        case ID_MOTOR_DIR_TPDO1:
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

void createCanMessage(uint8_t msj) {
    if(READY_RECIVE){
        switch(msj){
            case ENABLE_MOTOR_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case DISABLE_MOTOR_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case SPEED_MODE_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case POSITION_MODE_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_M_BUF2, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case READY_POSI_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_M_BUF2, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case SPEED_MOTOR_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case POS_MOTOR_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_M_BUF2, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case INVERTIR_1_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case INVERTIR_2_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case ACC_SPEED_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case DEC_SPEED_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case SPEED_POS_CMD:
                chargeToCanBuf(DATA_STD, &TX_CAN_M_BUF2, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0] + 0x600));
                break;
            case START_PDO_SPEED_CMD:
                chargeToCanBuf(PDO, &TX_CAN_BUF, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0]));
                break;
            case START_PDO_DIR_CMD:
                chargeToCanBuf(PDO, &TX_CAN_M_BUF2, auxbufTX, FLEXCAN_ID_STD(auxbufTX[0]));
                break;
        }
        READY_RECIVE = 0;
        timeoutCAN_MESSAGE = 5;
    }
}

void chargeToCanBuf(uint8_t format, flexcan_mb_transfer_t *CANTransfer, uint8_t *data, uint32_t id) {
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

void flexcanCallback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData) {
    switch (status){
        case kStatus_FLEXCAN_RxIdle:
            if (RX_MESSAGE_STD_BUFFER_NUM == result)
            {
                RX_CAN_COMPLETE = 1;
                FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_STD_CAN_BUF);
            }
            break;
        case kStatus_FLEXCAN_RxOverflow:
            if (RX_MESSAGE_STD_BUFFER_NUM == result)
            {
                RX_CAN_COMPLETE = 1;
                FLEXCAN_TransferReceiveNonBlocking(CAN0, &flexcanHandle, &RX_STD_CAN_BUF);
            }
            break;
        case kStatus_FLEXCAN_TxIdle:
            break;
        case kStatus_FLEXCAN_TxBusy:
            break;
        default:
            break;
    }
}
