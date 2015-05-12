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
	, m_pBMPINFO(NULL)
	, m_pBmpCompressData()
	, m_endMonitor(false)
	, m_isOneStuInScreen(false)
{

}

CItemData::CItemData(UINT id, SOCKET sock, SOCKADDR_IN *addr, HWND m_hWnd, CMySocket* mySocket)
{
//	m_Mysocket = mySocket;
	this->m_id = id;
	this->m_sock = sock;
	this->m_hWnd = m_hWnd;
	char *csIP = inet_ntoa(addr->sin_addr);
	m_IP = csIP;
//	m_Address = QQ.IP2Add(m_IP);
	m_Address = _T("未知");
	if (m_Address.IsEmpty())
	{
		m_Address.Format(_T("未知"));
	}
	m_screen = NULL;

	// 是否创建SOCKET 的标志
	m_isScreenFlag = true;
	m_isScreenAccept = true;

	m_screenSocket = INVALID_SOCKET;
//	m_pMySocket = new CMySocket();
}

DWORD WINAPI CItemData::OnListen(LPVOID lp)
{
	CItemData* t = (CItemData*)lp;
	t->OnBeginListen();
	return 0;
}

DWORD WINAPI CItemData::OnScreenSocketAccept(LPVOID self)
{
	CItemData* thread = (CItemData*)self;
	thread->ScreenSocketAccept();
	return 0;
}

void CItemData::ScreenSocketAccept()
{
// 	while (m_pMysocket == NULL)
// 	{
// 		Sleep(50);
// 	}
	m_pMySocket->SocketAccept(SCREEN_TRANS_PORT, MAX_LISTEN);
}

void CItemData::Run()
{
	::CloseHandle(CreateThread(0, 0, OnListen, (LPVOID)this, 0, NULL));
	
// 	// 创建一个线程用于监听屏幕数据传输的SOCKET
// 	::CloseHandle(CreateThread(NULL, 0, OnScreenSocketAccept, (LPVOID)this, 0, 0));
}



void CItemData::SetMsgType(int msgType)
{
	m_msgType = msgType;
}

void CItemData::SetScreenData()
{
	BMPDATA bmpData;
	memset(&bmpData, 0, sizeof(BMPDATA));
	int nRet = m_Mysocket.RecvDataTCP(m_sock, (char*)&bmpData, sizeof(BMPDATA));
	// 		if (0 == nRet)
	// 		{
	// 			CleanData();
	// 			break;
	// 		}
	//	memcpy(&bmpdata, &bmpdata_s, sizeof(bmpdata));

	switch (bmpData.infoType)
	{
	case 0:// 位图头信息
	{
	}
		break;
	case 1: // 位图数据信息
		if (NULL == m_pBmpCompressData)
		{
			int bmpHeadInfoSize = bmpData.bmpHeadInfo.bmiHeader.biSize;
			m_pBMPINFO = (BITMAPINFO*)LocalAlloc(LPTR, bmpHeadInfoSize);
			memcpy(m_pBMPINFO, &bmpData.bmpHeadInfo, bmpHeadInfoSize);

			m_pBmpCompressData = new BYTE[bmpData.bmpCompressSize];
			memset(m_pBmpCompressData, 0, bmpData.bmpCompressSize);
		}
		memcpy_s(m_pBmpCompressData + bmpData.beginPos,
			SCREEN_TRANS_SIZE, bmpData.transData, SCREEN_TRANS_SIZE);
		break;
	case 2: // 接收最后一次发送的数据
	{
				int lastTransSize = bmpData.bmpCompressSize - bmpData.beginPos;
				memcpy_s(m_pBmpCompressData + bmpData.beginPos, lastTransSize,
					bmpData.transData, lastTransSize);
	}
		break;
	default:
		MessageBox(_T("未知的图像数据ID"), _T("提示"), MB_OK);
		CleanData();
		exit(1);
	}
	if (bmpData.isShow && false == m_endMonitor)
	{

		BMP bmp;
		bmp.pBmpCompressData = m_pBmpCompressData;
		bmp.pBMPINFO = m_pBMPINFO;
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
	 	LocalFree(m_pBMPINFO);
	 	m_pBMPINFO = NULL;
	}
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
	// 	if (m_screenSocket != INVALID_SOCKET)
	// 	{
	// 		closesocket(m_screenSocket);
	// 		m_screenSocket = NULL;
	// 	}
	if (m_pBmpCompressData != NULL)
	{
		delete[] m_pBmpCompressData;
		m_pBmpCompressData = NULL;
	}
	if (m_pBMPINFO != NULL)
	{
		LocalFree(m_pBMPINFO);
		m_pBMPINFO = NULL;
	}
}


void CItemData::OnBeginListen()
{
	int nRet;
	MSGTYPE msgType;
// 	msgType.msgID = SYSINFO;
//  	nRet = m_Mysocket->SendDataTCP(m_sock, (char*)&msgType, sizeof(MSGTYPE));
//	CStuInfo stuInfo;
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
		{
// 			SYSTEMINFO systeminfo;
// 			memset(&systeminfo, 0, sizeof(SYSTEMINFO));
// 			m_Mysocket->RecvDataTCP(m_sock, (char*)&systeminfo, sizeof(SYSTEMINFO));
// 			GetSysVer(systeminfo);
			m_stuInfo.GetStuInfo(m_sock);
			::SendMessage(m_hWnd, ID_STUDENTLONGIN, (WPARAM)this, 0);
		}
			break;
// 		case SCREENINFO:
// 		{
// 		}
//			break;
		case BEGINSCREENMONITOR:
		{
// 			if (m_screenDlg.m_hWnd == NULL)
// 			{
// 				break;
// 			}
			SetScreenData();
		}
			break;
		case ENDSCREENMONITOR:
		{
//			m_screenDlg.SetScreenTranEnd(false);
		}
			break;
//  	case MULTICASTBEGIN:
// 			::CloseHandle(CreateThread(NULL, 0, OnSendScreenData, (LPVOID)this, 0, NULL));
// 			break;
// 		case MULTICASTEND:
// 			m_multicast.SetIsStop(true);
// 			break;
// 
// 		case MULTICAST:
// 			break;

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

void CItemData::SetScreenSocket(SOCKET screenSocket)
{
	m_screenSocket = screenSocket;
}


DWORD WINAPI CItemData::OnSendScreenData(LPVOID self)
{
	CItemData* thread = (CItemData*)self;
	thread->SendScreenData();
	return 0;
}

void CItemData::SendScreenData()
{
//	m_multicast.SendScreenData(m_sock);
}






//////////////////////////////////////////////////////////////////////////
	// 磁盘管理
//////////////////////////////////////////////////////////////////////////
void CItemData::RunToFileManager()
{
// 	if (m_filedlg == NULL)
// 	{
// 		m_filedlg = new CFileDlg(m_sock, this);
// 		m_filedlg->Create(IDD_FILE_DIALOG, this);
// 		m_filedlg->ShowWindow(SW_NORMAL);
// 	}
// 	else
// 	{
// 		m_filedlg->SetActiveWindow();
// 	}
	MSGTYPE msgType;
	msgType.msgID = BEGINMULTICAST;
	m_Mysocket.SendDataTCP(m_sock, (char*)&msgType, sizeof(MSGTYPE));
}

//////////////////////////////////////////////////////////////////////////
	// 屏幕监控
//////////////////////////////////////////////////////////////////////////
void CItemData::RunToScreen()
{
// 
// 	if (m_screenDlg.m_hWnd == NULL)
// 	{
// 		m_isScreenFlag = true;
// 		m_screenDlg.SetMsgSocket(m_sock);
// 		m_screenDlg.Create(IDD_SCREEN_DIALOG);
// 		m_screenDlg.ShowWindow(SW_NORMAL);		
// 	}
// 	else
// 	{
// 		m_screenDlg.SetActiveWindow();
// 	}
}


