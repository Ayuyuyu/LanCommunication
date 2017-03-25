#pragma once
#include "base_sock.h"

/************************************************************************/
/* 
FD_READ�����¼��ڳ����������֮һ�ͱ�������
1�����뻺���������ݿ��Զ��룻
2�������Ѿ��رգ��������ֹ��
3�����ѵ�����listen������accept�����ɹ�����һ�����ӡ� 

FD_WRITE�����¼��ڳ����������֮һ�ͱ�������
1�������ݿ��Է�����
2�����������˶�һ�����������ӵ��õĴ������Ӿͻ�ɹ�                  




*/
/************************************************************************/

#include <vector>
#include <string>

//typedef int (*)(unsigned char* szBuffer,int szBuffer_size,void* param);
typedef long (*LanMsgSendToDlg)(const int Msg_,const void* Buf);

class CEventSocket :public base_sock
{
public:
	CEventSocket(void);
	~CEventSocket(void);
public:
	static void   SelectThread( void * arg );
	bool SelectEvent( int nEvent = FD_READ | FD_WRITE | FD_CONNECT | FD_OOB | FD_CLOSE | FD_ACCEPT );
	void  SelectFunc();
	void StopSelect( bool bForce= false );
	bool CreateSocketInformation(SOCKET s);
	void FreeSocketInformation(DWORD Event);
public:
	//void Send(CHAR* buffer,ULONG buflen);
	void SendMsgDLG(int msg_,CHAR*buf,void*param);

public:
	  void  OnReceive( int nErrorCode,DWORD Event,SOCKET socket);
	  void  OnSend( int nErrorCode,DWORD Event);
	  void  OnAccept( int nErrorCode,DWORD Event);
	  void  OnClose( int nErrorCode,DWORD Event);
	  void  OnConnect( int nErrorCode,DWORD Event);
	  void  OnOutOfBandData( int nErrorCode,DWORD Event);
	  int OnFileRecv(int nErrorCode,DWORD Event);
	  LanMsgSendToDlg func_LanMsgSendToDlg;

private:
	bool      m_bSelect;				//Select��ʶ��
	bool      m_bEventStop;			//�¼�ֹͣ��ʶ��
	HANDLE    m_hThread;			//�߳̾��

	SOCKET s_accept;					//accept�����
	//DWORD Event;						//��ǰ�����¼�
	DWORD EventTotal;				//�¼�����
	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	LPSOCKET_INFORMATION SocketArray[WSA_MAXIMUM_WAIT_EVENTS];			//��Ϊ��Ҫ�Ľṹ��  �����շ���Ϣ��socket

	std::vector<std::string> vvv;
};

