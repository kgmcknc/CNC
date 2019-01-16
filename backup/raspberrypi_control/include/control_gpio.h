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
	double x;
	uint8_t x_l_stop;
	uint8_t x_r_stop;
	double y;
	uint8_t y_c_stop;
	uint8_t y_f_stop;
	double z;
	uint8_t z_t_stop;
	uint8_t z_b_stop;
};
extern struct machine_position start_position, current_position, end_position;

struct motor_movement {
	double x_period_count;
	double x_move_count;
	double x_move;
	double x_period;
	double x_current_period;
	double x_next_period;
	double x_arc_center;
	double x_arc_radius;
	int8_t x_arc;
	int8_t x_arc_cw;
	int8_t x_high;
	int8_t x_act;
	int8_t x_dir;
	double y_period_count;
	double y_period;
	double y_current_period;
	double y_next_period;
	double y_move_count;
	double y_move;
	double y_arc_center;
    double y_arc_radius;
	int8_t y_arc;
	int8_t y_arc_cw;
	int8_t y_high;
	int8_t y_act;
	int8_t y_dir;
	double z_period_count;
	double z_period;
	double z_current_period;
	double z_next_period;
	double z_move_count;
	double z_move;
	double z_arc_center;
	double z_arc_radius;
	int8_t z_arc;
	int8_t z_arc_cw;
	int8_t z_high;
	int8_t z_act;
	int8_t z_dir;
	double line_count;
	double arc_period;
};
extern struct motor_movement current_move, next_move;

struct work_area {
	double x;
	int8_t x_calibrated;
	double y;
	int8_t y_calibrated;
	double z;
	int8_t z_calibrated;
};
extern struct work_area w_size;

struct system_state {
	int8_t get_new_move;
	
};
extern struct system_state system_status;

extern int8_t system_calibrated;
extern int8_t motors_enabled;
