#include <wiringPi.h>
#include "cnc_pin_map.h"

#define NORMAL_SPEED 50

void process_motors(void);
void process_motion(void);
void reset_motor_state(void);
void reset_area_state(void);
void init_control_gpio(void);
void disable_control_gpio(void);
void motor_x_right(void);
void motor_x_left(void);
void motor_y_toward(void);
void motor_y_away(void);
void motor_x_high(void);
void motor_x_low(void);
void motor_y_high(void);
void motor_y_low(void);
void check_x_endstop(void);
void check_y_endstop(void);
void check_z_endstop(void);

struct m_position {
	unsigned long int x;
	char x_l_stop;
	char x_r_stop;
	unsigned long int y;
	char y_c_stop;
	char y_f_stop;
	unsigned long int z;
	char z_t_stop;
	char z_b_stop;
};
extern struct m_position beg_pos, cur_pos, end_pos;

struct m_movement {
	unsigned int x_count;
	unsigned int x_total;
	char x_act;
	char x_dir;
	unsigned int y_count;
	unsigned int y_total;
	char y_act;
	char y_dir;
	unsigned int z_count;
	unsigned int z_total;
	char z_act;
	char z_dir;
};
extern struct m_movement beg_mov, cur_mov, end_mov;

struct w_area {
	unsigned long int x;
	char x_calibrated;
	unsigned long int y;
	char y_calibrated;
	unsigned long int z;
	char z_calibrated;
};
extern struct w_area w_size;

extern char system_calibrated;
extern char motors_enabled;
