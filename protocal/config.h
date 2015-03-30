#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DEBUG_MODE       

// #define JNI_BUILD   // JNI      
#define LINUX_BUILD    // linux  

#define USE_STATISTICS_MODULE           // 编译统计模块
#define SEND_FRAME_CACHE_LEN   (1)     // 发送缓冲区的最大帧数目

#define DEV_SERIAL_PORT "/dev/ttyPCH0"
// #define DEV_SERIAL_PORT "/dev/ttyS0"

#define SERIAL_BAUDRATE        (921600) // 串口波特率
#define FRAME_RECEIVER_BUF_LEN (1024)   // 帧接收buffer长度，由最大帧长度决定




#endif

