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
	, m_pBMPINFO(NULL)
	, m_isFirst(false)
	//, m_isSendFlag(true)
	//, m_isInitBITMAPINFO(true)
	//, m_socketScreen(INVALID_SOCKET)
	//, m_isSocketConn(true)

	, m_isStop(false)
	, m_isInitBITMAPINFO(true)
{
}


CMulticast::~CMulticast()
{
	CleanData();
}

//把下面的函数加到你初始化的地方，然后你就可以使用printf函数了
void InitConsoleWindow()
{
	int nCrt = 0;
	FILE* fp;
	AllocConsole();
	nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	fp = _fdopen(nCrt, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}

void CMulticast::CleanData()
{
	if (m_pBMPINFO != NULL)
	{
		LocalFree(m_pBMPINFO);
		m_pBMPINFO = NULL;
	}

	if (m_pBmpTransData != NULL)
	{
		delete[] m_pBmpTransData;
		m_pBmpTransData = NULL;
	}

	if (m_socketMulticast != INVALID_SOCKET)
	{
		closesocket(m_socketMulticast);
		m_socketMulticast = NULL;
	}
}

/*
	获得桌面的截图数据并进行调用压缩函数
*/
void CMulticast::GetDeskScreeData()
{
	
	CDC* pDeskDC = CWnd::GetDesktopWindow()->GetDC(); //获取桌面画布对象
	int width = GetSystemMetrics(SM_CXSCREEN); //获取屏幕的宽度
	int height = GetSystemMetrics(SM_CYSCREEN); //获取屏幕的高度
	CDC memDC; //定义一个内存画布
	memDC.CreateCompatibleDC(pDeskDC); //创建一个兼容的画布
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDeskDC, width, height); //创建兼容位图
	memDC.SelectObject(&bmp); //选中位图对象
	BITMAP bitmap;
	bmp.GetBitmap(&bitmap);

	memDC.BitBlt(0, 0, width, height, pDeskDC, 0, 0, SRCCOPY);
//	m_imgTotalSize = bitmap.bmWidthBytes * bitmap.bmHeight;

// 	if (true == m_isInitBITMAPINFO)
// 	{
		// 初始化截图图像头信息
		InitBITMAPINFO(bitmap, height, width);
// 		m_isInitBITMAPINFO = false;
// 	}
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
	// 桌面截图的原始数据数组
	BYTE* pBmpOriginalData = new BYTE[m_imgTotalSize];
	// 将图像的数据拷贝到pBmpOriginalData 中
	if (::GetDIBits(memDC.m_hDC, bmp, 0, bitmap.bmHeight,
		pBmpOriginalData, m_pBMPINFO, DIB_RGB_COLORS) == 0)
	{
		AfxMessageBox(_T("GetDIBits Error"));
		delete[] pBmpOriginalData;
		pBmpOriginalData = NULL;
		LocalFree(m_pBMPINFO);
		m_pBMPINFO = NULL;
		return;
	}

	// 压缩截图数据
	CompressBmpData(pBmpOriginalData);

	delete[] pBmpOriginalData;
	pBmpOriginalData = NULL;
	pDeskDC->DeleteDC();
	DeleteDC(memDC);
	DeleteObject(bmp);
//	_CrtDumpMemoryLeaks();

}


/*
	压缩截图数据
	input:
		pBmpOriginalData--图像的数据信息
*/
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
		InitConsoleWindow();
		printf("compess error: %d", err);
		exit(1);
	}

	// 	InitConsoleWindow();
	// 	printf("\r\norignal size: %d, compressed size : %d\r\n", 
	// 		m_imgTotalSize, m_compressBmpDataLen);
	// 	cout << "orignal size: " << m_imgTotalSize
	// 		<< " , compressed size : " << m_compressBmpDataLen << '\n';
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


/*
	初始化截图的信息头结构体 BITMAPINFO
	input:
		bitmap--截图的大小等信息
		height--桌面的高
		width--桌面的宽
*/
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

	m_pBMPINFO = (BITMAPINFO*)LocalAlloc(LPTR, m_bmpHeadTotalSize);
	m_pBMPINFO->bmiHeader.biBitCount = bitmap.bmBitsPixel;
	m_pBMPINFO->bmiHeader.biClrImportant = 0;
	m_pBMPINFO->bmiHeader.biCompression = 0;
	m_pBMPINFO->bmiHeader.biHeight = height;
	m_pBMPINFO->bmiHeader.biPlanes = bitmap.bmPlanes;
	m_pBMPINFO->bmiHeader.biSize = m_bmpHeadTotalSize;//sizeof(BITMAPINFO);
	m_pBMPINFO->bmiHeader.biSizeImage = m_imgTotalSize;
	m_pBMPINFO->bmiHeader.biWidth = width;
	m_pBMPINFO->bmiHeader.biXPelsPerMeter = 0;
	m_pBMPINFO->bmiHeader.biYPelsPerMeter = 0;
}

/*
发送截图数据到客户端
*/
void CMulticast::SendBmpData(SOCKET socket)
{
		MULTICASTDATA multicastData;
		memset(&multicastData, 0, sizeof(MULTICASTDATA));

// 		m_mySocket.SendReadyInfo(socket, MULTICAST);
		//图像信息头
		memcpy(&multicastData.bmpHeadInfo, m_pBMPINFO, m_bmpHeadTotalSize);
		multicastData.bmpCompressSize = m_compressBmpDataLen;
		multicastData.isShow = false;
// 		multicastData.infoType = 0;
// 		m_mySocket.SendDataUDP(m_socketMulticast, (char*)&multicastData,
// 			sizeof(MULTICASTDATA), m_addr);


	 	// 发送图像数据
	 	int count = int(ceil(double(m_compressBmpDataLen) / MULTICAST_TRANS_SIZE));
	 
//		multicastData.isShow = false;
	 	UINT beginPos;
		multicastData.infoType = 1;
//		multicastData.infoType = 1;
	 	for (int i = 0; i < count; i++)
	 	{
// 	 		// 通知客户端可以开始接收屏幕数据了
//			m_mySocket.SendReadyInfo(socket, MULTICAST);

// 			m_mySocket.SendDataUDP(m_socketMulticast, (char*)&multicastData,
// 				sizeof(MULTICASTDATA), m_addr);


			memset(multicastData.transData, 0, MULTICAST_TRANS_SIZE);
	 		beginPos = i * MULTICAST_TRANS_SIZE;
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
			m_mySocket.SendDataUDP(m_socketMulticast, (char*)&multicastData, 
				sizeof(MULTICASTDATA), m_addr);
	 	}
//	}
}

void CMulticast::SendBmpData(SOCKET multicastSocket, SOCKADDR_IN addr)
{
	MULTICASTDATA multicastData;
	memset(&multicastData, 0, sizeof(MULTICASTDATA));
	//图像信息头
	memcpy(&multicastData.bmpHeadInfo, m_pBMPINFO, m_bmpHeadTotalSize);
	multicastData.bmpCompressSize = m_compressBmpDataLen;
	multicastData.isShow = false;
	// 发送图像数据
	int count = int(ceil(double(m_compressBmpDataLen) / MULTICAST_TRANS_SIZE));

	//		multicastData.isShow = false;
	UINT beginPos;
	multicastData.infoType = 1;
	//		multicastData.infoType = 1;
	for (int i = 0; i < count; i++)
	{
		// 	 		// 通知客户端可以开始接收屏幕数据了
		//			m_mySocket.SendReadyInfo(socket, MULTICAST);

		// 			m_mySocket.SendDataUDP(m_socketMulticast, (char*)&multicastData,
		// 				sizeof(MULTICASTDATA), m_addr);


		memset(multicastData.transData, 0, MULTICAST_TRANS_SIZE);
		beginPos = i * MULTICAST_TRANS_SIZE;
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
		m_mySocket.SendDataUDP(multicastSocket, (char*)&multicastData,
			sizeof(MULTICASTDATA), addr);
	}
	//	}
}

void CMulticast::SendBmpHeaderInfo(SOCKET socketMsg)
{
	// 通知客户端请求已经接收到了
//	m_mySocket.SendReadyInfo(socketMsg, MULTICASTINFO);
	BMPINFO bmpInfo;
	memcpy(&bmpInfo.bmpHeadInfo, m_pBMPINFO, m_bmpHeadTotalSize);
	bmpInfo.bmpCompressSize = m_compressBmpDataLen;

	m_mySocket.SendDataTCP(m_socketMulticast, (char*)&bmpInfo, sizeof(BMPINFO));
}


void CMulticast::SendScreenData(SOCKET socketMsg)
{
	m_socketMulticast = m_mySocket.InitMulticastSocket(MULTICAST_TRANS_PORT, 
		MULTICAST_IP, m_addr);

	while (false == m_isStop)
	{
		GetDeskScreeData();
//		SendBmpHeaderInfo(socketMsg);
		SendBmpData(socketMsg);

		Sleep(500);
	}

	CleanData();
}

void CMulticast::SendScreenData()
{
	SOCKADDR_IN addr;
	SOCKET socketMulticast = m_mySocket.InitMulticastSocket(MULTICAST_TRANS_PORT,
		MULTICAST_IP, addr);

	while (false == m_isStop)
	{
		GetDeskScreeData();
		//		SendBmpHeaderInfo(socketMsg);
		SendBmpData(socketMulticast, addr);

		Sleep(500);
	}

	CleanData();
}


void CMulticast::SetIsStop(bool isStop)
{
	m_isStop = isStop;
}