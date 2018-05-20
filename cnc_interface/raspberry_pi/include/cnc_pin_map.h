
// mapping of cnc pins on the raspberry pi 3
// has physical name and wiringpi number

/*
*
* SD CARD 
+-----+-----+---------+------+---+---Pi 3---+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
 |   2 |   8 |   SDA.1 |   IN | 1 |  3 || 4  |   |      | 5v      |     |     |
 |   3 |   9 |   SCL.1 |   IN | 1 |  5 || 6  |   |      | 0v      |     |     |
 |   4 |   7 | GPIO. 7 |   IN | 1 |  7 || 8  | 0 | IN   | TxD     | 15  | 14  |
 |     |     |      0v |      |   |  9 || 10 | 1 | IN   | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 |   IN | 0 | 11 || 12 | 0 | IN   | GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 |   IN | 0 | 13 || 14 |   |      | 0v      |     |     |
 |  22 |   3 | GPIO. 3 |   IN | 0 | 15 || 16 | 0 | IN   | GPIO. 4 | 4   | 23  |
 |     |     |    3.3v |      |   | 17 || 18 | 0 | IN   | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI |   IN | 0 | 19 || 20 |   |      | 0v      |     |     |
 |   9 |  13 |    MISO |   IN | 0 | 21 || 22 | 0 | IN   | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK |   IN | 0 | 23 || 24 | 1 | IN   | CE0     | 10  | 8   |
 |     |     |      0v |      |   | 25 || 26 | 1 | IN   | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 |   IN | 1 | 27 || 28 | 1 | IN   | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.21 |   IN | 1 | 29 || 30 |   |      | 0v      |     |     |
 |   6 |  22 | GPIO.22 |   IN | 1 | 31 || 32 | 0 | IN   | GPIO.26 | 26  | 12  |
 |  13 |  23 | GPIO.23 |   IN | 0 | 33 || 34 |   |      | 0v      |     |     |
 |  19 |  24 | GPIO.24 |   IN | 0 | 35 || 36 | 1 | IN   | GPIO.27 | 27  | 16  |
 |  26 |  25 | GPIO.25 |   IN | 0 | 37 || 38 | 0 | IN   | GPIO.28 | 28  | 20  |
 |     |     |      0v |      |   | 39 || 40 | 0 | IN   | GPIO.29 | 29  | 21  |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+---Pi 3---+---+------+---------+-----+-----+
*    
* ETHERNET | USB
*/

/*
              NC - 01 | 02 - MAIN POWER 5V
              NC - 03 | 04 - NC
              NC - 05 | 06 - NC
     RED F/B DIR - 07 | 08 - NC
              NC - 09 | 10 - NC
     RED F/B STP - 11 | 12 - LEFT GREEN (FRONT/MOTOR/CLOSE) END STOP
     RED F/B MS2 - 13 | 14 - NC
     RED F/B MS1 - 15 | 16 - RED LEFT (LEFT MOTOR) END STOP
              NC - 17 | 18 - RED RIGHT (RIGHT) END STOP
              NC - 19 | 20 - NC
              NC - 21 | 22 - RIGHT GREEN (BACK/FAR) END STOP
              NC - 23 | 24 - NC
              NC - 25 | 26 - NC
              NC - 27 | 28 - NC
              NC - 29 | 30 - NC
  YELLOW L/R DIR - 31 | 32 - NC
     RED L/R STP - 33 | 34 - GREEN GROUND LEFT 4
     RED L/R MS2 - 35 | 36 - MOTOR DRIVER RELAY POWER
     RED L/R MS1 - 37 | 38 - MOTOR DRIVER SLEEP
              NC - 39 | 40 - MOTOR DRIVER ENABLE
*/

// pin defines
#define DRIVER_POWER_RELAY    27
#define DRIVER_SLEEP          28
#define DRIVER_ENABLE         29

#define F_END_STOP            01
#define B_END_STOP            06
#define L_END_STOP            04
#define R_END_STOP            05

#define L_MOTOR_STP           23
#define L_MOTOR_DIR           22
#define L_MOTOR_MS2           24
#define L_MOTOR_MS1           25

#define F_MOTOR_STP           00
#define F_MOTOR_DIR           07
#define F_MOTOR_MS2           02
#define F_MOTOR_MS1           03

// value defines for pins
#define MOTOR_DRIVER_OFF      HIGH
#define MOTOR_DRIVER_ON       LOW

#define MOTOR_RESET_ON        LOW
#define MOTOR_RESET_OFF       HIGH

#define MOTOR_ACTIVE          LOW
#define MOTOR_STANDBY         HIGH

#define ENDSTOP_HIT           HIGH
#define ENDSTOP_OFF           LOW

#define MOTOR_MOVE_R          HIGH
#define MOTOR_MOVE_L          LOW
#define MOTOR_MOVE_F          HIGH
#define MOTOR_MOVE_B          LOW

