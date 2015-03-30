#ifndef _SERIAL_H_
#define _SERIAL_H_


#include "config.h"



 int setup_port(int fd, int baud, int databits, int parity, int stopbits);
 int reset_port(int fd);
 int read_data(int fd, char *buf, int len);
 int write_data(int fd, char *buf, int len);
int initSerial(void);
 void close_port(int fd);

#endif
