// BinString.cpp: implementation of the CBinString class.
//
//////////////////////////////////////////////////////////////////////

#include "BinString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBinString::CBinString():CString()
{

}

CBinString::~CBinString()
{

}

void CBinString::PutBinary(LPBYTE pByte , int len)
{
	Empty();
	CString hex;
	for (int i = 0 ; i < len ; i ++) 
	{
		hex.Format(_T("%02X") , pByte[i]);
		*this += hex;
	}
}

//专为RegisterTreeCtrl.cpp\ShowKeyValue()使用
void CBinString::PutBinary(LPBYTE pByte , int len,bool bHaveBlank = false)
{
	Empty();
	CString hex;
	for (int i = 0 ; i < len ; i ++) 
	{
		if (bHaveBlank)
		{
            hex.Format(_T("%02X ") , pByte[i]);
		}
		else
		{
			hex.Format(_T("%02X") , pByte[i]);
		}
		*this += hex;
	}
}

int  CBinString::GetBinary(LPBYTE pByte , int maxlen)
{
	// 保存只包含合法HEX字符
	if (SpanIncluding(_T("0123456789aAbBcCdDeEfF")) != *this) 
		return 0;
	
	int len = GetLength();
	
	// 限长
	if (len > maxlen * 2) 
		len = maxlen * 2; 

	CString hex = *this;
	
	// 补零
	if ((len % 2) != 0) 
	{
		len ++ ; 
		hex += "0";
	} 
	len /= 2;

	// 转换
	for (int i = 0 ; i < len ; i ++) 
	{ 
		int b ; 
		// 注意：sscanf返回整数值，如用BYTE可能崩溃!!
		swscanf_s(hex.Mid((i * 2) , 2).GetBuffer() , _T("%02X") , &b);
		pByte[i] = (BYTE) b;
	}
	return (len);
}
