
// LanCommunicationDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LanCommunication.h"
#include "LanCommunicationDlg.h"
#include "LanCommunicationRecvDLG.h"
#include "afxdialogex.h"
#include "LanSock.h"
#include "LanDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLanCommunicationDlg �Ի���



//CLanCommunicationDlg *pLanCommunicationDlg;

CLanCommunicationDlg::CLanCommunicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLanCommunicationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
//	pLanCommunicationRecvDLG = nullptr;
	nList = 0;
	bfile = FALSE;
}

void CLanCommunicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ONLINELIST, m_listctrl_online);
	DDX_Control(pDX, IDC_EDIT_MSG, m_myedit);
	DDX_Control(pDX, IDC_BUTTON_SENDFILENAME, m_ButtinFileName);
}

BEGIN_MESSAGE_MAP(CLanCommunicationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND_MSG, &CLanCommunicationDlg::OnBnClickedButtonSendMsg)
	ON_MESSAGE(WM_MSG_UDP_ADD_USER, &CLanCommunicationDlg::OnMsgUdpAddUser)
	ON_MESSAGE(WM_MSG_UDP_SEND_MSG, &CLanCommunicationDlg::OnMsgUdpSendMsg)
	ON_MESSAGE(WM_MSG_UDP_CLOSE, &CLanCommunicationDlg::OnMsgUdpClose)
	ON_MESSAGE(WM_MSG_BUTTON_SEND_FILENAME, &CLanCommunicationDlg::OnMsgButtonSendFilename)
	ON_MESSAGE(WM_MSG_UDP_FILEINFO_SEND, &CLanCommunicationDlg::OnMsgUdpFileinfoSend)
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_WM_CTLCOLOR()

	ON_NOTIFY(NM_CLICK, IDC_LIST_ONLINELIST, &CLanCommunicationDlg::OnClickListOnlinelist)
END_MESSAGE_MAP()


// CLanCommunicationDlg ��Ϣ�������

BOOL CLanCommunicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	//m_ButtinFileName.GetWindowRect(&rect);
	const char* ss =strstr("NEW#test","NEW");
	char sztmp1[MAX_PATH];
	char sztmp2[MAX_PATH];
	sscanf("usernametest#test#","%[^#]#%[^#]",sztmp1,sztmp2);
	OnInitControls();
	OnInitParam();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLanCommunicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLanCommunicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLanCommunicationDlg::OnInitControls()
{
	//��ʼ�� listctrl ���
	CRect rect;
	m_listctrl_online.GetClientRect(&rect);   
	// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����   
	m_listctrl_online.SetExtendedStyle(m_listctrl_online.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);  
	m_listctrl_online.InsertColumn(0, _T("UserName"), LVCFMT_CENTER, rect.Width()/3, 0);   
	m_listctrl_online.InsertColumn(1, _T("ComputerName"), LVCFMT_CENTER, rect.Width()/3, 1);   
	m_listctrl_online.InsertColumn(2, _T("IP"), LVCFMT_CENTER, rect.Width()/3, 2);   
	//m_listctrl_online.InsertColumn(3, _T("Link"), LVCFMT_CENTER, 0, 3)

	m_ButtinFileName.ShowWindow(SW_HIDE);

	CLanSock::GetInstance()->hMainhandle = this->GetSafeHwnd();
	//��ʼ��һЩ���� �뻭ˢ
// 	brushTitle.CreateSolidBrush(RGB(50,180,200));
// 	fontTitle.CreatePointFont(100,_T("΢���ź�"),NULL);//
		brushEdit.CreateSolidBrush(RGB(0, 0, 255));//��ˢ
		fontEdit.CreatePointFont(100,_T("΢���ź�"), NULL);//
// 	brushPort.CreateSolidBrush(RGB(220,55,20));
// 	fontPort.CreatePointFont(100,_T("΢���ź�"),NULL);
		m_myedit.SetFont(&fontEdit);
}


// ���� ��ť��Ӧ
void CLanCommunicationDlg::OnBnClickedButtonSendMsg()
{
	if (bfile)
	{
		CRect rc_edit;
		m_myedit.GetWindowRect(&rc_edit);
		m_myedit.SetWindowPos( NULL,2,240,rc_edit.right-rc_edit.left,rc_edit.bottom-rc_edit.top+20,SWP_NOZORDER ); 
		m_ButtinFileName.ShowWindow(SW_HIDE);

		TCHAR szMsgWrite[MAX_PATH*10] = {0};
		m_myedit.GetWindowText(szMsgWrite,MAX_PATH*10);

		CLanSock::GetInstance()->LanSockUDP_MSG_FILEINFO_SEND(vec_file,CW2A(szMsgWrite));

		bfile = FALSE;
	}
	else
	{
		TCHAR szMsgWrite[MAX_PATH*10] = {0};
		//_tcsncpy(szMsgWrite,_T(UDP_MSG_SEND),_tcslen(_T(UDP_MSG_SEND)));
		m_myedit.GetWindowText(szMsgWrite,MAX_PATH*10);
		// _tcsnccat(szMsgWrite,_T("#"),1);
		CLanSock::GetInstance()->LanSockUDP_MSG_SEND(CW2A(szMsgWrite),strlen(CW2A(szMsgWrite)));
	}	
}

void CLanCommunicationDlg::OnInitParam()
{
	CLanSock::GetInstance()->LanSockSelect();
	//������Ϣ����
	CLanSock::GetInstance()->LanSockUDP_MSG_ADD_USER();
}



//��Ӧ�յ�UDP������Ϣ ����list
afx_msg LRESULT CLanCommunicationDlg::OnMsgUdpAddUser(WPARAM wParam, LPARAM lParam)
{
	udp_hostinfo* hostinfo = (udp_hostinfo*)wParam;
	std:;string sip = inet_ntoa(hostinfo->addr.sin_addr);
	set_iter = set_addrip.find(sip);
	if (set_iter == set_addrip.end())
	{
		m_listctrl_online.InsertItem(nList, CA2W(hostinfo->szUserName));
		m_listctrl_online.SetItemText(nList, 1,CA2W(hostinfo->szComputerName));
		m_listctrl_online.SetItemText(nList, 2,CA2W(sip.c_str()));
		//����set
		set_addrip.insert(sip);
	}
// 	m_listctrl_online.SetItemText(nList, 1, ptmp->SearchSize);
// 	m_listctrl_online.SetItemText(nList, 2, ptmp->SearchHotPoint);
// 	m_listctrl_online.SetItemText(nList, 3, ptmp->SearchLink);
	nList++;
	return 0;
}

//��Ӧ�յ�UDP��Ϣ ������ʾ
afx_msg LRESULT CLanCommunicationDlg::OnMsgUdpSendMsg(WPARAM wParam, LPARAM lParam)
{
	char* szUdp_msg = (char*)wParam;
	Json::Reader JReader;
	Json::Value JValue;
	//��ʼ��Recv dlg ����

	CLanCommunicationRecvDLG *pLanCommunicationRecvDLG = new CLanCommunicationRecvDLG();
	pLanCommunicationRecvDLG->Create(IDD_LANCOMMUNICATION_RECV_DIALOG,this);
	pLanCommunicationRecvDLG->CenterWindow();
//	pLanCommunicationRecvDLG->ShowWindow(SW_HIDE);

	if(JReader.parse(szUdp_msg,JValue))
	{
		CString strDlgTitle;
		strDlgTitle.Format(_T("���� %s ����Ϣ"),CA2W(JValue["ip"].asString().c_str()));
		pLanCommunicationRecvDLG->SetWindowText(strDlgTitle); 
		pLanCommunicationRecvDLG->m_edit_recv_msg.SetWindowText(CA2W(JValue["msg"].asString().c_str()));
		pLanCommunicationRecvDLG->ShowWindow(SW_SHOW);
	}

	return 0;
}

//�յ������ļ���Ϣ��udp_MSG
afx_msg LRESULT CLanCommunicationDlg::OnMsgUdpFileinfoSend(WPARAM wParam, LPARAM lParam)
{
	char* szUdp_msg = (char*)wParam;
	Json::Reader Jread;
	Json::Value Jmsg,Jfile;
	vec_file_recv.clear();
	if(Jread.parse(szUdp_msg,Jmsg) < 0)
	{
		throw __T("json��������");
		AfxMessageBox(__T("json"));
	}
	//�п����ж��
	//std::string  strFIleinfo = Jmsg["filepaths"].asString();
	//std::string  strFIlename = Jmsg["filenames"].asString();
	Jfile = Jmsg["filepaths"];
	std::string strMsg = Jmsg["msg"].asString();
	std::string strpath;
	for (int i = 0 ; i< Jfile.size() ; i++)
	{
		strpath.append(PathFindFileNameA(Jfile[i].asString().c_str()));
		strpath.append(",");
		vec_file_recv.push_back(Jfile[i].asString());
	}
	//��������
	CLanCommunicationRecvDLG *pLanCommunicationRecvDLG = new CLanCommunicationRecvDLG();
	pLanCommunicationRecvDLG->Create(IDD_LANCOMMUNICATION_RECV_DIALOG,this);
	pLanCommunicationRecvDLG->CenterWindow();
	//�����������ڵľ��
	//CLanSock::GetInstance()->LanSockGetHandle(this->GetSafeHwnd());
	CLanSock::GetInstance()->hRecvhandle = pLanCommunicationRecvDLG->GetSafeHwnd();
	CString strDlgTitle;
	strDlgTitle.Format(_T("���� %s ����Ϣ"),CA2W(Jmsg["ip"].asString().c_str()));
	pLanCommunicationRecvDLG->SetWindowText(strDlgTitle);
	pLanCommunicationRecvDLG->m_recv_ButtonFileName.SetWindowText(CA2W(strpath.c_str()));
	pLanCommunicationRecvDLG->m_edit_recv_msg.SetWindowText(CA2W(strMsg.c_str()));
	//
	pLanCommunicationRecvDLG->strip = Jmsg["ip"].asString();
	//pLanCommunicationRecvDLG->strport = Jmsg["port"].asString();
	pLanCommunicationRecvDLG->vec_filerecv = vec_file_recv;
	//��ʾ�յ����ļ�����Ϣ
	CRect rc_edit;
	pLanCommunicationRecvDLG->m_edit_recv_msg.GetWindowRect(&rc_edit);
	pLanCommunicationRecvDLG->m_edit_recv_msg.SetWindowPos( NULL,1,23,rc_edit.right-rc_edit.left,rc_edit.bottom-rc_edit.top-20,SWP_NOZORDER );    
	pLanCommunicationRecvDLG->m_recv_ButtonFileName.ShowWindow(SW_SHOW);
	pLanCommunicationRecvDLG->ShowWindow(SW_SHOW);
	return 0;
}

//��Ӧ�յ���UDP�ر���Ϣ  ɾ��list
afx_msg LRESULT CLanCommunicationDlg::OnMsgUdpClose(WPARAM wParam, LPARAM lParam)
{	
	char* ip = (char*)wParam;
	int nList =m_listctrl_online.GetItemCount();
	for (int i = 0; i < nList;i++)
	{
		TCHAR szIP[30]={0};
		m_listctrl_online.GetItemText(i,2,szIP,30);
		if (_stricmp(ip,CW2A(szIP)) == 0)
		{
			m_listctrl_online.DeleteItem(i);
			break;
		}
	}
	return 0;
}


void CLanCommunicationDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: �ڴ˴������Ϣ����������
	CLanSock::GetInstance()->LanSockUDP_MSG_CLOSE();
}



//���ʹ��������ļ���Ӧ
afx_msg LRESULT CLanCommunicationDlg::OnMsgButtonSendFilename(WPARAM wParam, LPARAM lParam)
{
	//char* szPath = (char*) wParam;
	CString strPath = (TCHAR*)wParam;
	std::string tmp = CW2A(strPath);
	if (bfile)
	{
		CString strbutton;
		TCHAR szbutton[MAX_PATH*2] = {0};
		m_ButtinFileName.GetWindowText(szbutton,MAX_PATH*2);
		strbutton.Append(szbutton);
		strbutton.Append( _T(" , "));
		strbutton.Append(PathFindFileName(strPath));
		vec_file.push_back(tmp);
		m_ButtinFileName.SetWindowText(strbutton);
	}
	else
	{
		m_ButtinFileName.SetWindowText(PathFindFileName(strPath));
		CRect rc_edit;
		//CWnd *pWnd;  
		//pWnd = GetDlgItem( IDC_EDIT_MSG );  
		//pWnd->GetWindowRect(&rc_edit);
		//pWnd->SetWindowPos( NULL,2,260,rc_edit.right-rc_edit.left,rc_edit.bottom-rc_edit.top-20,SWP_NOZORDER );    
		m_myedit.GetWindowRect(&rc_edit);
		m_myedit.SetWindowPos( NULL,2,260,rc_edit.right-rc_edit.left,rc_edit.bottom-rc_edit.top-20,SWP_NOZORDER );    
		m_ButtinFileName.ShowWindow(SW_SHOW);
		m_ButtinFileName.SetWindowPos(NULL,0,0,rc_edit.right-rc_edit.left,25,SWP_NOZORDER|SWP_NOMOVE);
		vec_file.push_back(tmp);

		bfile = TRUE;
	}

	
	return 0;
}



void CLanCommunicationDlg::OnClickListOnlinelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	NM_LISTVIEW* pNMListView=(NM_LISTVIEW*)pNMHDR;  
	int n_Row = pNMListView->iItem;		//���ѡ�е���  
	int n_Col = pNMListView->iSubItem;//���ѡ����  

	if (pNMListView->iSubItem == 0 )		//�����sortportstart.ini 
	{

	}
	if   (pNMListView->iSubItem > 0) //���ѡ���������;  ��ѡ��
	{  
		CString strclickip = m_listctrl_online.GetItemText(n_Row,2);
	}  
	else
	{
		return;
	}
	*pResult = 0;
}


BOOL CLanCommunicationDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: �ڴ����ר�ô����/����û���
	//��ֹ�϶���ͷ
	NMHEADER* pNMHeader = (NMHEADER*)lParam;  
	if(((pNMHeader->hdr.code == HDN_BEGINTRACKW) |   
		(pNMHeader->hdr.code == HDN_DIVIDERDBLCLICKW)))  
	{  
		*pResult = TRUE;  
		return TRUE;  
	}  
	return CDialogEx::OnNotify(wParam, lParam, pResult);
}


HBRUSH CLanCommunicationDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (pWnd->GetDlgCtrlID() == IDC_EDIT_MSG)
	{
		//	pDC->SetBkColor(RGB(50,180,200));//����ɫ

		//	pDC->SetTextColor(RGB(0, 0, 0));//����
		//	pDC->SelectObject(&fontEdit);//����Ϊ15�����壬�����п�
		return hbr;
	}
	// 	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PORT)
	// 	{
	// 		pDC->SetBkColor(RGB(220,55,20));
	// 		pDC->SetTextColor(RGB(255,255,255));
	// 		pDC->SelectObject(&fontPort);
	// 		return brushPort;
	// 	}

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}