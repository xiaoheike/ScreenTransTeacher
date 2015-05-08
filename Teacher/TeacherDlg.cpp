
// TeacherDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Teacher.h"
#include "TeacherDlg.h"
#include "afxdialogex.h"
#include "ItemData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTeacherDlg 对话框



CTeacherDlg::CTeacherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTeacherDlg::IDD, pParent)
	, m_msgSocket(INVALID_SOCKET)
	, m_pScreenDlg(NULL)
	, m_oneStuScreenDlg(NULL)
//	, m_pScreenDlg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	memset(m_item, -1, sizeof(int)*MAX_MONITOR);
// 	for (int i = 0; i < MAX_LISTEN; i++)
// 	{
// 		m_pScreenDlg[i] = NULL;
// 	}
}

void CTeacherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(CTeacherDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(ID_ONLINE, OnAddStu)
	ON_MESSAGE(ID_OFFLINE, OnOffStu)
	ON_MESSAGE(ID_SETBMPDATA, OnSetBmpCompressData)
	ON_MESSAGE(ID_MONITOREND, OnMonitorEnd)
	ON_MESSAGE(ID_ONESTUINSCREEN, OnOneStuInScreen)
	ON_MESSAGE(ID_SETONESTUBMPDATA, OnSetOneStuBmpCompData)
	ON_MESSAGE(ID_ONESTUINSCREENEND, OnOneStuInScreenEnd)
	ON_COMMAND(ID_MULTICAST, OnMulticast)
	ON_COMMAND(ID_SCREEN, OnScreen)
//	ON_COMMAND(ID_SCREEN, OnScreen)
//ON_WM_LBUTTONUP()
ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTeacherDlg 消息处理程序

BOOL CTeacherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	UpdateMain(LISTEN_PORT);
	// 创建一个线程用于监听服务端上线SOCKET
	::CloseHandle(CreateThread(0, 0, OnMsgListen, (LPVOID)this, 0, NULL));

// 	//	创建一个线程用于监听屏幕数据传输的SOCKET
// 	::CloseHandle(CreateThread(NULL, 0, OnScreenSocketAccept, (LPVOID)this, 0, NULL));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


DWORD WINAPI CTeacherDlg::OnScreenSocketAccept(LPVOID self)
{
	CTeacherDlg* thread = (CTeacherDlg*)self;
	thread->ScreenSocketAccept();
	return 0;
}

void CTeacherDlg::ScreenSocketAccept()
{
	// 	//	m_Mysock.SocketAccept(SCREEN_TRANS_PORT, MAX_LISTEN);
	// 
	// 	CMySocket mySocket;
	// 	mySocket.InitSocketInfo(SCREEN_TRANS_PORT, MAX_LISTEN);
	// 	//	SOCKET SerSocket;
	// 	SOCKADDR_IN Seraddr;
	// 	int Seraddrsize = sizeof(Seraddr);
	// 
	// 	//	m_pMysocket = new CMySocket();
	// 
	// 	// 	while (true)
	// 	// 	{
	// 	m_servSocket = accept(mySocket.GetSocketInit(), (sockaddr *)&Seraddr, &Seraddrsize);
	// 	// 		if (m_servSocket == INVALID_SOCKET)
	// 	// 		{
	// 	// 			break;
	// 	// 		}
	// 	//	}
// 	while (INVALID_SOCKET == m_servSocket)
// 	{
// 		m_screenSocket = m_Mysock.SocketAccept(SCREEN_TRANS_PORT, MAX_LISTEN);
// 		Sleep(10);
// 	}
}

void CTeacherDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CTeacherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTeacherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI CTeacherDlg::OnMsgListen(LPVOID self) //初始化Socket线程
{
	CTeacherDlg* teacherDlg = (CTeacherDlg*)self;
	teacherDlg->MsgListen();
	return 0;
}

bool CTeacherDlg::MsgListen() //初始化SOCKET
{
	CMySocket mySocket;
	mySocket.InitSocketInfo(LISTEN_PORT, MAX_LISTEN);
	//	SOCKET m_servSocket;
	SOCKADDR_IN Seraddr;
	int Seraddrsize = sizeof(Seraddr);

	while (true)
	{
		m_msgSocket = accept(mySocket.GetSocketInit(), (sockaddr *)&Seraddr, &Seraddrsize);
		if (m_msgSocket != INVALID_SOCKET)
		{
// 			// 这里的i 是与ListControl 控件r ItamDat是对应的，每行都有一个序号
// 			// 例如ListControl 中的某一行ItemData 为1，则它对应m_pScreenDlg[1];
// 			for (int i = 0; i < MAX_LISTEN;  i++)
// 			{
// 				m_list.SetItemData(m_list.GetItemCount(), (DWORD_PTR)i);
// 				if (NULL == m_pScreenDlg[i])
// 				{
// 					m_pScreenDlg[i] = new CScreenDlg(this, m_msgSocket, i);
// 					m_pScreenDlg[i]->OnListen();
// 					break;
// 				}
// 			}

			int id;
			CItemData *itemData = NULL;
			int Count = m_list.GetItemCount();
			if (Count == 0)
			{
				id = 0;
			}
			else
			{
				id = 0;
				for (int i = 0; i < Count; i++)
				{
					itemData = (CItemData*)m_list.GetItemData(i);
					if (itemData->m_id == id)
					{
						i = 0;
						id = id + 1;
						continue;
					}
				}
			}
//			itemData = new CItemData(id, sock, &addr, this->m_hWnd, &m_Mysock);
			//	item = new CItemData(m_socketData, &addr, this->m_hWnd);
			itemData = new CItemData(id, m_hWnd, m_msgSocket);
			itemData->Run();

		}
	}
	return true;
}

/*
void CTeacherDlg::GetNewItemData()
{
	int itemCount = m_list.GetItemCount();
	

	int id;
	if (itemCount == 0)
	{
		id = 0;
	}
	else
	{
		id = 0;
		for (int i = 0; i < itemCount; i++)
		{
			m_itemData = (CItemData*)m_list.GetItemData(i);
			if (m_itemData->m_id == id)
			{
				i = 0;
				id = id + 1;
				continue;
			}
		}
	}
}
*/

void CTeacherDlg::UpdateMain(int port)
{
	m_imagelist.Create(32, 32, ILC_COLOR24 | ILC_MASK, 1, 1);
	CBitmap bmp;
	bmp.LoadBitmapW(IDB_MULTICAST);
	m_imagelist.Add(&bmp, RGB(255, 255, 255));
	bmp.DeleteObject();
	bmp.LoadBitmapW(IDB_SCREEN);
	m_imagelist.Add(&bmp, RGB(255, 255, 255));
	bmp.DeleteObject();
	// 设置每一个工具栏的ID，之后要添加消息响应
	//	const UINT t[10] = { 1001, 1002, 1003, 1004, 1005, 1006, 0, 1007, 1008, 1009};
	const UINT t[2] = { 1001, 1002};

	m_toolBar.CreateEx(this);
	m_toolBar.SetButtons(t, 2);
	m_toolBar.SetSizes(CSize(60, 56), CSize(24, 24));
	// 工具栏添加文字
	m_toolBar.SetButtonText(0, _T("屏幕投影"));
	m_toolBar.SetButtonText(1, _T("屏幕监控"));

	// 工具栏添加图片
	m_toolBar.GetToolBarCtrl().SetImageList(&m_imagelist);

	// 底部状态栏的创建
	BOOL hbar = m_statusbar.Create(this);
	// 状态栏的ID
	UINT b[2] = { 1009, 1010 };
	hbar = m_statusbar.SetIndicators(b, 2);
	m_statusbar.SetPaneInfo(0, b[0], SBPS_NORMAL, 400);
	m_statusbar.SetPaneInfo(1, b[1], SBPS_NORMAL, 180);
	CString ListemPort;
	ListemPort.Format(_T("监听端口：%d"), port);
	m_statusbar.SetPaneText(0, ListemPort);
	m_statusbar.SetPaneText(1, _T("在线主机：0 台"));

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	// 设置List Control 风格为整行选中，网格，带有复选框（控件的View属性值要设置为Report）
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// 给List Control 添加列
	m_list.InsertColumn(0, _T("姓名"), LVCFMT_CENTER, 180);
	m_list.InsertColumn(1, _T("姓名"), LVCFMT_CENTER, 180);
	m_list.InsertColumn(2, _T("学号"), LVCFMT_CENTER, 180);
	m_list.InsertColumn(3, _T("专业"), LVCFMT_CENTER, 180);
	m_list.InsertColumn(4, _T("班级"), LVCFMT_CENTER, 180);
	m_list.DeleteColumn(0);
	//	m_list.InsertColumn(4, _T("服务端版本号"), LVCFMT_LEFT, 100);

	GetClientRect(&m_rect);
}

/*
void CTeacherDlg::OnFileManager()
{
	static bool isStop = false;
	if (false == isStop)
	{
		m_multicast.SetIsStop(false);
// 		for (int i = 0; i < m_list.GetItemCount(); i++)
// 		{
// 			CItemData* t = (CItemData*)m_list.GetItemData(i);
// 			m_Mysock.SendReadyInfo(t->m_sock, MULTICASTBEGIN);
// 		}

//		::CloseHandle(CreateThread(0, 0, OnSendScreenData, (LPVOID)this, 0, NULL));
		isStop = true;
	}
	else
	{
		m_multicast.SetIsStop(true);
		//		closesocket(m_socketMulticast);
		//		m_socketMulticast = INVALID_SOCKET;
		for (int i = 0; i < m_list.GetItemCount(); i++)
		{
// 			CItemData* t = (CItemData*)m_list.GetItemData(i);
// 			m_Mysock.SendReadyInfo(t->m_sock, MULTICASTEND);
		}
		isStop = false;
	}

}
*/

LRESULT CTeacherDlg::OnAddStu(WPARAM wparam, LPARAM lparam)
{
// 	CStuInfo *stuInfo = (CStuInfo*)wparam;
// 	int i = m_list.GetItemCount();
// 	m_list.InsertItem(i, stuInfo->GetStuName(), 0);
// 	m_list.SetItemText(i, 1, stuInfo->GetStuID());
// 	m_list.SetItemText(i, 2, stuInfo->GetStuMajor());
// 	m_list.SetItemText(i, 3, stuInfo->GetStuClass());
// 	ShowStuInfo();
// 	return 0;

	CItemData *t = (CItemData*)wparam;
	int i = m_list.GetItemCount();
	m_list.InsertItem(i, t->m_stuInfo.GetStuName(), 0);
	m_list.SetItemText(i, 1, t->m_stuInfo.GetStuID());
	m_list.SetItemText(i, 2, t->m_stuInfo.GetStuMajor());
	//	m_list.SetItemText(i, 3, _T("未开启"));
	m_list.SetItemText(i, 3, t->m_stuInfo.GetStuClass());
	m_list.SetItemData(i, (DWORD)t);
	ShowStuInfo();
	return 0;
}

void  CTeacherDlg::ShowStuInfo()
{
	int c = m_list.GetItemCount();
	CString online;
	TCHAR* szText;
	online.Format(_T("在线主机：%d 台"), c);
	szText = online.GetBuffer(online.GetLength());
	::SendMessageW(m_statusbar, SB_SETTEXTW, (WPARAM)1, (LPARAM)szText);
	online.ReleaseBuffer();
}

LRESULT CTeacherDlg::OnOffStu(WPARAM wparam, LPARAM lparam) //主机下线消息体
{
	CItemData* t;
	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		t = (CItemData*)m_list.GetItemData(i);
		if (t->m_id == (int)wparam)
		{
			::closesocket(t->GetSocket());
			delete t;
			m_list.DeleteItem(i);
		}
	}
	ShowStuInfo();
	return 0;


// 	int id = (int)wparam;	
// 	delete m_pScreenDlg[id];
// 	m_pScreenDlg[id] = NULL;
// 	m_list.DeleteItem(id);
// 	ShowStuInfo();
// 	return 0;
}


LRESULT CTeacherDlg::OnSetBmpCompressData(WPARAM wparam, LPARAM lparam)
{
	BMP* bmp = (BMP*)wparam;
	int* itemOrder = (int*)lparam;
// 	BYTE* bmpCompressData = (BYTE*)wparam;
// 	BITMAPINFO* BMPINFO = (BITMAPINFO*)lparam;
	m_pScreenDlg->ShowBmp(bmp, *itemOrder);
// 	delete[] bmp->pBmpCompressData;
// 	bmp->pBmpCompressData = NULL;
// 	LocalFree(bmp->pBMPINFO);
// 	bmp->pBMPINFO = NULL;
	return 0;
}

LRESULT CTeacherDlg::OnMonitorEnd(WPARAM wparam, LPARAM lparam)
{
	for (int i = 0; i < MAX_MONITOR; i++)
	{
		if (-1 != m_item[i])
		{
			CItemData* itemData = (CItemData*)m_list.GetItemData(m_item[i]);
			itemData->EndMonitor();
		}
		else if (-1 == m_item[i])
		{
			break;
		}
	}
	// 使工具栏的屏幕监控按钮变亮且有效
	m_toolBar.GetToolBarCtrl().EnableButton(ID_SCREEN, true);
	return 0;
}

LRESULT CTeacherDlg::OnOneStuInScreen(WPARAM wparam, LPARAM lparam)
{
	int* itemOrder = (int*)lparam;
	int item = m_item[*itemOrder];
	if (-1 != item)
	{
		CItemData* itemData = (CItemData*)m_list.GetItemData(item);
		itemData->EndMonitor();
		if (m_oneStuScreenDlg != NULL)
		{
			delete m_oneStuScreenDlg;
			m_oneStuScreenDlg = NULL;
		}
		m_oneStuScreenDlg = new COneStuScreenDlg(this);
		m_oneStuScreenDlg->Create(IDD_SCREEN_DIALOG1);
//		m_oneStuScreenDlg->ShowWindow(SW_MAXIMIZE);
		m_oneStuScreenDlg->ShowWindow(SW_NORMAL);
		itemData->BeginMonitor(true, *itemOrder);
	}

	return 0;
}

LRESULT CTeacherDlg::OnSetOneStuBmpCompData(WPARAM wparam, LPARAM lparam)
{
	BMP* bmp = (BMP*)wparam;
	int* itemOrder = (int*)lparam;
	m_oneStuScreenDlg->ShowBmp(bmp, *itemOrder);
	return 0;
}

LRESULT CTeacherDlg::OnOneStuInScreenEnd(WPARAM wparam, LPARAM lparam)
{
	int* itemOrder = (int*)lparam;
	int item = m_item[*itemOrder];
	if (-1 != item)
	{
		CItemData* itemData = (CItemData*)m_list.GetItemData(item);
		itemData->EndMonitor();
		itemData->BeginMonitor(*itemOrder);
	}


	return 0;
}

void CTeacherDlg::OnScreen()
{
	int selCount = m_list.GetSelectedCount();
	if (selCount > 16)
	{
		MessageBox(_T("同时监控的学生机子数量不能超过16台"));
		return;
	}
	if (NULL == m_pScreenDlg)
	{
		int widthCount = (int)ceil((sqrt(double(selCount))));	
		m_pScreenDlg = new CScreenDlg(this, widthCount);
		m_pScreenDlg->Create(IDD_SCREEN_DIALOG);
//		m_pScreenDlg->ShowWindow(SW_MAXIMIZE);
		m_pScreenDlg->ShowWindow(SW_NORMAL);



		POSITION pos_s;
		int itemOrder = 0;
		pos_s = m_list.GetFirstSelectedItemPosition();
		while (pos_s != NULL)
		{
			int item = m_list.GetNextSelectedItem(pos_s);
			m_item[itemOrder] = item;
			CItemData* itemData = (CItemData*)m_list.GetItemData(item);
			itemData->BeginMonitor(itemOrder);
			itemOrder = itemOrder + 1;
		}
		// 使工具栏的屏幕监控按钮变灰且无效
		m_toolBar.GetToolBarCtrl().EnableButton(ID_SCREEN, false);
	}
	else
	{
		delete m_pScreenDlg;
		m_pScreenDlg = NULL;

		int widthCount = (int)ceil((sqrt(double(selCount))));
		m_pScreenDlg = new CScreenDlg(this, widthCount);
		m_pScreenDlg->Create(IDD_SCREEN_DIALOG);
		m_pScreenDlg->ShowWindow(SW_NORMAL);


		POSITION pos_s;
		int itemOrder = 0;
		pos_s = m_list.GetFirstSelectedItemPosition();
		while (pos_s != NULL)
		{
			int item = m_list.GetNextSelectedItem(pos_s);
			m_item[itemOrder] = item;
			CItemData* itemData = (CItemData*)m_list.GetItemData(item);
			itemData->BeginMonitor(itemOrder);
			itemOrder = itemOrder + 1;
		}
		// 使工具栏的屏幕监控按钮变灰且无效
		m_toolBar.GetToolBarCtrl().EnableButton(ID_SCREEN, false);
	}
}

void CTeacherDlg::OnMulticast()
{
	static bool isStop = false;
	if (false == isStop)
	{
		m_multicast.SetIsStop(false);
		for (int i = 0; i < m_list.GetItemCount(); i++)
		{
			CItemData* itemData = (CItemData*)m_list.GetItemData(i);
			itemData->BeginMulticast();
		}
		::CloseHandle(CreateThread(0, 0, OnSendScreenData, (LPVOID)this, 0, NULL));
		isStop = true;
	}
	else
	{
		m_multicast.SetIsStop(true);
		//		closesocket(m_socketMulticast);
		//		m_socketMulticast = INVALID_SOCKET;
		for (int i = 0; i < m_list.GetItemCount(); i++)
		{
			CItemData* itemData = (CItemData*)m_list.GetItemData(i);
			itemData->EndMulticast();
		}
		isStop = false;
	}
}

DWORD WINAPI CTeacherDlg::OnSendScreenData(LPVOID self)
{
	CTeacherDlg* t = (CTeacherDlg*)self;
	t->SendScreenData();
	return 0;
}

void CTeacherDlg::SendScreenData()
{
	//	CMulticast multicast;
// 	SOCKADDR_IN addr;
// 	SOCKET socketMulticast = m_Mysock.InitMulticastSocket(MULTICAST_TRANS_PORT,
// 		MULTICAST_IP, addr);
	m_multicast.SendScreenData();
	//	m_multicast.CleanData();
	// 	closesocket(socketMulticast);
	// 	socketMulticast = NULL;
}

//void CTeacherDlg::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO:  在此添加消息处理程序代码和/或调用默认值
//
//	CDialogEx::OnLButtonUp(nFlags, point);
//}


void CTeacherDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	if (m_pScreenDlg != NULL)
	{
		delete m_pScreenDlg;
		m_pScreenDlg = NULL;
	}
	if (m_oneStuScreenDlg != NULL)
	{
		delete m_oneStuScreenDlg;
		m_oneStuScreenDlg = NULL;
	}
	// TODO:  在此处添加消息处理程序代码
}
