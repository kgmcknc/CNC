/*
 * motor_control.c
 *
 *  Created on: May 22, 2018
 *      Author: RHOLLOWAY
 */
#include <stddef.h>

#include "em_device.h"

#include "prod_gpio.h"

#include "motor_control.h"

#if 0
Motor_State_t MotorStates[Motor_NumberOfDevices];

void Motor_Init(Motor_DeviceId_t motor)
{
   switch(motor)
   {
      case Motor_DeviceId_X:
         // Motor X initialization
         GPIO_DirOut(GPIO_MOTOR_X_EN);
         GPIO_DirOut(GPIO_MOTOR_X_DIR);
         GPIO_DirOut(GPIO_MOTOR_X_STEP);
         GPIO_DirIn(GPIO_MOTOR_X_STOP);
         GPIO_DirIn(GPIO_MOTOR_X_HOME);
         break;

      case Motor_DeviceId_Y:
         // Motor Y initialization
         GPIO_DirOut(GPIO_MOTOR_Y_EN);
         GPIO_DirOut(GPIO_MOTOR_Y_DIR);
         GPIO_DirOut(GPIO_MOTOR_Y_STEP);
         GPIO_DirIn(GPIO_MOTOR_Y_STOP);
         GPIO_DirIn(GPIO_MOTOR_Y_HOME);
         break;

      case Motor_DeviceId_Z1:
         break;

      case Motor_DeviceId_Z2:
         break;

      default:
         // Hmmm....
         break;
   }

   // If the timer is already running no need to configure it again
   if((TIMER0->STATUS & TIMER_STATUS_RUNNING) == 0)
   {
      /* Cartesian Coordinate Timer
       * This timer will be used to drive 4 motors on the XYZ
       * axis. Motors on the Z-axis will share a single CC value,
       * but motors can be enabled separately to allow for leveling.
       */
      // Enable Timer 0
      CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER0;

      // 10.5 MHz up counter
      TIMER0->CTRL = TIMER_CTRL_PRESC_DIV4 |
                     TIMER_CTRL_CLKSEL_PRESCHFPERCLK |
                     TIMER_CTRL_MODE_UP;

      // Disable CC interrupts
      // Interrupts will be enabled when it is time for the motors to run.
      TIMER0->IEN = _TIMER_IEN_RESETVALUE;

      // Setup IRQ so when we do enable interrupts it is ready
      NVIC_ClearPendingIRQ(TIMER0_IRQn);
      NVIC_EnableIRQ(TIMER0_IRQn);

      // Output compare
      TIMER0->CC[0].CTRL = TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
      TIMER0->CC[1].CTRL = TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
      TIMER0->CC[2].CTRL = TIMER_CC_CTRL_MODE_OUTPUTCOMPARE;

      // Start Timer
      TIMER0->CNT = 0;
      TIMER0->TOP = _TIMER_TOP_RESETVALUE;
      TIMER0->CMD = TIMER_CMD_START;
   }
}

void Motor_InitAll(void)
{
   for(uint8_t x; x < Motor_NumberOfDevices; x++)
   {
      Motor_Init(x);
   }
}

void Motor_Run(Motor_DeviceId_t motor)
{
   if(!MotorStates[motor].IsRunning)
   {
      switch(motor)
      {
         case Motor_DeviceId_X:
            // Enable CC interrupt
            TIMER0->IEN |= TIMER_IEN_CC0;
            // CCV/2 for 50% duty cycle
            TIMER0->CC[0].CCV = TIMER0->CNT + (TIMER0_CLOCK/(MotorStates[Motor_DeviceId_X].Frequency*2));
            break;

         case Motor_DeviceId_Y:
            // Enable CC interrupt
            TIMER0->IEN |= TIMER_IEN_CC1;
            // CCV/2 for 50% duty cycle
            TIMER0->CC[1].CCV = TIMER0->CNT + (TIMER0_CLOCK/(MotorStates[Motor_DeviceId_Y].Frequency*2));
            break;

         case Motor_DeviceId_Z1:
            // Enable CC interrupt
            TIMER0->IEN |= TIMER_IEN_CC2;
            TIMER0->CC[2].CCV = TIMER0->CNT + (TIMER0_CLOCK/(MotorStates[Motor_DeviceId_Z1].Frequency*2));
            break;

         case Motor_DeviceId_Z2:
            // Enable CC interrupt
            TIMER0->IEN |= TIMER_IEN_CC2;
            // CCV/2 for 50% duty cycle
            TIMER0->CC[2].CCV = TIMER0->CNT + (TIMER0_CLOCK/(MotorStates[Motor_DeviceId_Z2].Frequency*2));
            break;

         default:
            // Hmmm....
            break;
      }

      MotorStates[motor].IsRunning = true;
   }
   else
   {
      // Motor is already running. This probably isn't the
      // right way to do what you're trying to do.
   }
}

void Motor_Stop(Motor_DeviceId_t motor)
{
   switch(motor)
   {
      case Motor_DeviceId_X:
         // Disable CC interrupt
         TIMER0->IEN &= ~TIMER_IEN_CC0;
         break;

      case Motor_DeviceId_Y:
         // Disable CC interrupt
         TIMER0->IEN &= ~TIMER_IEN_CC1;
         break;

      case Motor_DeviceId_Z1:
         // Disable CC interrupt
         TIMER0->IEN &= ~TIMER_IEN_CC2;
         break;

      case Motor_DeviceId_Z2:
         // Disable CC interrupt
         TIMER0->IEN &= ~TIMER_IEN_CC2;
         break;

      default:
         // Hmmm....
         break;
   }

   MotorStates[motor].IsRunning = false;
}

// IRQ functions //
// Step Counters //

void TIMER0_IRQHandler(void)
{
   if(TIMER0->IF & TIMER_IF_CC0)
   {
      TIMER0->IFC = TIMER_IFC_CC0;

      // Update next interrupt
      TIMER0->CC[0].CCV = TIMER0->CNT + (TIMER0_CLOCK/(MotorStates[Motor_DeviceId_X].Frequency*2));

      GPIO_Not(GPIO_MOTOR_X_STEP);
   }

   if(TIMER0->IF & TIMER_IF_CC1)
   {
      TIMER0->IFC = TIMER_IFC_CC1;

      // Update next interrupt
      TIMER0->CC[1].CCVB = TIMER0->CNT + (TIMER0_CLOCK/(MotorStates[Motor_DeviceId_Y].Frequency*2));

      GPIO_Not(GPIO_MOTOR_Y_STEP);
   }

   if(TIMER0->IF & TIMER_IF_CC2)
   {
      TIMER0->IFC = TIMER_IFC_CC2;

      // Update next interrupt
      TIMER0->CC[2].CCVB = TIMER0->CNT + (TIMER0_CLOCK/(MotorStates[Motor_DeviceId_Z1].Frequency*2));

      //GPIO_Not(GPIO_MOTOR_Z1_STEP);
      //GPIO_Not(GPIO_MOTOR_Z2_STEP);
   }
}
#endif
