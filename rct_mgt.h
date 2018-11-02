/******************************************************************************

                  版权所有 (C), 2017-2020, 网络有限公司

 ******************************************************************************
  文 件 名   : rct_mgt.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2018年4月17日
  最近修改   :
  功能描述   : 核间消息处理
  函数列表   :
  修改历史   :
  1.日    期   : 2018年4月17日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/




/**************************************************************/
/************** 设计专门用于核间通信的消息队列                ****************/
/**************************************************************/


/*单个消息节点: 注意消息有大有小，目前就直接通用Malloc()的形式来传递数据*/
typedef struct tagRctMsgQueueNode
{
    VOS_DLIST_NODE_S    stEntry;           /*消息节点*/
    RCT_MSG_HEAD_S      stHead;            /*消息头部*/
    
    ULONG               ulMsgLen;          /*消息长度*/
    CHAR               *pcMsgData;         /*消息数据*/
    
}RCT_MQUENODE_S, *PRCT_MQUENODE_S;


/*返回类消息的数据检查节点信息: 只要看随机数是否为0，就表示该节点是否已经被完全释放了*/
typedef struct tagRctPeplyLockNode
{
    VOS_DLIST_NODE_S    stEntry;        /*消息节点*/
    UINT32              ulLockId;   /*消息随机ID: 用于确保当前的线程ID还在，如果不在，该消息丢弃
                                        否则，调用对应的消息会导致空指针Dump
                                        0: 表示新消息，>0表示回复消息，需要检查*/
    //VOID                *pcLockConn;    /*被锁住的信息，就是节点指针,框架中老化用的这个参数传递*/
}RCT_MRLYLOCKNODE_S, *PRCT_MRLYLOCKNODE_S;


/*本当个线程的队列上下文*/
typedef struct tagRctMsgQueueMgrCtx
{
    /*本线程的消息接收队列*/
    VOS_DLIST_NODE_S    stMsgList;          /*队列的头*/
    VOS_RW_LOCK_S       stMsgLock;          /*队列的锁*/
    ULONG               uiNum;              /*队列内消息挂载个数*/

    /*本线程的锁信息*/
    VOS_DLIST_NODE_S    stCheckRlyList;     /*检查返回队列的头*/
    VOS_RW_LOCK_S       stCheckRlyLock;     /*检查返回队列的锁*/
    ULONG               uiCheckRlyNum;      /*老化需要释放自己的节点*/
    
}RCT_MQUEMGRCTX_S, *PRCT_MQUEMGRCTX_S;

LONG    RCT_Reactor_MgtNormalSend(ULONG ulSrcSubType, ULONG ulDstRctType, ULONG ulDstSubType, ULONG ulDstPthIndex, CHAR *pcData, ULONG ulLen);

LONG    RCT_Reactor_MgtLockySend(            ULONG ulSrcSubType, ULONG ulDstRctType, 
                                             ULONG ulDstSubType, ULONG ulDstPthIndex, 
                                             CHAR *pcData, ULONG ulLen, UINT32 uiLockID);

LONG RCT_Reactor_MgtSendBack(RCT_MSG_HEAD_S            *pstSrcHead,  CHAR *pcData, ULONG ulLen);


VOID    RCT_Reactor_MsgQueLockNodeExpired(VOID *pvConn);


/****************************************************************************/
/*********************************[Mgmt Ops]*********************************/
/*消息事件*/
typedef struct tagRctReactorMgtHandler             RCT_REACTOR_MGT_HANDLER_S;


/*消息处理管理器*/
struct tagRctReactorMgtHandler
{
    RCT_EVTREACTOR_S                   *pstRctEvtReactor;
    /*使用网络事件实现*/
    RCT_REACTOR_NETEVT_OPT_S            stNetEvtOps;

    LONG                                lMsgEventfd;

    /*保存已经创建好的管理器指针, 主要是消息队列的管理*/
    PRCT_MQUEMGRCTX_S                   pstMsgQueMgr;    
};


LONG    RCT_Reactor_MgtHandlerCreate(RCT_EVTREACTOR_S *pstRctReactor);

LONG    RCT_Reactor_MgtHandlerRelease(RCT_REACTOR_MGT_HANDLER_S *pstMgtHandler);


