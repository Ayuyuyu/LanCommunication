/************************************************************************/
/* LanSock.cpp
	packing tcp and udp socket
	by yuc
	2017-03-06
*/
/************************************************************************/

#include "StdAfx.h"
#include "LanSock.h"
#include "CommonFunc.h"
#include "LanGetLocalSystemInfo.h"
#include "LanDefine.h"
#include "base64.h"

CLanSock *CLanSock::pInstance = nullptr;
CLanSock::CLanSock(void)
{
	LanSockInit();
}


CLanSock::~CLanSock(void)
{
	if (pInstance != nullptr)
	{
		pInstance->LanSockClose();
		delete pInstance;
		pInstance = nullptr;
	}
}


/************************************************************************/
/*  socket tcp - client
	1.create socket
	2.connect
	3.send/recv
	4.close socket

	socket tcp-server
	1.create socket
	2.bind
	3.listen
	4.accept
	5.send/recv
	6.wait clent
	7.close socket
*/
/************************************************************************/
long CLanSock::LanSockInit()
{
	long long_ret = 0;
	//初始化
	if (!s_tcp.Create(SOCK_STREAM,IPPROTO_TCP) || !s_udp.Create(SOCK_DGRAM,IPPROTO_UDP))
	{
		//std::string tmp = s_tcp.GetError();
		long_ret = -1;
		return long_ret;
	}


	//设置非阻塞
	//if (!s_tcp.SetNoblock(true) || !s_udp.SetNoblock(true))
	if( !s_udp.SetNoblock(true))
	{
		long_ret = -3;
		return long_ret;
	}

	//UDP 设置:广播模式
	if (!s_udp.SetBroadcast(true) || !s_udp.SetSendBufferSize(MAX_SOCKBUF) || !s_tcp.SetRecvBufferSize(MAX_SOCKBUF) )
	{
		long_ret = -4;
		return long_ret;
	}

	//int i =s_tcp.Connect(&addr);
	//绑定
	SOCKADDR_IN addr;
	LanSockSetADDR(addr);
	if(!s_tcp.Bind(&addr) || !s_udp.Bind(&addr))
		//if(!s_udp.Bind(&addr))
	{
		std::string tmp = s_tcp.GetError();
		long_ret = -2;
		return long_ret;
	}
	if (!s_tcp.Listen(5))
	{
		long_ret = -5;
		return long_ret;
	}
	s_tcp.func_LanMsgSendToDlg = LanMsgSendToDlg;
	s_udp.func_LanMsgSendToDlg = LanMsgSendToDlg;
	return long_ret;
}

long CLanSock::LanSockClose()
{
	long long_ret = 0;
	s_tcp.Close();
	s_udp.Close();

	return long_ret;
}

void CLanSock::LanSockSetADDR(SOCKADDR_IN& addr)
{
	CLanGetLocalSystemInfo m_local;
	CString IP = m_local.getHostIP();
	memset(&addr, 0, sizeof(addr));
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= inet_addr(CW2A(IP.GetBuffer()));
	//addr.sin_addr.s_addr = inet_addr("172.31.50.65");
	addr.sin_port = htons(Listen_Port);

}


//UDP send
long CLanSock::LanSockSend(void * pBuf, unsigned int nLen)
{
	long long_ret = 0;
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= INADDR_BROADCAST ;//inet_addr("255.255.255.255");
	addr.sin_port			=htons(Listen_Port);			//加上htons 很重要
	//UDP ; 普通数据发送，使用UDP
	char* buf  = (char*)pBuf;
	if (s_udp.SendTo(pBuf,nLen,&addr)== SOCKET_ERROR)
	{
		std::string tmp = s_udp.GetError();
		long_ret = -1;
		return long_ret;
	}
	//s_tcp.Send("tcp",3);

	return long_ret;
}

//UDP recv
long CLanSock::LanSockRecv(void * pBuf, unsigned int nLen)
{
	long long_ret = 0;
	SOCKADDR_IN addr;
	LanSockSetADDR(addr);
	//UDP ; 普通数据发送，使用UDP
	if (s_udp.RecvFrom(pBuf,nLen,&addr) == SOCKET_ERROR)
	{
		long_ret = -1;
		return long_ret;
	}

	return long_ret;
}

//Select 选择event
long CLanSock::LanSockSelect()
{
	s_tcp.SelectEvent(FD_ACCEPT|FD_READ|FD_CLOSE|FD_WRITE);
	s_udp.SelectEvent(FD_READ);
	return 1;
}

long CLanSock::LanSockGetHandle(HANDLE handleMain,HANDLE handleRecv)
{
	long long_ret = 0;

	hMainhandle = handleMain;
	hRecvhandle = handleRecv;
	return long_ret;
}


//
/************************************************************************/
/*UDP上线消息发送
	消息格式
	{
	"type":"UDP_MSG_ADD_USER",
	"computername":"WINDOWS-123",
	"username":"ADMINITORTER"
	}
*/
/************************************************************************/
long CLanSock::LanSockUDP_MSG_ADD_USER()
{
	long long_ret = 0;
	CLanGetLocalSystemInfo mlocal;
	 std::string strComputerName = CW2A(mlocal.getComputername());
	 std::string  strUsername = CW2A(mlocal.getUsername());

	Json::FastWriter writer;
	Json::Value Jsend_msg;
	Jsend_msg["type"] = UDP_MSG_ADD_USER;
	Jsend_msg["computername"] = strComputerName;
	Jsend_msg["username"] = strUsername;

	std::string strSendtmp = writer.write(Jsend_msg);
	if (strSendtmp.length()> 0)
	{
		if(CLanSock::GetInstance()->LanSockSend((void*)strSendtmp.c_str(),strSendtmp.length()))
		{
			long_ret = -1;
			return long_ret;
		}
	}
	return long_ret;
}

/************************************************************************/
/*按键消息。发送 UDP信息
/消息示例：
	{
	"type":"UDP_MSG_NORMAL",
	"msg":"示例消息"
	}
	"UDP_MSG_SEND
*/
/************************************************************************/
long CLanSock::LanSockUDP_MSG_SEND(void*buf,int buf_len)
{
	long long_ret = 0;
	Json::FastWriter writer;
	Json::Value Jsend_msg;
	Jsend_msg["type"] = UDP_MSG_SEND;
	Jsend_msg["msg"] = (char*)buf;

	std::string strSendtmp = writer.write(Jsend_msg);
	if (strSendtmp.length()> 0)
	{
		if(CLanSock::GetInstance()->LanSockSend((void*)strSendtmp.c_str(),strSendtmp.length()))
		{
			long_ret = -1;
			return long_ret;
		}
	}
	return long_ret;
}

/************************************************************************/
/*按键消息。发送 UDP信息 退出消息
/消息示例：
{
"type":"UDP_MSG_CLOSE",
}
*/
/************************************************************************/
long CLanSock::LanSockUDP_MSG_CLOSE()
{
	long long_ret = 0;
// 	CLanGetLocalSystemInfo mlocal;
// 	CStringA strIP = CW2A(mlocal.getHostIP());
	Json::FastWriter writer;
	Json::Value Jsend_msg;
	Jsend_msg["type"] = UDP_MSG_CLOSE;


	std::string strSendtmp = writer.write(Jsend_msg);
	if (strSendtmp.length()> 0)
	{
		if(CLanSock::GetInstance()->LanSockSend((void*)strSendtmp.c_str(),strSendtmp.length()))
		{
			long_ret = -1;
			return long_ret;
		}
	}
	return long_ret;
}

/************************************************************************/
/* 发送 UDP 文件信息 + 普通消息
/消息示例：
{
"type":"UDP_MSG_FILEINFO_SEND",
"msg":"示例消息"
"filepaths":["path1","path2"]
}
*/
/************************************************************************/
long CLanSock::LanSockUDP_MSG_FILEINFO_SEND(std::vector<std::string> vec_file,void*buf)
{
	long long_ret = 0;
	std::string strSendFilePath,strSendFileName;
	//strSendtmp.Format("%s%s#",UDP_MSG_FILEINFO_SEND,buf);
	Json::FastWriter writer;
	Json::Value Jsend_msg,Jfile_path,Jfile_name;;
	Jsend_msg["type"] = UDP_MSG_FILEINFO_SEND;
	Jsend_msg["msg"] = (char*)buf;
	for (int i = 0; i < vec_file.size() ; i++)
	{
		Jfile_path.append(vec_file[i].c_str());
		//strSendFilePath.append(vec_file[i].c_str());
		//strSendFilePath.append(",");
		//strSendFileName.append(PathFindFileNameA(vec_file[i].c_str()));
		//strSendFileName.append(",");
		//Jfile_name()
	}
	Jsend_msg["filepaths"] = Jfile_path;
	//Jsend_msg["filenames"] = str
	std::string strSendtmp = writer.write(Jsend_msg);
	if (strSendtmp.length()> 0)
	{
		if(CLanSock::GetInstance()->LanSockSend((void*)strSendtmp.c_str(),strSendtmp.length()))
		{
			long_ret = -1;
			return long_ret;
		}
	}
	return long_ret;

}


/************************************************************************/
/* 发送 TCP 准备接受文件信息   
RecvDLG 点击接收  
/消息示例：
{
"type":"UDP_MSG_READY_RECV_FILE",
"ip":
"filepaths":["path1","path2"]
}
*/
/************************************************************************/
long CLanSock::LanSockTCP_MSG_READY_RECV_FILE(std::vector<std::string> vec_file,SOCKADDR_IN addr)
{
	long long_ret = 0;
	CEventSocket c_tcp;
	//初始化
	if (!c_tcp.Create(SOCK_STREAM,IPPROTO_TCP))
	{
		//std::string tmp = s_tcp.GetError();
		long_ret = -1;
		return long_ret;
	}
	if(!c_tcp.Connect(&addr))
	{
		long_ret = -2;
		return long_ret;
	}
	Json::FastWriter writer;
	Json::Value Jsend_msg,Jfile_path,Jfile_name;;
	CLanGetLocalSystemInfo m_local;
	CString IP = m_local.getHostIP();
	string strip = CW2A(IP);
	Jsend_msg["type"] = TCP_MSG_READY_RECV_FILE;
	Jsend_msg["ip"] = strip;
	for (int i = 0; i < vec_file.size() ; i++)
	{
		Jfile_path.append(vec_file[i].c_str());
	}
	Jsend_msg["filepaths"] = Jfile_path;
	std::string strSendtmp = writer.write(Jsend_msg);
	if (strSendtmp.length()> 0)
	{
		if(c_tcp.Send((void*)strSendtmp.c_str(),strSendtmp.length()))
		{
			long_ret = -1;
			return long_ret;
		}
	}
	c_tcp.Close(true);

	return long_ret;
}



//client 端发送
/************************************************************************/
/* 
	file_name 为发送文件地址
	addr 为接收端信息
*/
/************************************************************************/
long CLanSock::LanSockTCP_MSG_FILE_SEND(const char*file_name,SOCKADDR_IN addr)
{
	long long_ret = 0;
	//初始化
	CEventSocket c_tcp;
	if (!c_tcp.Create(SOCK_STREAM,IPPROTO_TCP))
	{
		//std::string tmp = s_tcp.GetError();
		long_ret = -1;
		return long_ret;
	}
	if(!c_tcp.Connect(&addr))
	{
		long_ret = -2;
		return long_ret;
	}
	if (!c_tcp.SetRecvBufferSize(MAX_SOCKBUF) )
	{
		long_ret = -3;
		return long_ret;
	}
	Json::Value JValue;
	Json::FastWriter write;
	// 1、 打开要发送的文件
	FILE* fp = fopen(file_name, "rb");
	fseek (fp, 0, SEEK_END);
	INT64 file_len = ftell(fp);
	fseek(fp,0,SEEK_SET);
	INT64 nSum = 0;		//总发送大小
	OutputDebugPrintf(_T("正在发送文件......\n"));
	//2、先发送文件信息
	JValue["filename"] = PathFindFileNameA(file_name);
	JValue["bufsize"] =  10240;			//INT64  当前发送的数据块
	JValue["filesize"] = file_len;			//INT64	文件总大小
	JValue["type"] = TCP_MSG_FILE_SEND_REQUEST;
	string sendbuf = write.write(JValue);
	c_tcp.Send(sendbuf.c_str(), sendbuf.length());
	//AfxMessageBox(__T("request"));
	//3、循环开始发送
	char buf[1024] = {0};
	//c_tcp.Recv(buf,1024) != SOCKET_ERROR
	while (c_tcp.Recv(buf,1024) != SOCKET_ERROR)
	{
		//AfxMessageBox(CString(buf));
		unsigned char buf[10240] = {0};
		// 读取文件内容到buf中，每次读1024字节，返回值表示实际读取的字节数
		int nCount = fread(buf, 1,10240, fp);
		nSum += nCount;
		// 如果读取的字节数不大于0，说明读取出错或文件已经读取完毕
		if (nCount <= 0)
		{
			JValue.clear();
			JValue["type"] = TCP_MSG_SENDING_FILE;
			JValue["buf"] = "finish";
			sendbuf = write.write(JValue);
			c_tcp.Send(sendbuf.c_str(), sendbuf.length());
			OutputDebugPrintf(_T("文件发送完成......\n"));
			break;
		}
		string strbase64en = base64_encode(buf,nCount);
		INT64	bufsize = strbase64en.length();
		//清理重新发，详细buf
		JValue.clear();
		JValue["type"] = TCP_MSG_SENDING_FILE;
		JValue["buf"] = strbase64en;
		sendbuf = write.write(JValue);
		c_tcp.Send(sendbuf.c_str(), sendbuf.length());
		int ss = sendbuf.length();
		OutputDebugPrintf(_T("文件发送 %d.\n"),ss);
		Sleep(20);	
	}
	OutputDebugPrintf(_T("总发送大小（字节）：%d\n"), nSum);

	if (fp)
	{
		fclose(fp);
	}
	c_tcp.Close(true);
	return long_ret;
}

long CLanSock::LanMsgSendToDlg(const int Msg_,const void* Buf)
{
	long long_ret = 0;
	if (Msg_ < 0 || Buf == NULL)
	{
		long_ret = -1;
		return long_ret;
	}
	switch(Msg_)
	{
	case MSG_UDP_ADD_USER:
		{

			SendMessage((HWND)GetInstance()->hMainhandle,WM_MSG_UDP_ADD_USER,(WPARAM)(udp_hostinfo*)Buf,NULL);
			break;
		}
	case MSG_UDP_SEND_MSG:
		{
			SendMessage((HWND)GetInstance()->hMainhandle,WM_MSG_UDP_SEND_MSG,(WPARAM)Buf,NULL);
			break;
		}
	case MSG_UDP_CLOSE:
		{
			SendMessage((HWND)GetInstance()->hMainhandle,WM_MSG_UDP_CLOSE,(WPARAM)Buf,NULL);
			break;
		}
	case MSG_UDP_FILEINFO_SEND:
		{
			SendMessage((HWND)GetInstance()->hMainhandle,WM_MSG_UDP_FILEINFO_SEND,(WPARAM)Buf,NULL);
			break;
		}
	case MSG_TCP_FILE_SEND:
		{
			SendMessage((HWND)GetInstance()->hMainhandle,WM_MSG_TCP_FILE_SEND,(WPARAM)Buf,NULL);
			break;
		}
	case MSG_TCP_FILEINFO_RECV:
		{
			SendMessage((HWND)GetInstance()->hRecvhandle,WM_MSG_TCP_FILEINFO_RECV,(WPARAM)Buf,NULL);
			break;
		}
	case MSG_TCP_FILE_RECV:
		{
			SendMessage((HWND)GetInstance()->hRecvhandle,WM_MSG_TCP_FILE_RECV,(WPARAM)Buf,NULL);
			break;
		}
	}
	
	return long_ret;
}