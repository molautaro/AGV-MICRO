#ifndef QT_COMMS_H_
#define QT_COMMS_H_

#include <stdint.h>
#include <stdbool.h>
#include "canComms.h"
#include "motorControl.h"

#define MOTORSCMD                               0xD0
#define TESTCMD                                 0xD2
#define ALIVECMD                                0x0F

#define ENABLE_MOTOR_CMD                        0x01
#define DISABLE_MOTOR_CMD                       0x02
#define INVERTIR_1_CMD                          0x03
#define INVERTIR_2_CMD                          0x04
#define READY_POSI_CMD                          0x05
#define SPEED_MOTOR_CMD                         0x06
#define POS_MOTOR_CMD                           0x07
#define POSITION_MODE_CMD                       0xA1
#define SPEED_MODE_CMD                          0xA3
#define MANUAL_CMD                              0xA5
#define ACC_SPEED_CMD                           0xA6
#define DEC_SPEED_CMD                           0xA7
#define SPEED_POS_CMD                           0xA8
#define DISTANCE_SENSOR_CMD                     0xA9
#define DISTANCE_SENSOR_SIM_CMD                 0xAF
#define FAULT_CMD                               0xF0
#define RESET_PROGRAM                           0xA9
#define MOTOR_SPEED_DATA1_CMD                   0xB0
#define MOTOR_SPEED_DATA2_CMD                   0xB1
#define MOTOR_DIR_DATA1_CMD                     0xB2
#define MOTOR_DIR_DATA2_CMD                     0xB3
#define PID_PARAMETERS_CMD                      0xC0
#define MAG_SENSOR_SIM_CMD                      0xC1
#define START_PDO_SPEED_CMD                     0xD0
#define START_PDO_DIR_CMD                       0xD1
#define HMI_ALIVE_CMD                           0xD2
#define RESET_PROGRAM_CMD                       0xD3
#define DESTINATIONSTATION_CMD                  0xD4
#define DESTINOALCANZADO_CMD                    0xD5
#define ORIGENALCANZADO_CMD                     0xD6
#define CHANGE_CONTROL_CMD                      0xD8
#define OUT_OF_LINE_CMD                         0xD9
#define BATTERY_MODE_CMD                        0xDA
#define BATTERY_LEVEL_CMD                       0xDB
#define BATTERY_CONNECTION_CMD                  0xDC

#define ALIVE_RECIVE_CMD                        flagQT.bit.b0
#define ENABLE_RECIVE_CMD                       flagQT.bit.b1
#define DISABLE_RECIVE_CMD                      flagQT.bit.b2
#define INVERTIR1_RECIVE_CMD                    flagQT.bit.b3
#define INVERTIR2_RECIVE_CMD                    flagQT.bit.b4
#define SPEED_MODE_REC_CMD                      flagQT.bit.b5
#define POSITION_MODE_REC_CMD                   flagQT.bit.b6
#define READY_POS_REC_CMD                       flagQT.bit.b7

#define TARGET_SPEED_REC_CMD                    flagQT_2.bit.b0
#define TARGET_POS_REC_CMD                      flagQT_2.bit.b1
#define MANUAL_CMD_REC                          flagQT_2.bit.b2

extern volatile _sFlag flagQT;
extern volatile _sFlag flagQT_2;

extern volatile _rx ringRx, auxRX;
extern volatile _tx ringTx, auxTX;

extern uint8_t rxBuf[256];
extern uint8_t txBuf[256];
extern uint8_t auxbufRX[256];

void decodeQt(void);
void receiveData(uint8_t head);
void sendData(uint8_t cmd);
void actionQt(void);

#endif /* QT_COMMS_H_ */
