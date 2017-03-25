/************************************************************************/
/* LanDefine.h
	by yuc

*/
/************************************************************************/
#pragma once
#include "json.h"

#define  Listen_Port 4423
#define  MAX_SOCKBUF 32*1024  //32K

//
//回掉函数 msg参数
enum
{
	MSG_UDP_ADD_USER = 0,
	MSG_UDP_SEND_MSG,
	MSG_UDP_CLOSE,
	MSG_UDP_FILEINFO_SEND,
	MSG_TCP_FILE_SEND,
	MSG_TCP_FILE_RECV,
	MSG_TCP_FILEINFO_RECV =6
	//MSG_TCP_FILE_SEND_START_REQUEST,
};


//new system msg
#define WM_MSG_UDP_ADD_USER									WM_USER+1001
#define WM_MSG_UDP_SEND_MSG								WM_USER+1002
#define WM_MSG_UDP_CLOSE										WM_USER+1003
#define WM_MSG_TCP_FILE_SEND									WM_USER+1004
#define WM_MSG_TCP_FILE_SEND_REQUEST					WM_USER+1005		//暂没用
#define WM_MSG_UDP_FILEINFO_SEND							WM_USER+1006
#define WM_MSG_TCP_FILEINFO_RECV							WM_USER+1107
#define WM_MSG_TCP_FILE_RECV									WM_USER+1008
//cedit msg
#define WM_MSG_BUTTON_SEND_FILENAME					WM_USER+1009

//  socket send define
#define UDP_MSG_ADD_USER						"UDP_MSG_ADD_USER"
#define UDP_MSG_SEND								"UDP_MSG_NORMAL"
#define UDP_MSG_CLOSE							"UDP_MSG_CLOSE"
#define UDP_MSG_FILEINFO_SEND				"UDP_MSG_FILEINFO_SEND"
#define TCP_MSG_READY_RECV_FILE			"TCP_MSG_READY_RECV_FILE"
#define  TCP_MSG_SENDING_FILE				"TCP_MSG_SENDING_FILE"
#define TCP_MSG_FILE_SEND_REQUEST		"TCP_MSG_FILE_SEND_REQUEST"
//socket send struct
typedef struct 
{
	CHAR szUserName[MAX_PATH*2];
	CHAR szComputerName[MAX_PATH*2];
	SOCKADDR_IN addr;
}udp_hostinfo;

