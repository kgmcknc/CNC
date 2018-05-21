#include <wiringPi.h>
#include "cnc_pin_map.h"


void process_motors(void);
void reset_motor_state(void);
void reset_area_state(void);
void init_control_gpio(void);
void disable_control_gpio(void);
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

struct m_dynamics {
	char x;
	unsigned int x_total;
	char x_dir;
	char y;
	unsigned int y_total;
	char y_dir;
	char z;
	unsigned int z_total;
	char z_dir;
};
extern struct m_dynamics beg_dyn, cur_dyn, end_dyn;

struct w_area {
	unsigned long int x;
	char x_calibrated;
	unsigned long int y;
	char y_calibrated;
	unsigned long int z;
	char z_calibrated;
};
extern struct w_area w_size;
