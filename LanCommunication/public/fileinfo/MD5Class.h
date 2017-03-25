// MD5Class.h: interface for the CMD5Class class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MD5CLASS_H__ED16E7A8_F96E_4281_8A41_2DAE909D45CB__INCLUDED_)
#define AFX_MD5CLASS_H__ED16E7A8_F96E_4281_8A41_2DAE909D45CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/* typedef a 32 bit type */
#include <atlstr.h>
#include <windows.h>
class CMD5Class  
{

private:
	unsigned long int m_count[2];   /* number of _bits_ handled mod 2^64 */
	unsigned long int m_state[4]; /* scratch buffer */
	unsigned char m_buffer[64];     /* input buffer */
	unsigned char PADDING[64];

public:
	int MD5Buffer(BYTE * pData , BYTE * pMD5Buf, int bufLen);
	void MD5Update(unsigned char *inBuf,unsigned int inLen);
	void MD5Final(unsigned char outBuf[16]);
	void MD5Init(void);
	int MD5File(LPCTSTR filename,long offset,long length,unsigned char *outbuf);
	int MD5String(BYTE * pData , CString &sMD5, int bufLen);
	CMD5Class();
	virtual ~CMD5Class();

private:
	void Transform (unsigned long int state[4],unsigned char block[64]);
	void Decode (unsigned long int *output, unsigned char *input, unsigned int len);
	void Encode (unsigned char *output, unsigned long int *input, unsigned int len);
};


#endif // !defined(AFX_MD5CLASS_H__ED16E7A8_F96E_4281_8A41_2DAE909D45CB__INCLUDED_)
