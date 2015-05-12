#pragma once
#include "MySocket.h"
#include "Common.h"
class CStuInfo
{
public:
	CStuInfo();

	~CStuInfo();
	void SendStuInfoReq(SOCKET socket);

	void GetStuInfo(SOCKET socket);

	CString GetStuName() const;

	CString GetStuID() const;

	CString GetStuMajor() const;

	CString GetStuClass() const;
private:
	// 学生姓名
	CString m_stuName;
	// 学生的班级
	CString m_stuClass;
	// 学生的学号
	CString m_stuID;
	// 学生的专业
	CString m_stuMajor;

};

