/******************************************************************************

                  版权所有 (C), 2017-2020, 网络有限公司

 ******************************************************************************
  文 件 名   : rct_api.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2018年4月17日
  最近修改   :
  功能描述   : 提供给外部使用的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2018年4月17日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
        
#ifdef __cplusplus
    extern "C" {
#endif

INT32   RCT_API_EnvInit();

VOID    RCT_API_EnvUnInit();

INT32   RCT_API_EnvRun();

INT32   RCT_API_EnvTaskInitRegister(VOS_CALLBACK pfInitCb, VOID *pvArg, ULONG ulRctType,ULONG ulRctSubType, VOS_CALLBACK pfUnInitCb);

INT32   RCT_API_EnvTaskInitUnRegister(ULONG ulRctType, ULONG ulRctSubType);

VOID    RCT_API_EnvGetPthConfInfo(  UINT32    *pulRctType, UINT32    *pulSubPthIndex);

ULONG   RCT_API_EnvGetTypeCfgSubNums(ULONG ulRctType);

INT32   RCT_API_MgtHandlerRegister(ULONG ulRctType, ULONG ulSubType, const CHAR *pcModuName, pfComMsgHandlerCallBack pfMethod);

VOID    RCT_API_MgtHandlerUnRegister(ULONG ulRctType, ULONG ulSubType);

LONG    RCT_API_MgtNormalSend(ULONG ulSrcSubType,
                                         ULONG ulDstRctType,
                                         ULONG ulDstSubType,
                                         ULONG ulDstPthIndex,
                                         CHAR *pcData,ULONG ulLen);

LONG    RCT_API_MgtLockySend( ULONG ulSrcSubType, 
                                        ULONG ulDstRctType, 
                                        ULONG ulDstSubType, 
                                        ULONG ulDstPthIndex, 
                                        CHAR *pcData, ULONG ulLen, UINT32 uiLockID);

LONG    RCT_API_MgtSendBack(RCT_MSG_HEAD_S            *pstSrcHead, CHAR *pcData, ULONG ulLen);

/*网络事件*/
typedef struct tagRctNetOptsEvent
{
  LONG                  lSockfd;               
  ULONG                 ulEventMask;       
  VOS_CALLBACK_S        stRecv;               
  VOS_CALLBACK_S        stSend;               
}RCT_NETOPT_EVENT_S;

INT32 RCT_API_NetOpsEventRegister( RCT_NETOPT_EVENT_S *pstNetOps,  
                                                LONG lSockfd,  
                                                ULONG ulEMask, 
                                                VOS_CALLBACK pfRecv, 
                                                VOS_CALLBACK pfSend,
                                                VOID *pvConn);

INT32 RCT_API_NetOpsEventUnRegister(RCT_NETOPT_EVENT_S *pstNetOps);

INT32 RCT_API_NetOpsEventCtrl(RCT_NETOPT_EVENT_S *pstNetOps, ULONG ulEpollMask);

/*老化事件节点*/
typedef struct tagRctExpireOpts
{
    VOS_DLIST_NODE_S    stNode; 
    VOS_CALLBACK_S      stExpirecb;
    ULONG               ulExpireConfirm;
}RCT_EXPIROPT_EVENT_S;

INT32 RCT_API_ExpireOpsEventRegister(RCT_EXPIROPT_EVENT_S *pstExpireOps, 
                                                    VOS_CALLBACK pfRecv, 
                                                    VOID *pvConn);

INT32 RCT_API_ExpireOpsEventUnRegister(RCT_EXPIROPT_EVENT_S *pstExpireOps);

/*定时器类型: 一次定时器*/
#define RCT_TIMERTYPE_ONCE             0
/*定时器类型:循环定时器*/
#define RCT_TIMERTYPE_RECYLE           1

typedef struct tagRctTimerOpts
{
    VOS_DLIST_NODE_S    stTimerNode;
    ULONG               ulTimeType;
    ULONG               ulTimerID;
    ULONG               ulTimerOut;
    ULONG               ulTimeStamp;
    VOS_CALLBACK_S      stTimercb;
}RCT_TIMEROPT_EVENT_S;

INT32 RCT_API_TimerOpsEventRegister(RCT_TIMEROPT_EVENT_S *pstTimerOps, 
                                                ULONG ulTimerType, 
                                                ULONG ulTimerOut, 
                                                VOS_CALLBACK pfRecv, 
                                                VOID *pvConn);

INT32 RCT_API_TimerOpsEventUnRegister(RCT_TIMEROPT_EVENT_S *pstTimerOps);

    
#ifdef __cplusplus
        }
#endif


