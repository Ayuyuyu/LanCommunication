// LanCommunicationRecvDLG.cpp : 实现文件
//

#include "stdafx.h"
#include "LanCommunication.h"
#include "LanCommunicationRecvDLG.h"
#include "LanSock.h"
#include "LanDefine.h"
#include "base64.h"
#include "afxdialogex.h"

//CLanCommunicationRecvDLG *pLanCommunicationRecvDLG;
// CLanCommunicationRecvDLG 对话框

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



// CLanCommunicationRecvDLG 消息处理程序

BOOL CLanCommunicationRecvDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_recv_ButtonFileName.ShowWindow(SW_HIDE);
	CRect rc_edit;
	m_edit_recv_msg.GetWindowRect(&rc_edit);
	m_edit_recv_msg.SetWindowPos( NULL,1,4,rc_edit.right-rc_edit.left,rc_edit.bottom-rc_edit.top+21,SWP_NOZORDER );  

	nRecvBufSize = 0;
	nSaveFileSize = 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CLanCommunicationRecvDLG::OnBnClickedButtonReaded()
{
	// TODO: 在此添加控件通知处理程序代码
	//隐藏窗口
	this->ShowWindow(SW_HIDE);
	//回复原状
	m_recv_ButtonFileName.ShowWindow(SW_HIDE);
	m_recv_ButtonFileName.SetWindowText(_T(""));
	CRect rc_edit;
	m_edit_recv_msg.GetWindowRect(&rc_edit);
	m_edit_recv_msg.SetWindowPos( NULL,1,4,rc_edit.right-rc_edit.left,rc_edit.bottom-rc_edit.top+21,SWP_NOZORDER );    
	//清理数据
	vec_filerecv.clear();
}


void CLanCommunicationRecvDLG::OnClickedButtonRecvfilename()
{
	// TODO: 在此添加控件通知处理程序代码
	BROWSEINFO bi;            //BROWSEINFO结构体
	TCHAR Buffer[512]=L"";
	TCHAR FullPath[512]=L"";
	bi.hwndOwner = m_hWnd;   //m_hWnd你的程序主窗口
	bi.pidlRoot = NULL;
	bi.pszDisplayName = Buffer; //返回选择的目录名的缓冲区
	bi.lpszTitle = L"Selection"; //弹出的窗口的文字提示
	bi.ulFlags = BIF_RETURNONLYFSDIRS ; //只返回目录。其他标志看MSDN
	bi.lpfn = NULL;                         //回调函数，有时很有用
	bi.lParam = 0;
	bi.iImage = 0;
	ITEMIDLIST* pidl = ::SHBrowseForFolder (&bi); //显示弹出窗口，ITEMIDLIST很重要
	if(::SHGetPathFromIDList (pidl,FullPath))     //在ITEMIDLIST中得到目录名的整个路径
	{
		//成功
		//AfxMessageBox(FullPath);
		//保存接收路径
		strDir = FullPath;
		SOCKADDR_IN addr;
		addr.sin_family			= AF_INET;
		addr.sin_addr.s_addr	= inet_addr(strip.c_str());
		//addr.sin_addr.s_addr = inet_addr("172.31.50.65");
		//addr.sin_port = htons(atoi(strport.c_str()));
		addr.sin_port = htons(4423);
		CLanSock::GetInstance()->LanSockTCP_MSG_READY_RECV_FILE(vec_filerecv,addr);

		//服务端才是接收端
		//这里只要告诉 发送文件端 即客户端 可以开始发送即可

	}
	else
	{
		//失败
	}
}


////发送端响应 网目的IP 发送
//// 这个响应要改   发送端没有启动这个界面 没句柄  2017-4-5
//afx_msg LRESULT CLanCommunicationRecvDLG::OnMsgTcpFileSend(WPARAM wParam, LPARAM lParam)
//{
//	char* szip = (char*)wParam;
//	strIP = szip;		// 保存要发送到的IP
//	//SOCKET accept = (SOCKET)wParam;
//	HANDLE	hanlde = (HANDLE)_beginthreadex(NULL, 0, FileSendThread, this, 0, NULL);
//
//	return 0;
//}


//接收端消息响应
afx_msg LRESULT CLanCommunicationRecvDLG::OnMsgTcpFileRecv(WPARAM wParam, LPARAM lParam)
{
	char* szbuf = (char*)wParam;
	CString strRecvProc;

	if (strstr(szbuf,"finish"))
	{
		fclose(fp);
		m_edit_recv_msg.SetWindowText(__T("文件传输完成"));
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
				strRecvProc.Format(__T("已完成：%I64d 总长度：%I64d"),nRecvBufSize,nSaveFileSize);			//符合定义格式的输入
			//	strRecvProc.Format(_T("%I64d  %I64d"),nRecvBufSize,nSaveFileSize);
			//	m_edit_recv_msg.SetSel(-1);  
				m_edit_recv_msg.SetWindowText(strRecvProc);  
			}
			else
			{
				CString str(szbuf);
				AfxMessageBox(str);
				AfxMessageBox(__T("json解析错误"));
				throw "parse error";
			}
	}
	return 0;
}

//接收端消息响应
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
			AfxMessageBox(__T("保存文件路径错误。"));
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//AfxMessageBox(__T("保存文件路径错误。"));
	//隐藏窗口
	this->ShowWindow(SW_HIDE);
	//回复原状
	m_recv_ButtonFileName.ShowWindow(SW_HIDE);
	m_recv_ButtonFileName.SetWindowText(_T(""));
	CRect rc_edit;
	m_edit_recv_msg.GetWindowRect(&rc_edit);
	m_edit_recv_msg.SetWindowPos( NULL,1,4,rc_edit.right-rc_edit.left,rc_edit.bottom-rc_edit.top+21,SWP_NOZORDER );    
	//清理数据
	vec_filerecv.clear();
	CDialogEx::OnClose();
}

