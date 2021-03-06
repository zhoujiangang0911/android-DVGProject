/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termio.h>
#include <android/log.h>
#include <time.h>
/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
#define MAX_BUF_SIZE 2048
char buf[MAX_BUF_SIZE+2];

#define MY_END_CHAR 0x13
#define  LOG_TAG    "zhoujg77"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


int setup_port(int fd, int baud, int databits, int parity, int stopbits);
int reset_port(int fd);
int read_data(int fd, void *buf, int len);
int write_data(int fd, void *buf, int len);
void print_usage(char *program_name);

jstring
Java_com_example_hello_MainActivity_stringFromJNI( JNIEnv* env,
                                                  jobject thiz,jstring strdev
//                                                  jstring strbps,jstring strlen ,
//                                                  jstring strcount,jstring strdatabits,
//                                                  jstring strreadorwrite
												)
{
#if defined(__arm__)
  #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a/NEON (hard-float)"
      #else
        #define ABI "armeabi-v7a/NEON"
      #endif
    #else
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a (hard-float)"
      #else
        #define ABI "armeabi-v7a"
      #endif
    #endif
  #else
   #define ABI "armeabi"
  #endif
#elif defined(__i386__)
   #define ABI "x86"
#elif defined(__x86_64__)
   #define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
   #define ABI "mips64"
#elif defined(__mips__)
   #define ABI "mips"
#elif defined(__aarch64__)
   #define ABI "arm64-v8a"
#else
   #define ABI "unknown"
#endif
	const jbyte *dev ;
//	const jbyte *bps ;
//	const jbyte *lens ;
//	const jbyte *counts ;
//	const jbyte *databitss ;
//	const jbyte *readorwrite ;
	jint s;
	jboolean isCopy;
	dev  = (*env)->GetStringUTFChars(env,strdev,&isCopy);
//	bps  = (*env)->GetStringUTFChars(env,strbps,&isCopy);
//	lens  = (*env)->GetStringUTFChars(env,strlen,&isCopy);
//	counts  = (*env)->GetStringUTFChars(env,strcount,&isCopy);
//	databitss  = (*env)->GetStringUTFChars(env,strdatabits,&isCopy);
//	readorwrite  = (*env)->GetStringUTFChars(env,strreadorwrite,&isCopy);
  //  return (*env)->NewStringUTF(env, "Hello from JNI !  Compiled with ABI " ABI ".");
	int fd;
		    int baud;
		    int len;
		    int count;
		    int i;
		    int databits;
		    int stopbits;
		    int parity;
		    int read_or_write;
		    char *argv = dev;
		    char *a ;

	// int a  = __android_log_write(ANDROID_LOG_INFO,strdev,dev);
	 //__android_log_assert(strdev,dev);
	 //__android_log_print(ANDROID_LOG_ERROR,"HELLO-JNI","ASDFASDF%d",errno);

		   // log_verbose("ERRON IS NOE %d",errno,dev);

	  LOGI("zhoujg77------%s\n", argv);
	  //LOGI("zhoujg77argvargv[2]------%s\n", argv[2]);
//		    if (argc != 7) {
//		        print_usage(argv[0]);
//		        return 1;
//		    }


		    //baud = atoi(argv[2]);
		    baud = 921600;
		    LOGI("zhoujg77baud------%d\n", baud);
//		    if ((baud < 0) || (baud > 921600)) {
//		        fprintf(stderr, "Invalid baudrate!\n");
//		        return 1;
//		    }

		//    databits = atoi(argv[3]);
		    databits = 8;
		    LOGI("databits------%d\n", databits);
//		    if ((databits < 5) || (databits > 8)) {
//		        fprintf(stderr, "Invalid databits!\n");
//		        return 1;
//		    }

		   // parity = atoi(argv[4]);
		    parity = 0;
		    LOGI("parity------%d\n", parity);
//		    if ((parity < 0) || (parity > 2)) {
//		        fprintf(stderr, "Invalid parity!\n");
//		        return 1;
//		    }

		 //   stopbits = atoi(argv[5]);
		    stopbits = 1;
		    LOGI("stopbits------%d\n", stopbits);
//		    if ((stopbits < 1) || (stopbits > 2)) {
//		        fprintf(stderr, "Invalid stopbits!\n");
//		        return 1;
//		    }

		    read_or_write = 0;
		    LOGI("read_or_write------%d\n", read_or_write);

		    fd = open(argv, O_RDWR, 0);
		    LOGI("fd------a%d\n", fd);
		    LOGI(" errno---%s.", strerror(errno));
		    if (fd < 0) {
		       // fprintf(stderr, "open <%s> error %s\n", argv[1], strerror(errno));
		        LOGI("zhoujg77------%s\n", "meiyoudaikai");
		        return 1;
		    }

		    if (setup_port(fd, baud, databits, parity, stopbits)) {
		        fprintf(stderr, "setup_port error %s\n", strerror(errno));
		        close(fd);
		        return 1;
		    }

		    count = 0;

		    if (read_or_write) {
		      //  fprintf(stderr, "Begin to send:\n");

		        while ( (len = read(0, buf, MAX_BUF_SIZE)) > 0 ) {
		            if (len == 1) {
		                buf[0] = MY_END_CHAR;
		                buf[1] = 0;
		                write_data(fd, buf, len);
		                break;
		            }

		            /* send a pack */
		            i = write_data(fd, buf, len);
		            if (i == 0) {
		                fprintf(stderr, "Send data error!\n");
		                break;
		            }

		            //count += len;

		            //fprintf(stderr, "Send %d bytes\n", len);

		        }

		    } else {

		        fprintf(stderr, "Begin to recv:\n");
		        LOGI("zhoujg77------%s\n", "start read");
		        len = MAX_BUF_SIZE;

		        while (1) {
		            /* read a pack */
		        	  LOGI("zhoujg77------%s\n", "start read");
		            i = read_data(fd, buf, len);
		            if (i > 0) {
		                //count += i;
		                //fprintf(stderr, "Recv %d byte\n", i);
		                printf("%s", buf);
		                LOGI("zhoujg77buf------%s\n", buf);
		                int len = i;
		                int j;
		                LOGI("start\n");
		                for(j=0;j<len;j++){
		                	LOGI("0x%02X ",buf[j]&0xff);
		                }
		                LOGI("end\n");
		                	a = buf;
		                	LOGI("##############################\n");
		                if (buf[i-1] == MY_END_CHAR) {
		                    break;
		                }
		            }
		        }
		    }

		    reset_port(fd);
		    close(fd);
    return (*env)->NewStringUTF(env,a);
}
static int baudflag_arr[] = {
    B921600, B460800, B230400, B115200, B57600, B38400,
    B19200, B9600, B4800, B2400, B1800, B1200,
    B600, B300, B150, B110, B75, B50
};
static int speed_arr[] = {
    921600, 460800, 230400, 115200, 57600, 38400,
    19200, 9600, 4800, 2400, 1800, 1200,
    600, 300, 150, 110, 75, 50
};


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

    for (i = 0; i < sizeof(speed_arr)/sizeof(int); i++) {
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

    term_attr.c_iflag &= ~(INLCR | IGNCR | ICRNL | ISTRIP);
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
int read_data(int fd, void *buf, int len)
{
    int count;
    int ret;

    ret = 0;
    count = 0;

    //while (len > 0) {


    ret = read(fd, (char*)buf + count, len);
    if (ret < 1) {
        fprintf(stderr, "Read error %s\n", strerror(errno));
        //break;

    }

    count += ret;
    len = len - ret;

    //}


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
int write_data(int fd, void *buf, int len)
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
 * print_usage
 *
 * DESCRIPTION:
 * print usage info
 *
 * PARAMETERS:
 * program_name - The name of the program
 *
 * RETURN:
 * None
 ******************************************************************************/
void print_usage(char *program_name)
{
    fprintf(stderr,
            "*************************************\n"
            " A Simple Serial Port Test Utility\n"
            "*************************************\n\n"
            "Usage:\n %s <device> <baud> <databits> <parity> <stopbits> <read_or_write>\n"
            " databits: 5, 6, 7, 8\n"
            " parity: 0(None), 1(Odd), 2(Even)\n"
            " stopbits: 1, 2\n"
            " read_or_write: 0(read), 1(write)\n"
            "Example:\n %s /dev/ttyS0 9600 8 0 1 0\n\n",
            program_name, program_name
           );
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



