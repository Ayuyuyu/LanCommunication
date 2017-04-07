#pragma once
#include "EventSocket.h"
#include <vector>
#include <string>

typedef struct {
	TCHAR szIP[MAX_PATH];
	int port;
	TCHAR szUserName[MAX_PATH*2];
	TCHAR szComputerName[MAX_PATH*2];
}hostinfo;

class CLanSock
{
public:
	//CLanSock(void);
	~CLanSock(void);
	static CLanSock* GetInstance(){
		if (pInstance == nullptr)
		{
			pInstance = new CLanSock();
			return pInstance;
		}
		return pInstance;
	}
private:
	CLanSock(void);
	static CLanSock *pInstance;
public:
	long LanSockInit();
	long LanSockGetHandle(HANDLE handleMain,HANDLE handleRecv);
	long LanSockSelect();
public:
	long LanSockUDP_MSG_ADD_USER();								//程序打开 上线消息
	long LanSockUDP_MSG_SEND(void*buf,int buf_len);		//正常UDP消息发送
	long LanSockUDP_MSG_CLOSE();										//UDP下线消息
	long LanSockUDP_MSG_FILEINFO_SEND(std::vector<std::string> vec_file,void*buf);		  //UDP发送文件信息  ，接受文件路径信息
	long LanSockTCP_MSG_READY_RECV_FILE(std::vector<std::string> vec_file,SOCKADDR_IN addr);	//TCP准备接收文件
	static long LanSockTCP_MSG_FILE_SEND(const char*file_name,SOCKADDR_IN addr);			//TCP 文件发送
	//回调函数
	static long LanMsgSendToDlg(const int Msg_,const void* Buf);

protected:
	long LanSockClose();
	long LanSockSend(void * pBuf, unsigned int nLen);
	long LanSockRecv(void * pBuf, unsigned int nLen);
	void LanSockSetADDR(SOCKADDR_IN& addr);
	//static UINT myAcceptThread(LPVOID  param);

public:
	HANDLE hMainhandle;
	HANDLE hRecvhandle;

protected:
	//SOCKET s_udp;
	//SOCKET s_tcp;
	CEventSocket s_tcp;
	CEventSocket s_udp;
	//CEventSocket c_tcp;

	//base_sock sstcp;
	hostinfo m_hostinfo;

};

