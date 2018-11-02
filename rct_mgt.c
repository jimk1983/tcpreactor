/******************************************************************************

                  版权所有 (C), 2018-2028, 网络有限公司

 ******************************************************************************
  文 件 名   : rct_mgt.c
  版 本 号   : 初稿
  作    者   : 蒋康
  生成日期   : 2018年4月19日
  最近修改   :
  功能描述   : 消息通信
  函数列表   :
              RCT_Reactor_MgtHandlerCreate
              RCT_Reactor_MgtHandlerRelease
  修改历史   :
  1.日    期   : 2018年4月19日
    作    者   : 蒋康
    修改内容   : 创建文件
注：
    发现一个严重Bug, 需要完善一种对用户数据进行锁住的情况, 本次进行重构了
******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>


/*****************************************************************************
 函 数 名  : RCT_Reactor_MsgQueLockNodePush
 功能描述  : 锁节点操作
 输入参数  : PRCT_MQUEMGRCTX_S pstQueMgrCtx  
             RCT_MRLYLOCKNODE_S *pstMsgNode  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_Reactor_MsgQueLockNodePush(PRCT_MQUEMGRCTX_S pstQueMgrCtx, RCT_MRLYLOCKNODE_S *pstMsgNode)
{
    if ( NULL == pstQueMgrCtx )
    {
        return;
    }
    
    VOS_RWLOCK_LOCK(pstQueMgrCtx->stCheckRlyLock);
    VOS_DLIST_ADD_TAIL(&pstQueMgrCtx->stCheckRlyList, &pstMsgNode->stEntry);
    VOS_InterlockedIncrement(&pstQueMgrCtx->uiCheckRlyNum);
    VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);

    VOS_Printf("Lock Push NodeInfo: pstLockMsgNode=%p, lockid=%d", 
            &pstMsgNode->stEntry, pstMsgNode->ulLockId);
    
    return;
}

/*****************************************************************************
 函 数 名  : RCT_Reactor_MsgQueLockNodePop
 功能描述  : 操作锁数据的队列
 输入参数  : PRCT_MQUEMGRCTX_S pstQueMgrCtx  
 输出参数  : 无
 返 回 值  : RCT_MRLYLOCKNODE_S
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
RCT_MRLYLOCKNODE_S *RCT_Reactor_MsgQueLockNodePop(PRCT_MQUEMGRCTX_S pstQueMgrCtx)
{
    PVOS_DLIST_NODE_S    thisEntry, nextEntry, ListHead; 
    RCT_MRLYLOCKNODE_S  *pstMsgNode= NULL;
    
    if ( NULL == pstQueMgrCtx )
    {
        return NULL;
    }

    VOS_RWLOCK_LOCK(pstQueMgrCtx->stCheckRlyLock);
    if ( VOS_OK == VOS_Node_IsEmpty(&pstQueMgrCtx->stCheckRlyList) )
    {
        VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);
        return NULL;    
    }
    
    ListHead = &pstQueMgrCtx->stCheckRlyList;

    for(thisEntry = ListHead->next;
        thisEntry != ListHead;
        thisEntry = nextEntry)
    {
        nextEntry = thisEntry->next;
        pstMsgNode = VOS_DLIST_ENTRY(thisEntry, RCT_MRLYLOCKNODE_S, stEntry);
        VOS_Node_Remove(thisEntry);
        VOS_InterlockedDecrement(&pstQueMgrCtx->uiCheckRlyNum);
        break;
    }
    VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);
        
    return pstMsgNode;
}


/*****************************************************************************
 函 数 名  : RCT_Reactor_MsgQueLockNodeExpired
 功能描述  : 根据节点老化的动作，将自己的可靠锁定进行处理
 输入参数  : VOID *pvConn   --目前框架，这个锁定的数据必须是conn的节点，因为老化中用的是这个节点，来进行老化的
                           
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_Reactor_MsgQueLockNodeExpired(VOID *pvConn)
{
    PVOS_DLIST_NODE_S           thisEntry, nextEntry, ListHead; 
    RCT_COMMUTEINFO_S           stSrcEvntInfo   = {0};
    RCT_MRLYLOCKNODE_S         *pstMsgNode      = NULL;
    ULONG                       ulLockRplyId = 0;

    ulLockRplyId = (ULONG)pvConn;
    
    /*获取当前线程的信息*/
    if ( VOS_ERR == RCT_Task_ReactorGetCommunInfo(&stSrcEvntInfo) )
    {
        DbgRctModuleError("Get our pthread communication map info failed!");
        return;
    }

    if ( NULL == stSrcEvntInfo.pstQueMgrCtx )
    {
        return;
    }

    VOS_RWLOCK_LOCK(stSrcEvntInfo.pstQueMgrCtx->stCheckRlyLock);
    if ( VOS_OK == VOS_Node_IsEmpty(&stSrcEvntInfo.pstQueMgrCtx->stCheckRlyList) )
    {
        VOS_RWLOCK_UNLOCK(stSrcEvntInfo.pstQueMgrCtx->stCheckRlyLock);
        return;    
    }
    
    ListHead = &stSrcEvntInfo.pstQueMgrCtx->stCheckRlyList;

    for(thisEntry = ListHead->next;
        thisEntry != ListHead;
        thisEntry = nextEntry)
    {
        nextEntry = thisEntry->next;
        pstMsgNode = VOS_DLIST_ENTRY(thisEntry, RCT_MRLYLOCKNODE_S, stEntry);
        if ( ulLockRplyId == pstMsgNode->ulLockId )
        {
            VOS_Printf("RCT_Reactor_MsgQueLockNodeExpired OK!, ulLockId=%08x", pstMsgNode->ulLockId);
            VOS_Node_Remove(thisEntry);
            VOS_InterlockedDecrement(&stSrcEvntInfo.pstQueMgrCtx->uiCheckRlyNum);
            break;
        }
    }
    VOS_RWLOCK_UNLOCK(stSrcEvntInfo.pstQueMgrCtx->stCheckRlyLock);
    
    return;
}


/*****************************************************************************
 函 数 名  : RCT_Reactor_MsgQueLockNodeIsExist
 功能描述  : 该节点是否还在
 输入参数  : VOID *pvConn  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
BOOL RCT_Reactor_MsgQueLockNodeCheckAndExist(PRCT_MQUEMGRCTX_S pstQueMgrCtx, UINT32 ulLockyId)
{
    PVOS_DLIST_NODE_S    thisEntry, nextEntry, ListHead; 
    RCT_MRLYLOCKNODE_S  *pstLockNode      = NULL;
    
    if ( NULL == pstQueMgrCtx
        || 0 == ulLockyId )
    {
        return VOS_FALSE;
    }

    VOS_RWLOCK_LOCK(pstQueMgrCtx->stCheckRlyLock);
    if ( VOS_OK == VOS_Node_IsEmpty(&pstQueMgrCtx->stCheckRlyList) )
    {
        VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);
        return VOS_FALSE;    
    }
    
    ListHead = &pstQueMgrCtx->stCheckRlyList;

    for(thisEntry = ListHead->next;
        thisEntry != ListHead;
        thisEntry = nextEntry)
    {
        nextEntry = thisEntry->next;
        pstLockNode = VOS_DLIST_ENTRY(thisEntry, RCT_MRLYLOCKNODE_S, stEntry);

        /*ID对上即表示这个节点还存在，那么删除本节点，然后把消息处理完毕*/
        if ( ulLockyId == pstLockNode->ulLockId )
        {
            VOS_Node_Remove(thisEntry);
            VOS_InterlockedDecrement(&pstQueMgrCtx->uiCheckRlyNum);
            VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);
            return VOS_TRUE;
        }
    }
        
    VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);
    
    return VOS_FALSE;
}


/*****************************************************************************
 函 数 名  : RCT_Reactor_MsgQueNodePush
 功能描述  : 将节点推送进入
 输入参数  : PRCT_MQUEMGRCTX_S pstMegQueMgr  
             RCT_MQUENODE_S *pstMsgNode      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_Reactor_MsgQueNodePush(PRCT_MQUEMGRCTX_S pstQueMgrCtx, RCT_MQUENODE_S *pstMsgNode)
{
    if ( NULL == pstQueMgrCtx )
    {
        return;
    }
    
    VOS_RWLOCK_LOCK(pstQueMgrCtx->stMsgLock);
    VOS_DLIST_ADD_TAIL(&pstQueMgrCtx->stMsgList, &pstMsgNode->stEntry);
    VOS_InterlockedIncrement(&pstQueMgrCtx->uiNum);
    VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stMsgLock);
    return;
}


/*****************************************************************************
 函 数 名  : RCT_Reactor_MsgQueNodePop
 功能描述  : 将节点拿出
 输入参数  : PRCT_MQUEMGRCTX_S pstMegQueMgr  
 输出参数  : 无
 返 回 值  : RCT_MQUENODE_S
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
RCT_MQUENODE_S *RCT_Reactor_MsgQueNodePop(PRCT_MQUEMGRCTX_S pstQueMgrCtx)
{
    PVOS_DLIST_NODE_S  thisEntry, nextEntry, ListHead; 
    RCT_MQUENODE_S    *pstMsgNode= NULL;
    
    if ( NULL == pstQueMgrCtx )
    {
        return NULL;
    }

    VOS_RWLOCK_LOCK(pstQueMgrCtx->stMsgLock);
    if ( VOS_OK == VOS_Node_IsEmpty(&pstQueMgrCtx->stMsgList) )
    {
        VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stMsgLock);
        return NULL;    
    }
    
    ListHead = &pstQueMgrCtx->stMsgList;

    for(thisEntry = ListHead->next;
        thisEntry != ListHead;
        thisEntry = nextEntry)
    {
        nextEntry = thisEntry->next;
        pstMsgNode = VOS_DLIST_ENTRY(thisEntry, RCT_MQUENODE_S, stEntry);
        VOS_Node_Remove(thisEntry);
        VOS_InterlockedDecrement(&pstQueMgrCtx->uiNum);
        break;
    }
    VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stMsgLock);
        
    return pstMsgNode;
}


/*****************************************************************************
 函 数 名: RCT_Reactor_MgtNormalSend
 功能描述  :   一次性消息，无需返回的结果的，这种消息，内存由对方释放
 输入参数  :   ULONG ulSrcSubType  --这个接口用于对方收到消息后，可以直接获取SSL下的SWM\NEM\TCM之类的，方便返回
                                 提供出这个接口，可以让用户自行封装
           ULONG ulRctType     --目的的RctType
           ULONG ulSubType     --目的的SubType
           ULONG ulDstPthIndex --可能SSL这种，有多个线程的，需要告知目的线程Nums
           CHAR *pcData        --发送的消息指针，必须要VOS_Malloc()申请
           ULONG ulLen         --该消息的长度
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数
注意：因为是非拷贝的异步设计模式，
      因此所有的pcData, 都需要动态的VOS_Malloc()来申请, 不能用局部变量来发送

*****************************************************************************/
LONG RCT_Reactor_MgtNormalSend(ULONG ulSrcSubType,ULONG ulDstRctType,ULONG ulDstSubType,ULONG ulDstPthIndex,CHAR *pcData,ULONG ulLen)
{
    PRCT_MQUEMGRCTX_S   pstQueMgrCtx    = NULL;
    RCT_COMMUTEINFO_S   stDstEvntInfo   = {0};
    PRCT_MQUENODE_S     pstMgtNode      = NULL;
    RCT_INDEXINFO_S     stSrcIndexInfo  = {0};
    ULONG               uiVal           = 1;
    LONG                lRet            = 0;

    /*将准备发送消息，推送到对应的消息队列中*/
    /*根据业务主类型和业务多子线程索引号，来获取通信矩阵信息*/
    if ( VOS_ERR == RCT_TaskPthGetCommunMapInfo(ulDstRctType, ulDstPthIndex, &stDstEvntInfo)  )
    {
        DbgRctModuleError("Get communication map info failed!");
        return VOS_ERR;
    }
    
    /*获取发送目的消息队列管理器*/
    pstQueMgrCtx = stDstEvntInfo.pstQueMgrCtx;
    if ( NULL == pstQueMgrCtx )
    {
        DbgRctModuleError("Dst commuicate quemgr has NULL!");
        return VOS_ERR;
    }

    /*获取自己的线程索引信息*/
    if ( VOS_ERR == RCT_Task_ReactorGetPthIndexInfo(&stSrcIndexInfo)  )
    {
        DbgRctModuleError("Get pthread index info failed!");
        return VOS_ERR;
    }

    /*直接用VOS_Malloc()即可，因为有内存管理，不会影响速度*/
    pstMgtNode= (RCT_MQUENODE_S *)VOS_Malloc(RCT_MID_SID_EVT, sizeof(RCT_MQUENODE_S));
    if ( NULL == pstMgtNode )
    {
        DbgRctModuleError("mgt node malloc failed!");
        return VOS_ERR;
    }
    VOS_Mem_Zero((CHAR *)pstMgtNode, sizeof(RCT_MQUENODE_S));

    VOS_DLIST_INIT(&pstMgtNode->stEntry);

    /*填写消息头部信息*/
    //SRC
    pstMgtNode->stHead.ulSrcRctSubType  = ulSrcSubType;                 /*SSL下的SWM\NEM\TCM等子类型, 须由用户自己填写*/
    pstMgtNode->stHead.ulSrcRctType     = stSrcIndexInfo.ulRctType;     /*主类型:CLI\SSL\AAA\...*/
    pstMgtNode->stHead.ulSrcPthIndex    = stSrcIndexInfo.ulPthSubIndex; /*SSL多个线程，线程索引*/
    pstMgtNode->stHead.ulSrcMsgEventFd  = stSrcIndexInfo.ulMsgEventFd;  /*消息*/
    //DST
    pstMgtNode->stHead.ulDstRctType     = ulDstRctType;
    pstMgtNode->stHead.ulDstRctSubType  = ulDstSubType;
    pstMgtNode->stHead.ulDstPthIndex    = ulDstPthIndex;
    /*很重要: 如果是Need，那么需要根据这个标记进入到锁用户数据的判断*/
    //pstMgtNode->stHead.ulLockRplyFlag   = RCT_MSGLOCKPLY_NONEED;
    /*不是Back信息*/
    pstMgtNode->stHead.ulBackFlag       = 0;
    
    pstMgtNode->pcMsgData   = pcData;
    pstMgtNode->ulMsgLen    = ulLen;

    RCT_Reactor_MsgQueNodePush(pstQueMgrCtx, pstMgtNode);

    /*根据通信矩阵, 触发对应的消息线程*/
    lRet = VOS_EventfdWrite(stDstEvntInfo.ulEventFdMsg,  &uiVal, sizeof(ULONG));
    if( lRet < 0)
    {
        DbgRctModuleError("message action eventfd=[%d] failed!", stDstEvntInfo.ulEventFdMsg);
    }

    //VOS_Printf("NormalSend: [%d:%d:%d] --> [%d:%d:%d]-%d ,QueMgrCtx=%p,MsgNode=%p,",
    //    stSrcIndexInfo.ulRctType, ulSrcSubType, stSrcIndexInfo.ulPthSubIndex, 
    //    ulDstRctType, ulDstSubType, ulDstPthIndex, stDstEvntInfo.ulEventFdMsg, pstQueMgrCtx, pstMgtNode);
    
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_Reactor_MgtLockyDataSend
 功能描述  :    数据锁定发送,
            提供消息需要返回的，并且有数据需要检查该数据是否可靠
            因为如果不可靠，那么会出问题，由于是异步模式，无法判断出该节点是否已经被释放
            所以通过一个lock的机制，来保证这个异步模式框架的可靠性
 输入参数  : ULONG ulSrcSubType   
             ULONG ulDstRctType   
             ULONG ulDstSubType   
             ULONG ulDstPthIndex  
             CHAR *pcData         
             ULONG ulLen          
             *pcLockConn          --待锁定的用户数据，一般是一个节点信息
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Reactor_MgtLockySend(ULONG ulSrcSubType, ULONG ulDstRctType, ULONG ulDstSubType, 
                                          ULONG ulDstPthIndex, CHAR *pcData, ULONG ulLen, UINT32 uiLockID)
{
    PRCT_MQUEMGRCTX_S   pstQueMgrCtx    = NULL;
    RCT_COMMUTEINFO_S   stDstEvntInfo   = {0};
    PRCT_MQUENODE_S     pstMgtNode      = NULL;
    PRCT_MRLYLOCKNODE_S pstLockNode     = NULL;
    RCT_INDEXINFO_S     stSrcIndexInfo  = {0};
    ULONG               uiVal           = 1;
    LONG                lRet            = 0;


    /*将准备发送消息，推送到对应的消息队列中*/
    /*根据业务主类型和业务多子线程索引号，来获取通信矩阵信息*/
    if ( VOS_ERR == RCT_TaskPthGetCommunMapInfo(ulDstRctType, ulDstPthIndex, &stDstEvntInfo)  )
    {
        DbgRctModuleError("Get communication map info failed!");
        return VOS_ERR;
    }
    
    /*获取发送目的消息队列管理器*/
    pstQueMgrCtx = stDstEvntInfo.pstQueMgrCtx;
    if ( NULL == pstQueMgrCtx )
    {
        DbgRctModuleError("Dst commuicate quemgr has NULL!");
        return VOS_ERR;
    }

    /*获取自己的线程索引信息*/
    if ( VOS_ERR == RCT_Task_ReactorGetPthIndexInfo(&stSrcIndexInfo)  )
    {
        DbgRctModuleError("Get pthread index info failed!");
        return VOS_ERR;
    }

    /*直接用VOS_Malloc()即可，因为有内存管理，不会影响速度*/
    pstMgtNode= (RCT_MQUENODE_S *)VOS_Malloc(RCT_MID_SID_EVT, sizeof(RCT_MQUENODE_S));
    if ( NULL == pstMgtNode )
    {
        DbgRctModuleError("mgt node malloc failed!");
        return VOS_ERR;
    }
    VOS_Mem_Zero((CHAR *)pstMgtNode, sizeof(RCT_MQUENODE_S));

    VOS_DLIST_INIT(&pstMgtNode->stEntry);

    /*填写消息头部信息*/
    //SRC
    pstMgtNode->stHead.ulSrcRctType     = stSrcIndexInfo.ulRctType;     /*主类型:CLI\SSL\AAA\...*/
    pstMgtNode->stHead.ulSrcRctSubType  = ulSrcSubType;                 /*SSL下的SWM\NEM\TCM等子类型*/
    pstMgtNode->stHead.ulSrcPthIndex    = stSrcIndexInfo.ulPthSubIndex; /*SSL多个线程，线程索引*/
    pstMgtNode->stHead.ulSrcMsgEventFd  = stSrcIndexInfo.ulMsgEventFd;  /*用于消息返回*/
    
    //DST
    pstMgtNode->stHead.ulDstRctType     = ulDstRctType;
    pstMgtNode->stHead.ulDstRctSubType  = ulDstSubType;
    pstMgtNode->stHead.ulDstPthIndex    = ulDstPthIndex;

    pstMgtNode->pcMsgData   = pcData;
    pstMgtNode->ulMsgLen    = ulLen;

    /*不是Back信息*/
    pstMgtNode->stHead.ulBackFlag       = 0;
    
    //pstMgtNode->stHead.ulLockRplyFlag   = RCT_MSGLOCKPLY_NEED;
    /*需要锁定的节点，用于返回的时候检测*/
    pstMgtNode->stHead.ulLockRplyId     = uiLockID;

    /*推送到对方的队列中*/
    RCT_Reactor_MsgQueNodePush(pstQueMgrCtx, pstMgtNode);
    

    /*****************************************************************************/
    /*****************************锁定在本队列************************************/

    /*用户锁定的节点*/
    pstLockNode= (RCT_MRLYLOCKNODE_S *)VOS_Malloc(RCT_MID_SID_EVT, sizeof(RCT_MRLYLOCKNODE_S));
    if ( NULL == pstLockNode )
    {
        DbgRctModuleError("mgt node malloc failed!");
        return VOS_ERR;
    }
    
    VOS_Mem_Zero((CHAR *)pstLockNode, sizeof(RCT_MRLYLOCKNODE_S));

    VOS_DLIST_INIT(&pstLockNode->stEntry);

    //pstLockNode->pcLockConn       = pcLockConn;
    pstLockNode->ulLockId     = uiLockID;

    /*存放在本机的*/
    RCT_Reactor_MsgQueLockNodePush(g_th_pstReactor->pstPthCfgCtx->stEvtInfo.pstQueMgrCtx, pstLockNode);    
    
    /*根据通信矩阵, 触发对应的消息线程*/
    lRet = VOS_EventfdWrite(stDstEvntInfo.ulEventFdMsg,  &uiVal, sizeof(ULONG));
    if( lRet < 0)
    {
        DbgRctModuleError("message action eventfd=[%d] failed!", stDstEvntInfo.ulEventFdMsg);
    }
    
    return VOS_OK;
}


/*****************************************************************************
函 数 名  : RCT_Reactor_MgtNormalSendBack
功能描述  : 将消息根据原先的头部，原路返回
输入参数  : RCT_MSG_HEAD_S            *pstBackHead  
           CHAR *pcData                            
           ULONG ulLen                             
           VOID *pcLockConn                        
输出参数  : 无
返 回 值  : 
调用函数  : 
被调函数  : 

修改历史      :
1.日    期   : 2018年6月5日
  作    者   : jimk
  修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Reactor_MgtSendBack(RCT_MSG_HEAD_S            *pstHead, CHAR *pcData, ULONG ulLen)
{
  PRCT_MQUEMGRCTX_S   pstQueMgrCtx    = NULL;
  RCT_COMMUTEINFO_S   stDstEvntInfo   = {0};
  PRCT_MQUENODE_S     pstMgtNode      = NULL;
  ULONG               uiVal           = 1;
  LONG                lRet            = 0;

  /*将准备发送消息，推送到对应的消息队列中*/
  /*根据业务主类型和业务多子线程索引号，来获取通信矩阵信息*/
  if ( VOS_ERR == RCT_TaskPthGetCommunMapInfo(pstHead->ulSrcRctType, pstHead->ulSrcPthIndex, &stDstEvntInfo)  )
  {
      DbgRctModuleError("Get communication map info failed!");
      return VOS_ERR;
  }
  
  /*获取发送目的消息队列管理器*/
  pstQueMgrCtx = stDstEvntInfo.pstQueMgrCtx;
  if ( NULL == pstQueMgrCtx )
  {
      DbgRctModuleError("Dst commuicate quemgr has NULL!");
      return VOS_ERR;
  }

  /*直接用VOS_Malloc()即可，因为有内存管理，不会影响速度*/
  pstMgtNode= (RCT_MQUENODE_S *)VOS_Malloc(RCT_MID_SID_EVT, sizeof(RCT_MQUENODE_S));
  if ( NULL == pstMgtNode )
  {
      DbgRctModuleError("mgt node malloc failed!");
      return VOS_ERR;
  }
  VOS_Mem_Zero((CHAR *)pstMgtNode, sizeof(RCT_MQUENODE_S));

  VOS_DLIST_INIT(&pstMgtNode->stEntry);

  /*填写消息头部信息*/
  //SRC
  pstMgtNode->stHead.ulSrcRctType     = pstHead->ulDstRctType;     /*主类型:CLI\SSL\AAA\...*/
  pstMgtNode->stHead.ulSrcRctSubType  = pstHead->ulDstRctSubType;  /*SSL下的SWM\NEM\TCM等子类型*/
  pstMgtNode->stHead.ulSrcPthIndex    = pstHead->ulDstPthIndex;   /*SSL多个线程，线程索引*/
  
  //DST
  pstMgtNode->stHead.ulDstRctType     = pstHead->ulSrcRctType;
  pstMgtNode->stHead.ulDstRctSubType  = pstHead->ulSrcRctSubType;
  pstMgtNode->stHead.ulDstPthIndex    = pstHead->ulSrcPthIndex;

  pstMgtNode->pcMsgData   = pcData;
  pstMgtNode->ulMsgLen    = ulLen;

  /*****************************************************************************/
  //pstMgtNode->stHead.ulLockRplyFlag   = RCT_MSGLOCKPLY_NEED;
  pstMgtNode->stHead.ulLockRplyId     = pstHead->ulLockRplyId;
  pstMgtNode->stHead.ulBackFlag       = 1;

  RCT_Reactor_MsgQueNodePush(pstQueMgrCtx, pstMgtNode);

  if ( stDstEvntInfo.ulEventFdMsg != pstHead->ulSrcMsgEventFd  )
  {
      /*表示消息的ID存在问题，通信矩阵的信息不对*/
      DbgRctModuleError("rct reactor message eventfd=[%d] not equal the srcmsgeventfd=[%d]!", 
      stDstEvntInfo.ulEventFdMsg, pstHead->ulSrcMsgEventFd);
  }
  
  /*根据通信矩阵, 触发对应的消息线程*/
  lRet = VOS_EventfdWrite(stDstEvntInfo.ulEventFdMsg,  &uiVal, sizeof(ULONG));
  if( lRet < 0)
  {
      DbgRctModuleError("message action eventfd=[%d] failed!", stDstEvntInfo.ulEventFdMsg);
  }
  
  return VOS_OK;
}

/*****************************************************************************
 函 数 名  :    RCT_Reactor_MessageMainHandler
 功能描述  :     将取出的消息进行处理
 输入参数  :     PRCT_MQUEMGRCTX_S  pstQueMgrCtx --管理器        
             RCT_MQUENODE_S     *pstMsgNode  --取出的消息
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Reactor_MessageMainHandler(PRCT_MQUEMGRCTX_S pstQueMgrCtx, RCT_MQUENODE_S              *pstMsgNode)
{
    ULONG       ulDropFlag = VOS_FALSE;
    
    if ( NULL== pstQueMgrCtx
        || NULL == pstMsgNode
        || NULL == pstMsgNode->pcMsgData )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

    /*查看消息是否为返回时候需要检查的，锁定数据的消息, 不为0，表示有锁定的消息*/
    if ( pstMsgNode->stHead.ulBackFlag == 1 && pstMsgNode->stHead.ulLockRplyId != 0 )
    {
        /*处理消息锁定的数据*/
        if ( VOS_FALSE == RCT_Reactor_MsgQueLockNodeCheckAndExist(pstQueMgrCtx, pstMsgNode->stHead.ulLockRplyId)  )
        {
            /*表明此时节点已经被用户老化掉了，该消息丢弃*/
            ulDropFlag = VOS_TRUE;
        }    
    }
    
    /*将消息丢弃, 不进行处理了, 这种情况也算处理成功*/
    if ( VOS_TRUE == ulDropFlag  )
    {
        DbgRctModuleError("message drop handle!");
        VOS_Free((CHAR *)pstMsgNode->pcMsgData);
            
        /*由本线程释放掉该消息节点*/
        VOS_Free((CHAR *)pstMsgNode);
        return VOS_OK;
    }

    /*处理消息，调用注册的业务回调函数*/
    if ( VOS_ERR == RCT_TaskArryBizMsgHandlerRunCall(pstMsgNode) )
    {
        DbgRctModuleError("message drop handle!");
        VOS_Free((CHAR *)pstMsgNode->pcMsgData);
        /*由本线程释放掉该消息节点*/
        VOS_Free((CHAR *)pstMsgNode);
        return VOS_ERR;
    }
    
    VOS_Free((CHAR *)pstMsgNode->pcMsgData);
        
    /*由本线程释放掉该消息节点*/
    VOS_Free((CHAR *)pstMsgNode);
        
    return VOS_OK;
}


/*****************************************************************************
 函 数 名: RCT_Reactor_MgtHandlerCb
 功能描述  :    本线程单元的消息触发回调函数
 输入参数  :    VOID *pvHandler            ---设置的消息回调
 输出参数  :    无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Reactor_MgtHandlerCb(VOID *pvHandler)
{
    RCT_REACTOR_MGT_HANDLER_S   *pstMgtHandler  = NULL;
    RCT_MQUENODE_S              *pstMsgNode     = NULL;
    LONG                        lRet            = 0;
    ULONG                       uiVal           = 0;
    RCT_COMMUTEINFO_S           stSrcEvntInfo   = {0};
    
    pstMgtHandler = (RCT_REACTOR_MGT_HANDLER_S *)pvHandler;

    /*读取掉触发的数据*/
    lRet = VOS_EventfdRead(pstMgtHandler->lMsgEventfd, &uiVal, sizeof(ULONG));
    if( VOS_ERR == lRet)
    {
        DbgRctModuleError("Mgt: event fd:[%d] read error!ret=%d", pstMgtHandler->lMsgEventfd, lRet);
        return VOS_ERR;
    }
    
    /*获取当前线程的信息*/
    if ( VOS_ERR == RCT_Task_ReactorGetCommunInfo(&stSrcEvntInfo) )
    {
        DbgRctModuleError("Get our pthread communication map info failed!");
        return VOS_ERR;
    }

    //VOS_Printf("NormalRecv: Src:%d, pid=[%08x], QueMgrCtx=%p ",
    //   pstMgtHandler->lMsgEventfd,  stSrcEvntInfo.ulPid, stSrcEvntInfo.pstQueMgrCtx);

    if ( NULL == stSrcEvntInfo.pstQueMgrCtx )
    {
        DbgRctModuleError("Get our pthread que mgr ctx failed!");
        return VOS_ERR;
    }

    /*循环处理完毕所有消息*/
    while(1)
    {
        /*加锁判断是否为空*/
        VOS_RWLOCK_LOCK(stSrcEvntInfo.pstQueMgrCtx->stMsgLock);
        if ( VOS_OK == VOS_Node_IsEmpty(&stSrcEvntInfo.pstQueMgrCtx->stMsgList) )
        {
            VOS_RWLOCK_UNLOCK(stSrcEvntInfo.pstQueMgrCtx->stMsgLock);   
            break;
        }
        VOS_RWLOCK_UNLOCK(stSrcEvntInfo.pstQueMgrCtx->stMsgLock);  

        /*取出消息进行处理, 这个消息节点已经解锁了，不在队列中了*/
        pstMsgNode = RCT_Reactor_MsgQueNodePop(stSrcEvntInfo.pstQueMgrCtx);

        //VOS_Printf("NormalRecv: RCT_Reactor_MsgQueNodePop=%p ",pstMsgNode);
        if (VOS_ERR == RCT_Reactor_MessageMainHandler(stSrcEvntInfo.pstQueMgrCtx, pstMsgNode))
        {
            /*处理出错了，也退出*/
            return VOS_ERR;
        }
    }
    
    return VOS_OK;
}





/*****************************************************************************
 函 数 名  : RCT_Reactor_MgtHandlerCreate
 功能描述  : 核间消息处理器
 输入参数  : RCT_EVTREACTOR_S *pstRctReactor  
             ULONG ulMgtPort                  
             VOS_MSG_QUE_S  *pstMgtQue        
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
*****************************************************************************/
LONG RCT_Reactor_MgtHandlerCreate(RCT_EVTREACTOR_S *pstRctReactor)
{
   
    RCT_REACTOR_MGT_HANDLER_S   *pstMgtHandler  = NULL;
    
    if ( NULL == pstRctReactor 
        || NULL == pstRctReactor->pstPthCfgCtx )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    VOS_Mem_Zero((CHAR *)(&pstRctReactor->stMgtHandler), sizeof(RCT_REACTOR_MGT_HANDLER_S));
    
    pstMgtHandler = (&pstRctReactor->stMgtHandler);
    
    pstMgtHandler->pstRctEvtReactor = pstRctReactor;
    
    /*将之前创建好的配置内容保存，便于操作本消息队列的消息
      通过通信矩阵也可以获取*/
    pstMgtHandler->pstMsgQueMgr     = pstRctReactor->pstPthCfgCtx->stEvtInfo.pstQueMgrCtx;

    pstMgtHandler->lMsgEventfd = VOS_EventfdCreate(0);
    if ( VOS_ERR ==  pstMgtHandler->lMsgEventfd )
    {
        DbgRctModuleError("eventfd create error!");
        return VOS_ERR;
    }

    /*记录到通信矩阵*/
    pstRctReactor->pstPthCfgCtx->stEvtInfo.ulEventFdMsg     = pstMgtHandler->lMsgEventfd;
    /*记录到消息返回*/
    pstRctReactor->pstPthCfgCtx->stIndexInfo.ulMsgEventFd   = pstMgtHandler->lMsgEventfd;

    /*Init MgtHandler NetEvent */
    RCT_REACTOR_NETEVTOPTS_INIT(
                &pstMgtHandler->stNetEvtOps,
                pstMgtHandler->lMsgEventfd,
                VOS_EPOLL_MASK_POLLIN,
                RCT_Reactor_MgtHandlerCb,
                NULL,
                pstMgtHandler
                );

    /*register the network epoll events*/
    if(VOS_ERR == RCT_Net_EventOptsRegister(pstRctReactor, &pstMgtHandler->stNetEvtOps))
    {
        DbgRctModuleError("RCT_Net_EventOptsRegister error!");
        VOS_EventfdClose(pstMgtHandler->lMsgEventfd);
        return VOS_ERR;
    }
    
    DbgRctModuleEvent("Rct msg Handler create OK!");
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_Reactor_MgtHandlerRelease
 功能描述  : 释放核间消息管理器
 输入参数  : RCT_REACTOR_MGT_HANDLER_S *pstMgtHandler  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Reactor_MgtHandlerRelease(RCT_REACTOR_MGT_HANDLER_S *pstMgtHandler)
{
    if(NULL == pstMgtHandler)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    (VOID)RCT_Net_EventOptsUnRegister(pstMgtHandler->pstRctEvtReactor, &pstMgtHandler->stNetEvtOps);
    
    VOS_EventfdClose(pstMgtHandler->lMsgEventfd);
    
    return VOS_OK;
}
