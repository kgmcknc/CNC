
#define SPI_CHANNEL 0
#define SPI_SPEED 10000000

void control_functions(void);
void enable_motors(void);
void disable_motors(void);
void move(char move_string[200]);
void calibrate_system(void);
void process_calibration(void);
void send_spi_string(char spi_string[200]);
void receive_spi_string(void);

extern char quit_system;
extern char calibration_running;
