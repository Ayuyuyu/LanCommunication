
// LanCommunicationDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "LanCommunicationRecvDLG.h"
#include <set>
#include <vector>
#include <string>
#include "myedit.h"

// CLanCommunicationDlg �Ի���
class CLanCommunicationDlg : public CDialogEx
{
// ����
public:
	CLanCommunicationDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LANCOMMUNICATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonSendMsg();
	afx_msg LRESULT OnMsgUdpAddUser(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgUdpSendMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgUdpClose(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	CListCtrl m_listctrl_online;
	CMyEdit m_myedit;
	int nList;
public:
	void OnInitControls();		//��ʼ���ؼ�
	void OnInitParam();			//��ʼ����������

protected:
	//CLanCommunicationRecvDLG *pLanCommunicationRecvDLG;
	std::set<std::string> set_addrip;
	std::set<std::string>::iterator set_iter;  
	std::vector<std::string> vec_file;					//�����ļ���vector
	std::vector<std::string> vec_file_recv;			//�յ��ļ���vector
	BOOL bfile;
	//std::set<CString> ::iterator set_file_iter;
	CFont fontEdit,fontTitle,fontPort;
	CBrush brushEdit,brushTitle,brushPort;

public:

	CButton m_ButtinFileName;
protected:
	afx_msg LRESULT OnMsgButtonSendFilename(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgUdpFileinfoSend(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClickListOnlinelist(NMHDR *pNMHDR, LRESULT *pResult);
};
//extern CLanCommunicationDlg *pLanCommunicationDlg;