#pragma once
#include "zlib.h"
#include "zconf.h"
#pragma comment(lib,"zdll.lib")
#include "Common.h"

// COneStuScreenDlg 对话框

class COneStuScreenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(COneStuScreenDlg)

public:
	COneStuScreenDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COneStuScreenDlg();

// 对话框数据
	enum { IDD = IDD_SCREEN_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CRect m_clientRect;
	int m_itemOrder;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BYTE* UnCompressData(uLongf biSizeImage, uLongf bmpCompressSize, BYTE* pBmpCompressData);
	void ShowBmp(BMP* bmp, int itemOrder);
	afx_msg void OnClose();
};
