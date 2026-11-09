#ifndef CAN_COMMS_H_
#define CAN_COMMS_H_

#include <stdint.h>
#include <stdbool.h>
#include "fsl_flexcan.h"
#include "Util.h"

// ID definitions
#define ID_M_DIREC                              FLEXCAN_ID_STD(0x601) //ID CAN MOTOR DIRECCION
#define ID_M_VEL                                FLEXCAN_ID_STD(0x607) //ID CAN MOTOR VELOCIDAD O TRACCION
#define RX_MESSAGE_STD_BUFFER_NUM               (0)
#define RX_MESSAGE_EXT_BUFFER_NUM               (1)
#define TX_MESSAGE_BUFFER_NUM                   (2)
#define NEW_TX_MESSAGE_BUFFER_NUM               (3)
#define ID_RFID_SENSOR                          FLEXCAN_ID_STD(0x100) //ID SENSOR RFID
#define ID_MAGNETIC_SENSOR                      FLEXCAN_ID_STD(0x123) //ID SENSOR MAGNETICO
#define ID_SEND_BATERIA                         FLEXCAN_ID_EXT(0x1806E5F4)//ID de msjs enviados desde placa a la bateria
#define ID_REC_BATERIA                          FLEXCAN_ID_EXT(0x18FF50E5)//ID de msjs que recibe la placa desde la bateria
#define ID_REC_MOTOR_SPEED                      FLEXCAN_ID_STD(0x587) //ID RECEPCION SDO MOTOR VELOCIDAD
#define ID_REC_MOTOR_DIRECTION                  FLEXCAN_ID_STD(0x581) //ID RECEPCION SDO MOTOR DIRECCION
#define ID_MOTOR_SPEED_ALIVE                    FLEXCAN_ID_STD(0x707) //ID ALIVE MOTOR VELOCIDAD
#define ID_MOTOR_DIRECTION_ALIVE                FLEXCAN_ID_STD(0x701) //ID ALIVE MOTOR DIRECCION
#define ID_MOTOR_SPEED_TPDO1                    FLEXCAN_ID_STD(0x187) //ID TPDO1 MOTOR VELOCIDAD
#define ID_MOTOR_DIR_TPDO1                      FLEXCAN_ID_STD(0x181) //ID TPDO1 MOTOR DIRECCION
#define ID_MOTOR_SPEED_RPDO1                    FLEXCAN_ID_STD(0x207) //ID RPDO1 MOTOR VELOCIDAD
#define ID_MOTOR_DIR_RPDO1                      FLEXCAN_ID_STD(0x201) //ID RPDO1 MOTOR DIRECCION
#define ID_MOTOR_SPEED_RPDO2                    FLEXCAN_ID_STD(0x307) //ID RPDO2 MOTOR VELOCIDAD

// Format types
#define DATA_STD 0x0A
#define REMOTE_STD 0x1A
#define DATA_EXT 0x2B
#define REMOTE_EXT 0x3B
#define PDO 0x4B

// Flags & state types
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

extern flexcan_handle_t flexcanHandle;
extern flexcan_mb_transfer_t TX_CAN_BUF;
extern flexcan_mb_transfer_t TX_CAN_M_BUF2;
extern flexcan_mb_transfer_t RX_STD_CAN_BUF;

extern volatile _sFlag flag1;
extern volatile _sFlag flag2;
extern volatile _sFlag flag3;
extern volatile _sFlag flagFaults;

#define w                                   flag1.bit.b0
#define RX_CAN_COMPLETE                     flag1.bit.b1
#define BATT_FULL_CHARGE                    flag1.bit.b2
#define INIT_COMPLETE                       flag1.bit.b3
#define SELECT_MANUAL                       flag1.bit.b4
#define SELECT_AUTO                         flag1.bit.b5
#define FIRST_INIT                          flag1.bit.b6
#define READY_RECIVE                        flag1.bit.b7

#define MOTOR_VEL_CONNECTED                 flag2.bit.b0
#define MOTOR_DIR_CONNECTED                 flag2.bit.b1
#define MAG_SENSOR_CONNECTED                flag2.bit.b2
#define RFID_SENSOR_CONNECTED               flag2.bit.b3
#define HMI_CONNECTED                       flag2.bit.b4
#define datos_PDO_SPEED                     flag2.bit.b5
#define datos_PDO_DIR                       flag2.bit.b6
#define ALL_DEVICES_CONNECTED               flag2.bit.b7

#define CHARGER_CONNECTED                   flag3.bit.b0
#define CAN_MESSAGE_TEST                    flag3.bit.b1

#define GLOBAL_FAULT                        flagFaults.bit.b0
#define COMMUNICATION_FAULT                 flagFaults.bit.b1


extern uint16_t timeoutMotorVel;
extern uint16_t timeoutMotorDir;
extern uint16_t timeoutMagSensor;
extern uint16_t timeoutRFIDSensor;
extern uint16_t timeoutHMI;
extern uint16_t timeoutCAN_MESSAGE;
extern uint16_t timeoutCONNETIONBAT;
extern uint8_t auxbufTX[256];


void initCanComms(void);
void decodeCanMessage(void);
void createCanMessage(uint8_t msj);
void chargeToCanBuf(uint8_t format, flexcan_mb_transfer_t *CANTransfer, uint8_t *data, uint32_t id);
void flexcanCallback(CAN_Type *base, flexcan_handle_t *handle, status_t status, uint32_t result, void *userData);

#endif /* CAN_COMMS_H_ */
