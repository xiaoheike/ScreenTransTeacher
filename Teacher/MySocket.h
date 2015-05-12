#pragma once
#include "Common.h"

class CSocketCenter
{
public:
	CSocketCenter();
	~CSocketCenter();
	void Clean();
	int SendDataTCP(SOCKET socket, const char* buf, int bytes);
	int RecvDataTCP(SOCKET socket, char* buf, int bytes);
//	SOCKET SocketAccept(int port, int listenCount);
//	void SocketAccept(int port, int listenCount, SOCKET socketRecv);
	SOCKET SocketAccept(int port, int listenCount);
//	void SocketAccept(int port, int listenCount, SOCKADDR_IN& Seraddr);
	bool InitSocketInfo(int port, int listenCount);
	void DiskDownLoadSocketAccept(int port, int listenCount);
	void DiskInfoSocketAccept(int port, int listenCount);

	SOCKET GetSocketRecv();
	SOCKET GetSocketInit();
	SOCKET GetDiskDownLoadSocket();
	SOCKET GetDiskInfoSocket();
	SOCKET InitMulticastSocket(int port, char* multicastIp, sockaddr_in& addr);
	int SendDataUDP(SOCKET socket, const char* sendBuf, int sendBytes, sockaddr_in addr);
	void SendReadyInfo(SOCKET socket, int msgID);
	//	void SetRomateControlDlg(CRomateControlDlg* dlg);
private:
	SOCKET m_screenSocketRecv;
//	CScreenDlg* m_screenDlg;
//	CRomateControlDlg* m_romateControlDlg;

	SOCKET m_socketInit;
	SOCKET m_diskDownLoadSocket;
	SOCKET m_diskInfoSocket;
};

