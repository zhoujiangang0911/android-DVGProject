#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termio.h>
#include <time.h>
#include "serial.h"
#include "util.h"

int baudflag_arr[] = {
    B921600, B460800, B230400, B115200, B57600, B38400,
    B19200, B9600, B4800, B2400, B1800, B1200,
    B600, B300, B150, B110, B75, B50
};

int speed_arr[] = {
    921600, 460800, 230400, 115200, 57600, 38400,
    19200, 9600, 4800, 2400, 1800, 1200,
    600, 300, 150, 110, 75, 50
};

int initSerial(void){
    int fd = open(DEV_SERIAL_PORT, O_RDWR, 0);
    fprintf(stderr, "fd=%d\n",fd);
    
    if (fd < 0) {
        fprintf(stderr, "open <%s> error %s\n", DEV_SERIAL_PORT, strerror(errno));
        return fd;
    }

    if (setup_port(fd, SERIAL_BAUDRATE, 8, 0, 1)) {
        fprintf(stderr, "setup_port error %s\n", strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}

/******************************************************************************
 * NAME:
 * speed_to_flag
 *
 * DESCRIPTION:
 * Translate baudrate into flag
 *
 * PARAMETERS:
 * speed - The baudrate to convert
 *
 * RETURN:
 * The flag
 ******************************************************************************/
int speed_to_flag(int speed)
{
    int i;

    for (i = 0; i < (int)(sizeof(speed_arr)/sizeof(int)); i++) {
        if (speed == speed_arr[i]) {
            return baudflag_arr[i];
        }
    }

    fprintf(stderr, "Unsupported baudrate, use 9600 instead!\n");
    return B9600;
}


static struct termio oterm_attr;

/******************************************************************************
 * NAME:
 * stup_port
 *
 * DESCRIPTION:
 * Set serial port (include baudrate, line control)
 *
 * PARAMETERS:
 * fd - The fd of serial port to setup
 * baud - Baudrate: 9600, ...
 * databits - Databits: 5, 6, 7, 8
 * parity - Parity: 0(None), 1(Odd), 2(Even)
 * stopbits - Stopbits: 1, 2
 *
 * RETURN:
 * 0 for OK; Others for ERROR
 ******************************************************************************/
int setup_port(int fd, int baud, int databits, int parity, int stopbits)
{
    struct termio term_attr;

    /* Get current setting */
    if (ioctl(fd, TCGETA, &term_attr) < 0) {
        return -1;
    }

    /* Backup old setting */
    memcpy(&oterm_attr, &term_attr, sizeof(struct termio));

    term_attr.c_iflag &= ~(INLCR | IGNCR | ICRNL | ISTRIP | IXON);
    term_attr.c_oflag &= ~(OPOST | ONLCR | OCRNL);
    term_attr.c_lflag &= ~(ISIG | ECHO | ICANON | NOFLSH);
    term_attr.c_cflag &= ~CBAUD;
    term_attr.c_cflag |= CREAD | speed_to_flag(baud);

    /* Set databits */
    term_attr.c_cflag &= ~(CSIZE);
    switch (databits) {
        case 5:
            term_attr.c_cflag |= CS5;
            break;

        case 6:
            term_attr.c_cflag |= CS6;
            break;

        case 7:
            term_attr.c_cflag |= CS7;
            break;

        case 8:
        default:
            term_attr.c_cflag |= CS8;
            break;
    }

    /* Set parity */
    switch (parity) {
        case 1: /* Odd parity */
            term_attr.c_cflag |= (PARENB | PARODD);
            break;

        case 2: /* Even parity */
            term_attr.c_cflag |= PARENB;
            term_attr.c_cflag &= ~(PARODD);
            break;

        case 0: /* None parity */
        default:
            term_attr.c_cflag &= ~(PARENB);
            break;
    }


    /* Set stopbits */
    switch (stopbits) {
        case 2: /* 2 stopbits */
            term_attr.c_cflag |= CSTOPB;
            break;

        case 1: /* 1 stopbits */
        default:
            term_attr.c_cflag &= ~CSTOPB;
            break;
    }

    term_attr.c_cc[VMIN] = 1;
    term_attr.c_cc[VTIME] = 0;

    if (ioctl(fd, TCSETAW, &term_attr) < 0) {
        return -1;
    }

    if (ioctl(fd, TCFLSH, 2) < 0) {
        return -1;
    }

    return 0;
}


/******************************************************************************
 * NAME:
 * read_data
 *
 * DESCRIPTION:
 * Read data from serial port
 *
 *
 * PARAMETERS:
 * fd - The fd of serial port to read
 * buf - The buffer to keep readed data
 * len - The max count to read
 *
 * RETURN:
 * Count of readed data
 ******************************************************************************/
int read_data(int fd, char *buf, int len)
{
    int count;
    int ret;

    ret = 0;
    count = 0;

    ret = read(fd, (char*)buf + count, len);
    if (ret < 1) {
        fprintf(stderr, "Read error %s\n", strerror(errno));
    }

    count += ret;
    len = len - ret;


    *((char*)buf + count) = 0;
    return count;
}


/******************************************************************************
 * NAME:
 * write_data
 *
 * DESCRIPTION:
 * Write data to serial port
 *
 *
 * PARAMETERS:
 * fd - The fd of serial port to write
 * buf - The buffer to keep data
 * len - The count of data
 *
 * RETURN:
 * Count of data wrote
 ******************************************************************************/
int write_data(int fd, char *buf, int len)
{
    int count;
    int ret;

    ret = 0;
    count = 0;

    while (len > 0) {

        ret = write(fd, (char*)buf + count, len);
        if (ret < 1) {
            fprintf(stderr, "Write error %s\n", strerror(errno));
            break;
        }

        count += ret;
        len = len - ret;
    }

    return count;
}


/******************************************************************************
 * NAME:
 * reset_port
 *
 * DESCRIPTION:
 * Restore original setting of serial port
 *
 * PARAMETERS:
 * fd - The fd of the serial port
 *
 * RETURN:
 * 0 for OK; Others for ERROR
 ******************************************************************************/
int reset_port(int fd)
{
    if (ioctl(fd, TCSETAW, &oterm_attr) < 0) {
        return -1;
    }

    return 0;
}


/******************************************************************************
 * NAME:
 * close_port
 *
 * DESCRIPTION:
 * close the serial port
 *
 * PARAMETERS:
 * fd - The fd of the serial port
 *
 * RETURN:
 * void
 ******************************************************************************/
void close_port(int fd)
{
    close(fd);
}
