
// LanCommunicationDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "LanCommunicationRecvDLG.h"
#include <set>
#include <vector>
#include <string>
#include "myedit.h"

// CLanCommunicationDlg 对话框
class CLanCommunicationDlg : public CDialogEx
{
// 构造
public:
	CLanCommunicationDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_LANCOMMUNICATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	void OnInitControls();		//初始化控件
	void OnInitParam();			//初始化其他参数

protected:
	//CLanCommunicationRecvDLG *pLanCommunicationRecvDLG;
	std::set<std::string> set_addrip;
	std::set<std::string>::iterator set_iter;  
	std::vector<std::string> vec_file;					//发送文件的vector
	std::vector<std::string> vec_file_recv;			//收到文件的vector
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