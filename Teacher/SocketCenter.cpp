/******************************************************************* 
FileName: MySocket.cpp
Author	: xiaoheike
Date	: 2015-05-12 16:11:57
Desc	: SOCKET 网络数据传输的核心类
*******************************************************************/ 
#include "stdafx.h"
#include "SocketCenter.h"


CSocketCenter::CSocketCenter()
// 	: m_screenSocketRecv(INVALID_SOCKET)
	: m_socketInit(INVALID_SOCKET)
// 	, m_diskDownLoadSocket(INVALID_SOCKET)
// 	, m_diskInfoSocket(INVALID_SOCKET)
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

/******************************************************************
Function	: SendDataTCP
Date		: 2015-05-12 16:13:52
Author		: xiaoheike
Parameter	: socket--SOCKET
			  buf--传输的数据的基地址
			  bytes--传输的数据的大小，buf中数据的大小
Return		: int--发送成功的数据的字节数
Desc		: 基于TCP 传输数据到学生机
******************************************************************/
int CSocketCenter::SendDataTCP(SOCKET socket, const char* buf, int bytes)
{
	const char *b = buf;
	while (bytes > 0)
	{
		int r = send(socket, b, bytes, 0);
// 		if (SOCKET_ERROR == r)
// 		{
// 			OutputError();
// 		}
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

/******************************************************************
Function	: OutputError
Date		: 2015-05-12 16:15:52
Author		: xiaoheike
Parameter	: 无
Return		: void
Desc		: 数据传输出错后输出的信息
******************************************************************/
void CSocketCenter::OutputError()
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

/******************************************************************
Function	: RecvDataTCP
Date		: 2015-05-12 16:16:16
Author		: xiaoheike
Parameter	: socket--SOCKET
			  buf--传输的数据的基地址  
			  bytes--传输的数据的大小，buf中数据的大小
Return		: int--传输成功的数据的大小
Desc		: 基于TCP接收来自学生机的数据
******************************************************************/
int CSocketCenter::RecvDataTCP(SOCKET socket, char* buf, int bytes)
{
	char *b = (char*)buf;
	while (bytes > 0)
	{
		int r = recv(socket, b, bytes, 0);
// 		if (SOCKET_ERROR == r)
// 		{
// 			OutputError();
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

/******************************************************************
Function	: InitSocketInfo
Date		: 2015-05-12 16:17:16
Author		: xiaoheike
Parameter	: port--监听的端口号
			  listenCount--最大监听的学生机的数量
Return		: bool 连接成功与否
Desc		: 初始化SOCKET 信息，之后可以建立连接
******************************************************************/
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

/******************************************************************
Function	: SendReadyInfo
Date		: 2015-05-12 16:18:49
Author		: xiaoheike
Parameter	: socket--与学生机连接成功的SOCKET
			  msgID--请求的类型，比如屏幕监控
Return		: void
Desc		: 发送请求消息到学生机
******************************************************************/
void CSocketCenter::SendReadyInfo(SOCKET socket, int msgID)
{
	MSGTYPE msgType;
	msgType.msgID = msgID;
	// 	msgType.flag = false;
	SendDataTCP(socket, (char*)&msgType, sizeof(MSGTYPE));
}

/******************************************************************
Function	: GetSocketInit
Date		: 2015-05-12 16:20:00
Author		: xiaoheike
Parameter	: 无
Return		: SOCKET
Desc		: 获得初始化的SOCKET，不是用于接发数据
******************************************************************/
SOCKET CSocketCenter::GetSocketInit()
{
	return m_socketInit;
}


/******************************************************************
Function	: InitMulticastSocket
Date		: 2015-05-12 16:20:50
Author		: xiaoheike
Parameter	: port--广播监听的消息
			  multicastIp--组播IP
			  addr--sockaddr_in
Return		: 
Desc		: 初始化广播的SOCKET 信息
******************************************************************/
SOCKET CSocketCenter::InitMulticastSocket(int port, char* multicastIp, sockaddr_in& addr)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Error in WSAStartup");
	}

	int fd;

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
}


/******************************************************************
Function	: SendDataUDP
Date		: 2015-05-12 16:21:55
Author		: xiaoheike
Parameter	: socket--广播SOCKET
			  sendBuf--要发送的数据的大小
			  sendBytes--发送的数据的大小，sendBuf指向的数据的大小
			  addr--sockaddr_in
Return		: 
Desc		: 基于UDP 发送数据到学生机
******************************************************************/
int CSocketCenter::SendDataUDP(SOCKET socket, const char* sendBuf, int sendBytes, sockaddr_in addr)
{
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
			AfxMessageBox(str);
			return sendedAmount;
		}
		else if (sendedAmount == 0)
		{
			AfxMessageBox(_T("CSocketCenter::SendDataUDP"));
			break;
		}
		sendBytes = sendBytes - sendedAmount;
		pNeedToSend += sendedAmount;
	}
	return pNeedToSend - sendBuf;
}