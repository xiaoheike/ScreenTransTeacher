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

	void SendDataTCP(int &nRet, MSGTYPE& msgType);

	UINT m_id;

	SOCKET GetSocket();

	void SetScreenBmpData();

	void SendBmpDataToDlg(BMPDATA &bmpData);

	void SetBmpTransDataLast(BMPDATA &bmpData);

	void SetBmpTransDataNotLast(BMPDATA &bmpData);

	BYTE* UnCompressData(uLongf biSizeImage, uLongf bmpCompressSize);

	void BeginMonitor(int item);

	void BeginMonitor(bool isOneSutInScreen, int itemOrder);

	void EndScreenMonitor();

	void BeginMulticast();

	void EndMulticast();
	
	void CleanData();

	void DeletepBitMapInfo();

	void DeletepBmpCompressData();

	static DWORD WINAPI GetScreen(LPVOID self);
	
	void go();

	CStuInfo m_stuInfo;
private:
	BYTE* m_pBmpCompressData;
	BITMAPINFO* m_pBitMapInfo;
	int m_itemOrder;
	bool m_endMonitor;
	bool m_isOneStuInScreen;
	SOCKET m_socketMsg;
	CSocketCenter m_Mysocket;
	HWND m_hWnd;
	CScreenMonitorDlg* m_screenMonitorDlg;
//	int m_msgType;
};

