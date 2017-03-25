/************************************************************************/
/* LanGetLocalSystemInfo .cpp
	get some system info
	by yuc 
	2017-03-06
*/
/************************************************************************/

#include "StdAfx.h"
#include "LanGetLocalSystemInfo.h"
#include <iphlpapi.h>
#pragma comment ( lib, "Iphlpapi.lib" )


CLanGetLocalSystemInfo::CLanGetLocalSystemInfo(void)
{
}


CLanGetLocalSystemInfo::~CLanGetLocalSystemInfo(void)
{
}


CString CLanGetLocalSystemInfo::getHostIP()
{
	PIP_ADAPTER_INFO pAdapterInfo;  
	DWORD AdapterInfoSize;  
	TCHAR szMac[32]   =   {0};  
	DWORD Err;    
	AdapterInfoSize   =   0;  
	Err   =   GetAdaptersInfo(NULL,   &AdapterInfoSize);  
	if((Err   !=   0)   &&   (Err   !=   ERROR_BUFFER_OVERFLOW)){  
		OutputDebugString(_T("���������Ϣʧ�ܣ�"));  
		return __T("");
	}  
	//   ����������Ϣ�ڴ�  
	pAdapterInfo   =   (PIP_ADAPTER_INFO)   GlobalAlloc(GPTR,   AdapterInfoSize);  
	if(pAdapterInfo   ==   NULL){  
		OutputDebugString(_T("����������Ϣ�ڴ�ʧ��"));  
		return  __T("");
	}    
	if(GetAdaptersInfo(pAdapterInfo,   &AdapterInfoSize)   !=   0){  
		OutputDebugString(_T("���������Ϣʧ�ܣ�\n"));  
		GlobalFree(pAdapterInfo);  
		return __T("");
	}    
	//��ǰ����
	CString strMainMac,strIP;
	strMainMac.Format(_T("%02X-%02X-%02X-%02X-%02X-%02X"),    
		pAdapterInfo->Address[0],  
		pAdapterInfo->Address[1],  
		pAdapterInfo->Address[2],  
		pAdapterInfo->Address[3],  
		pAdapterInfo->Address[4],  
		pAdapterInfo->Address[5]);  
	IP_ADDR_STRING pIpAddrString =pAdapterInfo->IpAddressList;
	strIP=pIpAddrString.IpAddress.String;
	GlobalFree(pAdapterInfo); 
	return strIP;
}


CString CLanGetLocalSystemInfo::getComputername()
{
	TCHAR  szBuffer[MAX_PATH*2] = {0} ;
	DWORD dwNameLen = MAX_PATH*2;  
	if (GetComputerName(szBuffer, &dwNameLen))  
	{
		return szBuffer;
	}
	else
	{
		return __T("");
	}
}

CString CLanGetLocalSystemInfo::getUsername()
{
	TCHAR  szBuffer[MAX_PATH*2] = {0} ;
	DWORD dwNameLen = MAX_PATH*2;  
	if (GetUserName(szBuffer, &dwNameLen))  
	{
		return szBuffer;
	}
	else
	{
		return __T("");
	}
}