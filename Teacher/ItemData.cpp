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
, m_sock(msgSocket)
, m_pBitMapInfo(NULL)
, m_pBmpCompressData()
, m_endMonitor(false)
, m_isOneStuInScreen(false)
{
}

DWORD WINAPI CItemData::OnListen(LPVOID lp)
{
	CItemData* t = (CItemData*)lp;
	t->OnBeginListen();
	return 0;
}

void CItemData::Run()
{
	::CloseHandle(CreateThread(0, 0, OnListen, (LPVOID)this, 0, NULL));
}

// void CItemData::SetMsgType(int msgType)
// {
// 	m_msgType = msgType;
// }

void CItemData::SendScreenBmpData()
{
	BMPDATA bmpData;
	memset(&bmpData, 0, sizeof(BMPDATA));
	int nRet = m_Mysocket.RecvDataTCP(m_sock, (char*)&bmpData, sizeof(BMPDATA));

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
	ShowBmp(bmpData);
}

void CItemData::ShowBmp(BMPDATA &bmpData)
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

void CItemData::BeginMonitor(int itemOrder)
{
	m_itemOrder = itemOrder;
	m_endMonitor = false;
	m_isOneStuInScreen = false;
	m_Mysocket.SendReadyInfo(m_sock, BEGINSCREENMONITOR);
}

void CItemData::BeginMonitor(bool isOneSutInScreen, int itemOrder)
{
	m_itemOrder = itemOrder;
	m_isOneStuInScreen = isOneSutInScreen;
	m_endMonitor = false;
	m_Mysocket.SendReadyInfo(m_sock, BEGINSCREENMONITOR);
}

void CItemData::EndMonitor()
{
	m_Mysocket.SendReadyInfo(m_sock, ENDSCREENMONITOR);
	//	m_itemOrder = itemOrder;
	m_endMonitor = true;
}

void CItemData::BeginMulticast()
{
	m_Mysocket.SendReadyInfo(m_sock, BEGINMULTICAST);
}

void CItemData::EndMulticast()
{
	m_Mysocket.SendReadyInfo(m_sock, ENDMULTICAST);
}

void CItemData::CleanData()
{
	if (m_pBmpCompressData != NULL)
	{
		delete[] m_pBmpCompressData;
		m_pBmpCompressData = NULL;
	}
	if (m_pBitMapInfo != NULL)
	{
		LocalFree(m_pBitMapInfo);
		m_pBitMapInfo = NULL;
	}
}

void CItemData::OnBeginListen()
{
	int nRet;
	MSGTYPE msgType;
	m_stuInfo.SendStuInfoReq(m_sock);
	while (true)
	{
		nRet = m_Mysocket.RecvDataTCP(m_sock, (char*)&msgType, sizeof(MSGTYPE));
		// SOCKET 连接是否关闭
		if (nRet == 0)
		{
			// SOCKET 是否发生阻塞
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				Sleep(50);
				continue;
			}
			else
			{
				::closesocket(m_sock);
				::SendMessage(this->m_hWnd, ID_STUDENTLOGINOUT, this->m_id, 0);
				return;
			}
		}
		switch (msgType.msgID)
		{
		case STUDENTINFO:
			::SendMessage(m_hWnd, ID_STUDENTLONGIN, (WPARAM)this, 0);
			break;
		case BEGINSCREENMONITOR:
		    SendScreenBmpData();
			break;
		case ENDSCREENMONITOR:
		{
								 //			m_screenDlg.SetScreenTranEnd(false);
		}
			break;
		default:
			MessageBox(_T("消息有问题"), _T("警告"), MB_OK);
			break;
		}
	}
}

SOCKET CItemData::GetSocket()
{
	return this->m_sock;
}

// DWORD WINAPI CItemData::OnSendScreenData(LPVOID self)
// {
// 	CItemData* thread = (CItemData*)self;
// 	thread->SendScreenData();
// 	return 0;
// }
// 
// void CItemData::SendScreenData()
// {
// 	//	m_multicast.SendScreenData(m_sock);
// }

//////////////////////////////////////////////////////////////////////////
// 磁盘管理
//////////////////////////////////////////////////////////////////////////
// void CItemData::RunToFileManager()
// {
// 	MSGTYPE msgType;
// 	msgType.msgID = BEGINMULTICAST;
// 	m_Mysocket.SendDataTCP(m_sock, (char*)&msgType, sizeof(MSGTYPE));
// }

//////////////////////////////////////////////////////////////////////////
// 屏幕监控
//////////////////////////////////////////////////////////////////////////
// void CItemData::RunToScreen()
// {
// }