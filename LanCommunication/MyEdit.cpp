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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int  nFileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, MAX_PATH);   //获取拖入的文件数量  
	CString m_FilePath;
	TCHAR  pFilePath[MAX_PATH*2 ]= {0};  
	for (int i=0; i<nFileCount; i++)  
	{  
		UINT nChars = ::DragQueryFile(hDropInfo, i, pFilePath,MAX_PATH*2);   // 获取拖入的第i个文件的文件名  
		 m_FilePath =pFilePath;   
		 //SetWindowText(m_FilePath);
		 ::SendMessage(this->GetParent()->GetSafeHwnd(),WM_MSG_BUTTON_SEND_FILENAME,(WPARAM)pFilePath,NULL);
	}  
	::DragFinish (hDropInfo);  
	CEdit::OnDropFiles(hDropInfo);
}//dyc01050115

