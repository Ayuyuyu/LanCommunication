#pragma once
#include "afxwin.h"
#include <string>
#include <vector>
// CLanCommunicationRecvDLG 对话框


typedef struct  
{
	char szFileBeSend[MAX_PATH*2];
	char szip[MAX_PATH];
}SendFile;

class CLanCommunicationRecvDLG : public CDialogEx
{

	DECLARE_DYNAMIC(CLanCommunicationRecvDLG)

public:
	CLanCommunicationRecvDLG(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLanCommunicationRecvDLG();

// 对话框数据
	enum { IDD = IDD_LANCOMMUNICATION_RECV_DIALOG };
public:
	//文件发送线程
	static unsigned int __stdcall FileSendThread(void* param);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

protected:
	//afx_msg LRESULT OnMsgTcpFileSend(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgTcpFileRecv(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgTcpFileinfoRecv(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnClickedButtonRecvfilename();
	afx_msg void OnBnClickedButtonReaded();
	DECLARE_MESSAGE_MAP()

public:
	CEdit m_edit_recv_msg;
	CButton m_recv_ButtonFileName;

public:
	FILE* fp;
	std::string strip;
	//std::string strport;
	CString strDir;
	CString strSaveFullPath;
	CString strSaveFileName;		//保存收到的文件名
	INT64 nSaveFileSize;				//保存收到的文件大小
	INT64 nRecvBufSize;				//收到的数据大小

	std::vector<std::string> vec_filerecv;				//用于显示RecvDLG的文件信息

};
//extern CLanCommunicationRecvDLG *pLanCommunicationRecvDLG;