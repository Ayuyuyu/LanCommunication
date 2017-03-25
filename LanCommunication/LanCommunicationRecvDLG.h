#pragma once
#include "afxwin.h"
#include <string>
#include <vector>
// CLanCommunicationRecvDLG �Ի���

class CLanCommunicationRecvDLG : public CDialogEx
{
	afx_msg void OnClickedButtonRecvfilename();
	afx_msg void OnBnClickedButtonReaded();
	DECLARE_DYNAMIC(CLanCommunicationRecvDLG)

public:
	CLanCommunicationRecvDLG(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLanCommunicationRecvDLG();

// �Ի�������
	enum { IDD = IDD_LANCOMMUNICATION_RECV_DIALOG };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	std::vector<std::string> vec_filerecv;				//������ʾRecvDLG���ļ���Ϣ
protected:
	afx_msg LRESULT OnMsgTcpFileSend(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgTcpFileRecv(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgTcpFileinfoRecv(WPARAM wParam, LPARAM lParam);
};
//extern CLanCommunicationRecvDLG *pLanCommunicationRecvDLG;