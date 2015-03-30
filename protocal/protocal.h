#ifndef _PROTOCAL_H_
#define _PROTOCAL_H_


#include "config.h"
#include <pthread.h>
#include <stdbool.h>


#define FRAME_FLAG	(0x7E)
#define FRAME_VERSION	(0x0105)
#define FRAME_PRODUCER	(0x7423)
#define FRAME_DEVICE 	(0xF1)

#define FRAME_NUSERDATA_LEN (9)

#define FRAME_CMD_CHILDBOARD_POWER_ON		(0x01)		// 从机上电/应答
#define FRAME_CMD_CHILDBOARD_LINK_DETECT 	(0x02)		// 外设链路探寻/应答      
#define FRAME_CMD_CHILDBOARD_IC_CONFIRM 	(0x40)		// IC卡认证请/求应答
#define FRAME_CMD_CHILDBOARD_IC_READ_RESULT 	(0x41) 		// IC卡请求结果/应答
#define FRAME_CMD_CHILDBOARD_IC_PULLOUT_NOTIFY 	(0x42) 		// IC卡拔出通知/应答
#define FRAME_CMD_CHILDBOARD_VARIABLE_CONTINUS 	(0xA0) 		// 连续量上报/应答 
#define FRAME_CMD_CHILDBOARD_VARIABLE_BOOL 	(0xA1) 		// 开关量上报/应答 
#define FRAME_CMD_CHILDBOARD_VARIABLE_CHAR 	(0xA2) 		// 字符串上报/应答 
#define FRAME_CMD_CHILDBOARD_VARIABLE_BLOCK 	(0xA3) 		// 块数据上报/应答 
#define FRAME_CMD_CHILDBOARD_TO_SERVER	 	(0xB1) 		// 子板向服务器透传/应答
	
#define FRAME_CMD_MAINBOARD_POWER_MANAGE	(0x03)		// 从机电源控制/应答
#define FRAME_CMD_MAINBOARD_VERSION		(0x04)		// 查询从机版本号/应答
#define FRAME_CMD_MAINBOARD_CHECK		(0x05)		// 从机自检/应答
#define FRAME_CMD_MAINBOARD_FIRMWARE_UPDATE	(0x06)		// 从机固件更新/应答
#define FRAME_CMD_MAINBOARD_DEVICE_ATTR		(0x07)		// 查询外设属性应答
#define FRAME_CMD_MAINBOARD_READ_IC		(0x43)		// 读IC卡/应答
#define FRAME_CMD_MAINBOARD_POWER_ON		(0x8A)		// 主机上电提示/应答
#define FRAME_CMD_MAINBOARD_CONFIG_QUERY	(0x8B)		// 基本配置信息查询命令/应答
#define FRAME_CMD_MAINBOARD_QUERY		(0x8C)		// 查询命令/应答
#define FRAME_CMD_MAINBOARD_CTRL		(0x8D)		// 控制命令/应答
#define FRAME_CMD_MAINBOARD_LOCATION		(0x8E)		// 定位数据下发/应答
#define FRAME_CMD_MAINBOARD_CONFIG		(0xB0)		// 配置命令/应答	
#define FRAME_CMD_MAINBOARD_SERVER_DATA		(0xB2)		// 服务器向采集子板透传/应答
#define FRAME_CMD_MAINBOARD_CFIRMWARE_UPDATE	(0xB3)		// 采集子板固件更新/应答

#define FRAME_CMD_ERROR_CTRL                  (0xFE)              //  

#define FRAME_CMD_ERROR_FRAME          	(0xFF)		        // 帧错误应答/应答

#define DEVICE_TERMINAL				(0x01)		// 行业信息终端机
#define DEVICE_SCHED_LCD			(0x02) 		// 调度显示屏
#define DEVICE_NAVI_LCD				(0x03)		// 车载导航显示屏
#define DEVICE_SENSOR_OIL			(0x04)		// 油量检测器
#define DEVICE_SENSOR_ACC			(0x05)		// 加速度传感器
#define DEVICE_SENSOR_ANTI_THEFT		(0x06)		// 防盗报警器
#define DEVICE_COM_EXT				(0x07)		// 接口扩展器
#define DEVICE_SENSOR_LOAD			(0x08)		// 载重检测器
#define DEVICE_SENSOR_FLOW			(0x09)		// 客流检测器
#define DEVICE_SENSOR_GENERAL			(0x0A)		// 通用传感器
#define DEVICE_IC_READER			(0x0B)		// 道路运输证IC卡读卡器
#define DEVICE_CHILDBOARD			(0xF1)		// 定华采集子板

extern int initProtocal(void);
extern void finalizeProtocal(void);
extern int getData(int cmd, void* data, int length);
#endif

 
