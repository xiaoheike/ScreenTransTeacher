#pragma once
#include <wtypesbase.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define SCREEN_TRANS_SIZE (20*1024)
#define MULTICAST_TRANS_SIZE (50*1024)
#define LISTEN_PORT 8000
#define SCREEN_TRANS_PORT 8010
#define MULTICAST_IP "224.0.0.210"
#define MULTICAST_TRANS_PORT 8030
// 能够同时被屏幕监控的学生机的数量
#define  MAX_MONITOR 16
//#define DISKINFO_TRANS_PORT 8020
//#define DISKDOWNLOAD_TRANS_PORT 8030
#define MAX_LISTEN  200

#define STUDENTINFO  0x01
#define BEGINSCREENMONITOR 0x0E
//#define SCREENINFO 0x10
#define ENDSCREENMONITOR 0x11
#define BEGINMULTICAST 0x20
//#define MULTICASTINFO 0x21
#define ENDMULTICAST 0X22
//#define MULTICAST 0x23



#define ID_STUDENTLONGIN WM_USER + 1
#define ID_STUDENTLOGINOUT WM_USER + 2
#define ID_SETBMPDATA WM_USER + 3
#define ID_MONITOREND WM_USER + 4
#define ID_ONESTUINSCREEN WM_USER + 5
#define ID_SETONESTUBMPDATA WM_USER + 6
#define ID_ONESTUINSCREENEND  WM_USER + 7

// typedef struct tagMSGINFO //传输消息结构体
// {
// 	int Msg_id;
// 	BYTE context[1024 * 4];
// }MSGINFO;
typedef struct tagITEMDATA
{
	LPVOID screenDlg;
	LPVOID multicastDlg;
}ITEMDATA;


typedef struct tagMSGTYPE
{
	int msgID;
//	bool flag;// 标志位，用于结束或开始的判断
}MSGTYPE;

typedef struct tagSTUINFO
{
	// 学生姓名
	TCHAR m_stuName[20];
	// 学生的班级
	TCHAR m_stuClass[20];
	// 学生的学号
	TCHAR m_stuID[20];
	// 学生的专业
	TCHAR m_stuMajor[20];
}STUINFODATA;

typedef struct tagSYSTEMINFO
{
	int os;
//	bool Cam; //摄像头
	double ver;
}SYSTEMINFO;


typedef struct tagBMPHEADINFO
{
//	int imgTotalSize;
	//	int HeadSize;
	unsigned long bmpCompressSize;
	BITMAPINFO bmpHeadInfo;
}BMPINFO;

typedef struct tagBMPDATA
{
	bool isShow; // 是否可以显示图像
	int infoType;
	UINT beginPos;
//	int ID;
	unsigned long bmpCompressSize;
	BITMAPINFO bmpHeadInfo;
	BYTE transData[SCREEN_TRANS_SIZE];
}BMPDATA;

typedef struct tagMULTICAST
{
	bool isShow; // 是否可以显示图像
	int infoType;    
	UINT beginPos;
	int ID;      // 一次屏幕数据的第几次传输
	unsigned long bmpCompressSize;
	BITMAPINFO bmpHeadInfo;
	BYTE transData[MULTICAST_TRANS_SIZE];
}MULTICASTDATA;

typedef struct tagBMP
{
	BYTE* pBmpCompressData;
	BITMAPINFO* pBMPINFO;
	unsigned long bmpCompressSize;
}BMP;