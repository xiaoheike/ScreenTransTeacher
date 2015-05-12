#pragma once
//////////////////////////////////////////////////////////////////////////
// ZLIB¿â
#include "zlib.h"
#include "zconf.h"
#pragma comment(lib,"zdll.lib")
#include "SocketCenter.h"
//////////////////////////////////////////////////////////////////////////
class CMulticast
{
public:
	CMulticast();
	~CMulticast();
	void GetDeskScreeData();

	void SetCursorIcon(CDC &memDC);

	void GetDeskScreenDC(CDC &memDC, CBitmap &bmp, BITMAP& bitmap);

	void CompressBmpData(BYTE* pBmpOriginalData);

	void InitBITMAPINFO(BITMAP &bitmap, int height, int width);

	void CleanData();

	void CloseSocketMulticast();

	void DeletepBmpTransData();

	void DeletepBitMapInfo();

	void SendScreenData();

	void SendScreenBmpData(SOCKET multicastSocket, SOCKADDR_IN addr);

	void SetBmpData(MULTICASTDATA &multicastData, int i, int count);

	int InitBmpData(MULTICASTDATA &multicastData);

	void SetIsMulticastEnd(bool isStop);

private:
	uLongf m_imgTotalSize;
	BITMAPINFO* m_pBitMapInfo;
	uLongf m_compressBmpDataLen;
	BYTE* m_pBmpTransData;
	bool m_isFirst;
	int m_bmpHeadTotalSize;
	CSocketCenter m_mySocket;
	SOCKET m_socketMulticast;
	sockaddr_in m_addr;
	bool m_isMulticastEnd;
	bool m_isInitBITMAPINFO;
};

