#pragma once
#include <string>

class CLanGetLocalSystemInfo
{
public:
	CLanGetLocalSystemInfo(void);
	~CLanGetLocalSystemInfo(void);
public:
	CString getHostIP();
	CString getComputername();
	CString getUsername();
};

