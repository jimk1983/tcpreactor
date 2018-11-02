/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_sock.h
  版 本 号   : 初稿
  作    者   : jiangkang
  生成日期   : 2015年9月25日
  最近修改   :
  功能描述   : SOCK封装的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2015年9月25日
    作    者   : jiangkang
    修改内容   : 创建文件

******************************************************************************/

#ifndef _VOS_SOCK_H_
#define _VOS_SOCK_H_

//Create TCP type socket , AF_INET
#define VOS_SOCK_TYPE_TCP   0 
//Create UDP type socket , AF_INET
#define VOS_SOCK_TYPE_UDP   1
//Create RAW type socket , AF_INET
#define VOS_SOCK_TYPE_RAW   2
//Create ARP type socket , PF_INET
#define VOS_SOCK_TYPE_ARP   3

//INVALID Socket defined
#define VOS_SOCKET_INVALID  0

//Socket Status
#define VOS_SOCK_OK                             0
#define VOS_SOCK_ERROR                      -1
#define VOS_SOCK_EWOULDBLOCK         -2
#define VOS_SOCK_TCP_CONNECTING     -3
#define VOS_SOCK_TCP_CONNECTED       -4
#define VOS_SOCK_SSL_CONNECTING      -5
#define VOS_SOCK_SSL_ACCEPTING         -6
#define VOS_SOCK_SSL_ACCEPTED           -7
#define VOS_SOCK_SSL_EWOULDBLOCK   -8
#define VOS_SOCK_SSL_RECONNECTING  -9
#define VOS_SOCK_SSL_ERROR               -10
#define VOS_SOCK_FINISH                      -11
#define VOS_SOCK_SSL_CONNECTED       -12

//Socket windows size
#define VOS_SOCK_SNDWINSIZE         65535
#define VOS_SOCK_RCVWINSIZE         65535

/*MacOS中最大设置为6M, 因此定位6M， 设置这么大的原因是
开辟足够大的缓存，防止127.0.0.1的内部发送出现丢包*/
#define VOS_SOCK_MSGUDP_RCVWINSIZE    6*1024*1024   

#define VOS_SOCK_IPADDR_LEN         16

/*最大的系统socket号，linux是从小到大依次增加的*/
#define VOS_SOCK_FDSIZE_MAX     20000

#define VOS_SOCK_LOCALHOST_STR     "127.0.0.1"


#define VOS_SOCK_INVALID        -1

LONG VOS_SOCK_Init();
LONG VOS_SOCK_UnInit();
LONG VOS_SOCK_Send(LONG lSockfd, CHAR *pcBuf, ULONG ulBufLen, LONG *peErrorStatus);
LONG VOS_SOCK_SendTo(LONG lSockfd, CHAR *pcBuf, ULONG ulBufLen, CHAR *pcPeerAddress, ULONG ulPeerPort, LONG *peErrorStatus);
LONG VOS_SOCK_SendToN(LONG lSockfd, CHAR *pcBuf, ULONG ulBufLen, VOS_IPADDR_T uiPeerAddress, ULONG ulPeerPort, LONG *plErrorStatus);

LONG VOS_SOCK_Recv(LONG lSockfd, CHAR *pcBuf, ULONG ulBufLen, LONG *peErrorStatus);
LONG VOS_SOCK_Recvfrom(LONG lSockfd, CHAR *pcBuf, ULONG ulBufLen, CHAR *pcPeerAddress, ULONG *ulPeerPort, LONG *peErrorStatus);
LONG VOS_SOCK_RecvfromN(LONG lSockfd, CHAR *pcBuf, ULONG ulBufLen, VOS_IPADDR_T *puiPeerAddress, ULONG *pulPeerPort, LONG *plErrorStatus);
LONG VOS_SOCK_Bind(LONG lSockfd, CHAR *pcLocalAddress, ULONG ulPort);
LONG VOS_SOCK_Listen(LONG lSockfd, ULONG ulNum);
LONG VOS_SOCK_Accept(LONG lListenfd, CHAR *pcClientAddr, USHORT *pusClientPort, LONG *peErrorStatus);
LONG VOS_SOCK_Connect(LONG lSockfd, CHAR *pcServAddr, USHORT usServPort, LONG *peErrorStatus);
LONG VOS_SOCK_GetHostSockAddr(INT32 lSockfd, CHAR *pcHostAddress, UINT32 *pulPort);
LONG VOS_SOCK_GetPeerSockAddr(LONG lSockfd, CHAR *pcPeerAddress, UINT32 *pulPort);
LONG VOS_SOCK_Create(ULONG ulType);
LONG VOS_SOCK_Close(LONG lSockfd);
LONG VOS_SOCK_Shutdown(LONG lSockfd);
LONG VOS_SOCK_SetOption(LONG lSockfd);
LONG VOS_SOCK_UDPSetOption(LONG lUdpSockfd);
LONG VOS_SOCK_SetBPFOption(LONG lSockfd);
LONG VOS_SOCK_SetProMiscMode(LONG lSockfd, CHAR *pcIfName);

LONG VOS_SOCK_RAW_SendTo(LONG lSockfd, CHAR *pcBuf, ULONG ulBufLen, CHAR *acIfName, LONG *plErrorStatus);
LONG VOS_SOCK_RAW_Recvfrom(LONG lSockfd, CHAR *pcBuf, ULONG ulBufLen, LONG *plErrorStatus);

VOS_IPADDR_T  VOS_SOCK_GetHostIpByName(CHAR *acDomain);
LONG VOS_SOCK_ConnectNetAddr(LONG lSockfd, VOS_IPADDR_T uiIpaddr, USHORT usServPort, LONG *plErrorStatus);

#endif

