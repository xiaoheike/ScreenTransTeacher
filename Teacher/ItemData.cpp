/******************************************************************* 
FileName: ItemData.cpp
Author	: xiaoheike
Date	: 2015-05-12 15:50:53
Desc	: 获得来自学生机的屏幕图像数据并转发到显示对话框


*******************************************************************/ 
#include "stdafx.h"
#include "ItemData.h"

CItemData::CItemData()
{
}

CItemData::~CItemData()
{
}

CItemData::CItemData(int id, HWND m_hWnd, SOCKET msgSocket)
	: m_id(id)
	, m_hWnd(m_hWnd)
	, m_socketMsg(msgSocket)
	, m_pBitMapInfo(NULL)
	, m_pBmpCompressData()
	, m_endMonitor(false)
	, m_isOneStuInScreen(false)
{
}

/******************************************************************
Function	: OnListen
Date		: 2015-05-12 15:19:28
Author		: xiaoheike
Parameter	: lp--CItemData 对象
Return		: DWORD WINAPI
Desc		: 监听主消息的线程调用函数
******************************************************************/
DWORD WINAPI CItemData::OnListen(LPVOID lp)
{
	CItemData* t = (CItemData*)lp;
	t->OnBeginListen();
	return 0;
}

/******************************************************************
Function	: Run
Date		: 2015-05-12 15:23:40
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 在这里启动监听消息的线程
******************************************************************/
void CItemData::Run()
{
	::CloseHandle(CreateThread(0, 0, OnListen, (LPVOID)this, 0, NULL));
}

/******************************************************************
Function	: SendScreenBmpData
Date		: 2015-05-12 15:24:04
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 接收来自学生机的屏幕图像数据并将数据发送给显示图像的对话框
******************************************************************/
void CItemData::SetScreenBmpData()
{
	BMPDATA bmpData;
	memset(&bmpData, 0, sizeof(BMPDATA));
	int nRet = m_Mysocket.RecvDataTCP(m_socketMsg, (char*)&bmpData, sizeof(BMPDATA));

	switch (bmpData.infoType)
	{
	case 1: // 位图数据信息
		SetBmpTransDataNotLast(bmpData);
		break;
	case 2: // 接收最后一次发送的数据
		SetBmpTransDataLast(bmpData);
		break;
	default:
		MessageBox(_T("未知的图像数据ID"), _T("提示"), MB_OK);
		CleanData();
		exit(1);
	}
	SendBmpDataToDlg(bmpData);
}

/******************************************************************
Function	: SendBmpDataToDlg
Date		: 2015-05-12 15:26:18
Author		: xiaoheike
Parameter	: bmpData--MBPDATA图像数据的结构体，图像的必要信息
Return		: void
Desc		: 将学生机接收到的屏幕图像数据发送到显示图像的对话框
******************************************************************/
void CItemData::SendBmpDataToDlg(BMPDATA &bmpData)
{
	if (bmpData.isShow && false == m_endMonitor)
	{
		BMP bmp;
		bmp.pBmpCompressData = m_pBmpCompressData;
		bmp.pBMPINFO = m_pBitMapInfo;
		bmp.bmpCompressSize = bmpData.bmpCompressSize;
		if (false == m_isOneStuInScreen)
		{
			::SendMessage(this->m_hWnd, ID_SETBMPDATA, (WPARAM)&bmp, (LPARAM)&m_itemOrder);
		}
		else
		{
			::SendMessage(this->m_hWnd, ID_SETONESTUBMPDATA, (WPARAM)&bmp, (LPARAM)&m_itemOrder);
		}

		// 发送的消息被处理完成返回之后才会运行之后的代码
		delete[] m_pBmpCompressData;
		m_pBmpCompressData = NULL;
		LocalFree(m_pBitMapInfo);
		m_pBitMapInfo = NULL;
	}
}

/******************************************************************
Function	: SetBmpTransDataLast
Date		: 2015-05-12 15:27:38
Author		: xiaoheike
Parameter	: bmpData--MBPDATA图像数据的结构体，图像的必要信息
Return		: void
Desc		: 接收来自学生机的一幅屏幕图像数据的最后一部分
******************************************************************/
void CItemData::SetBmpTransDataLast(BMPDATA &bmpData)
{
	int lastTransSize = bmpData.bmpCompressSize - bmpData.beginPos;
	memcpy_s(m_pBmpCompressData + bmpData.beginPos, lastTransSize,
		bmpData.transData, lastTransSize);
}

void CItemData::SetBmpTransDataNotLast(BMPDATA &bmpData)
{
	if (NULL == m_pBmpCompressData)
	{
		int bmpHeadInfoSize = bmpData.bmpHeadInfo.bmiHeader.biSize;
		m_pBitMapInfo = (BITMAPINFO*)LocalAlloc(LPTR, bmpHeadInfoSize);
		memcpy(m_pBitMapInfo, &bmpData.bmpHeadInfo, bmpHeadInfoSize);

		m_pBmpCompressData = new BYTE[bmpData.bmpCompressSize];
		memset(m_pBmpCompressData, 0, bmpData.bmpCompressSize);
	}
	memcpy_s(m_pBmpCompressData + bmpData.beginPos,
		SCREEN_TRANS_SIZE, bmpData.transData, SCREEN_TRANS_SIZE);
}

/******************************************************************
Function	: UnCompressData
Date		: 2015-05-12 15:28:43
Author		: xiaoheike
Parameter	: biSizeImage--未压缩时的图像大小
			  bmpCompressSize--压缩后的图像大小
Return		: BYTE*
Desc		: 将接收到的屏幕图像数据解压
******************************************************************/
BYTE* CItemData::UnCompressData(uLongf biSizeImage, uLongf bmpCompressSize)
{
	uLongf unCompressDataLen = (uLongf)((biSizeImage + 12)*(100.1 / 100)) + 1;
	BYTE* pUnCompressData = new BYTE[unCompressDataLen];
	int err = uncompress(pUnCompressData, &unCompressDataLen,
		m_pBmpCompressData, bmpCompressSize);
	if (err != Z_OK)
	{
		CString str;
		str.Format(_T("uncompess error: %d"), err);
		MessageBox(str);
		exit(1);
	}

	BYTE* bmpShowData = new BYTE[unCompressDataLen];
	memcpy(bmpShowData, pUnCompressData, unCompressDataLen);

	delete[] pUnCompressData;
	pUnCompressData = NULL;

	return bmpShowData;
}

/******************************************************************
Function	: BeginMonitor
Date		: 2015-05-12 15:29:51
Author		: xiaoheike
Parameter	: itemOrder--被监控学生面在对话框上显示的序号
Return		: void
Desc		: 1对多监控时发送屏幕监控消息到学生机，表示教师机要监控你的画面
******************************************************************/
void CItemData::BeginMonitor(int itemOrder)
{
	m_itemOrder = itemOrder;
	m_endMonitor = false;
	m_isOneStuInScreen = false;
	m_Mysocket.SendReadyInfo(m_socketMsg, BEGINSCREENMONITOR);
}

/******************************************************************
Function	: BeginMonitor
Date		: 2015-05-12 15:31:13
Author		: xiaoheike
Parameter	: isOneSutInScreen--false 将学生机屏幕图像数据发送给class CScreenMonitorDlg
true 将学生机屏幕图像数据发送给class COneStuScreenDlg
			  itemOrder--学生屏幕在对话框上显示的序号
Return		: void
Desc		: 1对1监控时发送屏幕监控消息到学生机，表示教师机要监控你的画面
******************************************************************/
void CItemData::BeginMonitor(bool isOneSutInScreen, int itemOrder)
{
	m_itemOrder = itemOrder;
	m_isOneStuInScreen = isOneSutInScreen;
	m_endMonitor = false;
	m_Mysocket.SendReadyInfo(m_socketMsg, BEGINSCREENMONITOR);
}

/******************************************************************
Function	: EndMonitor
Date		: 2015-05-12 15:34:26
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 发送消息，结束对学生机的屏幕图像监控
******************************************************************/
void CItemData::EndScreenMonitor()
{
	m_Mysocket.SendReadyInfo(m_socketMsg, ENDSCREENMONITOR);
	//	m_itemOrder = itemOrder;
	m_endMonitor = true;
}

/******************************************************************
Function	: BeginMulticast
Date		: 2015-05-12 15:35:26
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 开始教师机屏幕广播
******************************************************************/
void CItemData::BeginMulticast()
{
	m_Mysocket.SendReadyInfo(m_socketMsg, BEGINMULTICAST);
}

/******************************************************************
Function	: EndMulticast
Date		: 2015-05-12 15:35:47
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 结束教师机屏幕广播
******************************************************************/
void CItemData::EndMulticast()
{
	m_Mysocket.SendReadyInfo(m_socketMsg, ENDMULTICAST);
}

void CItemData::CleanData()
{
	DeletepBmpCompressData();
	DeletepBitMapInfo();
}

/******************************************************************
Function	: DeletepBitMapInfo
Date		: 2015-05-12 15:36:36
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 释放new 的内存
******************************************************************/
void CItemData::DeletepBitMapInfo()
{
	if (m_pBitMapInfo != NULL)
	{
		LocalFree(m_pBitMapInfo);
		m_pBitMapInfo = NULL;
	}
}

/******************************************************************
Function	: DeletepBmpCompressData
Date		: 2015-05-12 15:36:40
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 释放new 的内存
******************************************************************/
void CItemData::DeletepBmpCompressData()
{
	if (m_pBmpCompressData != NULL)
	{
		delete[] m_pBmpCompressData;
		m_pBmpCompressData = NULL;
	}
}

/******************************************************************
Function	: OnBeginListen
Date		: 2015-05-12 15:37:08
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 监听来自学生机的消息
******************************************************************/
void CItemData::OnBeginListen()
{
	int nRet;
	MSGTYPE msgType;
	m_stuInfo.SendStuInfoReq(m_socketMsg);
	while (true)
	{
		SendDataTCP(nRet, msgType);
		if (nRet  != 0)
		{
			switch (msgType.msgID)
			{
			case STUDENTINFO:
				m_stuInfo.GetStuInfo(m_socketMsg);
				::SendMessage(m_hWnd, ID_STUDENTLONGIN, (WPARAM)this, 0);
				break;
			case BEGINSCREENMONITOR:
				SetScreenBmpData();
				break;
			default:
				MessageBox(_T("消息有问题"), _T("警告"), MB_OK);
				break;
			}
		}
	}
}

/******************************************************************
Function	: SendDataTCP
Date		: 2015-05-12 15:38:03
Author		: xiaoheike
Parameter	: nRet--接收消息的返回值
			  msgType--来自学生机的消息的请求内容
Return		: void
Desc		: 接收来自学生机的消息
******************************************************************/
void CItemData::SendDataTCP(int &nRet, MSGTYPE& msgType)
{
	nRet = m_Mysocket.RecvDataTCP(m_socketMsg, (char*)&msgType, sizeof(MSGTYPE));
	// SOCKET 连接是否关闭
	if (nRet == 0)
	{
		// SOCKET 是否发生阻塞
		if (::WSAGetLastError() == WSAEWOULDBLOCK)
		{
			Sleep(50);
		}
		else
		{
			::closesocket(m_socketMsg);
			::SendMessage(this->m_hWnd, ID_STUDENTLOGINOUT, this->m_id, 0);
			return;
		}
	}
}

/******************************************************************
Function	: GetSocket
Date		: 2015-05-12 15:39:21
Author		: xiaoheike
Parameter	: 无
Return		: SOCKET
Desc		: 返回一个连接的SOCKET
******************************************************************/
SOCKET CItemData::GetSocket()
{
	return this->m_socketMsg;
}
