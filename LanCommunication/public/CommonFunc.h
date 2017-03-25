#pragma  once 
#include "StdAfx.h"
#include "MD5Class.h"
// 方便其他人使用，还是分为头文件、源文件
#define MAX_KEY_LENGTH 1024
#define MAX_VALUE_NAME 2048


static inline void OutputDebugPrintf(const TCHAR * strOutputString,...)
{
	TCHAR strBuffer[4096]={0};
	va_list vlArgs;
	va_start(vlArgs,strOutputString);
	_vstprintf(strBuffer,sizeof(strBuffer)-1,strOutputString,vlArgs);
	//vsprintf(strBuffer,strOutputString,vlArgs);
	va_end(vlArgs);
	//_tprintf(strBuffer);
	printf(CW2A(strBuffer));
	OutputDebugString(strBuffer);
}

/*
static inline SYSTEMTIME LARGE_INTEGERToSystemTime(LARGE_INTEGER ularge)  
{  
	FILETIME ft;  
	SYSTEMTIME st;  
	ft.dwLowDateTime = ularge.LowPart;  
	ft.dwHighDateTime = ularge.HighPart;  
	FileTimeToSystemTime(&ft, &st);  
	return st;  
}
*/
static inline  void GetNowTime(char* sztime)
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [128] = {0};

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",timeinfo);
	strcpy(sztime,buffer);
}


static inline int QueryRegValue(HKEY hKey, LPCTSTR SubKey , LPCTSTR ValueName, TCHAR * szValuestr, DWORD &dwSizeValuestr)
{
	int ret = 0;
	HKEY h_key;
	DWORD dwType, dwerr;

	//	AfxMessageBox(SubKey);
	if (RegOpenKeyEx(hKey, SubKey, 0, KEY_READ, &h_key) == ERROR_SUCCESS)
	{
		DWORD dwQueryRet = RegQueryValueEx(h_key, ValueName, 0, &dwType, (LPBYTE)szValuestr, &dwSizeValuestr);
		if (dwQueryRet == ERROR_SUCCESS)
		{
			ret = 1;
		}
		else 
		{
			dwerr = GetLastError();
			//return 0;
		}
		RegCloseKey(h_key);
	}
	else
	{
	}

	return ret;
}

static inline int QueryRegValue(HKEY hKey, LPCTSTR SubKey , LPCTSTR ValueName, DWORD *dwValue)
{
	int ret = 0;
	HKEY h_key;
	DWORD dwType, dwerr, dwSizeValuestr;
	TCHAR szValuestr[MAX_VALUE_NAME] = {0};
	if (dwValue == NULL)
	{
		return -1;
	}
	if (RegOpenKeyEx(hKey, SubKey, 0, KEY_READ, &h_key) == ERROR_SUCCESS)
	{
		DWORD dwQueryRet = RegQueryValueEx(h_key, ValueName, 0, &dwType, (LPBYTE)szValuestr, &dwSizeValuestr);

		if(dwType == REG_DWORD || dwType == REG_BINARY)
		{
			*dwValue = (long)*(short *)szValuestr;
		}
		if (dwQueryRet == ERROR_SUCCESS)
		{
			ret = 1;
		}
		else 
		{
			dwerr = GetLastError();
			//return 0;
		}
		RegCloseKey(h_key);
	}
	else
	{
	}

	return ret;
}

//设置注册表.
static inline int  SetREG_SZ(TCHAR*szName,TCHAR* szValue)
{
	HKEY   hKey;
	LPCTSTR lpPath = _T("SOFTWARE\\Antiy Labs");			//path
	long lRet = RegCreateKey(HKEY_LOCAL_MACHINE, lpPath, &hKey);
	//std::cout<<CW2A(szValue)<<std::endl;
	if (lRet == ERROR_SUCCESS)
	{
		lRet = RegSetValueEx(hKey, szName, 0, REG_SZ, (const BYTE *)szValue, (_tcslen(szValue)+1)*sizeof(TCHAR));
		RegCloseKey(hKey);
	}

	return lRet;
}

static inline BOOL Is64BitOS()
{
	typedef VOID (WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo );
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = (LPFN_GetNativeSystemInfo)GetProcAddress(GetModuleHandle(_T("kernel32")),"GetNativeSystemInfo");
	if(fnGetNativeSystemInfo)
	{
		SYSTEM_INFO stInfo = {0};
		fnGetNativeSystemInfo( &stInfo);
		if( stInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64
			|| stInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		{
			return TRUE;
		}
	}
	return FALSE;
}


static inline int  CreateRunExample()
{
	//添加以下代码
	HKEY   hKey;
	TCHAR pFileName[MAX_PATH] = { 0 };
	//得到程序自身的全路径 
	DWORD dwRet = GetModuleFileName(NULL, pFileName, MAX_PATH);
	//wsprintf(pFileNamea, L"\"%s\" -a", pFileName);

	//找到系统的启动项 
	LPCTSTR lpRun = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
	//打开启动项Key 
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
	if (lRet == ERROR_SUCCESS)
	{
		//添加注册
		RegSetValueEx(hKey, _T("SystemCTL"), 0, REG_SZ, (const BYTE*)(LPCTSTR)pFileName, MAX_PATH);
		RegCloseKey(hKey);
	}
	return 0;
}


static inline int GetMD5(LPCSTR szFilePath , DWORD dwfilesize,char* szMD5)
{
	CMD5Class md5;
	unsigned char szMD5buf[32] = {0};
	//CStringA hex,out;
	char szhex[3]={0};
	char szout[33] = {0};
	if(!md5.MD5File(CA2W(szFilePath),0,dwfilesize,szMD5buf))
	{
		OutputDebugPrintf(_T("上传文件MD5获取失败!\n"));
	}

	for (int i = 0 ; i < 16 ; i ++) 
	{
		memset(szhex,0,sizeof(szhex));
		sprintf(szhex,"%02X", szMD5buf[i]);
		strcat(szout,szhex);
		//hex.Format("%02X" , szMD5buf[i]);
		//out += hex;
	}
	//return out ;
	strncpy(szMD5,szout,strlen(szout));
	return 1;
}

static inline BOOL IsPE(const TCHAR* fileName)
{
	//判断是否为PE文件
	HANDLE	hFile    = NULL;
	HANDLE  hMapping = NULL;
	PBYTE	pBuf     = NULL;
	UINT	uSize    = 0; 

	hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;

	}

	hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (INVALID_HANDLE_VALUE == hMapping)
	{
		CloseHandle(hMapping);
		hMapping = NULL;
	}

	pBuf = (PBYTE)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
	if (NULL == pBuf)
	{
		UnmapViewOfFile(pBuf);
		pBuf = NULL;
	}

	uSize = GetFileSize(hFile, 0);

	BOOL bRet = FALSE;
	PIMAGE_DOS_HEADER   pDosHdr  = NULL;
	PIMAGE_NT_HEADERS   pNTHdr   = NULL;

	if (NULL == pBuf || 0 == uSize)
		goto Exit0;

	pDosHdr = (PIMAGE_DOS_HEADER)pBuf;
	if (IsBadReadPtr(pDosHdr, sizeof(IMAGE_DOS_HEADER)))
		goto Exit0;
	if (IMAGE_DOS_SIGNATURE != pDosHdr->e_magic)
		goto Exit0;

	pNTHdr = (PIMAGE_NT_HEADERS)((PBYTE)pDosHdr + pDosHdr->e_lfanew);
	if (IsBadReadPtr(pNTHdr, sizeof(IMAGE_NT_HEADERS32)))
		goto Exit0;
	if (IMAGE_NT_SIGNATURE != pNTHdr->Signature)
		goto Exit0;

	bRet = TRUE;
Exit0:
	return bRet;
}

static CString GetINIPath()
{
	TCHAR szPath[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH,szPath);
	CString strPath;
	strPath.Format(_T("%s\\CTLconfig.ini"),szPath);
	//strPath.Format(//("THconfig.ini"));
	return strPath;
}


static inline int iniReadInteger(LPCTSTR szKey, LPCTSTR szSection = _T("InstallRecord"),int iDefaultValue = 0)
{
	int iResult = GetPrivateProfileInt(szSection, szKey, iDefaultValue, GetINIPath());
	return iResult;
}

static inline void iniWriteInteger(LPCTSTR szKey, int iValue,LPCTSTR szSection = _T("InstallRecord"))
{
	TCHAR szValue[255] = {0};
	_stprintf_s(szValue, 255, TEXT("%d"), iValue);
	WritePrivateProfileString(szSection, szKey, szValue, GetINIPath());
}




static inline wchar_t* ANSIToUnicode( const char* str )   
{   
	int textlen ;   
	wchar_t * result;   
	textlen = MultiByteToWideChar( CP_ACP, 0, str,-1, NULL,0 );   
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));   
	memset(result,0,(textlen+1)*sizeof(wchar_t));   
	MultiByteToWideChar(CP_ACP, 0,str,-1,(LPWSTR)result,textlen );   
	return result;   
}   

static inline char* UnicodeToANSI( const wchar_t* str )   
{   
	char* result;   
	int textlen;   
	textlen = WideCharToMultiByte( CP_ACP, 0, str, -1, NULL, 0, NULL, NULL );   
	result =(char *)malloc((textlen+1)*sizeof(char));   
	memset( result, 0, sizeof(char) * ( textlen + 1 ) );   
	WideCharToMultiByte( CP_ACP, 0, str, -1, result, textlen, NULL, NULL );   
	return result;   
}   

static inline wchar_t* UTF8ToUnicode( const char* str )   
{   
	int textlen ;   
	wchar_t * result;   
	textlen = MultiByteToWideChar( CP_UTF8, 0, str,-1, NULL,0 );   
	result = (wchar_t *)malloc((textlen+1)*sizeof(wchar_t));   
	memset(result,0,(textlen+1)*sizeof(wchar_t));   
	MultiByteToWideChar(CP_UTF8, 0,str,-1,(LPWSTR)result,textlen );   
	return result;   
}   

static inline char* UnicodeToUTF8( const wchar_t* str )   
{   
	char* result;   
	int textlen;   
	textlen = WideCharToMultiByte( CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL );   
	result =(char *)malloc((textlen+1)*sizeof(char));   
	memset(result, 0, sizeof(char) * ( textlen + 1 ) );   
	WideCharToMultiByte( CP_UTF8, 0, str, -1, result, textlen, NULL, NULL );   
	return result;   
}   

/*宽字符转换为多字符Unicode - ANSI*/  
static inline char* w2m(const wchar_t* wcs)   
{   
	int len;   
	char* buf;   
	len =wcstombs(NULL,wcs,0);   
	if (len == 0)   
		return NULL;   
	buf = (char *)malloc(sizeof(char)*(len+1));   
	memset(buf, 0, sizeof(char) *(len+1));   
	len =wcstombs(buf,wcs,len+1);   
	return buf;   
}   
/*多字符转换为宽字符ANSI - Unicode*/  
static inline wchar_t* m2w(const char* mbs)   
{   
	int len;   
	wchar_t* buf;   
	len =mbstowcs(NULL,mbs,0);   
	if (len == 0)   
		return NULL;   
	buf = (wchar_t *)malloc(sizeof(wchar_t)*(len+1));   
	memset(buf, 0, sizeof(wchar_t) *(len+1));   
	len =mbstowcs(buf,mbs,len+1);   
	return buf;   
}   

static inline char* ANSIToUTF8(const char* str)   
{   
	return UnicodeToUTF8(ANSIToUnicode(str));   
}   

static inline char* UTF8ToANSI(const char* str)   
{   
	return UnicodeToANSI(UTF8ToUnicode(str));   
}   




/*
static CRITICAL_SECTION g_cs;
class Lock
{
public:
	Lock(){
		
		InitializeCriticalSection(&g_cs);}

	void LockOn(){
		EnterCriticalSection(&g_cs);
	}
	void LockOff(){
		LeaveCriticalSection(&g_cs);
	}
	~Lock(){
		DeleteCriticalSection(&g_cs);
	}
};
*/