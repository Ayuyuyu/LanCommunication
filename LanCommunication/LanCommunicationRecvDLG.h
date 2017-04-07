#pragma once
#include "afxwin.h"
#include <string>
#include <vector>
// CLanCommunicationRecvDLG �Ի���


typedef struct  
{
	char szFileBeSend[MAX_PATH*2];
	char szip[MAX_PATH];
}SendFile;

class CLanCommunicationRecvDLG : public CDialogEx
{

	DECLARE_DYNAMIC(CLanCommunicationRecvDLG)

public:
	CLanCommunicationRecvDLG(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLanCommunicationRecvDLG();

// �Ի�������
	enum { IDD = IDD_LANCOMMUNICATION_RECV_DIALOG };
public:
	//�ļ������߳�
	static unsigned int __stdcall FileSendThread(void* param);

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	CString strSaveFileName;		//�����յ����ļ���
	INT64 nSaveFileSize;				//�����յ����ļ���С
	INT64 nRecvBufSize;				//�յ������ݴ�С

	std::vector<std::string> vec_filerecv;				//������ʾRecvDLG���ļ���Ϣ

};
//extern CLanCommunicationRecvDLG *pLanCommunicationRecvDLG;