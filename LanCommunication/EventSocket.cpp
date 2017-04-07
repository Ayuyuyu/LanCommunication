#include "StdAfx.h"
#include "EventSocket.h"
#include "LanDefine.h"
#include "LanGetLocalSystemInfo.h"
#include "base64.h"
//

CEventSocket::CEventSocket(void)
{
	m_bSelect    = false;
	m_hThread    = NULL;
	m_bEventStop = false;
	EventTotal = 0;
	recv_filesize = 0;
	filesize = 0;
}


CEventSocket::~CEventSocket(void)
{
}

void CEventSocket::SelectThread( void * arg )
{
	CEventSocket * pSock = (CEventSocket*)arg;
	if( pSock == NULL )
	{
		return;
	}
	pSock->SelectFunc();
}

bool CEventSocket::SelectEvent( int nEvent /*= FD_READ | FD_WRITE | FD_CONNECT | FD_OOB | FD_CLOSE | FD_ACCEPT*/ )
{
	if( m_bSelect )
	{
		return false;
	}
	if( GetHandle() == INVALID_SOCKET )
	{
		return false;
	}
	// EventTotal 自增
	CreateSocketInformation(GetHandle());
	if(EventArray[EventTotal - 1] == NULL)
	{
		return false;
	}
	if (::WSAEventSelect(GetHandle(),EventArray[EventTotal - 1],nEvent) == SOCKET_ERROR)
	{
		StopSelect();
		return false;
	}
	m_bSelect = true;

	DWORD dwThreadId;
	m_hThread = chBEGINTHREADEX( NULL, 0, SelectThread, this, 0, &dwThreadId );
	if( m_hThread  == NULL )
	{
		StopSelect();
		return false;
	}
	m_bSelect = true;
	return true;
}

void  CEventSocket::SelectFunc()
{
	if( m_bSelect == false || EventArray == NULL || GetHandle() == INVALID_SOCKET )
	{
		return;
	}
	while( !m_bEventStop )
	{
		DWORD Event = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE,WSA_INFINITE, FALSE);
		if( Event == WSA_WAIT_FAILED || Event == WSA_WAIT_TIMEOUT )
		{
			continue;
		}
		else
		{
			WSANETWORKEVENTS event;
			if(WSAEnumNetworkEvents(SocketArray[Event - WSA_WAIT_EVENT_0]->Socket, EventArray[Event - WSA_WAIT_EVENT_0], &event) == SOCKET_ERROR)
			{
				printf("WSAEnumNetworkEvents failed with error %d\n", WSAGetLastError());
				return;
			}

			if( event.lNetworkEvents & FD_ACCEPT )
			{
				OnAccept( event.iErrorCode[FD_ACCEPT_BIT],Event);
			}
			else if( event.lNetworkEvents & FD_READ )
			{
				OnReceive(event.iErrorCode[FD_READ_BIT],Event,SocketArray[Event - WSA_WAIT_EVENT_0]->Socket);		
			}
			else if( event.lNetworkEvents & FD_CLOSE )
			{
				OnClose( event.iErrorCode[FD_CLOSE_BIT] ,Event);				
			}
			else if( event.lNetworkEvents & FD_CONNECT )
			{
				OnConnect( event.iErrorCode[FD_CONNECT_BIT],Event );		
			}
			else if( event.lNetworkEvents & FD_WRITE )
			{
				OnSend( event.iErrorCode[FD_WRITE_BIT] ,Event);
			}
			else if( event.lNetworkEvents & FD_OOB )
			{
				OnOutOfBandData( event.iErrorCode[FD_OOB_BIT],Event );
			}
			else{}
		}
	}
}

void CEventSocket::StopSelect( bool bForce )
{
	if( m_bSelect )
	{
		m_bEventStop = true;
		//		Sleep( 1000 );
		if( EventArray != NULL )
		{
			::WSACloseEvent( EventArray );
			//EventArray = NULL;
		}
		if( m_hThread != NULL )
		{
			if( bForce )
				TerminateThread( m_hThread, -3 );
			CloseHandle( m_hThread );
			m_hThread = NULL;
		}
		m_bSelect = false;
	}
}

bool CEventSocket::CreateSocketInformation(SOCKET s)
{
	LPSOCKET_INFORMATION SI;

	if ((EventArray[EventTotal] = WSACreateEvent()) == WSA_INVALID_EVENT)
	{
		printf("WSACreateEvent() failed with error %d\n", WSAGetLastError());
		return false;
	}

	if ((SI = (LPSOCKET_INFORMATION) GlobalAlloc(GPTR,
		sizeof(SOCKET_INFORMATION))) == NULL)
	{
		printf("GlobalAlloc() failed with error %d\n", GetLastError());
		return false;
	}

	// Prepare SocketInfo structure for use.

	SI->Socket = s;
	SI->BytesSEND = 0;
	SI->BytesRECV = 0;

	SocketArray[EventTotal] = SI;

	EventTotal++;

	return(true);
}

void CEventSocket::FreeSocketInformation(DWORD Event)
{
	LPSOCKET_INFORMATION SI = SocketArray[Event];
	DWORD i;

	closesocket(SI->Socket);

	GlobalFree(SI);

	WSACloseEvent(EventArray[Event]);

	// Squash the socket and event arrays
	for (i = Event; i < EventTotal; i++)
	{
		EventArray[i] = EventArray[i + 1];
		SocketArray[i] = SocketArray[i + 1];
	}

	EventTotal--;
}
int CEventSocket::OnFileRecv(int nErrorCode,DWORD Event)
{
	int int_ret = 0;
	LPSOCKET_INFORMATION SocketInfo = SocketArray[Event - WSA_WAIT_EVENT_0];
	DWORD Flags = 0;
	DWORD RecvBytes=0;
	FILE* fp;


	SocketInfo->DataBuf.buf = SocketInfo->Buffer;
	SocketInfo->DataBuf.len = DATA_BUFSIZE;
	if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes,&Flags, NULL, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("WSARecv() failed with error %d\n", WSAGetLastError());
			FreeSocketInformation(Event - WSA_WAIT_EVENT_0);
			return 0;
		}
	}
	else
	{
		if (strstr(SocketInfo->DataBuf.buf,TCP_MSG_SENDING_FILE))
		{
			CString str(SocketInfo->DataBuf.buf);
			AfxMessageBox(str);
			
		}
		else
		{
			int_ret = 1;
		}
		memset(SocketInfo->Buffer,0,DATA_BUFSIZE);
	}
	
	return int_ret;
}

//处理所有MSG收发
void CEventSocket::OnReceive( int nErrorCode,DWORD Event,SOCKET socket )
{
	LPSOCKET_INFORMATION SocketInfo = SocketArray[Event - WSA_WAIT_EVENT_0];
	DWORD Flags = 0;
	DWORD RecvBytes=0;


	SocketInfo->DataBuf.buf = SocketInfo->Buffer;
	SocketInfo->DataBuf.len = DATA_BUFSIZE;
	SOCKADDR_IN  pAddr;
	int nAddrLen  = sizeof(pAddr);

	if(WSARecvFrom(SocketInfo->Socket, &(SocketInfo->DataBuf),1,&RecvBytes,&Flags, (SOCKADDR *) &pAddr,&nAddrLen,NULL,NULL))
	//	if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes,&Flags, NULL, NULL) == SOCKET_ERROR)
 	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			printf("WSARecv() failed with error %d\n", WSAGetLastError());
			FreeSocketInformation(Event - WSA_WAIT_EVENT_0);
			return;
		}
	}
	else  
	{
		SocketInfo->BytesRECV = 0;
		
		//接收：UDP 新增上线
		if (strstr(SocketInfo->DataBuf.buf,UDP_MSG_ADD_USER))
		{
			udp_hostinfo hs;
			memset(&hs,0,sizeof(udp_hostinfo));
			char* recvbuf = SocketInfo->DataBuf.buf;
			Json::Reader Jread;
			Json::Value Jmsg;
			 if(!Jread.parse(recvbuf,Jmsg))
			 {
				 throw __T("json解析错误");
				 AfxMessageBox(__T("json"));
			 }
			strcpy(hs.szComputerName,Jmsg["computername"].asString().c_str());
			strcpy(hs.szUserName,Jmsg["username"].asString().c_str());
			//sscanf(recvbuf+strlen(UDP_MSG_ADD_USER),"%[^#]#%[^#]#",hs.szUserName,hs.szComputerName);
			hs.addr = pAddr;

			func_LanMsgSendToDlg(MSG_UDP_ADD_USER,&hs);
		}
			
		//接收：UDP 消息
		else if (strstr(SocketInfo->DataBuf.buf,UDP_MSG_SEND))
		{
			//char szRecvMsg[MAX_PATH*10] = {0};
			char* recvbuf = SocketInfo->DataBuf.buf;
			Json::Reader Jread;
			Json::Value Jmsg;
			Json::FastWriter write;
			if(!Jread.parse(recvbuf,Jmsg))
			{
				throw __T("json解析错误");
				AfxMessageBox(__T("json"));
			}
			//判断是不是本IP发出
			CLanGetLocalSystemInfo m_local;
			CString IP = m_local.getHostIP();

			std::string strip = inet_ntoa(pAddr.sin_addr);
			Jmsg["ip"] = strip;
			if(IP.CompareNoCase(CA2W(strip.c_str())) != 0)
			{
				std::string ipmsg= write.write(Jmsg);
				//sscanf(recvbuf+strlen(UDP_MSG_SEND),"%[^#]#",szRecvMsg);
				func_LanMsgSendToDlg(MSG_UDP_SEND_MSG,ipmsg.c_str());
			}

		}
		// 接收：UDP 消息 带文件信息发送
		else if(strstr(SocketInfo->DataBuf.buf,UDP_MSG_FILEINFO_SEND))
		{
			char* recvbuf = SocketInfo->DataBuf.buf;
			Json::Reader Jread;
			Json::Value Jmsg;
			Json::FastWriter write;
			if(!Jread.parse(recvbuf,Jmsg))
			{
				throw __T("json解析错误");
				AfxMessageBox(__T("json"));
			}
			//判断是不是本IP发出
			CLanGetLocalSystemInfo m_local;
			CString IP = m_local.getHostIP();
			std::string strip = inet_ntoa(pAddr.sin_addr);
			Jmsg["ip"] = strip;
			if(IP.CompareNoCase(CA2W(strip.c_str())) != 0)
			{
				std::string ipmsg= write.write(Jmsg);
				//	sscanf(recvbuf+strlen(UDP_MSG_FILEINFO_SEND),"%[^#]#%[^#]",szFileMsg,szRecvMsg);
				func_LanMsgSendToDlg(MSG_UDP_FILEINFO_SEND,ipmsg.c_str());  //这个自己转
			}
		}
		//接收：UDP 退出
		else if (strstr(SocketInfo->DataBuf.buf,UDP_MSG_CLOSE))
		{
			func_LanMsgSendToDlg(MSG_UDP_CLOSE,inet_ntoa(pAddr.sin_addr));
			//func_LanMsgSendToDlg(MSG_UDP_SEND_MSG,SocketInfo->DataBuf.buf);
		}
		//：TCP 准备接收文件  
		//由文件接收端发来，告诉文件发送端可以发送文件
		else if (strstr(SocketInfo->DataBuf.buf,TCP_MSG_READY_RECV_FILE))
		{
			//准备接受
			CString str(SocketInfo->DataBuf.buf);
			AfxMessageBox(str);
			Json::Reader Jread;
			Json::Value Jmsg;
			if(!Jread.parse(SocketInfo->DataBuf.buf,Jmsg))
			{
				throw __T("json解析错误");
				AfxMessageBox(__T("json"));
			}
			func_LanMsgSendToDlg(MSG_TCP_FILE_SEND,Jmsg["ip"].asString().c_str());

		}
		//文件接收端用来接收文件信息的
		//s_accept OnAccept得到的，给予文件发送端 收到反馈
		else if(strstr(SocketInfo->DataBuf.buf,TCP_MSG_FILE_SEND_REQUEST))
		{
			//Send("test",4);
			Json::Reader Jread;
			Json::Value Jmsg;
			if(!Jread.parse(SocketInfo->DataBuf.buf,Jmsg))
			{
				throw __T("json解析错误");
				AfxMessageBox(__T("json"));
			}
			filesize =Jmsg["filesize"].asInt64();
			//recv_filesize = 0;
			func_LanMsgSendToDlg(MSG_TCP_FILEINFO_RECV,SocketInfo->DataBuf.buf);

			if (socket != NULL)
			{
				::send(socket,"test",4,0);
			}

		}
		//文件接收端用来接收文件信息的
		else// if(strstr(SocketInfo->DataBuf.buf,TCP_MSG_SENDING_FILE))
		{
			//接收到  给反馈
			//FILE* fp = fopen("recv.txt","a+");
			//fwrite(SocketInfo->DataBuf.buf,1,strlen(SocketInfo->DataBuf.buf),fp);
			//fclose(fp);
			Json::Reader Jread;
			Json::Value Jmsg;
			string tim = SocketInfo->DataBuf.buf;
			if ( !Jread.parse(tim.c_str(),Jmsg) || recvbuf.length() < DATA_BUFSIZE )
			{
				recvbuf+= SocketInfo->DataBuf.buf;
				int j = Jread.parse(recvbuf.c_str(),Jmsg);
				if (Jread.parse(recvbuf.c_str(),Jmsg))
				{
					func_LanMsgSendToDlg(MSG_TCP_FILE_RECV,recvbuf.c_str());
					if (socket != NULL)
					{
						::send(socket,"test",4,0);
					}
					recvbuf.clear();
				}
			}
			//else if(strstr(SocketInfo->DataBuf.buf,"finish"))
			//{	
			//	//std::string strRecvBuf = base64_decode(Jmsg["buf"].asString());
			//	//recv_filesize+=strRecvBuf.length();
			//	func_LanMsgSendToDlg(MSG_TCP_FILE_RECV,SocketInfo->DataBuf.buf);
			//	if (socket != NULL)
			//	{
			//		::send(socket,"test",4,0);
			//	}

			//}
			//else
			//{
			//	func_LanMsgSendToDlg(MSG_TCP_FILE_RECV,SocketInfo->DataBuf.buf);
			//	if (socket != NULL)
			//	{
			//		::send(socket,"test",4,0);
			//	}

			//}
		}
	
		memset(SocketInfo->Buffer,0,DATA_BUFSIZE);
	}
	

}

void CEventSocket::OnSend( int nErrorCode,DWORD Event )
{
	DWORD SendBytes;
	LPSOCKET_INFORMATION SocketInfo = SocketArray[Event - WSA_WAIT_EVENT_0];
	//send(SocketInfo->Socket,"123",3,0);
	if (SocketInfo->BytesRECV > SocketInfo->BytesSEND)
	{
		SocketInfo->DataBuf.buf = SocketInfo->Buffer + SocketInfo->BytesSEND;
		SocketInfo->DataBuf.len = SocketInfo->BytesRECV - SocketInfo->BytesSEND;

		if (WSASend(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &SendBytes, 0,
			NULL, NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				printf("WSASend() failed with error %d\n", WSAGetLastError());
				//FreeSocketInformation(Event - WSA_WAIT_EVENT_0);
				return;
			}

			// A WSAEWOULDBLOCK error has occured. An FD_WRITE event will be posted
			// when more buffer space becomes available
		}
		else
		{
			SocketInfo->BytesSEND += SendBytes;

			if (SocketInfo->BytesSEND == SocketInfo->BytesRECV)
			{
				SocketInfo->BytesSEND = 0;
				SocketInfo->BytesRECV = 0;
			}
		}
	}
	//MessageBox(NULL,L"OnSend",NULL,NULL);
}

//若是UDP好像不会对这个触发
void CEventSocket::OnAccept(int nErrorCode,DWORD Event)
{
	LPSOCKET_INFORMATION SocketInfo = SocketArray[Event - WSA_WAIT_EVENT_0];
	//MessageBox(NULL,L"accept",NULL,NULL);

	if ((s_accept = accept(SocketInfo->Socket, NULL, NULL)) == INVALID_SOCKET)
	{		
		printf("accept() failed with error %d\n", WSAGetLastError());
		return;
	}

	if (EventTotal > WSA_MAXIMUM_WAIT_EVENTS)
	{
		printf("Too many connections - closing socket.\n");
		closesocket(s_accept);
		return;
	}

	CreateSocketInformation(s_accept);

	if (WSAEventSelect(s_accept, EventArray[EventTotal - 1], FD_READ|FD_WRITE|FD_CLOSE) == SOCKET_ERROR)
	{
		printf("WSAEventSelect() failed with error %d\n", WSAGetLastError());
		return;
	}
	//Send("testsend",8);
}

//若是UDP好像不会对这个触发
void CEventSocket::OnClose( int nErrorCode,DWORD Event )
{
	//LPSOCKET_INFORMATION SocketInfo = SocketArray[Event - WSA_WAIT_EVENT_0];

	FreeSocketInformation(Event - WSA_WAIT_EVENT_0);
}

void CEventSocket::OnConnect( int nErrorCode,DWORD Event )
{
	MessageBox(NULL,L"OnConnect",NULL,NULL);

}

void CEventSocket::OnOutOfBandData( int nErrorCode ,DWORD Event)
{

}

/*
void CEventSocket::Send(CHAR* buffer,ULONG buflen)
{
	DWORD SendBytes;
	for (int i_evt = 1; i_evt < EventTotal; i_evt ++)
	{
		LPSOCKET_INFORMATION SocketInfo = SocketArray[i_evt - WSA_WAIT_EVENT_0];
		SocketInfo->DataBuf.buf = buffer ;
		SocketInfo->DataBuf.len = buflen;
		if (WSASend(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &SendBytes, 0,NULL, NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				printf("WSASend() failed with error %d\n", WSAGetLastError());
			//	FreeSocketInformation(i_evt - WSA_WAIT_EVENT_0);
				return;
			}
		}
	}
		// A WSAEWOULDBLOCK error has occured. An FD_WRITE event will be posted
		// when more buffer space becomes available
	
}
*/