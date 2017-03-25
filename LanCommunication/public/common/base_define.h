#pragma once
#if defined(WIN32)
#include <process.h>
typedef unsigned( __stdcall *PTHREAD_START) (void*);

#define chBEGINTHREADEX( psa, cbStack, pfnStartAddr,\
						pvParam, fdwCreate, pdwThreadID) \
						( (HANDLE) _beginthreadex(       \
						(void*) (psa),           \
						(unsigned) (cbStack),    \
						(PTHREAD_START) (pfnStartAddr),\
						(void*) (pvParam),       \
						(unsigned) (fdwCreate),  \
						(unsigned*) (pdwThreadID)) )
#else
#endif

#define DATA_BUFSIZE 13697//1409

typedef struct _SOCKET_INFORMATION {
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	DWORD BytesSEND;
	DWORD BytesRECV;
} SOCKET_INFORMATION, * LPSOCKET_INFORMATION;


