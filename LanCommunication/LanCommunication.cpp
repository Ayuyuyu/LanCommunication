
// LanCommunication.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "LanCommunication.h"
#include "LanCommunicationDlg.h"
#include "LanGetLocalSystemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLanCommunicationApp

BEGIN_MESSAGE_MAP(CLanCommunicationApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLanCommunicationApp ����

CLanCommunicationApp::CLanCommunicationApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CLanCommunicationApp ����

CLanCommunicationApp theApp;


// CLanCommunicationApp ��ʼ��

BOOL CLanCommunicationApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	SetRegistryKey(_T("yuc LanCommunication"));

 	CLanCommunicationDlg dlg;
 	m_pMainWnd = &dlg;
	//pLanCommunicationDlg = &dlg;
 	INT_PTR nResponse = dlg.DoModal();
	

	CLanGetLocalSystemInfo minfo;
	minfo.getHostIP();
	minfo.getComputername();
	minfo.getUsername();
// 	if (nResponse == IDOK)
// 	{
// 		// TODO: �ڴ˷��ô����ʱ��
// 		//  ��ȷ�������رնԻ���Ĵ���
// 	}
// 	else if (nResponse == IDCANCEL)
// 	{
// 		// TODO: �ڴ˷��ô����ʱ��
// 		//  ��ȡ�������رնԻ���Ĵ���
// 	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

