/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_epr.c
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年5月26日
  最近修改   :
  功能描述   : 老化事件
  函数列表   :
  修改历史   :
  1.日    期   : 2016年5月26日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>

/*****************************************************************************
 函 数 名  : RCT_Expire_EventHandlerCb
 功能描述  : 老化处理回调，通过网络事件，进入到该回调，进行老化回调函数处理
 输入参数  : VOID *pvHandler  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
*****************************************************************************/
VOID RCT_Expire_EventHandlerCb(VOID *pvHandler)
{
    RCT_REACTOR_EXPIREHANDLER_S     *pstExpireHandler   = NULL;
    RCT_REACTOR_EXPIRE_OPT_S        *pstExpireOpsTmp    = NULL;
    RCT_REACTOR_EXPIRE_OPT_S        *pstExpireOpsSave   = NULL;
    rtm_reactor_expireops_cb         pfExpirecb         = NULL;       
    VOS_DLIST_NODE_S                *pstList            = NULL;
    ULONG                uiVal          = 0;
    LONG                 lRet           = 0;
     
    if ( NULL == pvHandler )
    {
        DbgRctModuleError("param error!");
        return;
    }

    pstExpireHandler = (RCT_REACTOR_EXPIREHANDLER_S *)pvHandler;

    lRet = VOS_EventfdRead(pstExpireHandler->lExprEventfd, &uiVal, sizeof(ULONG));
    if( VOS_ERR == lRet)
    {
        DbgRctModuleError("Expire: event fd:[%d] read error!ret=%d",
            pstExpireHandler->lExprEventfd, lRet);
        return;
    }
    
    /*先检查老化链表是不是为空*/
    pstList = &pstExpireHandler->stExpireList;
    
    if ( VOS_OK == VOS_Node_IsEmpty(pstList) )
    {
        return;
    }    

    /*遍历所有注册老化的回调函数*/
    #if 1
    VOS_DLIST_FOR_EACH_ENTRY(pstExpireOpsTmp, &pstExpireHandler->stExpireList, RCT_REACTOR_EXPIRE_OPT_S, stNode)
    {
        if ( NULL == pstExpireOpsTmp )
        {
            DbgRctModuleError("ASSERT error!");
            break;
        }

        pfExpirecb = pstExpireOpsTmp->stExpirecb.pvcbFunc;

        /*如果该老化节点已经设置了老化标记，那么执行老化回调*/
        if ( (NULL != (pfExpirecb) ) && (pstExpireOpsTmp->ulExpireConfirm == VOS_TRUE) )
        {   
            /*注意不能直接脱链,先保留该节点的下一个节点*/
            pstExpireOpsSave = VOS_DLIST_ENTRY(pstExpireOpsTmp->stNode.prev, RCT_REACTOR_EXPIRE_OPT_S, stNode);
             
            /*直接将该老化节点先进行脱链*/
            VOS_Node_Remove((&pstExpireOpsTmp->stNode));
            
            /*再执行老化函数，防止老化节点中先进行脱链*/
            ((rtm_reactor_expireops_cb)pfExpirecb)(pstExpireOpsTmp->stExpirecb.pvData);

            /*进行可靠性的节点老化*/
            RCT_Reactor_MsgQueLockNodeExpired(pstExpireOpsTmp->stExpirecb.pvData);
            
            /*脱离链表后，继续下一个节点循环*/
            pstExpireOpsTmp = pstExpireOpsSave;
            continue;
        }
    }
    #else /*实现和上面一样的效果*/
    LONG                 lIndex          = 0;
    pstExpireOpsTmp = VOS_DLIST_ENTRY(pstExpireHandler->stExpireList.next, RCT_REACTOR_EXPIRE_OPT_S, stNode);
    
    do
    {
        if ( NULL == pstExpireOpsTmp )
        {
            DbgRctModuleError("ASSERT error!");
            break;
        }

        pfExpirecb = pstExpireOpsTmp->stExpirecb.pvcbFunc;

        /*如果该老化节点已经设置了老化标记，那么执行老化回调,并进行脱链*/
        if ( (NULL != (pfExpirecb) ) && (pstExpireOpsTmp->ulExpireConfirm == VOS_TRUE) )
        {   
            /*再执行老化函数，防止老化节点中先进行脱链*/
            ((rtm_reactor_expireops_cb)pfExpirecb)(pstExpireOpsTmp->stExpirecb.pvData);
            
            /*注意不能直接脱链,先保留该节点的下一个节点*/
            pstExpireOpsSave = VOS_DLIST_ENTRY(pstExpireOpsTmp->stNode.next, RCT_REACTOR_EXPIRE_OPT_S, stNode);
             
            /*然后直接将该老化节点先进行脱链*/
            VOS_Node_Remove((&pstExpireOpsTmp->stNode));

            pstExpireOpsTmp = pstExpireOpsSave;
        }
    }while(pstExpireOpsTmp != &pstExpireHandler->stExpireList);
    

    #endif

    
    return;
}


/*****************************************************************************
 函 数 名  : RCT_Expire_EventHandlerCreate
 功能描述  : 创建老化事件管理器
 输入参数  : RCT_EVTREACTOR_S *pstRctReactor  
             ULONG ulExpirePort               
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 

*****************************************************************************/
LONG RCT_Expire_EventHandlerCreate(RCT_EVTREACTOR_S *pstRctReactor)
{
    
    RCT_REACTOR_EXPIREHANDLER_S *pstExpireHandler = NULL;
    
    if ( NULL == pstRctReactor )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    VOS_Mem_Zero((CHAR *)(&pstRctReactor->stExpireHandler), sizeof(RCT_REACTOR_EXPIREHANDLER_S));
    
    pstExpireHandler = (&pstRctReactor->stExpireHandler);
    
    pstExpireHandler->pstRctEvtReactor = pstRctReactor;
    
    /*创建本老化管理器的socket*/
    pstExpireHandler->lExprEventfd = VOS_EventfdCreate(0);
    if ( VOS_SOCKET_INVALID ==  pstExpireHandler->lExprEventfd )
    {
        DbgRctModuleError("Expire eventfd create error!");
        return VOS_ERR;
    }

    /*初始化老化链表*/
    VOS_Node_Init(&pstExpireHandler->stExpireList);

    /*将老化回调注册到网络事件中*/
    RCT_REACTOR_NETEVTOPTS_INIT(
                &pstExpireHandler->stNetEvtOps,
                pstExpireHandler->lExprEventfd,
                VOS_EPOLL_MASK_POLLIN,
                RCT_Expire_EventHandlerCb,
                NULL,
                pstExpireHandler);

    /*将该老化socket回调先注册网络事件中*/
    if(VOS_ERR == RCT_Net_EventOptsRegister(pstRctReactor, &pstExpireHandler->stNetEvtOps))
    {
       DbgRctModuleError("RCT_Net_EventOptsRegister error!");
       VOS_EventfdClose(pstExpireHandler->lExprEventfd);
       return VOS_ERR;
    }

    
    /*更新配置*/
    pstRctReactor->pstPthCfgCtx->stEvtInfo.ulEventFdExpr  = pstExpireHandler->lExprEventfd;

    //DbgRctModuleEvent("Expire event handler create OK!");
    DbgRctModuleEvent("Expire event handler create OK!eventfd=[%d]", pstRctReactor->pstPthCfgCtx->stEvtInfo.ulEventFdExpr);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_Expire_EventHandlerRelease
 功能描述  : 老化处理器内存释放
 输入参数  : RCT_REACTOR_EXPIREHANDLER_S *pstExpireHandler  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
*****************************************************************************/
LONG RCT_Expire_EventHandlerRelease(RCT_REACTOR_EXPIREHANDLER_S *pstExpireHandler)
{
    if(NULL == pstExpireHandler)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }

    (VOID)RCT_Net_EventOptsUnRegister(pstExpireHandler->pstRctEvtReactor, &pstExpireHandler->stNetEvtOps);

    /*释放本身网络事件的socket*/
    VOS_EventfdClose(pstExpireHandler->lExprEventfd);
    
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_Expire_EventOptsRegister
 功能描述  : 提供内部使用的老化注册函数
 输入参数  : RCT_EVTREACTOR_S *pstRctReactor          
             RCT_REACTOR_EXPIRE_OPT_S  *pstExpireOps  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
*****************************************************************************/
LONG RCT_Expire_EventOptsRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_EXPIRE_OPT_S  *pstExpireOps)
{
    RCT_REACTOR_EXPIREHANDLER_S *pstExpireHandler = NULL;
    VOS_DLIST_NODE_S *pstList    = NULL;
    VOS_DLIST_NODE_S *pstNode = NULL;
    
    if(NULL == pstRctReactor || NULL == pstExpireOps )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    pstExpireHandler = (&pstRctReactor->stExpireHandler);
    pstExpireOps->ulExpireConfirm = VOS_FALSE;
    pstNode = &pstExpireOps->stNode;
    
    /*将节点添加到老化链表中*/
    pstList = &pstExpireHandler->stExpireList;
    VOS_Node_TailAdd(pstList, pstNode);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_Expire_EventOptsUnRegister
 功能描述  : 单个节点去注册老化
 输入参数  : RCT_EVTREACTOR_S *pstRctReactor          
             RCT_REACTOR_EXPIRE_OPT_S  *pstExpireOps  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
*****************************************************************************/
LONG RCT_Expire_EventOptsUnRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_EXPIRE_OPT_S  *pstExpireOps)
{
    VOS_DLIST_NODE_S *pstNode = NULL;
    
    if ( NULL == pstRctReactor 
        || NULL == pstExpireOps )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }

    pstNode = &pstExpireOps->stNode;
        
    VOS_Node_Remove(pstNode);

    return VOS_OK;
}





