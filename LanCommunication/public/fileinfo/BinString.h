// BinString.h: interface for the CBinString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINSTRING_H__D5524413_0267_4ADD_B45D_6EAF5DCD1468__INCLUDED_)
#define AFX_BINSTRING_H__D5524413_0267_4ADD_B45D_6EAF5DCD1468__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <atlstr.h>
#include <windows.h>

class CBinString : public CString 
{
public:
	CBinString();
	virtual ~CBinString();
public:
	CBinString::CBinString(LPCTSTR lpsz):CString(lpsz){;};
	CBinString::CBinString(const CString& str):CString(str){;};
	
	// ������ת�ַ���
	void PutBinary(LPBYTE pByte , int len);

	// �ַ���ת�����ƣ����س���
	int  GetBinary(LPBYTE pByte , int maxlen);


	void PutBinary(LPBYTE pByte , int len,bool bHaveBlank);

};


#endif // !defined(AFX_BINSTRING_H__D5524413_0267_4ADD_B45D_6EAF5DCD1468__INCLUDED_)
