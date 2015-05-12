#pragma once
//////////////////////////////////////////////////////////////////////////
// ZLIB 库使用的头文件与动态链接库
#include "zlib.h"
#include "zconf.h"
#pragma comment(lib,"zdll.lib")
//////////////////////////////////////////////////////////////////////////
#include "Common.h"
#include "MySocket.h"
#include "StuInfo.h"
#include "OneStuScreenDlg.h"


// CScreenDlg 对话框

class CScreenMonitorDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScreenMonitorDlg)

public:
	CScreenMonitorDlg(CWnd* pParent = NULL, int widthCount = 0);

	virtual ~CScreenMonitorDlg();

// 对话框数据
	enum { IDD = IDD_SCREEN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	void SetScreenData();

	void ShowBmp(BMP* bmp, int item);

	void DetermineShowRegion(int itemOrder, int &xDest, int &yDest);

	void SixteenRegion(int itemOrder,  int &xDest, int &yDest);

	void NineRegion(int itemOrder, int &xDest, int &yDest);

	void FourRegion(int itemOrder, int &xDest, int &yDest);

	void OneRegion(int itemOrder, int &xDest, int &yDest);

	afx_msg void OnClose();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual BOOL OnInitDialog();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	static DWORD WINAPI ScreenSocketConn(LPVOID self);

	void SetMsgSocket(/*SOCKET socketMsg*/SOCKET msgSocket);

	void SetScreenTranEnd(bool screenTranEnd);

	void CleanData();

	void DeletepBitMapInfo();

	void DeletepBmpTransData();

	BYTE* UnCompressData(uLongf biSizeImage, uLongf bmpCompressSize, BYTE* pBmpCompressData);

	void FourRegionClk(CPoint point, int& itemOrder);

	void FourRegionClk(CPoint point, int& itemOrder, int offset);

	void NineRegionClk(CPoint point, int& itemOrder);

	void NineRegionClk(CPoint point, int& itemOrder, int offset);

	void SixteenRegionClk(CPoint point, int& itemOrder);

	void SixteenRegionClk(CPoint point, int& itemOrder, int offset);

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
private:
	BMPINFO m_bmpInfo;
	CRect m_rect;
	bool m_isFirst;
	int m_widthCount;
	int m_blockWidth;
	int m_blockHeight;
	BYTE* m_pBmpCompressData;
	BITMAPINFO* m_pBitMapInfo;
	COneStuScreenDlg* m_oneStuScreenDlg;
};