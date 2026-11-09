#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

#include <stdint.h>
#include <stdbool.h>
#include "canComms.h"
#include "fsl_flexcan.h"

#define INIT_MODE                               0x00
#define MANUAL_MODE                             0x01
#define AUTOMATIC_MODE                          0x02
#define BRAKE_MODE                              0x03
#define CHARGE_BATTERY_MODE                     0x04

#define DISTANCE1 20 //distancia maxima de medida
#define DISTANCE2 10 //distancia media
#define DISTANCE3 5 //distancia minima

extern uint8_t operationMode;
extern uint8_t init_comp;
extern uint8_t brakestatus;
extern uint16_t timeoutINIT;
extern uint16_t timeoutBRAKE;
extern uint16_t timeoutMOTORS;
extern uint16_t timeoutBAT;
extern uint16_t timeoutDIREC;

extern _sWork volt_bateria;
extern _sWork cor_bateria;
extern _sWork Distance_Sensor_REAL;
extern _sWork Distance_Sensor_SIMULATION;
extern _sWork Kp_SteeringMotor;
extern _sWork Kd_SteeringMotor;
extern _sWork Ki_SteeringMotor;
extern _sWork DestinationStation[2];
extern _sWork RFIDData[2];

extern _sWork RealPositionDIR, StatusWordDIR, RealCurrentDIR;
extern _sWork RealSpeedVEL, StatusWordVEL, RealCurrentVEL;

extern volatile _sWork SpeedMotorCalcRPM, SpeedMotorCalcRPMAUX, SpeedMotorCalcDEC;
extern volatile _sWork PosSend;

extern const uint8_t CARGA_BAT_MESSAGE[];
extern const uint8_t SPEED_MODE_MESSAGE[];
extern const uint8_t POS_MODE_MESSAGE[];
extern const uint8_t ENABLE_MVEL[];
extern const uint8_t ENABLE_MPOS[];
extern const uint8_t DISABLE_MVEL[];
extern const uint8_t DISABLE_MPOS[];
extern const uint8_t READY_POS[];
extern const uint8_t INVERTIR_1[];
extern const uint8_t INVERTIR_2[];
extern const uint8_t START_PDO_SPEED_MESSAGE[];
extern const uint8_t START_PDO_DIR_MESSAGE[];
extern const uint8_t ACC_SPEED_10[];
extern const uint8_t ACC_SPEED_100[];
extern const uint8_t ACC_SPEED_150[];
extern const uint8_t DESA_SPEED_10[];
extern const uint8_t DESA_SPEED_100[];
extern const uint8_t DESA_SPEED_150[];
extern const uint8_t POS_SPEED[];

extern uint16_t magneticSensorBitStatus;
extern uint16_t COORD_SENSORES[8];
extern uint8_t cont_perd;

void workingMode(void);
void speedMotorControl(void);
uint32_t speedConvertionRPMtoDEC(float rpmSpeed);
float speedControlCalc(float d, int Vmax);
void steeringMotorControl(void);
int32_t positionConvert(int32_t pos_degree);
void positionMotorControl(void);
void brakeControl(void);
void checkDeviceConnection(void);
void checkFaults(void);
void decodeRfidSensor(flexcan_frame_t *rxFrame);
void decodeMagneticSensor(flexcan_mb_transfer_t *rxBuf);

#endif /* MOTOR_CONTROL_H_ */
