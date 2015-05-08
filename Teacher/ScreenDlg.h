#pragma once
#include "zlib.h"
#include "zconf.h"
#pragma comment(lib,"zdll.lib")
#include "Common.h"
#include "MySocket.h"
#include "StuInfo.h"
#include <mutex>
#include "OneStuScreenDlg.h"


// CScreenDlg 对话框

class CScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenDlg)

public:
//	CScreenDlg(CWnd* pParent = NULL, SOCKET sock = NULL, int id = 0);   // 标准构造函数
	CScreenDlg(CWnd* pParent = NULL, int widthCount = 0);
	virtual ~CScreenDlg();

// 对话框数据
	enum { IDD = IDD_SCREEN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void SetScreenData();
	void ShowBmp(BMP* bmp, int item);

	//void ShowBmp(BYTE* bmpShowData, BITMAPINFO* pBMPINFO);
	void DetermineShowRegion(int itemOrder, int &xDest, int &yDest);

	void SixteenRegion(int itemOrder,  int &xDest, int &yDest);

	void NineRegion(int itemOrder, int &xDest, int &yDest);

	void FourRegion(int itemOrder, int &xDest, int &yDest);

	void OneRegion(int itemOrder, int &xDest, int &yDest);

	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CRect m_rect;
	virtual BOOL OnInitDialog();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void SetSocket(SOCKET socketMsg/*, SOCKET socketScreen*/);
	static DWORD WINAPI ScreenSocketConn(LPVOID self);
	void SetMsgSocket(/*SOCKET socketMsg*/SOCKET msgSocket);
	void SetScreenTranEnd(bool screenTranEnd);
	void CleanData();
	BYTE* UnCompressData(uLongf biSizeImage, uLongf bmpCompressSize);
	BYTE* UnCompressData(uLongf biSizeImage, uLongf bmpCompressSize, BYTE* pBmpCompressData);
	void OnListen();
	static DWORD WINAPI OnListen(LPVOID lp);
	static DWORD WINAPI OnSetScreenData(LPVOID self);
	void Listen();

	int RecvDataTCP();

	void SendDataTCP();
	
	static DWORD WINAPI OnShowDlg(LPVOID lp);
private:
	BMPINFO m_bmpInfo;
	SOCKET m_msgSocket;
	bool m_isScreenFlag;
	BYTE* m_pBmpCompressData;
	// false--屏幕传输结束
	bool m_isScreenTranEnd;
//	CStuInfo m_stuInfo;
//	CMySocket m_Mysocket;
	SOCKET m_servSocket;
	//CWnd* pParent;
	int m_id;
	bool m_isFirst;
	int m_widthCount;
	int m_blockWidth;
	int m_blockHeight;
	// 多线程并发操作保护数据而使用的互斥量
	std::mutex m_mtx;
	bool isLButtonUp;
	BYTE* m_bmpShowData;
	BITMAPINFO* m_pBMPINFO;
	COneStuScreenDlg* m_oneStuScreenDlg;
public:
//	afx_msg void OnClose();
//	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	void FourRegionClk(CPoint point, int& itemOrder);
	void FourRegionClk(CPoint point, int& itemOrder, int offset);
	void NineRegionClk(CPoint point, int& itemOrder);
	void NineRegionClk(CPoint point, int& itemOrder, int offset);
	void SixteenRegionClk(CPoint point, int& itemOrder);
	void SixteenRegionClk(CPoint point, int& itemOrder, int offset);
private:
	// LButtonUp消息是否已经响应过的消息
	bool m_isLBtnUp;
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};