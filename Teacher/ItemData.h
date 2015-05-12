#pragma once
#include "Common.h"
//#include "MySocket.h"
//#include "CmdShell.h"
//#include "TaskDlg.h"
#include "ScreenMonitorDlg.h"
//#include "Multicast.h"
#include "StuInfo.h"
//#include "FileDlg.h"

//class CFileDlg;
class CItemData : public CDialogEx
{
public:
	CItemData();
	CItemData(UINT id, SOCKET sock, SOCKADDR_IN *addr, HWND m_hWnd, CMySocket* mySocket);
	CItemData(int id, HWND m_hWnd, SOCKET msgSocket);
	//	CItemData(SOCKETDATA socketData, SOCKADDR_IN *addr, HWND m_hWnd);
	~CItemData();
	static DWORD WINAPI OnListen(LPVOID lp);
	void Run();
	void OnBeginListen();
	UINT m_id;
//	void Run();
	SOCKET GetSocket();
// 	void GetSysVer(SYSTEMINFO sys);
	void RunToFileManager();
//	void RunToCmdShell();
//	void RunToTask();
	void RunToScreen();
	void SetMsgType(int msgType);
	void SetScreenData();
	BYTE* UnCompressData(uLongf biSizeImage, uLongf bmpCompressSize);
	void BeginMonitor(int item);
	void BeginMonitor(bool isOneSutInScreen, int itemOrder);
	void EndMonitor();
	void BeginMulticast();
	void EndMulticast();
	void CleanData();
	static DWORD WINAPI GetScreen(LPVOID self);
	void go();
	void SetScreenSocket(SOCKET screenSocket);
	static DWORD WINAPI OnScreenSocketAccept(LPVOID self);
	void ScreenSocketAccept();
	static DWORD WINAPI OnSendScreenData(LPVOID self);
	void SendScreenData();
	CString m_IP;
	CString m_Address;
	CString m_OS;
	CString m_Ver;
	bool Cam;
	SOCKET m_sock;
//	void OnBeginListen();
	CMySocket m_Mysocket;
	HWND m_hWnd;
//	CFileDlg* m_filedlg;
//	CCmdShell* m_cmdshell;
//	CTaskDlg* m_task;
	CScreenMonitorDlg* m_screen;
//	CCmdShell* m_cmd;
	int m_msgType;
//	SOCKET m_socketData;
	CStuInfo m_stuInfo;
private:
	bool m_isScreenFlag;
	bool m_isScreenAccept;
	SOCKET m_screenSocket;
	CMySocket* m_pMySocket;
//	CItemData *m_itemData;
//	CScreenDlg m_screenDlg;
	BYTE* m_pBmpCompressData;
	BITMAPINFO* m_pBMPINFO;
	int m_itemOrder;
	bool m_endMonitor;
	bool m_isOneStuInScreen;
	//	CMulticast m_multicast;
};

