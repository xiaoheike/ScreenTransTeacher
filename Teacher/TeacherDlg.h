// TeacherDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
//////////////////////////////////////////////////////////////////////////
// ZLIB 库的头文件
#include "zlib.h"
#include "zconf.h"
#pragma comment(lib,"zdll.lib")
//////////////////////////////////////////////////////////////////////////
#include "ScreenMonitorDlg.h"
#include "Multicast.h"

// 这里的ID 是与工具栏产生时赋值的ID 是对应的
#define ID_MULTICAST 1001
#define ID_SCREENMONITOR	 1002

// CTeacherDlg 对话框
class CTeacherDlg : public CDialogEx
{
	// 构造
public:
	CTeacherDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_TEACHER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()

	static DWORD WINAPI OnMsgListen(LPVOID self);

	afx_msg LRESULT OnAddStudent(WPARAM wparam, LPARAM lparam);

	afx_msg LRESULT OnLoginOutStudent(WPARAM wparam, LPARAM lparam);

	afx_msg LRESULT OnSetBmpCompressData(WPARAM wparam, LPARAM lparam);

	afx_msg LRESULT OnEndMonitor(WPARAM wparam, LPARAM lparam);

	afx_msg LRESULT OnCreatOneStudentDlg(WPARAM wparam, LPARAM lparam);

	afx_msg LRESULT OnSetOneStuBmpCompressData(WPARAM wparam, LPARAM lparam);

	afx_msg LRESULT OnEndOneStudentMonitor(WPARAM wparam, LPARAM lparam);

	afx_msg void OnBeginScreenMonitor();

	afx_msg void OnMulticast();

private:
	CListCtrl m_list;
	CImageList m_imagelist;
	CToolBar m_toolBar;
	CStatusBar m_statusBar;
	CRect m_rect;
	SOCKET m_socketMsg;
	CScreenMonitorDlg* m_pScreenDlg;
	// 记录被监控的学生机的ID
	int m_item[MAX_MONITOR];
	COneStuScreenDlg* m_oneStuScreenDlg;
	CMulticast m_multicast;
	CMulticast* m_pMulticast;
	bool m_isStopMulticast;
public:
	afx_msg void OnDestroy();

	void DeletepMulticast();

	void DeletepScreenDlg();

	void DeletepOneStuScreenDlg();

	void DeleteSocketMsg();

	void BeginScreenMonitor(int selCount);

	static DWORD WINAPI OnSendScreenData(LPVOID self);

	void SendScreenData();

	void ShowStudentInfo();

	void SetMainDlgSytle(int port);

	void InsertColumn();

	void CreateStatusBar(int port);

	void CreatToolBar();

	void LoadBitBmp();

	bool ListenStudentLogin();

	void SetLoginStudentID();
};
