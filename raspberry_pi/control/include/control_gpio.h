#include <wiringPi.h>
#include "cnc_pin_map.h"

#define NORMAL_SPEED 50
#define START_SPEED 500
#define FASTEST_SPEED 2
#define PERIOD_RAMP 5

void process_motors(void);
void process_motion(void);
void process_instruction(void);
void process_speed(void);
void reset_motor_state(void);
void reset_area_state(void);
void init_control_gpio(void);
void disable_control_gpio(void);
void motor_x_right(void);
void motor_x_left(void);
void motor_y_toward(void);
void motor_y_away(void);
void motor_z_down(void);
void motor_z_up(void);
void motor_x_high(void);
void motor_x_low(void);
void motor_y_high(void);
void motor_y_low(void);
void motor_zr_high(void);
void motor_zr_low(void);
void motor_zl_high(void);
void motor_zl_low(void);
void check_x_endstop(void);
void check_y_endstop(void);
void check_z_endstop(void);

struct machine_position {
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
extern struct machine_position start_position, current_position, end_position;

struct motor_movement {
	unsigned int x_period_count;
	unsigned long int x_move_count;
	unsigned long int x_move;
	unsigned int x_period;
	unsigned int x_current_period;
	unsigned int x_next_period;
	unsigned long int x_arc_center;
	unsigned long int x_arc_radius;
	char x_arc;
	char x_arc_cw;
	char x_high;
	char x_act;
	char x_dir;
	unsigned int y_period_count;
	unsigned int y_period;
	unsigned int y_current_period;
	unsigned int y_next_period;
	unsigned long int y_move_count;
	unsigned long int y_move;
	unsigned long int y_arc_center;
	unsigned long int y_arc_radius;
	char y_arc;
	char y_arc_cw;
	char y_high;
	char y_act;
	char y_dir;
	unsigned int z_period_count;
	unsigned int z_period;
	unsigned int z_current_period;
	unsigned int z_next_period;
	unsigned long int z_move_count;
	unsigned long int z_move;
	unsigned long int z_arc_center;
	unsigned long int z_arc_radius;
	char z_arc;
	char z_arc_cw;
	char z_high;
	char z_act;
	char z_dir;
	unsigned long int line_count;
	unsigned long int arc_period;
};
extern struct motor_movement current_move, next_move;

struct work_area {
	unsigned long int x;
	char x_calibrated;
	unsigned long int y;
	char y_calibrated;
	unsigned long int z;
	char z_calibrated;
};
extern struct work_area w_size;

struct system_state {
	char get_new_move;
	
};
extern struct system_state system_status;

extern char system_calibrated;
extern char motors_enabled;
