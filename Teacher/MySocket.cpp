#include "stdafx.h"
#include "MySocket.h"


CSocketCenter::CSocketCenter()
	: m_screenSocketRecv(INVALID_SOCKET)
	, m_socketInit(INVALID_SOCKET)
	, m_diskDownLoadSocket(INVALID_SOCKET)
	, m_diskInfoSocket(INVALID_SOCKET)
{
}


CSocketCenter::~CSocketCenter()
{
	if (m_socketInit != INVALID_SOCKET)
	{
		closesocket(m_socketInit);
		m_socketInit = INVALID_SOCKET;
	}
}

void CSocketCenter::Clean()
{
	::WSACleanup();
}

int CSocketCenter::SendDataTCP(SOCKET socket, const char* buf, int bytes)
{
	const char *b = buf;
	while (bytes > 0)
	{
		int r = send(socket, b, bytes, 0);
		if (r == SOCKET_ERROR)
		{
			int iError = WSAGetLastError();
			if (iError == WSAEWOULDBLOCK)
				AfxMessageBox(_T("recv failed with error : WSAEWOULDBLOCK\n"));
			else
			{
				CString str;
				str.Format(_T("recv failed with error: %ld\n"), iError);
				AfxMessageBox(str);
			}
		}
		if (r < 0)
		{
			return r;
		}
		else if (r == 0)
		{
			break;
		}
		bytes -= r;
		b += r;
	}
	return b - (char*)buf;
}

int CSocketCenter::RecvDataTCP(SOCKET socket, char* buf, int bytes)
{
	char *b = (char*)buf;
	while (bytes > 0)
	{
		int r = recv(socket, b, bytes, 0);
// 		if (r == SOCKET_ERROR)
// 		{
// 			int iError = WSAGetLastError();
// 			if (iError == WSAEWOULDBLOCK)
// 				AfxMessageBox(_T("recv failed with error : WSAEWOULDBLOCK\n"));
// 			else
// 			{
// 				CString str;
// 				str.Format(_T("recv failed with error: %ld\n"), iError);
// 				AfxMessageBox(str);
// 			}
// 		}
		if (r < 0)
		{
			return 0;
		}
		else if (r == 0)
		{
			break;
		}
		bytes = bytes - r;
		b = b + r;
	}
	return b - (char*)buf;
}

// void CMySocket::SetRomateControlDlg(CRomateControlDlg* romateControlDlg)
// {
// 	m_romateControlDlg = romateControlDlg;
// }
bool CSocketCenter::InitSocketInfo(int port, int listenCount)
{
	WSADATA wsaData;
	int err = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0)
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return 0 ;
	}
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup();
		return 0;
	}
//	SOCKET socketInit;
	m_socketInit = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_socketInit == SOCKET_ERROR)
	{
		AfxMessageBox(_T("创建连接失败"));
		//		return SOCKET_ERROR;
		return false;
	}

	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	saddr.sin_port = htons(port);

	int nRet;
	nRet = ::bind(m_socketInit, (SOCKADDR *)&saddr, sizeof(saddr));
	if (nRet == SOCKET_ERROR)
	{
		AfxMessageBox(_T("绑定端口失败"));
		return false;
	}

	nRet = listen(m_socketInit, listenCount);
	if (nRet == SOCKET_ERROR)
	{
		AfxMessageBox(_T("监听端口失败"));
		return false;
	}
	return true;
}


SOCKET CSocketCenter::SocketAccept(int port, int listenCount)
{
	if (false == InitSocketInfo(port, listenCount))
	{
		return INVALID_SOCKET;
	}

	SOCKADDR_IN Seraddr;
	int Seraddrsize = sizeof(Seraddr);

// 	while (true)
// 	{
		m_screenSocketRecv = accept(m_socketInit, (sockaddr *)&Seraddr, &Seraddrsize);
		return m_screenSocketRecv;
// 	}
}


void CSocketCenter::DiskDownLoadSocketAccept(int port, int listenCount)
{
	if (false == InitSocketInfo(port, listenCount))
	{
		return;
	}

	SOCKADDR_IN Seraddr;
	int Seraddrsize = sizeof(Seraddr);

	while (true)
	{
		m_diskDownLoadSocket = accept(m_socketInit, (sockaddr *)&Seraddr, &Seraddrsize);
	}
}

void CSocketCenter::DiskInfoSocketAccept(int port, int listenCount)
{
	if (false == InitSocketInfo(port, listenCount))
	{
		return;
	}

	SOCKADDR_IN Seraddr;
	int Seraddrsize = sizeof(Seraddr);

	while (true)
	{
		m_diskInfoSocket = accept(m_socketInit, (sockaddr *)&Seraddr, &Seraddrsize);
	}
}


/*
服务商已经收到请求，客户端可以开始接收请求的消息
*/
void CSocketCenter::SendReadyInfo(SOCKET socket, int msgID)
{
	MSGTYPE msgType;
	msgType.msgID = msgID;
	// 	msgType.flag = false;
	SendDataTCP(socket, (char*)&msgType, sizeof(MSGTYPE));
}

SOCKET CSocketCenter::GetSocketRecv()
{
	return m_screenSocketRecv;
}

SOCKET CSocketCenter::GetSocketInit()
{
	return m_socketInit;
}

SOCKET CSocketCenter::GetDiskDownLoadSocket()
{
	return m_diskDownLoadSocket;
}

SOCKET CSocketCenter::GetDiskInfoSocket()
{
	return m_diskInfoSocket;
}


SOCKET CSocketCenter::InitMulticastSocket(int port, char* multicastIp, sockaddr_in& addr)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Error in WSAStartup");
		//return 0;
	}

//	struct sockaddr_in addr;
	int fd;
//	struct ip_mreq mreq;
//	char message[16] = "Hello, World!";

	/* create what looks like an ordinary UDP socket */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("socket");
		exit(1);
	}

	/* set up destination address */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(multicastIp);
	addr.sin_port = htons(port);

	return fd;
// 	/* now just sendto() our destination! */
// 	while (1) {
// 		if (sendto(fd, message, sizeof(message), 0, (struct sockaddr *) &addr,
// 			sizeof(addr)) < 0) {
// 			printf("sendto");
// 			exit(1);
// 		}
// 		Sleep(1);
// 	}
}


int CSocketCenter::SendDataUDP(SOCKET socket, const char* sendBuf, int sendBytes, sockaddr_in addr)
{
	/* now just sendto() our destination! */
// 	while (1) {
// 		if (sendto(fd, message, sizeof(message), 0, (struct sockaddr *) &addr,
// 			sizeof(addr)) < 0) {
// 			printf("sendto");
// 			exit(1);
// 		}
// 	}
	int addrlen = sizeof(sockaddr_in);
	const char *pNeedToSend = sendBuf;
	while (sendBytes > 0)
	{
		int sendedAmount = sendto(socket, pNeedToSend, sendBytes, 0, 
			(sockaddr *)&addr, addrlen);
		if (sendedAmount < 0)
		{
			int err = GetLastError();
			CString str;
			str.Format(_T("Socket_Error: %d"), err);
			// 			MessageBox(AfxGetMainWnd()->m_hWnd, str, _T("error"), MB_OK);
			AfxMessageBox(str);
			//printf("Socket_Error: %d\n", err);
			return sendedAmount;
		}
		else if (sendedAmount == 0)
		{
			//printf("Socket_Error\n");
			//MessageBox(AfxGetMainWnd()->m_hWnd, _T("SOCKET ERROR"), _T("error"), MB_OK);
			AfxMessageBox(_T("CSocketCenter::SendDataUDP"));
			break;
		}
		sendBytes = sendBytes - sendedAmount;
		pNeedToSend += sendedAmount;
	}
	return pNeedToSend - sendBuf;
}