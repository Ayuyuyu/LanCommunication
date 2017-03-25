// FileMap.h: interface for the CFileMap class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _MY_FILEMAP_H
#define _MY_FILEMAP_H
#include "MD5Class.h"

class CFileMap  
{
public:
	CFileMap(LPCTSTR lpFileName)
	{
		m_hMapping = m_hFile = INVALID_HANDLE_VALUE;
		m_pBase = NULL;

		m_hFile = CreateFile(lpFileName , GENERIC_READ , 0, NULL , 
			OPEN_EXISTING , 0 , NULL);

		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			// 取数据长度
			m_dwSize = GetFileSize(m_hFile , NULL);

			m_hMapping = CreateFileMapping(m_hFile ,  NULL , PAGE_READONLY , 0 , 0 , NULL);
			if (m_hMapping )
			{
				m_pBase = (LPBYTE)MapViewOfFile(m_hMapping , FILE_MAP_READ , 0 , 0 , 0);
			}
		}
	}
	virtual ~CFileMap()
	{
		if (m_pBase) 
		{
			UnmapViewOfFile(m_pBase);
			m_pBase = NULL;
		}
		if (m_hMapping != INVALID_HANDLE_VALUE) 
		{
			CloseHandle(m_hMapping);
			m_hMapping = INVALID_HANDLE_VALUE;
		}
		if (m_hFile != INVALID_HANDLE_VALUE) 
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}
/***************************************************************************/
public:
	//取文件buffer
	BOOL GetData(LPBYTE * ppBuffer)
	{
		if (m_pBase)
		{
			*ppBuffer = m_pBase;
			return true;
		}
		return false;
	}
	//取文件size
	DWORD GetSize()
	{
		return m_dwSize;
	}

	//md5操作
	CString GetMD5(LPCTSTR path)
	{
		WIN32_FIND_DATA Info; 
		HANDLE hFind; 
		DWORD size;
		hFind = FindFirstFile(path ,&Info); 
		if(hFind != INVALID_HANDLE_VALUE) 
		{
			size = Info.nFileSizeLow;
			FindClose(hFind); 
		}
		else
		{
			return "0";
		}
		unsigned char szMD5[32] ={0};
		CMD5Class md5;
		if(!md5.MD5File((LPTSTR)(LPCTSTR)path,0,size,szMD5))
		{
			return "0";
		}
		//PutBinary(szMD5, 16);
		//binString.MakeUpper();
		return PutBinary(szMD5, 16);
	}

	CString PutBinary(LPBYTE pByte , int len)
	{

		CString hex,out;
		for (int i = 0 ; i < len ; i ++) 
		{
			hex.Format(_T("%02X") , pByte[i]);
			out += hex;
		}
		return out;
	}

	//关闭文件句柄
	int CloseX()
	{
			if (m_pBase) 
		{
			UnmapViewOfFile(m_pBase);
			m_pBase = NULL;
		}
		if (m_hMapping != INVALID_HANDLE_VALUE) 
		{
			CloseHandle(m_hMapping);
			m_hMapping = INVALID_HANDLE_VALUE;
		}
		if (m_hFile != INVALID_HANDLE_VALUE) 
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
		return 1;
	}
private:
	HANDLE  m_hMapping, m_hFile;
	LPBYTE  m_pBase;
	DWORD   m_dwSize;
};

#endif//_MY_FILEMAP_H
