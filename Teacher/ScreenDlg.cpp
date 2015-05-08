// ScreenDlg.cpp : 实现文件
//

#include "stdafx.h"
//#include "RomateControl.h"
#include "ScreenDlg.h"
#include "afxdialogex.h"
//#include "ItemData.h"


// CScreenDlg 对话框

IMPLEMENT_DYNAMIC(CScreenDlg, CDialogEx)

// CScreenDlg::CScreenDlg(CWnd* pParent /*=NULL*/, SOCKET sock/*=NULL*/, int id /*=0*/)
// 	: CDialogEx(CScreenDlg::IDD, pParent)
// 	, m_isScreenTranEnd(true)
// 	, m_pBmpCompressData(NULL)
// 	, m_pBMPINFO(NULL)
// 	// , m_screenSocket(INVALID_SOCKET)
// 	, m_servSocket(INVALID_SOCKET)
// 	, m_id(id)
// 	, m_isFirst(false)
// {
// 	//	m_pParent = pParent;
// 	m_msgSocket = sock;
// 	m_isScreenFlag = true;
// }

CScreenDlg::CScreenDlg(CWnd* pParent /*=NULL*/, int widthCount /*=0*/)
	: CDialogEx(CScreenDlg::IDD, pParent)
	, m_widthCount(widthCount)
	, isLButtonUp(false)
	, m_pBMPINFO(NULL)
	, m_bmpShowData(NULL)
	, m_isLBtnUp(false)
{

}

CScreenDlg::~CScreenDlg()
{
	// 	if (m_sockMsg != INVALID_SOCKET)
	// 	{
	// 		closesocket(m_sockMsg);
	// 		m_sockMsg = NULL;
	// 	}
	CleanData();
}

void CScreenDlg::CleanData()
{
// 	if (m_screenSocket != INVALID_SOCKET)
// 	{
// 		closesocket(m_screenSocket);
// 		m_screenSocket = NULL;
// 	}
	if (m_bmpShowData != NULL)
	{
		delete[] m_bmpShowData;
		m_bmpShowData = NULL;
	}
	if (m_pBMPINFO != NULL)
	{
		LocalFree(m_pBMPINFO);
		m_pBMPINFO = NULL;
	}
}
void CScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CScreenDlg 消息处理程序


void CScreenDlg::OnClose()
{
//	closesocket(m_screenSocket);
//	m_screenSocket = INVALID_SOCKET;

//	m_Mysocket.SendReadyInfo(m_msgSocket, SCREENTRANEND);
//	CleanData();
//	DestroyWindow();
	// 发送消息给TeacherDlg 让它发送结束屏幕监控的消息给学生机
	::SendMessage(m_pParentWnd->m_hWnd, ID_MONITOREND, 0, 0);
	DestroyWindow();
//	delete this;
	CDialogEx::OnClose();
}


void CScreenDlg::SetScreenTranEnd(bool screenTranEnd)
{
	m_isScreenTranEnd = screenTranEnd;
}

BOOL CScreenDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (WM_KEYDOWN == pMsg->message)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (nVirtKey == VK_RETURN || nVirtKey == VK_ESCAPE)
		{
			return TRUE;
		}
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}


void CScreenDlg::OnSize(UINT nType, int cx, int cy)
{
	//	CDialogEx::OnSize(nType, cx, cy);
	CDialogEx::OnSize(nType, cx, cy);

//	GetClientRect(&m_rect);
	this->GetClientRect(&m_rect);
	m_blockWidth = m_rect.Width() / m_widthCount;
	m_blockHeight = m_rect.Height() / m_widthCount;

}

// void CScreenDlg::SetBMPHEADINFO()
// {
// 	m_Mysock.RecvDataTCP(m_socektScreen, (char*)&m_bmpInfo, sizeof(BMPINFO));
// 	int bmpHeadInfoSize = m_bmpInfo.bmpHeadInfo.bmiHeader.biSize;
// 	m_pBMPINFO = (BITMAPINFO*)LocalAlloc(LPTR, bmpHeadInfoSize);
// 	memcpy(m_pBMPINFO, &m_bmpInfo.bmpHeadInfo, bmpHeadInfoSize);
// 
// 	m_pBmpCompressData = new BYTE[m_bmpInfo.bmpCompressSize];
// 	memset(m_pBmpCompressData, 0, m_bmpInfo.bmpCompressSize);
// }

void CScreenDlg::SetSocket(/*SOCKET socketMsg*/SOCKET socketScreen)
{
//	m_screenSocket = socketScreen;
}

// void CScreenDlg::SetMsgSocket(/*SOCKET socketMsg*/SOCKET msgSocket)
// {
// 	m_sockMsg = msgSocket;
// }

void CScreenDlg::SetScreenData() //显示图像
{
		//BMPDATA bmpData;
		//memset(&bmpData, 0, sizeof(BMPDATA));
		//int nRet = m_Mysocket.RecvDataTCP(m_msgSocket, (char*)&bmpData, sizeof(BMPDATA));

		//switch (bmpData.infoType)
		//{
		//case 0:// 位图头信息
		//{
		//}
		//	break;
		//case 1: // 位图数据信息
		//	if (NULL == m_pBmpCompressData)
		//	{
		//		int bmpHeadInfoSize = bmpData.bmpHeadInfo.bmiHeader.biSize;
		//		m_pBMPINFO = (BITMAPINFO*)LocalAlloc(LPTR, bmpHeadInfoSize);
		//		memcpy(m_pBMPINFO, &bmpData.bmpHeadInfo, bmpHeadInfoSize);

		//		m_pBmpCompressData = new BYTE[bmpData.bmpCompressSize];
		//		memset(m_pBmpCompressData, 0, bmpData.bmpCompressSize);
		//	}
		//	memcpy_s(m_pBmpCompressData + bmpData.beginPos,
		//		SCREEN_TRANS_SIZE, bmpData.transData, SCREEN_TRANS_SIZE);
		//	break;
		//case 2: // 接收最后一次发送的数据
		//{
		//			int lastTransSize = bmpData.bmpCompressSize - bmpData.beginPos;
		//			memcpy_s(m_pBmpCompressData + bmpData.beginPos, lastTransSize,
		//				bmpData.transData, lastTransSize);
		//}
		//	break;
		//default:
		//	MessageBox(_T("未知的图像数据ID"), _T("提示"), MB_OK);
		//	CleanData();
		//	exit(1);
		//}
		//判断传送完以后是否可以显示图像
		//if (bmpData.isShow)
		//{

		//	BYTE* bmpShowData = UnCompressData(m_pBMPINFO->bmiHeader.biSizeImage, 
		//		bmpData.bmpCompressSize);
		//	HDC hdc = GetDC()->m_hDC;
		//	if (hdc != NULL)
		//	{
		//		::StretchDIBits(hdc,
		//			0,
		//			0,
		//			m_rect.Width(),
		//			m_rect.Height(),
		//			0,
		//			0,
		//			m_pBMPINFO->bmiHeader.biWidth,
		//			m_pBMPINFO->bmiHeader.biHeight,
		//			bmpShowData, //位图数据
		//			m_pBMPINFO, //BITMAPINFO 位图信息头
		//			DIB_RGB_COLORS,
		//			SRCCOPY
		//			);
		//	}
		//	delete[]m_pBmpCompressData;
		//	m_pBmpCompressData = NULL;
		//	 		delete[] pUnCompressData;
		//	 		pUnCompressData = NULL;
		//	delete[] bmpShowData;
		//	bmpShowData = NULL;
		//	LocalFree(m_pBMPINFO);
		//	m_pBMPINFO = NULL;
		//}
}




/*
	功能：显示学生机的桌面
	Input:
		bmp--结构体，里边保存了学生机桌面截图的数据
		itemOrder--学生机的顺序，取值为0-15而且是连续的
	Output:

*/
void CScreenDlg::ShowBmp(BMP* bmp, int itemOrder)
{


	m_bmpShowData = UnCompressData(bmp->pBMPINFO->bmiHeader.biSizeImage,
		bmp->bmpCompressSize, bmp->pBmpCompressData);

	int xDest = 0;
	int yDest = 0;
	DetermineShowRegion(itemOrder, xDest, yDest);

	HDC hdc = GetDC()->m_hDC;
	if (hdc != NULL)
	{
		::StretchDIBits(hdc,
			xDest,
			yDest,
			m_blockWidth,
			m_blockHeight,
			0,
			0,
			bmp->pBMPINFO->bmiHeader.biWidth,
			bmp->pBMPINFO->bmiHeader.biHeight,
			m_bmpShowData, //位图数据
			bmp->pBMPINFO, //BITMAPINFO 位图信息头
			DIB_RGB_COLORS,
			SRCCOPY
			);
	}

// 	for (uLongf i = 0; i < bmp->bmpCompressSize; i++)
// 	{
// 		delete bmp->pBmpCompressData[i];
// 	}
// 	BYTE* temp1 = bmp->pBmpCompressData;
// 	delete[] temp1;
// 	temp1 = NULL;
// 
// 	BITMAPINFO* temp2 = bmp->pBMPINFO;
// 	LocalFree(temp2);
// 	temp2 = NULL;
// 	m_pBMPINFO = (BITMAPINFO*)LocalAlloc(LPTR, bmp->pBMPINFO->bmiHeader.biSize);
// 	memcpy_s(m_pBMPINFO, bmp->pBMPINFO->bmiHeader.biSize, 
// 		bmp->pBMPINFO, bmp->pBMPINFO->bmiHeader.biSize);
//	std::unique_lock<std::mutex> lock(m_mtx);
// 	if (false == isLButtonUp)
// 	{
//		CleanData();
// 	}
//	lock.unlock();

	delete[] m_bmpShowData;
	m_bmpShowData = NULL;

}

void CScreenDlg::DetermineShowRegion(int itemOrder, int &xDest, int &yDest)
{
// 	int blockWidth = m_rect.Width() / m_widthCount;
// 	int blockHeight = m_rect.Height() / m_widthCount;
	switch (m_widthCount)
	{
	case 1:
		OneRegion(itemOrder, xDest,  yDest);
		break;
	case 2:
		FourRegion(itemOrder, xDest, yDest);
		break;
	case 3:
		NineRegion(itemOrder, xDest, yDest);
		break;
	case 4:
		SixteenRegion(itemOrder, xDest, yDest);
		break;
	}
}

void CScreenDlg::SixteenRegion(int itemOrder, int &xDest, int &yDest)
{
	NineRegion(itemOrder, xDest, yDest);
	if (3 * m_widthCount <= itemOrder
		&& itemOrder < 4 * m_widthCount)
	{
		xDest = (itemOrder - 3 * m_widthCount) * m_blockWidth;
		yDest = 3 * m_blockHeight;
	}
}

void CScreenDlg::NineRegion(int itemOrder, int &xDest, int &yDest)
{
	FourRegion(itemOrder, xDest, yDest);
	if (2 * m_widthCount <= itemOrder
		&& itemOrder < 3 * m_widthCount)
	{
		xDest = (itemOrder - 2 * m_widthCount) * m_blockWidth;
		yDest = 2 * m_blockHeight;
	}
}

void CScreenDlg::FourRegion(int itemOrder, int &xDest, int &yDest)
{
	OneRegion(itemOrder, xDest, yDest);
	if (m_widthCount <= itemOrder
		&& itemOrder < 2 * m_widthCount)
	{
		xDest = (itemOrder - m_widthCount) * m_blockWidth;
		yDest = m_blockHeight;
	}

}

void CScreenDlg::OneRegion(int itemOrder, int &xDest, int &yDest)
{
	if (itemOrder < m_widthCount)
	{
		xDest = itemOrder * m_blockWidth;
		yDest = 0;
	}
}

BYTE* CScreenDlg::UnCompressData(uLongf biSizeImage, uLongf bmpCompressSize, BYTE* pBmpCompressData)
{
	uLongf unCompressDataLen = (uLongf)((biSizeImage + 12)*(100.1 / 100)) + 1;
	BYTE* pUnCompressData = new BYTE[unCompressDataLen];
	int err = uncompress(pUnCompressData, &unCompressDataLen, 
		pBmpCompressData, bmpCompressSize);
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

BYTE* CScreenDlg::UnCompressData(uLongf biSizeImage, uLongf bmpCompressSize)
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


BOOL CScreenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

// 	MSGTYPE msgType;
// 	msgType.msgID = SCREENDATA;
//	msgType.flag = true;
//	int result = m_Mysock.SendDataTCP(m_sockMsg, (char*)&msgType, sizeof(MSGTYPE));


// 	//	创建一个线程用于监听屏幕数据传输的SOCKET
// 	::CloseHandle(CreateThread(NULL, 0, OnScreenSocketAccept, (LPVOID)this, 0, NULL));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}



DWORD WINAPI CScreenDlg::OnSetScreenData(LPVOID self)
{
	CScreenDlg* screenDlg = (CScreenDlg*) self;
// 	while (true)
// 	{
		screenDlg->SetScreenData();
// 	}
	return 0;
}

void CScreenDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nSBCode != SB_ENDSCROLL)
	{
		SCROLLINFO hStructure;
		GetScrollInfo(SB_HORZ, &hStructure);
		hStructure.nPos = nPos;
		SetScrollInfo(SB_HORZ, &hStructure);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CScreenDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (nSBCode != SB_ENDSCROLL)
	{
		SCROLLINFO vStructure;
		GetScrollInfo(SB_VERT, &vStructure);
		vStructure.nPos = nPos;
		SetScrollInfo(SB_VERT, &vStructure);
	}

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}



DWORD WINAPI CScreenDlg::OnListen(LPVOID lp)
{
	CScreenDlg* t = (CScreenDlg*)lp;
	t->Listen();
	return 0;
}

void CScreenDlg::OnListen()
{
	::CloseHandle(CreateThread(0, 0, OnListen, (LPVOID)this, 0, NULL));
}

void CScreenDlg::Listen()
{
	//m_stuInfo.SendStuInfoReq(m_msgSocket);
	//while (true)
	//{
	//	int msgID = this->RecvDataTCP();
	//	if (0 == msgID)
	//	{
	//		break;
	//	}
	//	switch (msgID)
	//	{
	//	case SYSINFO:
	//					// 			SYSTEMINFO systeminfo;
	//					// 			memset(&systeminfo, 0, sizeof(SYSTEMINFO));
	//					// 			m_Mysocket->RecvDataTCP(m_sock, (char*)&systeminfo, sizeof(SYSTEMINFO));
	//					// 			GetSysVer(systeminfo);
	//		m_stuInfo.GetStuInfo(m_msgSocket);
	//		::SendMessage(m_pParentWnd->m_hWnd, ID_ONLINE, (WPARAM)&m_stuInfo, 0);
	//		break;
	//	case SCREENINFO:

	//		break;
	//	case SCREENDATA:
	//		this->SetScreenData();
	//		break;
	//	case SCREENTRANEND:
	//	{
	//						  // 							  m_screenDlg.SetScreenTranEnd(false);
	//	}
	//		break;
	//	default:
	//		MessageBox(_T("消息有问题"), _T("警告"), MB_OK);
	//		break;
	//	}

	//}
}

int CScreenDlg::RecvDataTCP()
{
// 	MSGTYPE msgType;
// 	int nRet = m_Mysocket.RecvDataTCP(m_msgSocket, (char*)&msgType, sizeof(MSGTYPE));
// 	// SOCKET 连接是否关闭
// 	if (nRet == 0)
// 	{
// 		// SOCKET 是否发生阻塞
// 		if (::WSAGetLastError() == WSAEWOULDBLOCK)
// 		{
// 			Sleep(50);
// 			this->RecvDataTCP();
// 			//			continue;
// 		}
// 		else
// 		{
// 			::closesocket(m_msgSocket);
// 			::SendMessage(m_pParentWnd->m_hWnd, ID_OFFLINE, m_id, 0);
// 			return 0;
// 		}
// 	}
// 	return msgType.msgID;
	return 0;
}


void CScreenDlg::SendDataTCP()
{
//	m_Mysocket.SendReadyInfo(m_msgSocket, SCREENDATA);
}


DWORD WINAPI CScreenDlg::OnShowDlg(LPVOID lp)
{
	CScreenDlg* t = (CScreenDlg*)lp;
	t->DoModal();
	return 0;
}


void CScreenDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (false == m_isLBtnUp)
	{
// 		int itemOrder = 0;
// 		switch (m_widthCount)
// 		{
// 		case 1:
// 			itemOrder = 0;
// 			break;
// 		case 2:
// 			FourRegionClk(point, itemOrder);
// 			break;
// 		case 3:
// 			NineRegionClk(point, itemOrder);
// 			break;
// 		case 4:
// 			SixteenRegionClk(point, itemOrder);
// 			break;
// 		default:
// 			break;
// 		}
// 		// 发送消息给TeacherDlg 对话框，让它处理后续的事情
// 		::SendMessage(m_pParentWnd->m_hWnd, ID_ONESTUINSCREEN, (WPARAM)&itemOrder, 0);
	}
		CDialogEx::OnLButtonUp(nFlags, point);
}


void CScreenDlg::FourRegionClk(CPoint point, int& itemOrder)
{
	if (0 < point.x && point.x < m_blockWidth)
	{
		FourRegionClk(point, itemOrder, 0);
	}
	else if (m_blockWidth < point.x && point.x < 2 * m_blockWidth)
	{
		FourRegionClk(point, itemOrder, 1);
	}
}

void CScreenDlg::NineRegionClk(CPoint point, int& itemOrder)
{
	if (0 < point.x && point.x < m_blockWidth)
	{
		NineRegionClk(point, itemOrder, 0);
	}
	else if (m_blockWidth < point.x && point.x < 2 * m_blockWidth)
	{
		NineRegionClk(point, itemOrder, 1);
	}
	else if (2 * m_blockWidth < point.x && point.x < 3 * m_blockWidth)
	{
		NineRegionClk(point, itemOrder, 2);
	}
}


void CScreenDlg::SixteenRegionClk(CPoint point, int& itemOrder)
{
	if (0 < point.x && point.x < m_blockWidth)
	{
		SixteenRegionClk(point, itemOrder, 0);
	}
	else if (m_blockWidth < point.x && point.x < 2 * m_blockWidth)
	{
		SixteenRegionClk(point, itemOrder, 1);
	}
	else if (2 * m_blockWidth < point.x && point.x < 3 * m_blockWidth)
	{
		SixteenRegionClk(point, itemOrder, 2);
	}
	else if (3 * m_blockWidth < point.x && point.y < 4 * m_blockWidth)
	{
		SixteenRegionClk(point, itemOrder, 3);
	}
}

void CScreenDlg::FourRegionClk(CPoint point, int& itemOrder, int offset)
{
	if (0 < point.y && point.y < m_blockHeight)
	{
		itemOrder = 0 * m_widthCount + offset;
	}
	else if (m_blockHeight < point.y && point.y < 2 * m_blockHeight)
	{
		itemOrder = 1 * m_widthCount + offset;
	}
}

void CScreenDlg::NineRegionClk(CPoint point, int& itemOrder, int offset)
{
	FourRegionClk(point, itemOrder, offset);
	if (2 * m_blockHeight < point.y && point.y < 3 * m_blockHeight)
	{
		itemOrder = 2 * m_widthCount + offset;
	}
}

void CScreenDlg::SixteenRegionClk(CPoint point, int& itemOrder, int offset)
{
	NineRegionClk(point, itemOrder, offset);
	if (3 * m_blockHeight < point.y && 4 * m_blockHeight)
	{
		itemOrder = 3 * m_widthCount + offset;
	}
}

void CScreenDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int itemOrder = 0;
	switch (m_widthCount)
	{
	case 1:
//		itemOrder = 0;
		break;
	case 2:
		FourRegionClk(point, itemOrder);
		break;
	case 3:
		NineRegionClk(point, itemOrder);
		break;
	case 4:
		SixteenRegionClk(point, itemOrder);
		break;
	default:
		break;
	}
	if (m_widthCount != 1)
	{
		// 发送消息给TeacherDlg 对话框，让它处理后续的事情
		::SendMessage(m_pParentWnd->m_hWnd, ID_ONESTUINSCREEN, 0, (LPARAM)&itemOrder);
	}
	CDialogEx::OnLButtonDblClk(nFlags, point);
}
