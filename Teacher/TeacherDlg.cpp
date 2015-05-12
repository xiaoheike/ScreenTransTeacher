/******************************************************************* 
FileName: TeacherDlg.cpp
Author	: xiaoheike
Date	: 2015-05-11 15:13:57
Desc	: 教师机的主对话框，类似一个中转站，监听以及数据经过这个类转
到其他的类中
*******************************************************************/ 
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
	, m_socketMsg(INVALID_SOCKET)
	, m_pScreenDlg(NULL)
	, m_oneStuScreenDlg(NULL)
	, m_pMulticast(NULL)
	, m_isStopMulticast(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	memset(m_item, -1, sizeof(int)*MAX_MONITOR);
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
	ON_MESSAGE(ID_STUDENTLONGIN, OnAddStudent)
	ON_MESSAGE(ID_STUDENTLOGINOUT, OnLoginOutStudent)
	ON_MESSAGE(ID_SETBMPDATA, OnSetBmpCompressData)
	ON_MESSAGE(ID_MONITOREND, OnEndMonitor)
	ON_MESSAGE(ID_ONESTUINSCREEN, OnCreatOneStudentDlg)
	ON_MESSAGE(ID_SETONESTUBMPDATA, OnSetOneStuBmpCompressData)
	ON_MESSAGE(ID_ONESTUINSCREENEND, OnEndOneStudentMonitor)
	ON_COMMAND(ID_MULTICAST, OnMulticast)
	ON_COMMAND(ID_SCREENMONITOR, OnBeginScreenMonitor)
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
	SetMainDlgSytle(LISTEN_PORT);
	// 创建一个线程用于监听服务端上线SOCKET
	::CloseHandle(CreateThread(0, 0, OnMsgListen, (LPVOID)this, 0, NULL));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

/******************************************************************
Function	: OnMsgListen
Date		: 2015-05-11 15:15:25
Author		: xiaoheike
Parameter	: self--CTeacherDlg类
Return		: DWORD WINAPI
Desc		: 监听学生机登录线程的运行方法
******************************************************************/
DWORD WINAPI CTeacherDlg::OnMsgListen(LPVOID self)
{
	CTeacherDlg* teacherDlg = (CTeacherDlg*)self;
	teacherDlg->ListenStudentLogin();
	return 0;
}

/******************************************************************
Function	: ListenStudentLogin
Date		: 2015-05-11 15:16:18
Author		: xiaoheike
Parameter	: 无
Return		: bool
Desc		: 监听学生登录并产生唯一的ID
******************************************************************/
bool CTeacherDlg::ListenStudentLogin()
{
	CMySocket mySocket;
	mySocket.InitSocketInfo(LISTEN_PORT, MAX_LISTEN);
	//	SOCKET m_servSocket;
	SOCKADDR_IN Seraddr;
	int Seraddrsize = sizeof(Seraddr);

	while (true)
	{
		m_socketMsg = accept(mySocket.GetSocketInit(), (sockaddr *)&Seraddr, &Seraddrsize);
		if (m_socketMsg != INVALID_SOCKET)
		{
			SetLoginStudentID();
		}
	}
	return true;
}

/******************************************************************
Function	: SetLoginStudentID
Date		: 2015-05-11 15:19:50
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 给登录学生机分配唯一的ID
******************************************************************/
void CTeacherDlg::SetLoginStudentID()
{
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
	itemData = new CItemData(id, m_hWnd, m_socketMsg);
	// 监听消息的线程运行
	itemData->Run();
}

/******************************************************************
Function	: SetMainDlgSytle
Date		: 2015-05-11 15:20:51
Author		: xiaoheike
Parameter	: port--监听学生登录的端口号
Return		: void
Desc		: 设置教师机的主界面
******************************************************************/
void CTeacherDlg::SetMainDlgSytle(int port)
{
	LoadBitBmp();

	CreatToolBar();

	CreateStatusBar(port);

	InsertColumn();

	GetClientRect(&m_rect);
}


/******************************************************************
Function	: InsertColumn
Date		: 2015-05-11 15:29:24
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 为ListControl 插入列头
******************************************************************/
void CTeacherDlg::InsertColumn()
{
	// 设置List Control 风格为整行选中，网格，带有复选框（控件的View属性值要设置为Report）
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	// 给List Control 添加列
	m_list.InsertColumn(0, _T("姓名"), LVCFMT_CENTER, 180);
	m_list.InsertColumn(1, _T("姓名"), LVCFMT_CENTER, 180);
	m_list.InsertColumn(2, _T("学号"), LVCFMT_CENTER, 180);
	m_list.InsertColumn(3, _T("专业"), LVCFMT_CENTER, 180);
	m_list.InsertColumn(4, _T("班级"), LVCFMT_CENTER, 180);
	m_list.DeleteColumn(0);
}

/******************************************************************
Function	: CreateStatusBar
Date		: 2015-05-11 15:30:17
Author		: xiaoheike
Parameter	: port--监听学生登录的端口号
Return		: void
Desc		: 创建状态栏
******************************************************************/
void CTeacherDlg::CreateStatusBar(int port)
{
	// 底部状态栏的创建
	BOOL hbar = m_statusBar.Create(this);
	// 状态栏的ID
	UINT b[2] = { 1009, 1010 };
	hbar = m_statusBar.SetIndicators(b, 2);
	m_statusBar.SetPaneInfo(0, b[0], SBPS_NORMAL, 400);
	m_statusBar.SetPaneInfo(1, b[1], SBPS_NORMAL, 180);
	CString ListemPort;
	ListemPort.Format(_T("监听端口：%d"), port);
	m_statusBar.SetPaneText(0, ListemPort);
	m_statusBar.SetPaneText(1, _T("在线主机：0 台"));
	// 显示工具条与状态条
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}

/******************************************************************
Function	: CreatToolBar
Date		: 2015-05-11 15:31:02
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 创建工具栏
******************************************************************/
void CTeacherDlg::CreatToolBar()
{
	// 设置每一个工具栏的ID，之后要添加消息响应
	const UINT t[2] = { 1001, 1002 };
	m_toolBar.CreateEx(this);
	m_toolBar.SetButtons(t, 2);
	m_toolBar.SetSizes(CSize(60, 56), CSize(24, 24));
	// 工具栏添加文字
	m_toolBar.SetButtonText(0, _T("屏幕投影"));
	m_toolBar.SetButtonText(1, _T("屏幕监控"));

	// 工具栏添加图片
	m_toolBar.GetToolBarCtrl().SetImageList(&m_imagelist);
}

/******************************************************************
Function	: LoadBitBmp
Date		: 2015-05-11 15:32:49
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 加载位图资源
******************************************************************/
void CTeacherDlg::LoadBitBmp()
{
	m_imagelist.Create(32, 32, ILC_COLOR24 | ILC_MASK, 1, 1);
	CBitmap bmp;
	bmp.LoadBitmapW(IDB_MULTICAST);
	m_imagelist.Add(&bmp, RGB(255, 255, 255));
	bmp.DeleteObject();
	bmp.LoadBitmapW(IDB_SCREEN);
	m_imagelist.Add(&bmp, RGB(255, 255, 255));
	bmp.DeleteObject();
}

/******************************************************************
Function	: OnAddStudent
Date		: 2015-05-11 15:40:25
Author		: xiaoheike
Parameter	: wparam--响应消息时传入的数值，这里是CItemData类对象
lparam--响应消息时传入的数值，这里没有使用
Return		: LRESULT
Desc		: 响应ID_STUDENTLOGIN 消息
******************************************************************/
LRESULT CTeacherDlg::OnAddStudent(WPARAM wparam, LPARAM lparam)
{
	CItemData *t = (CItemData*)wparam;
	int i = m_list.GetItemCount();
	m_list.InsertItem(i, t->m_stuInfo.GetStuName(), 0);
	m_list.SetItemText(i, 1, t->m_stuInfo.GetStuID());
	m_list.SetItemText(i, 2, t->m_stuInfo.GetStuMajor());
	m_list.SetItemText(i, 3, t->m_stuInfo.GetStuClass());
	m_list.SetItemData(i, (DWORD)t);
	ShowStudentInfo();
	return 0;
}

/******************************************************************
Function	: ShowStudentInfo
Date		: 2015-05-11 15:43:48
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 显示登录学生机的信息
******************************************************************/
void  CTeacherDlg::ShowStudentInfo()
{
	int c = m_list.GetItemCount();
	CString online;
	TCHAR* szText;
	online.Format(_T("在线主机：%d 台"), c);
	szText = online.GetBuffer(online.GetLength());
	::SendMessageW(m_statusBar.m_hWnd, SB_SETTEXTW, (WPARAM)1, (LPARAM)szText);
	online.ReleaseBuffer();
}

/******************************************************************
Function	: OnLoginOutStudent
Date		: 2015-05-11 15:44:36
Author		: xiaoheike
Parameter	: wparam--响应消息时传入的数值，这里是登录学生的ID 值
lparam--响应消息时传入的数值，这里没有使用
Return		: LRESULT
Desc		: 登录学生下线，响应ID_STUDENTLOGINOUT 消息
******************************************************************/
LRESULT CTeacherDlg::OnLoginOutStudent(WPARAM wparam, LPARAM lparam)
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
	ShowStudentInfo();
	return 0;
}


/******************************************************************
Function	: OnSetBmpCompressData
Date		: 2015-05-11 15:47:17
Author		: xiaoheike
Parameter	: wparam--响应消息时传入的数值，这里是BMP结构体对象
lparam--响应消息时传入的数值，这里没有使用
Return		: LRESULT
Desc		: 接收来自学生机的屏幕图像数据ID_SETBMPDATA 消息
******************************************************************/
LRESULT CTeacherDlg::OnSetBmpCompressData(WPARAM wparam, LPARAM lparam)
{
	BMP* bmp = (BMP*)wparam;
	int* itemOrder = (int*)lparam;
	m_pScreenDlg->ShowBmp(bmp, *itemOrder);
	return 0;
}

/******************************************************************
Function	: OnEndMonitor
Date		: 2015-05-11 15:48:39
Author		: xiaoheike
Parameter	: wparam--响应消息时传入的数值，这里没有使用
lparam--响应消息时传入的数值，这里没有使用
Return		: LRESULT
Desc		: 结束对学生机的监控响应ID_MONITOREND
******************************************************************/
LRESULT CTeacherDlg::OnEndMonitor(WPARAM wparam, LPARAM lparam)
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
	m_toolBar.GetToolBarCtrl().EnableButton(ID_SCREENMONITOR, true);
	return 0;
}

/******************************************************************
Function	: OnCreatOneStudentDlg
Date		: 2015-05-11 15:50:39
Author		: xiaoheike
Parameter	: wparam--响应消息时传入的数值，这里没有使用
lparam--响应消息时传入的数值，这里是学生在一对多对话框中显示的序号
Return		: LRESULT
Desc		: 响应一对多屏幕监控双击选中学生机实现一对一屏幕监控
响应ID_ONSTUINSCREEN 消息
******************************************************************/
LRESULT CTeacherDlg::OnCreatOneStudentDlg(WPARAM wparam, LPARAM lparam)
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
		m_oneStuScreenDlg->ShowWindow(SW_NORMAL);
		itemData->BeginMonitor(true, *itemOrder);
	}

	return 0;
}

/******************************************************************
Function	: OnSetOneStuBmpCompressData
Date		: 2015-05-11 15:52:26
Author		: xiaoheike
Parameter	: wparam--响应消息时传入的数值，这里是BMP结构体对象
lparam--响应消息时传入的数值，这里是学生在一对多对话框中显示的序号
Return		: LRESULT
Desc		: 接收来自学生机的屏幕图像数据响应消息ID_STEONSTUSCREENDATA
******************************************************************/
LRESULT CTeacherDlg::OnSetOneStuBmpCompressData(WPARAM wparam, LPARAM lparam)
{
	BMP* bmp = (BMP*)wparam;
	int* itemOrder = (int*)lparam;
	m_oneStuScreenDlg->ShowBmp(bmp, *itemOrder);
	return 0;
}

/******************************************************************
Function	: OnEndOneStudentMonitor
Date		: 2015-05-11 15:54:21
Author		: xiaoheike
Parameter	: wparam--响应消息时传入的数值，这里没有使用
lparam--响应消息时传入的数值，这里是学生在一对多对话框中显示的序号
Return		: LRESULT
Desc		: 结束一对一屏幕监控
******************************************************************/
LRESULT CTeacherDlg::OnEndOneStudentMonitor(WPARAM wparam, LPARAM lparam)
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

/******************************************************************
Function	: OnBeginScreenMonitor
Date		: 2015-05-11 15:55:07
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 响应消息ID_ONSCREENMONITOR并调用发送屏幕监控消息到
学生机的方法
******************************************************************/
void CTeacherDlg::OnBeginScreenMonitor()
{
	int selCount = m_list.GetSelectedCount();
	if (selCount > 16)
	{
		MessageBox(_T("同时监控的学生机子数量不能超过16台"));
		return;
	}
	if (0 == selCount)
	{
		MessageBox(_T("请选择要监控的学生机"));
		return;
	}
	DeletepScreenDlg();
	BeginScreenMonitor(selCount);
}

/******************************************************************
Function	: BeginScreenMonitor
Date		: 2015-05-11 15:57:07
Author		: xiaoheike
Parameter	: selCount--要监控的学生机的数量
Return		: void
Desc		: 发送开始屏幕监控消息到教师机
******************************************************************/
void CTeacherDlg::BeginScreenMonitor(int selCount)
{
	int widthCount = (int)ceil((sqrt(double(selCount))));
	m_pScreenDlg = new CScreenDlg(this, widthCount);
	m_pScreenDlg->Create(IDD_SCREEN_DIALOG);
	m_pScreenDlg->ShowWindow(SW_NORMAL);


	POSITION pos_s;
	int itemOrder = 0;
	pos_s = m_list.GetFirstSelectedItemPosition();
	memset(m_item, -1, sizeof(int)*MAX_MONITOR);
	while (pos_s != NULL)
	{
		int item = m_list.GetNextSelectedItem(pos_s);
		m_item[itemOrder] = item;
		CItemData* itemData = (CItemData*)m_list.GetItemData(item);
		itemData->BeginMonitor(itemOrder);
		itemOrder = itemOrder + 1;
	}
	// 使工具栏的屏幕监控按钮变灰且无效
	m_toolBar.GetToolBarCtrl().EnableButton(ID_SCREENMONITOR, false);
}

/******************************************************************
Function	: OnMulticast
Date		: 2015-05-11 15:57:50
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 开始广播响应消息ID_MULTICAST
******************************************************************/
void CTeacherDlg::OnMulticast()
{
	m_pMulticast = new CMulticast();
	if (false == m_isStopMulticast)
	{
		m_pMulticast->SetIsStop(false);
		for (int i = 0; i < m_list.GetItemCount(); i++)
		{
			CItemData* itemData = (CItemData*)m_list.GetItemData(i);
			itemData->BeginMulticast();
		}
		::CloseHandle(CreateThread(0, 0, OnSendScreenData, (LPVOID)this, 0, NULL));
		m_isStopMulticast = true;
	}
	else
	{
		for (int i = 0; i < m_list.GetItemCount(); i++)
		{
		 	CItemData* itemData = (CItemData*)m_list.GetItemData(i);
		 	itemData->EndMulticast();
		}
		m_pMulticast->SetIsStop(true);
		m_isStopMulticast = false;
		DeletepMulticast();
	}
}

/******************************************************************
Function	: OnSendScreenData
Date		: 2015-05-11 15:58:37
Author		: xiaoheike
Parameter	: self--CTeacherDlg类对象
Return		: DWORD WINAPI
Desc		: 发送教师机屏幕图像数据线程的调用方法
******************************************************************/
DWORD WINAPI CTeacherDlg::OnSendScreenData(LPVOID self)
{
	CTeacherDlg* t = (CTeacherDlg*)self;
	t->SendScreenData();
	return 0;
}

/******************************************************************
Function	: SendScreenData
Date		: 2015-05-11 15:59:38
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 发送教师机屏幕图像数据到学生机
******************************************************************/
void CTeacherDlg::SendScreenData()
{
	m_pMulticast->SendScreenData();
}

void CTeacherDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	DeletepScreenDlg();
	DeletepOneStuScreenDlg();
	DeleteSocketMsg();
}

void CTeacherDlg::DeletepMulticast()
{
	if (m_pMulticast != NULL)
	{
		delete m_pMulticast;
		m_pMulticast = NULL;
	}
}

/******************************************************************
Function	: DeletepScreenDlg
Date		: 2015-05-11 16:00:08
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 释放new 的内存
******************************************************************/
void CTeacherDlg::DeletepScreenDlg()
{
	if (m_pScreenDlg != NULL)
	{
		delete m_pScreenDlg;
		m_pScreenDlg = NULL;
	}
}

/******************************************************************
Function	: DeletepOneStuScreenDlg
Date		: 2015-05-11 16:00:23
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 释放new 的内存
******************************************************************/
void CTeacherDlg::DeletepOneStuScreenDlg()
{
	if (m_oneStuScreenDlg != NULL)
	{
		delete m_oneStuScreenDlg;
		m_oneStuScreenDlg = NULL;
	}
}

/******************************************************************
Function	: DeleteSocketMsg
Date		: 2015-05-11 16:07:15
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 释放SOCKET 资源
******************************************************************/
void CTeacherDlg::DeleteSocketMsg()
{
	if (m_socketMsg != INVALID_SOCKET)
	{
		closesocket(m_socketMsg);
		m_socketMsg = NULL;
	}
}
