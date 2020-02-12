
#ifndef SRC_INTERFACE_SERIAL_H_
#define SRC_INTERFACE_SERIAL_H_

#include "stdint.h"
#include "common_serial.h"
#include "wiringSerial.h"

extern int serial_fd;
extern serial_class cnc_serial;

#define MAX_COMM_TRANSFER MAX_TRANSFER_SIZE

#endif /* SRC_INTERFACE_SERIAL_H_ */
