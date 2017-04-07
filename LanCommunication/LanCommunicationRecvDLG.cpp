// LanCommunicationRecvDLG.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LanCommunication.h"
#include "LanCommunicationRecvDLG.h"
#include "LanSock.h"
#include "LanDefine.h"
#include "base64.h"
#include "afxdialogex.h"

//CLanCommunicationRecvDLG *pLanCommunicationRecvDLG;
// CLanCommunicationRecvDLG �Ի���

IMPLEMENT_DYNAMIC(CLanCommunicationRecvDLG, CDialogEx)

CLanCommunicationRecvDLG::CLanCommunicationRecvDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLanCommunicationRecvDLG::IDD, pParent)
{

}

CLanCommunicationRecvDLG::~CLanCommunicationRecvDLG()
{
}

void CLanCommunicationRecvDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_edit_recv_msg);
	DDX_Control(pDX, IDC_BUTTON_RECVFILENAME, m_recv_ButtonFileName);
}


BEGIN_MESSAGE_MAP(CLanCommunicationRecvDLG, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_READED, &CLanCommunicationRecvDLG::OnBnClickedButtonReaded)
	ON_BN_CLICKED(IDC_BUTTON_RECVFILENAME, &CLanCommunicationRecvDLG::OnClickedButtonRecvfilename)
	//ON_MESSAGE(WM_MSG_TCP_FILE_SEND, &CLanCommunicationRecvDLG::OnMsgTcpFileSend)
	ON_MESSAGE(WM_MSG_TCP_FILE_RECV, &CLanCommunicationRecvDLG::OnMsgTcpFileRecv)
	ON_MESSAGE(WM_MSG_TCP_FILEINFO_RECV, &CLanCommunicationRecvDLG::OnMsgTcpFileinfoRecv)
	ON_WM_CLOSE()
END_MESSAGE_MAP()



// CLanCommunicationRecvDLG ��Ϣ�������

BOOL CLanCommunicationRecvDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_recv_ButtonFileName.ShowWindow(SW_HIDE);
	CRect rc_edit;
	m_edit_recv_msg.GetWindowRect(&rc_edit);
	m_edit_recv_msg.SetWindowPos( NULL,1,4,rc_edit.right-rc_edit.left,rc_edit.bottom-rc_edit.top+21,SWP_NOZORDER );  

	nRecvBufSize = 0;
	nSaveFileSize = 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CLanCommunicationRecvDLG::OnBnClickedButtonReaded()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���ش���
	this->ShowWindow(SW_HIDE);
	//�ظ�ԭ״
	m_recv_ButtonFileName.ShowWindow(SW_HIDE);
	m_recv_ButtonFileName.SetWindowText(_T(""));
	CRect rc_edit;
	m_edit_recv_msg.GetWindowRect(&rc_edit);
	m_edit_recv_msg.SetWindowPos( NULL,1,4,rc_edit.right-rc_edit.left,rc_edit.bottom-rc_edit.top+21,SWP_NOZORDER );    
	//��������
	vec_filerecv.clear();
}


void CLanCommunicationRecvDLG::OnClickedButtonRecvfilename()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO bi;            //BROWSEINFO�ṹ��
	TCHAR Buffer[512]=L"";
	TCHAR FullPath[512]=L"";
	bi.hwndOwner = m_hWnd;   //m_hWnd��ĳ���������
	bi.pidlRoot = NULL;
	bi.pszDisplayName = Buffer; //����ѡ���Ŀ¼���Ļ�����
	bi.lpszTitle = L"Selection"; //�����Ĵ��ڵ�������ʾ
	bi.ulFlags = BIF_RETURNONLYFSDIRS ; //ֻ����Ŀ¼��������־��MSDN
	bi.lpfn = NULL;                         //�ص���������ʱ������
	bi.lParam = 0;
	bi.iImage = 0;
	ITEMIDLIST* pidl = ::SHBrowseForFolder (&bi); //��ʾ�������ڣ�ITEMIDLIST����Ҫ
	if(::SHGetPathFromIDList (pidl,FullPath))     //��ITEMIDLIST�еõ�Ŀ¼��������·��
	{
		//�ɹ�
		//AfxMessageBox(FullPath);
		//�������·��
		strDir = FullPath;
		SOCKADDR_IN addr;
		addr.sin_family			= AF_INET;
		addr.sin_addr.s_addr	= inet_addr(strip.c_str());
		//addr.sin_addr.s_addr = inet_addr("172.31.50.65");
		//addr.sin_port = htons(atoi(strport.c_str()));
		addr.sin_port = htons(4423);
		CLanSock::GetInstance()->LanSockTCP_MSG_READY_RECV_FILE(vec_filerecv,addr);

		//����˲��ǽ��ն�
		//����ֻҪ���� �����ļ��� ���ͻ��� ���Կ�ʼ���ͼ���

	}
	else
	{
		//ʧ��
	}
}


////���Ͷ���Ӧ ��Ŀ��IP ����
//// �����ӦҪ��   ���Ͷ�û������������� û���  2017-4-5
//afx_msg LRESULT CLanCommunicationRecvDLG::OnMsgTcpFileSend(WPARAM wParam, LPARAM lParam)
//{
//	char* szip = (char*)wParam;
//	strIP = szip;		// ����Ҫ���͵���IP
//	//SOCKET accept = (SOCKET)wParam;
//	HANDLE	hanlde = (HANDLE)_beginthreadex(NULL, 0, FileSendThread, this, 0, NULL);
//
//	return 0;
//}


//���ն���Ϣ��Ӧ
afx_msg LRESULT CLanCommunicationRecvDLG::OnMsgTcpFileRecv(WPARAM wParam, LPARAM lParam)
{
	char* szbuf = (char*)wParam;
	CString strRecvProc;

	if (strstr(szbuf,"finish"))
	{
		fclose(fp);
		m_edit_recv_msg.SetWindowText(__T("�ļ��������"));
	}
	else
	{
			Json::Reader JReader;
			Json::Value JValue;
			if(JReader.parse(szbuf,JValue))
			{
				std::string strRecvBase64 =JValue["buf"].asString();
				std::string strRecvBuf = base64_decode(strRecvBase64);
				fwrite(strRecvBuf.c_str(),1,strRecvBuf.length(),fp);
				nRecvBufSize += strRecvBuf.length();
				strRecvProc.Format(__T("����ɣ�%I64d �ܳ��ȣ�%I64d"),nRecvBufSize,nSaveFileSize);			//���϶����ʽ������
			//	strRecvProc.Format(_T("%I64d  %I64d"),nRecvBufSize,nSaveFileSize);
			//	m_edit_recv_msg.SetSel(-1);  
				m_edit_recv_msg.SetWindowText(strRecvProc);  
			}
			else
			{
				CString str(szbuf);
				AfxMessageBox(str);
				AfxMessageBox(__T("json��������"));
				throw "parse error";
			}
	}
	return 0;
}

//���ն���Ϣ��Ӧ
afx_msg LRESULT CLanCommunicationRecvDLG::OnMsgTcpFileinfoRecv(WPARAM wParam, LPARAM lParam)
{
	char* szbuf = (char*)wParam;
	Json::Reader JReader;
	Json::Value JValue;
	CString str(szbuf);
// 	if (!strstr(szbuf,TCP_MSG_FILE_SEND_REQUEST))
// 	{
// 		return 1;
// 	}
	if(JReader.parse(szbuf,JValue))
	{
#if _DEBUG
		AfxMessageBox(str);
#endif
		strSaveFileName = CA2W(JValue["filename"].asString().c_str());
		nSaveFileSize = JValue["filesize"].asInt64();
		strSaveFullPath.Format(__T("%s\\%s"),strDir,strSaveFileName);
		fp = fopen(CW2A(strSaveFullPath.GetBuffer(strSaveFullPath.GetLength())),"wb");
		if (!fp)
		{
			AfxMessageBox(__T("�����ļ�·������"));
		}
	}
	else
	{
		//throw "parse error";
	}

	return 0;
}



void CLanCommunicationRecvDLG::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//AfxMessageBox(__T("�����ļ�·������"));
	//���ش���
	this->ShowWindow(SW_HIDE);
	//�ظ�ԭ״
	m_recv_ButtonFileName.ShowWindow(SW_HIDE);
	m_recv_ButtonFileName.SetWindowText(_T(""));
	CRect rc_edit;
	m_edit_recv_msg.GetWindowRect(&rc_edit);
	m_edit_recv_msg.SetWindowPos( NULL,1,4,rc_edit.right-rc_edit.left,rc_edit.bottom-rc_edit.top+21,SWP_NOZORDER );    
	//��������
	vec_filerecv.clear();
	CDialogEx::OnClose();
}

