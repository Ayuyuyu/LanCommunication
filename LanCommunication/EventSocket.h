#pragma once
#include "base_sock.h"

/************************************************************************/
/* 
FD_READ网络事件在出现下列情况之一就被触发：
1、输入缓冲区有数据可以读入；
2、连接已经关闭，重设或中止；
3、若已调用了listen，而且accept函数成功接收一个连接。 

FD_WRITE网络事件在出现下列情况之一就被触发：
1、有数据可以发出；
2、如果已完成了对一个非锁定连接调用的处理连接就会成功                  




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
	bool      m_bSelect;				//Select标识符
	bool      m_bEventStop;			//事件停止标识符
	HANDLE    m_hThread;			//线程句柄

	SOCKET s_accept;					//accept保存的
	//DWORD Event;						//当前受信事件
	DWORD EventTotal;				//事件总数
	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	LPSOCKET_INFORMATION SocketArray[WSA_MAXIMUM_WAIT_EVENTS];			//较为重要的结构体  保存收发信息与socket

	std::vector<std::string> vvv;
};

