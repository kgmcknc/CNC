/*
 * motor_control.h
 *
 *  Created on: May 22, 2018
 *      Author: RHOLLOWAY
 */

#ifndef CNC_CONTROL_MOTOR_CONTROL_H_
#define CNC_CONTROL_MOTOR_CONTROL_H_

#if 0
#define TIMER0_CLOCK     (HFPER_CLOCK/4UL)
#endif

typedef enum
{
   Motor_DeviceId_X,
   Motor_DeviceId_Y,
   Motor_DeviceId_Z1,
   Motor_DeviceId_Z2,
   Motor_NumberOfDevices,      ///< Number of Supported Motor Devices. (Must be last in list)
}  Motor_DeviceId_t;

typedef struct
{
   Motor_DeviceId_t            DeviceId;
   uint16_t                    Frequency;
   uint16_t                    Position;
   uint16_t                    Max;
   uint16_t                    Min;
   bool                        IsRunning;
}  Motor_State_t;

extern Motor_State_t MotorStates[Motor_NumberOfDevices];

void Motor_InitAll(void);
void Motor_Run(Motor_DeviceId_t motor);
void Motor_Stop(Motor_DeviceId_t motor);

#endif /* CNC_CONTROL_MOTOR_CONTROL_H_ */
