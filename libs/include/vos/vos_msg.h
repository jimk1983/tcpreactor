/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_ipcmsg.h
  版 本 号   : 初稿
  作    者   : jiangkang
  生成日期   : 2015年9月25日
  最近修改   :
  功能描述   : 适配系统线程间消息的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2015年9月25日
    作    者   : jiangkang
    修改内容   : 创建文件

******************************************************************************/


#ifndef _VOS_IPCMSG_H_
#define _VOS_IPCMSG_H_

/*消息发送的数据缓存最大长度*/
#define  VOS_MSG_BUF_SIZE            8192

/*消息发送*/
typedef struct tagVOSMsg
{
    LONG lMsgType;          /*消息类型，根据消息类型接收*/
    INTPTR iMsgPtr[4];       /*消息发送接收的地址， 一般就使用一个结构体地址进行传输*/
}VOS_MSG_S;


INT32 VOS_GetSelfPhreadID();

INT32 VOS_MSGCreateByKeyID(INT32 ulMsgKeyId);
VOID VOS_MSGReleaseByKeyID(INT32 ulMsgKeyId);
INT32 VOS_MSGKeyIDIsExist(INT32 ulMsgKeyId);

INT32 VOS_MSGSend(INT32 ulMsgQueId, INTPTR iMsgPtr[4]);
INT32 VOS_MSGRecv(INT32 ulMsgQueId, INTPTR *iMsgPtr);

#if 0
LONG VOS_MsgQueSend(ULONG ulMsgQueId, CHAR *pcSndBuf, ULONG ulSndSize);
LONG VOS_MsgQueRecv(ULONG ulMsgQueId, CHAR *pcRcvBuf, ULONG *pulRcvSize);

LONG VOS_MsgQueSendCpy(ULONG ulMsgQueId, CHAR *pcSndBuf);
LONG VOS_MsgQueRecvCpy(ULONG ulMsgQueId, CHAR *pcRcvBuf);
#endif

#endif
