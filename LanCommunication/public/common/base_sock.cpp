//#include "stdafx.h"
#include "base_sock.h"


base_sock::base_sock(void)
{
//	m_bSelect    = false;
	m_hSocket    = INVALID_SOCKET;
// 	m_hEvent     = NULL;
// 	m_hThread    = NULL;
// 	m_bEventStop = false;
// 	EventTotal = 0;
}

base_sock::~base_sock(void)
{
	Close();
}
void base_sock::Attach( SOCKET sock )
{
	m_hSocket = sock;
}
bool base_sock::Create( int nType /*= SOCK_STREAM*/, int nProtocol /*= IPPROTO_TCP*/, unsigned short uPort/* = 0*/ )
{
	Close();
	WORD socketVersion = MAKEWORD(2,2);
	WSADATA wsaData; 
	if(WSAStartup(socketVersion, &wsaData) != 0)
	{
		return false;
	}
	if(LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)  
	{  
		WSACleanup();   //释放为该程序分配的资源，终止对winsock动态库的使用  
		printf("Invalid WinSock version!\n");  
		//return 0;  
	}  
	m_hSocket = ::socket( AF_INET, nType, nProtocol );
	if( m_hSocket == INVALID_SOCKET )
	{
		return false;
	}
// 	if( nProtocol == IPPROTO_UDP )
// 	{
// 		this->Bind();
// 	}
	return true;
}
bool base_sock::Listen( int nBlock /*= 5*/ )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		return false;
	}
	if( ::listen( m_hSocket, nBlock ) == SOCKET_ERROR )
	{
		return false;
	}
	return true;
}
bool base_sock::Connect( SOCKADDR_IN * pAddr )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		return false;
	}
	if( ::connect( m_hSocket, (sockaddr*)pAddr, sizeof( SOCKADDR_IN ) ) == SOCKET_ERROR )			//这里坑人不浅啊。sizeof(pAddr)求的是指针，这里需要给入结构体大小
	{
		if( GetErrno() == WSAEWOULDBLOCK )
		{
			return true;
		}
		return false;
	}
	return true;
}

bool base_sock::Bind( SOCKADDR_IN * pAddr )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		return false;
	}
	if( ::bind( m_hSocket, (sockaddr*)pAddr, sizeof(SOCKADDR_IN) ) == SOCKET_ERROR )
	{
		return false;
	}
	return true;
}
bool base_sock::ShutDown()
{
	if( ::shutdown( m_hSocket, SD_SEND ) == SOCKET_ERROR )
	{
		return false;
	}
	return true;
}
void base_sock::Close( bool bForce )
{
	//StopSelect( bForce );
	if( m_hSocket != INVALID_SOCKET )
	{
//		ShutDown();
		::closesocket( m_hSocket );
		m_hSocket = INVALID_SOCKET;
		WSACleanup();
	}
}


SOCKET base_sock::GetHandle()
{
	return m_hSocket;
}
void base_sock::SetHandle( SOCKET sock )
{
	m_hSocket = sock;
}
/*
bool base_sock::SelectEvent( int nEvent)
{
	if( m_bSelect )
	{
		return false;
	}
	if( m_hSocket == INVALID_SOCKET )
	{
		return false;
	}
	CreateSocketInformation(m_hSocket);
	if(EventArray[EventTotal - 1] == NULL)
	{
		return false;
	}
	//m_hEvent = ::WSACreateEvent();
// 	if( m_hEvent == NULL )
// 	{
// 		return false;
// 	}
//	if( ::WSAEventSelect( m_hSocket ,m_hEvent, nEvent ) == SOCKET_ERROR )
	if (::WSAEventSelect(m_hSocket,EventArray[EventTotal - 1],nEvent) == SOCKET_ERROR)
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
*/
int  base_sock::Send( const void * pBuf, int nLen )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		return -1;
	}
	int nSend = ::send( m_hSocket, (const char*)pBuf, nLen, 0 );
	return nSend;
}
int  base_sock::Recv( void * pBuf, int nLen )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		return -1;
	}
	return(::recv( m_hSocket, (char*)pBuf, nLen, 0 ));
}
int  base_sock::SendTo( const void * pBuf, int nLen, const char * lpszIp, unsigned short uPort )
{
	SOCKADDR_IN addr;
	memset( &addr, 0, sizeof(addr) );
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = ::inet_addr( lpszIp );
	addr.sin_port        = ::htons( uPort );
	return SendTo( pBuf, nLen, &addr );
}
int  base_sock::SendTo( const void * pBuf, int nLen, SOCKADDR_IN * pAddr )
{
	int ret = ::sendto( GetHandle(), (const char *)pBuf, nLen, 0, (SOCKADDR*)pAddr, sizeof(SOCKADDR_IN) );
	return ret;
}
int  base_sock::RecvFrom( void * pBuf, int nLen, char Ip[16]/* = 0*/, unsigned short * uPort/* = 0*/ )
{
	SOCKADDR_IN addr;
	int nRecv = RecvFrom( pBuf, nLen, &addr );

	if( nRecv > 0) 
	{
		if( Ip )
		{
			strcpy( Ip, ::inet_ntoa(addr.sin_addr) );
		}
		if( uPort ) 
		{
			*uPort = ::ntohs( addr.sin_port );
		}
	}
	return nRecv;
}
int  base_sock::RecvFrom( void * pBuf, int nLen, SOCKADDR_IN * pAddr )
{
	int nAddrLen = sizeof((*pAddr));
	int  nRet  = ::recvfrom( GetHandle(), (char*)pBuf, nLen, 0, (SOCKADDR*)pAddr, &nAddrLen );
	return nRet;
}

bool base_sock::SetBroadcast( bool bBroadcast )
{
	if( m_hSocket == INVALID_SOCKET )
	{
		return false;
	}
	int nOpt = ( bBroadcast ? 1 : 0 );
	unsigned int nLen = sizeof( nOpt );
	int nRet = ::setsockopt( GetHandle(), SOL_SOCKET, SO_BROADCAST, (const char *)&nOpt, nLen );
	return ( nRet != SOCKET_ERROR );
}

unsigned short base_sock::GetLocalPort()
{
	sockaddr_in addr;
	int addrlen = sizeof(addr);
	::getsockname( GetHandle(), (sockaddr*)&addr, &addrlen );
	return ::ntohs( addr.sin_port );
}
unsigned int   base_sock::GetLocalIpn()
{
	sockaddr_in addr;
	int addrlen = sizeof(addr);
	::getsockname( GetHandle(), (sockaddr*)&addr, &addrlen );
	return addr.sin_addr.s_addr;
}
string         base_sock::GetLocalIps()
{
	sockaddr_in addr;
	int addrlen = sizeof(addr);
	::getsockname( GetHandle(), (sockaddr*)&addr, &addrlen );
	return string( ::inet_ntoa(addr.sin_addr) );
}
unsigned short base_sock::GetRemotePort()
{
	sockaddr_in addr;
	int addrlen = sizeof(addr);
	::getpeername( GetHandle(), (sockaddr*)&addr, &addrlen );
	return ::ntohs(addr.sin_port);
}
unsigned int   base_sock::GetRemoteIpn()
{
	sockaddr_in addr;
	int addrlen = sizeof(addr);
	::getpeername( GetHandle(), (sockaddr*)&addr, &addrlen );
	return addr.sin_addr.s_addr;
}
string base_sock::GetRemoteIps()
{
	sockaddr_in addr;
	int addrlen = sizeof(addr);
	::getpeername( GetHandle(), (sockaddr*)&addr, &addrlen );
	return string( ::inet_ntoa(addr.sin_addr) );
}
int    base_sock::GetErrno()
{
	return ::WSAGetLastError();
}
string base_sock::GetError()
{
	char msg[512];
	 // lcy note 2008_10_10:
	::FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM, 0, GetErrno(), 0, msg, sizeof(msg), 0 );
	return string( msg );
}

bool base_sock::_SetRecvBufferSize( SOCKET sock, int nSize )
{
	if( sock == INVALID_SOCKET )
	{
		return false;
	}
	int ret = ::setsockopt( sock, SOL_SOCKET, SO_RCVBUF, (const char*)&nSize, sizeof(nSize) );
	return ( ret != SOCKET_ERROR );
}
bool base_sock::_SetSendBufferSize( SOCKET sock, int nSize )
{
	if( sock == INVALID_SOCKET ) 
	{
		return false;
	}
	int ret = ::setsockopt( sock, SOL_SOCKET, SO_SNDBUF, (const char*)&nSize, sizeof(nSize) );
	return ( ret != SOCKET_ERROR );
}
bool base_sock::_SetNoblock( SOCKET sock, bool b )
{
	if( sock == INVALID_SOCKET )
	{
		return false;
	}
	unsigned long opt = ( b ? 1 : 0 );
	int ret = ::ioctlsocket( sock, FIONBIO, &opt );
	return ( ret != SOCKET_ERROR );
}
bool base_sock::_SetReuseAddr( SOCKET sock, bool b )
{
	if( sock == INVALID_SOCKET )
	{
		return false;
	}
	unsigned long opt = ( b ? 1 : 0 );
	int ret = ::ioctlsocket( sock, SO_REUSEADDR, &opt );
	return ( ret != SOCKET_ERROR );
}
bool    base_sock::_SetRecvTimeOut( SOCKET sock, int nTime )
{
	if( sock == INVALID_SOCKET )
	{
		return false;
	}
	int ret = ::setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&nTime, sizeof(nTime) );
	return (ret != SOCKET_ERROR);
}
bool    base_sock::_SetSendTimeOut( SOCKET sock, int nTime )
{
	if( sock == INVALID_SOCKET )
	{
		return false;
	}
	int ret = ::setsockopt( sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&nTime, sizeof(nTime) );
	return (ret != SOCKET_ERROR);
}
int base_sock::_GetHostName( char * buf, int len )
{
	return ::gethostname( buf, len );
}
hostent * base_sock::_GetHostByName( const char * name )
{
	return ::gethostbyname( name );
}

/*
void base_sock::SelectFunc()
{
	//if( m_bSelect == false || m_hEvent == NULL || m_hSocket == INVALID_SOCKET )
	if( m_bSelect == false || m_hSocket == INVALID_SOCKET )
	{
		return;
	}
	while( !m_bEventStop )
	{
		//		printf("loop\n");
		//int nIndex = ::WSAWaitForMultipleEvents( 1, &m_hEvent, TRUE, 1000, false );
		//int nIndex = ::WSAWaitForMultipleEvents( 1, &m_hEvent, FALSE, 1000, FALSE );
		int nIndex = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE,WSA_INFINITE, FALSE);
		if( nIndex == WSA_WAIT_FAILED || nIndex == WSA_WAIT_TIMEOUT )
		{
			continue;
		}
		else
		{
			WSANETWORKEVENTS event;
		//	::WSAEnumNetworkEvents( m_hSocket, m_hEvent, &event );
			if(WSAEnumNetworkEvents(SocketArray[nIndex - WSA_WAIT_EVENT_0]->Socket, EventArray[nIndex - WSA_WAIT_EVENT_0], &event) == SOCKET_ERROR)
			{
				printf("WSAEnumNetworkEvents failed with error %d\n", WSAGetLastError());
				return;
			}

			if( event.lNetworkEvents & FD_ACCEPT )
			{
				OnAccept( event.iErrorCode[FD_ACCEPT_BIT] ,SocketArray[nIndex - WSA_WAIT_EVENT_0]->Socket);
			}
			else if( event.lNetworkEvents & FD_READ )
			{
				OnReceive( event.iErrorCode[FD_READ_BIT] );		
			}
			else if( event.lNetworkEvents & FD_CLOSE )
			{
				OnClose( event.iErrorCode[FD_CLOSE_BIT] );				
			}
			else if( event.lNetworkEvents & FD_CONNECT )
			{
				OnConnect( event.iErrorCode[FD_CONNECT_BIT] );		
			}
			else if( event.lNetworkEvents & FD_WRITE )
			{
				OnSend( event.iErrorCode[FD_WRITE_BIT] );
			}
			else if( event.lNetworkEvents & FD_OOB )
			{
				OnOutOfBandData( event.iErrorCode[FD_OOB_BIT] );
			}
			else{}
		}
	}
}

void base_sock::StopSelect( bool bForce )
{
	if( m_bSelect )
	{
		m_bEventStop = true;
//		Sleep( 1000 );
		if( m_hEvent != NULL )
		{
			::WSACloseEvent( m_hEvent );
			m_hEvent = NULL;
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
*/