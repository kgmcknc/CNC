
#include "machine_gpio.h"
#include <SoftwareSerial.h>

#define MOTOR_ON 0
#define MOTOR_OFF 1
#define MOTOR_DIR_POS 0
#define MOTOR_DIR_NEG 1
#define MOTOR_SET_STEP 0
#define MOTOR_CLR_STEP 1

void check_motor(int es_min, int es_max, int m_en, int m_dir, int m_step);

void setup(){
   pinMode(X_MIN_ES, INPUT);
   pinMode(X_MAX_ES, INPUT);
   pinMode(Y_MIN_ES, INPUT);
   pinMode(Y_MAX_ES, INPUT);
   pinMode(ZL_MIN_ES, INPUT);
   pinMode(ZL_MAX_ES, INPUT);
   pinMode(ZR_MIN_ES, INPUT);
   pinMode(ZR_MAX_ES, INPUT);
   
   pinMode(M_0_STEP, OUTPUT);
   pinMode(M_0_DIR, OUTPUT);
   pinMode(M_0_EN, OUTPUT);
   digitalWrite(M_0_STEP, 1);
   digitalWrite(M_0_DIR, 1);
   digitalWrite(M_0_EN, 1);
   
   pinMode(M_1_STEP, OUTPUT);
   pinMode(M_1_DIR, OUTPUT);
   pinMode(M_1_EN, OUTPUT);
   digitalWrite(M_1_STEP, 1);
   digitalWrite(M_1_DIR, 1);
   digitalWrite(M_1_EN, 1);
   
   pinMode(M_2_STEP, OUTPUT);
   pinMode(M_2_DIR, OUTPUT);
   pinMode(M_2_EN, OUTPUT);
   digitalWrite(M_2_STEP, 1);
   digitalWrite(M_2_DIR, 1);
   digitalWrite(M_2_EN, 1);
   
   pinMode(M_3_STEP, OUTPUT);
   pinMode(M_3_DIR, OUTPUT);
   pinMode(M_3_EN, OUTPUT);
   digitalWrite(M_3_STEP, 1);
   digitalWrite(M_3_DIR, 1);
   digitalWrite(M_3_EN, 1);
   
   pinMode(M_4_STEP, OUTPUT);
   pinMode(M_4_DIR, OUTPUT);
   pinMode(M_4_EN, OUTPUT);
   digitalWrite(M_4_STEP, 1);
   digitalWrite(M_4_DIR, 1);
   digitalWrite(M_4_EN, 1);
   
   pinMode(M_5_STEP, OUTPUT);
   pinMode(M_5_DIR, OUTPUT);
   pinMode(M_5_EN, OUTPUT);
   digitalWrite(M_5_STEP, 1);
   digitalWrite(M_5_DIR, 1);
   digitalWrite(M_5_EN, 1);
   
   Serial.begin(57600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("Machine!");
   
}

char motor = 0;
void loop(){
   delay(10);
   check_motor(X_MIN_ES, X_MAX_ES, M_0_EN, M_0_DIR, M_0_STEP);
   check_motor(Y_MIN_ES, Y_MAX_ES, M_1_EN, M_1_DIR, M_1_STEP);
   check_motor(ZL_MIN_ES, ZL_MAX_ES, M_2_EN, M_2_DIR, M_2_STEP);
   check_motor(ZR_MIN_ES, ZR_MAX_ES, M_3_EN, M_3_DIR, M_3_STEP);
}

void check_motor(int es_min, int es_max, int m_en, int m_dir, int m_step){
   if(!digitalRead(es_max)){
      digitalWrite(m_en, MOTOR_ON);
      digitalWrite(m_dir, MOTOR_DIR_POS);
      digitalWrite(m_step, MOTOR_SET_STEP);
   } else {
      if(digitalRead(es_min)){
         digitalWrite(m_en, MOTOR_OFF);
      }
   }
   if(!digitalRead(es_min)){
      digitalWrite(m_en, MOTOR_ON);
      digitalWrite(m_dir, MOTOR_DIR_NEG);
      digitalWrite(m_step, MOTOR_SET_STEP);
   } else {
      if(digitalRead(es_max)){
         digitalWrite(m_en, MOTOR_OFF);
      }
   }
   digitalWrite(m_step, MOTOR_CLR_STEP);
}


