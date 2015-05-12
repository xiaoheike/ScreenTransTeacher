#include "stdafx.h"
#include "Multicast.h"
#include <io.h>
#include <fcntl.h>
//////////////////////////////////////////////////////////////////////////
// 内存泄露检测需要添加的头文件
// #define _CRTDBG_MAP_ALLOC
// #include <stdlib.h>
// #include <crtdbg.h>
//////////////////////////////////////////////////////////////////////////
CMulticast::CMulticast()
	: m_pBmpTransData(NULL)
	, m_pBitMapInfo(NULL)
	, m_isFirst(false)
	, m_isMulticastEnd(false)
	, m_isInitBITMAPINFO(true)
	, m_socketMulticast(INVALID_SOCKET)
{
}

CMulticast::~CMulticast()
{
	CleanData();
}

void CMulticast::CleanData()
{
	DeletepBitMapInfo();

	DeletepBmpTransData();

	CloseSocketMulticast();
}

/******************************************************************
Function	: CloseSocketMulticast
Date		: 2015-05-12 09:13:26
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 释放SOCKET资源
******************************************************************/
void CMulticast::CloseSocketMulticast()
{
	if (m_socketMulticast != INVALID_SOCKET)
	{
		closesocket(m_socketMulticast);
		m_socketMulticast = INVALID_SOCKET;
	}
}

/******************************************************************
Function	: DeletepBmpTransData
Date		: 2015-05-12 09:13:20
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 释放new 的内存
******************************************************************/
void CMulticast::DeletepBmpTransData()
{
	if (m_pBmpTransData != NULL)
	{
		delete[] m_pBmpTransData;
		m_pBmpTransData = NULL;
	}
}

/******************************************************************
Function	: DeletepBitMapInfo
Date		: 2015-05-12 09:13:05
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 释放new 的内存
******************************************************************/
void CMulticast::DeletepBitMapInfo()
{
	if (m_pBitMapInfo != NULL)
	{
		LocalFree(m_pBitMapInfo);
		m_pBitMapInfo = NULL;
	}
}

/******************************************************************
Function	: GetDeskScreeData
Date		: 2015-05-12 09:14:42
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 获得桌面的图像数据并调用压缩方法
******************************************************************/
void CMulticast::GetDeskScreeData()
{
	CDC memDC; //定义一个内存画布
	CBitmap bmp;
	BITMAP bitmap;
	GetDeskScreenDC(memDC, bmp, bitmap);
	SetCursorIcon(memDC);
	// 桌面截图的原始数据数组
	BYTE* pBmpOriginalData = new BYTE[m_imgTotalSize];
	// 将图像的数据拷贝到pBmpOriginalData 中
	if (::GetDIBits(memDC.m_hDC, bmp, 0, bitmap.bmHeight,
		pBmpOriginalData, m_pBitMapInfo, DIB_RGB_COLORS) == 0)
	{
		AfxMessageBox(_T("GetDIBits Error"));
		delete[] pBmpOriginalData;
		pBmpOriginalData = NULL;
		LocalFree(m_pBitMapInfo);
		m_pBitMapInfo = NULL;
		return;
	}

	// 压缩截图数据
	CompressBmpData(pBmpOriginalData);

	delete[] pBmpOriginalData;
	pBmpOriginalData = NULL;
	DeleteDC(memDC);
	DeleteObject(bmp);
	//	_CrtDumpMemoryLeaks();
}

/******************************************************************
Function	: SetCursorIcon
Date		: 2015-05-12 09:15:03
Author		: xiaoheike
Parameter	: memDC--CDC类
Return		: void
Desc		: 获得鼠标信息并绘制到桌面图像中
******************************************************************/
void CMulticast::SetCursorIcon(CDC &memDC)
{
	//获取当前光标及其位置
	HCURSOR hCursor = GetCursor();
	POINT ptCursor;
	GetCursorPos(&ptCursor);
	//获取光标的图标数据
	ICONINFO IconInfo;
	if (GetIconInfo(hCursor, &IconInfo))
	{
		ptCursor.x -= ((int)IconInfo.xHotspot);
		ptCursor.y -= ((int)IconInfo.yHotspot);
		if (IconInfo.hbmMask != NULL)
			DeleteObject(IconInfo.hbmMask);
		if (IconInfo.hbmColor != NULL)
			DeleteObject(IconInfo.hbmColor);
	}
	//在兼容设备描述表上画出该光标
	DrawIconEx(
		memDC.m_hDC,         // handle to device context
		ptCursor.x, ptCursor.y,
		hCursor,         // handle to icon to draw
		0, 0,          // width of the icon
		0,           // index of frame in animated cursor
		NULL,          // handle to background brush
		DI_NORMAL | DI_COMPAT      // icon-drawing flags
		);
}

/******************************************************************
Function	: GetDeskScreenDC
Date		: 2015-05-12 09:15:34
Author		: xiaoheike
Parameter	: memDC--CDC类  bmp--位图   bitmap--位图结构体
Return		: void
Desc		: 获得桌面图像
******************************************************************/
void CMulticast::GetDeskScreenDC(CDC &memDC, CBitmap &bmp, BITMAP& bitmap)
{
	CDC* pDeskDC = CWnd::GetDesktopWindow()->GetDC(); //获取桌面画布对象
	int width = GetSystemMetrics(SM_CXSCREEN); //获取屏幕的宽度
	int height = GetSystemMetrics(SM_CYSCREEN); //获取屏幕的高度
	memDC.CreateCompatibleDC(pDeskDC); //创建一个兼容的画布
	bmp.CreateCompatibleBitmap(pDeskDC, width, height); //创建兼容位图
	memDC.SelectObject(&bmp); //选中位图对象
	bmp.GetBitmap(&bitmap);

	memDC.BitBlt(0, 0, width, height, pDeskDC, 0, 0, SRCCOPY);

	InitBITMAPINFO(bitmap, height, width);
	pDeskDC->DeleteDC();
}

/******************************************************************
Function	: CompressBmpData
Date		: 2015-05-12 09:15:52
Author		: xiaoheike
Parameter	: pBmpOriginalData--桌面图像的数据
Return		: void
Desc		: 使用ZLIB 库压缩桌面图像数据
******************************************************************/
void CMulticast::CompressBmpData(BYTE* pBmpOriginalData)
{
	BYTE* pCompressData = NULL;
	//	m_compressBmpDataLen = 0;
	// 需要一个足够大的空间
	m_compressBmpDataLen = (uLongf)((m_imgTotalSize + 12)*(100.1 / 100)) + 1;

	pCompressData = new BYTE[m_compressBmpDataLen];
	// 将数据进行压缩并保存到pCompressData 中
	int err = compress(pCompressData, &m_compressBmpDataLen, pBmpOriginalData, m_imgTotalSize);

	if (err != Z_OK) {
		// 		InitConsoleWindow();
		// 		printf("compess error: %d", err);
		exit(1);
	}

	if (m_pBmpTransData != NULL)
	{
		delete[] m_pBmpTransData;
		m_pBmpTransData = NULL;
	}
	// 将压缩后的数据保存到m_pBmpTransData 中
	m_pBmpTransData = new BYTE[m_compressBmpDataLen];
	memcpy(m_pBmpTransData, pCompressData, m_compressBmpDataLen);
	delete[] pCompressData;
	pCompressData = NULL;
}

/******************************************************************
Function	: InitBITMAPINFO
Date		: 2015-05-12 09:16:31
Author		: xiaoheike
Parameter	: bitmap--图像的大小等信息  height--桌面的高
width--桌面的宽
Return		: void
Desc		: 初始化图像的信息头结构体 BITMAPINFO
******************************************************************/
void CMulticast::InitBITMAPINFO(BITMAP &bitmap, int height, int width)
{
	m_imgTotalSize = bitmap.bmWidthBytes * bitmap.bmHeight;
	double paletteSize = 0; //记录调色板大小
	if (bitmap.bmBitsPixel < 16) //判断是否为真彩色位图
	{
		//paletteSize = pow(2.0, (double)bitmap.bmBitsPixel*sizeof(RGBQUAD));
		paletteSize = (1 << bitmap.bmBitsPixel)*sizeof(RGBQUAD);
	}
	m_bmpHeadTotalSize = (int)paletteSize + sizeof(BITMAPINFO);

	m_pBitMapInfo = (BITMAPINFO*)LocalAlloc(LPTR, m_bmpHeadTotalSize);
	m_pBitMapInfo->bmiHeader.biBitCount = bitmap.bmBitsPixel;
	m_pBitMapInfo->bmiHeader.biClrImportant = 0;
	m_pBitMapInfo->bmiHeader.biCompression = 0;
	m_pBitMapInfo->bmiHeader.biHeight = height;
	m_pBitMapInfo->bmiHeader.biPlanes = bitmap.bmPlanes;
	m_pBitMapInfo->bmiHeader.biSize = m_bmpHeadTotalSize;//sizeof(BITMAPINFO);
	m_pBitMapInfo->bmiHeader.biSizeImage = m_imgTotalSize;
	m_pBitMapInfo->bmiHeader.biWidth = width;
	m_pBitMapInfo->bmiHeader.biXPelsPerMeter = 0;
	m_pBitMapInfo->bmiHeader.biYPelsPerMeter = 0;
}

/******************************************************************
Function	: SendScreenBmpData
Date		: 2015-05-12 09:17:16
Author		: xiaoheike
Parameter	: multicastSocket--广播SOCKET
addr--SOCKADDR_IN组播技术发送数据时需要使用
Return		: void
Desc		: 发送屏幕图像数据到学生机
******************************************************************/
void CMulticast::SendScreenBmpData(SOCKET multicastSocket, SOCKADDR_IN addr)
{
	MULTICASTDATA multicastData;
	int count = InitBmpData(multicastData);

	for (int i = 0; i < count; i++)
	{
		SetBmpData(multicastData, i, count);

		m_mySocket.SendDataUDP(multicastSocket, (char*)&multicastData,
			sizeof(MULTICASTDATA), addr);
	}
}

/******************************************************************
Function	: SetBmpData
Date		: 2015-05-12 09:18:31
Author		: xiaoheike
Parameter	: multicastData--需要被赋值的MULTICASTDATA 结构体 
i--一幅桌面图像的第几次传输
count--一幅桌面图像需要被传输的次数
Return		: void
Desc		: 发送屏幕图像数据到学生机
******************************************************************/
void CMulticast::SetBmpData(MULTICASTDATA &multicastData, int i, int count)
{
	memset(multicastData.transData, 0, MULTICAST_TRANS_SIZE);

	UINT beginPos = i * MULTICAST_TRANS_SIZE;
	multicastData.beginPos = beginPos;
	multicastData.ID = i;
	if (i == count - 1) // 最后一次发送数据
	{
		multicastData.isShow = true;
		multicastData.infoType = 2;
		memcpy_s(multicastData.transData, MULTICAST_TRANS_SIZE,
			m_pBmpTransData + beginPos, m_compressBmpDataLen - beginPos);
		m_isFirst = false;
	}
	else
	{
		memcpy_s(multicastData.transData, MULTICAST_TRANS_SIZE,
			m_pBmpTransData + beginPos, MULTICAST_TRANS_SIZE);
	}
}

/******************************************************************
Function	: InitBmpData
Date		: 2015-05-12 09:19:57
Author		: xiaoheike
Parameter	: multicastData--桌面图像数据的结构体
Return		: int--一幅桌面图像需要被传输的次数
Desc		: 为桌面图像结构体BMPDATA初始化一些必要信息
******************************************************************/
int CMulticast::InitBmpData(MULTICASTDATA &multicastData)
{
	memset(&multicastData, 0, sizeof(MULTICASTDATA));
	//图像信息头
	memcpy(&multicastData.bmpHeadInfo, m_pBitMapInfo, m_bmpHeadTotalSize);
	multicastData.bmpCompressSize = m_compressBmpDataLen;
	multicastData.isShow = false;
	// 发送图像数据
	int count = int(ceil(double(m_compressBmpDataLen) / MULTICAST_TRANS_SIZE));

	//		multicastData.isShow = false;
	//	UINT beginPos;
	multicastData.infoType = 1;
	return count;
}

/******************************************************************
Function	: SendScreenData
Date		: 2015-05-12 09:22:39
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 不断发送数据到学生机
******************************************************************/
void CMulticast::SendScreenData()
{
	SOCKADDR_IN addr;
	SOCKET socketMulticast = m_mySocket.InitMulticastSocket(MULTICAST_TRANS_PORT,
		MULTICAST_IP, addr);

	while (false == m_isMulticastEnd)
	{
		GetDeskScreeData();
		//		SendBmpHeaderInfo(socketMsg);
		SendScreenBmpData(socketMulticast, addr);

		Sleep(500);
	}
}

/******************************************************************
Function	: SetIsMulticastEnd
Date		: 2015-05-12 09:24:17
Author		: xiaoheike
Parameter	: isScreenMonitorEnd
Return		: void
Desc		: 发送图像数据到学生机的线程是否结束的标志
IsMulticastEnd--true运行线程，false停止线程
******************************************************************/
void CMulticast::SetIsMulticastEnd(bool IsMulticastEnd)
{
	m_isMulticastEnd = IsMulticastEnd;
}