#include "stdafx.h"
#include "StuInfo.h"


CStuInfo::CStuInfo()
{
}


CStuInfo::~CStuInfo()
{
}

void CStuInfo::SendStuInfoReq(SOCKET socket)
{
// 	MSGTYPE msgType;
// 	msgType.msgID = SYSINFO;
// 	nRet = m_Mysocket->SendDataTCP(m_sock, (char*)&msgType, sizeof(MSGTYPE));
	CMySocket mySocket;
	mySocket.SendReadyInfo(socket, STUDENTINFO);
}

void CStuInfo::GetStuInfo(SOCKET socket)
{
	STUINFODATA stuInfoData;
	memset(&stuInfoData, 0, sizeof(STUINFODATA));
	CMySocket mySocket;
	mySocket.RecvDataTCP(socket, (char*)&stuInfoData, sizeof(STUINFODATA));

	m_stuName.Format(_T("%s"), stuInfoData.m_stuName);

	m_stuID.Format(_T("%s"), stuInfoData.m_stuID);
	m_stuMajor.Format(_T("%s"), stuInfoData.m_stuMajor);
	m_stuClass.Format(_T("%s"), stuInfoData.m_stuClass);
//	return stuInfoData;
}

CString CStuInfo::GetStuName() const
{
	return m_stuName;
}


CString CStuInfo::GetStuID() const
{
	return m_stuID;
}


CString CStuInfo::GetStuMajor() const
{
	return m_stuMajor;
}


CString CStuInfo::GetStuClass() const
{
	return m_stuClass;
}
