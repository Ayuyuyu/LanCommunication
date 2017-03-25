/* 编写：王岁明  
	时间：2010-12-15
	功能：winsock封装类
*/
#pragma once
#include <string>
#include <winsock2.h>
#include "base_define.h"

#pragma comment(lib,"Ws2_32.lib")
using namespace std;



class base_sock
{
public:
	base_sock(void);
public:
	virtual ~base_sock(void);
public:
	void Attach( SOCKET sock );
	bool Create( int nType = SOCK_STREAM, int nProtocol = IPPROTO_TCP, unsigned short uPort = 0 );
	bool Listen( int nBlock = 5 );
	bool Connect( SOCKADDR_IN * pAddr );
	bool Bind( SOCKADDR_IN * pAddr );
	bool ShutDown();
	void Close( bool bForce = false );
	SOCKET GetHandle();
	void SetHandle( SOCKET sock );
//	bool SelectEvent( int nEvent = FD_READ | FD_WRITE | FD_CONNECT | FD_OOB | FD_CLOSE | FD_ACCEPT );
	int  Send( const void * pBuf, int nLen );
	int  Recv( void * pBuf, int nLen );
	int  SendTo( const void * pBuf, int nLen, const char * lpszIp, unsigned short uPort );
	int  SendTo( const void * pBuf, int nLen, SOCKADDR_IN * pAddr );
	int  RecvFrom( void * pBuf, int nLen, char Ip[16] = 0, unsigned short * uPort = 0 );
	int  RecvFrom( void * pBuf, int nLen, SOCKADDR_IN * pAddr );
	bool Accept(SOCKADDR_IN * pAddr);
	


	bool   SetBroadcast( bool bBroadcast );
	bool   SetNoblock( bool bBlock )		     	{ return _SetNoblock( GetHandle(), bBlock );				}
	bool   SetRecvBufferSize( int nSize )	{ return _SetRecvBufferSize( GetHandle(), nSize );	}
	bool   SetSendBufferSize( int nSize )	{ return _SetSendBufferSize( GetHandle(), nSize );	}
	bool   SetRecvTimeOut( int nTime ){ return _SetRecvTimeOut( GetHandle(), nTime ); }
	bool   SetSendTimeOut( int nTime ){ return _SetSendTimeOut( GetHandle(), nTime ); }
	bool	  SetReuseAddr(bool bReuse) {	return _SetReuseAddr(GetHandle(),bReuse) ;}

	unsigned short GetLocalPort();
	unsigned int   GetLocalIpn();
	string         GetLocalIps();
	unsigned short GetRemotePort();
	unsigned int   GetRemoteIpn();
	string         GetRemoteIps();
	int            GetErrno();
	string       GetError();

	static bool    _SetRecvBufferSize( SOCKET sock, int nSize );
	static bool    _SetSendBufferSize( SOCKET sock, int nSize );
	static bool    _SetNoblock( SOCKET sock, bool b );
	static bool    _SetReuseAddr( SOCKET sock, bool b );
	static bool    _SetRecvTimeOut( SOCKET sock, int nTime );
	static bool    _SetSendTimeOut( SOCKET sock, int nTime );


	static int     _GetHostName( char * buf, int len );
	static hostent*_GetHostByName( const char * name );

	//static void    SelectThread( void * arg );
	//void           SelectFunc(){ };
	void           StopSelect( bool bForce = false );

// 	virtual  void  OnReceive( int nErrorCode,SOCKET s_index) = 0;
// 	virtual  void  OnSend( int nErrorCode ) =0;
// 	virtual  void  OnAccept( int nErrorCode ,SOCKET S){}
// 	virtual  void  OnClose( int nErrorCode ){}
// 	virtual  void  OnConnect( int nErrorCode ){MessageBox(NULL,L"connect",NULL,NULL);}
// 	virtual  void  OnOutOfBandData( int nErrorCode ){}

private:
// 	bool      m_bSelect;
// 	bool      m_bEventStop;

	SOCKET    m_hSocket;
// 	WSAEVENT  m_hEvent; //
// 	HANDLE    m_hThread;


};
