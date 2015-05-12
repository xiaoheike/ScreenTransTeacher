#pragma once
#include "Common.h"
#include "ScreenMonitorDlg.h"
#include "StuInfo.h"

//class CFileDlg;
class CItemData : public CDialogEx
{
public:
	CItemData();

	CItemData(int id, HWND m_hWnd, SOCKET msgSocket);

	~CItemData();

	static DWORD WINAPI OnListen(LPVOID lp);

	void Run();

	void OnBeginListen();

	UINT m_id;

	SOCKET GetSocket();

// 	void RunToFileManager();

//	void RunToScreen();
	
//	void SetMsgType(int msgType);

	void SendScreenBmpData();

	void ShowBmp(BMPDATA &bmpData);

	void SetBmpTransDataLast(BMPDATA &bmpData);

	void SetBmpTransDataNotLast(BMPDATA &bmpData);

	BYTE* UnCompressData(uLongf biSizeImage, uLongf bmpCompressSize);

	void BeginMonitor(int item);

	void BeginMonitor(bool isOneSutInScreen, int itemOrder);

	void EndMonitor();

	void BeginMulticast();

	void EndMulticast();
	
	void CleanData();

	static DWORD WINAPI GetScreen(LPVOID self);
	
	void go();

//	void SetScreenSocket(SOCKET screenSocket);

//	static DWORD WINAPI OnScreenSocketAccept(LPVOID self);

//	void ScreenSocketAccept();

// 	static DWORD WINAPI OnSendScreenData(LPVOID self);
// 
// 	void SendScreenData();

	CStuInfo m_stuInfo;
private:
	BYTE* m_pBmpCompressData;
	BITMAPINFO* m_pBitMapInfo;
	int m_itemOrder;
	bool m_endMonitor;
	bool m_isOneStuInScreen;
	SOCKET m_sock;
	CMySocket m_Mysocket;
	HWND m_hWnd;
	CScreenMonitorDlg* m_screenMonitorDlg;
//	int m_msgType;
};

