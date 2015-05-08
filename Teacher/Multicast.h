#pragma once
//////////////////////////////////////////////////////////////////////////
// ZLIB¿â
#include "zlib.h"
#include "zconf.h"
#pragma comment(lib,"zdll.lib")
#include "MySocket.h"
//////////////////////////////////////////////////////////////////////////
class CMulticast
{
public:
	CMulticast();
	~CMulticast();
	void GetDeskScreeData();
	void CompressBmpData(BYTE* pBmpOriginalData);
	void InitBITMAPINFO(BITMAP &bitmap, int height, int width);
	void CleanData();
	void SendScreenData(SOCKET socket);
	void SendScreenData();
	void SendBmpData(SOCKET socket);
	void SendBmpData(SOCKET multicastSocket, SOCKADDR_IN addr);
	void SetIsStop(bool isStop);
	void SendBmpHeaderInfo(SOCKET socketMsg);
	uLongf m_imgTotalSize;
	BITMAPINFO* m_pBMPINFO;
	uLongf m_compressBmpDataLen;
	BYTE* m_pBmpTransData;
	bool m_isFirst;
	int m_bmpHeadTotalSize;
	CMySocket m_mySocket;
private:
	SOCKET m_socketMulticast;
	sockaddr_in m_addr;
	bool m_isStop;
	bool m_isInitBITMAPINFO;
};

