#include "StdAfx.h"
#include "MyEdit.h"
#include "LanDefine.h"

CMyEdit::CMyEdit(void)
{
}


CMyEdit::~CMyEdit(void)
{
}
BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	ON_WM_DROPFILES()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


void CMyEdit::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int  nFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, MAX_PATH);   //��ȡ������ļ�����  
	CString m_FilePath;
	TCHAR  pFilePath[MAX_PATH*2 ]= {0};  
	for (int i=0; i<nFileCount; i++)  
	{  
		UINT nChars = ::DragQueryFile(hDropInfo, i, pFilePath,MAX_PATH*2);   // ��ȡ����ĵ�i���ļ����ļ���  
		 m_FilePath =pFilePath;   
		 //SetWindowText(m_FilePath);
		 ::SendMessage(this->GetParent()->GetSafeHwnd(),WM_MSG_BUTTON_SEND_FILENAME,(WPARAM)pFilePath,NULL);
	}  
	::DragFinish (hDropInfo);  
	CEdit::OnDropFiles(hDropInfo);
}//dyc01050115

