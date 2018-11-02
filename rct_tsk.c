/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_tsk.c
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年2月15日
  最近修改   :
  功能描述   : 主任务创建
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月15日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>


/*每个线程自己的触发器*/
__thread  RCT_EVTREACTOR_S      *g_th_pstReactor = NULL;

/*每个线程的信息保存，主要是信息映射，用于快速查询*/
static PRCT_PTHCTX_INFO_S         g_pstPthMapCfgCtx = NULL;

/*全局信息: 注册的业务处理上下文回调初始化信息*/
static RCT_TASKCONF_INFO_S       g_stArryRegInfo;

/*每个线程完成时候的，通知同步, 线程一个个的按序初始化*/
VOS_SM_T    g_stTaskCompelete = {0};


/*****************************************************************************
 函 数 名  : RCT_Task_MainCompeleteNotify
 功能描述  : 同步通知完成
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月17日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_Task_MainCompeleteNotify()
{
    VOS_SM_V(&g_stTaskCompelete);
}

/*****************************************************************************
 函 数 名  : RCT_Task_MainCompeleteWaitfor
 功能描述  : 线程等待完成
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月17日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_Task_MainCompeleteWaitfor()
{
    VOS_SM_P(&g_stTaskCompelete,0);
}

/*****************************************************************************
 函 数 名  : RCT_Task_Main
 功能描述  : 每个线程的触发器的主任务循环
 输入参数  : VOID *pstConf  --根据配置来进行本线程的任务上下文初始化
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年7月26日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_Task_Main(VOID *pstRctInfo)
{
    LONG                lRet           =    VOS_ERR;
    RCT_INDEXINFO_S     *pstIndexInfo  =    pstRctInfo;
    PRCT_INICFG_CTX_S    pstCfgCtx     =    NULL;
    
    if ( NULL == pstRctInfo )
    {
        DbgRctModuleError("param error!");
        return;
    }

    if ( VOS_ERR == RCT_InitPthCfgCtxCreate(&pstCfgCtx, pstIndexInfo) )
    {
        DbgRctModuleError("param error!");
        return;
    }
    
    /*创建Epoll触发器对象*/
    if ( VOS_ERR == RCT_Task_ReactorCreate(pstCfgCtx) )
    { 
        DbgRctModuleError("RCT_Reactor_Create error!");
        return;
    }

    /*将配置保存到全局的记录信息中*/
    if ( VOS_ERR == RCT_TaskPthCtxRecord(pstCfgCtx)  )
    {
        DbgRctModuleError("RCT_PthreadTaskInitRun error!");
        (VOID)RCT_Task_ReactorRelease();
        return;
    }
    
    /*线程任务初始化, 注册的业务初始化函数*/
    if (VOS_ERR == RCT_TaskPthreadArrayInitRun(pstCfgCtx->stIndexInfo.ulRctType) )
    {
        DbgRctModuleError("RCT_PthreadTaskInitRun error!");
        (VOID)RCT_Task_ReactorRelease();
        return;
    }
    
    DbgRctModuleEvent("Pthread[type=%d] Init OK! Readry to EventWaiting...!");
    
    VOS_MSleep(100);
    RCT_Task_MainCompeleteNotify();
    
    while(VOS_TRUE)
    {   
        lRet = RCT_Task_ReactorWaitAndDispatch(g_th_pstReactor);
        if(VOS_ERR == lRet )
        {
            DbgRctModuleError("RCT_Reactor_WaitAndDispatch !error!,epollid=[%08x]!", g_th_pstReactor->pstPthCfgCtx->ulEpollId);
            break;
        }
    }

    DbgRctModuleError("Something wrong!This pthread [type=%d] has error!", g_th_pstReactor->pstPthCfgCtx->stIndexInfo.ulRctType);
    
    (VOID)RCT_Task_ReactorRelease();
    return;
}

/*****************************************************************************
 函 数 名  : RCT_Task_ReactorCreate
 功能描述  : RCT每个线程都需要创建出的EPOLL
 输入参数  : RCT_INICFG_CTX_S *pstRctCfgInfo  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年7月26日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Task_ReactorCreate(RCT_INICFG_CTX_S *pstRctCfgInfo)
{
    RCT_EVTREACTOR_S *pstRctReactor = NULL;

    if ( NULL == pstRctCfgInfo )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }

    /*先创建一个总管理器，也就是EPOLL*/
    pstRctReactor = (RCT_EVTREACTOR_S *)VOS_Malloc(RCT_MID_SID_TSK, sizeof(RCT_EVTREACTOR_S));
    if(NULL == pstRctReactor)
    {
        DbgRctModuleError("malloc error!");
        return VOS_ERR;
    }

    VOS_Mem_Zero((CHAR *)pstRctReactor, sizeof(RCT_EVTREACTOR_S));
    /*保存到线程变量, 主要给消息处理用, 获取队列信息*/
    pstRctReactor->pstPthCfgCtx     = pstRctCfgInfo;
    /*自身保存可以继承*/
    pstRctReactor->pstRctEvtReactor = pstRctReactor;

    /*本线程的EPOLL, 每个线程都有一个EPOLL, 创建的单次关心事件数*/
    pstRctCfgInfo->ulEpollId  = VOS_EPOLL_Create(pstRctCfgInfo->ulEpollWaitNum);
    if ( VOS_EPOLL_INVALID == pstRctCfgInfo->ulEpollId )
    {
        VOS_Free((CHAR *)pstRctReactor);
        DbgRctModuleError("VOS_EPOLL_Create error!");
        return VOS_ERR;
    }
    
    pstRctCfgInfo->stEvtInfo.ulPid             = VOS_GetSelfPhreadID();

    /*1 . 网络EPOLL事件,所有事件的基础*/
    if(VOS_ERR == RCT_Net_EventHandlerCreate(pstRctReactor) )
    {
        DbgRctModuleError("RCT_Net_EventHandlerCreate error!");
        VOS_EPOLL_Release(pstRctCfgInfo->ulEpollId);
        VOS_Free((CHAR *)pstRctReactor);
        return VOS_ERR;
    }
    
    /*2. 定时器 事件*/
    if ( VOS_ERR == RCT_Timer_EventHandlerCreate(pstRctReactor) )
    {
        DbgRctModuleError("RCT_Timer_EventHandlerCreate error!");
        RCT_Net_EventHandlerRelease(&pstRctReactor->stNetEvtHandler);
        VOS_EPOLL_Release(pstRctCfgInfo->ulEpollId);
        VOS_Free((CHAR *)pstRctReactor);
        return VOS_ERR;
    }
  
    /*3.老化事件*/
    if ( VOS_ERR == RCT_Expire_EventHandlerCreate(pstRctReactor) )
    {
        DbgRctModuleError("RCT_Expire_EventHandlerCreate error!");
        RCT_Net_EventHandlerRelease(&pstRctReactor->stNetEvtHandler);
        RCT_Timer_EventHandlerRelease(&pstRctReactor->stTimerHandler);
        VOS_EPOLL_Release(pstRctCfgInfo->ulEpollId);
        VOS_Free((CHAR *)pstRctReactor);
        return VOS_ERR;
    }

    /*4. 线程间消息: 本线程也有一个socket*/
    if ( VOS_ERR == RCT_Reactor_MgtHandlerCreate(pstRctReactor) )
    {
        DbgRctModuleError("RCT_Reactor_MsgHandlerCreate error!");
        RCT_Net_EventHandlerRelease(&pstRctReactor->stNetEvtHandler);
        RCT_Timer_EventHandlerRelease(&pstRctReactor->stTimerHandler);
        RCT_Expire_EventHandlerRelease(&pstRctReactor->stExpireHandler);
        VOS_EPOLL_Release(pstRctCfgInfo->ulEpollId);
        VOS_Free((CHAR *)pstRctReactor);
        return VOS_ERR;
    }

    g_th_pstReactor = pstRctReactor;

    DbgRctModuleEvent("Rct Create epollId=%d, pthreadID=%08x successful!", 
        pstRctCfgInfo->ulEpollId, pstRctCfgInfo->stEvtInfo.ulPid);
    
    return VOS_OK;
}


/*将EPOLL触发器所在的线程资源释放*/
LONG RCT_Task_ReactorRelease()
{
    RCT_EVTREACTOR_S *pstRctReactor = g_th_pstReactor;

    if(NULL == pstRctReactor)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
 
    if ( VOS_ERR == RCT_Net_EventHandlerRelease(&pstRctReactor->stNetEvtHandler))
    {
        DbgRctModuleError("RCT_Net_EventHandlerRelease error!");
    }
    
    if ( VOS_ERR == RCT_Timer_EventHandlerRelease(&pstRctReactor->stTimerHandler))
    {
        DbgRctModuleError("RCT_Timer_EventHandlerRelease error!");
    }

    if ( VOS_ERR == RCT_Expire_EventHandlerRelease(&pstRctReactor->stExpireHandler))
    {
        DbgRctModuleError("RCT_Expire_EventHandlerRelease error!");
    }

    if ( VOS_ERR == RCT_Reactor_MgtHandlerRelease(&pstRctReactor->stMgtHandler))
    {
        DbgRctModuleError("RCT_Expire_EventHandlerRelease error!");
    }
    
    if ( NULL != pstRctReactor->pstPthCfgCtx )    
    {
        VOS_EPOLL_Release(pstRctReactor->pstPthCfgCtx->ulEpollId);
        RCT_InitPthCfgCtxRelease(pstRctReactor->pstPthCfgCtx);
        pstRctReactor->pstPthCfgCtx = NULL;
    }
    
    VOS_Free((CHAR *)pstRctReactor);
    
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_Task_ReactorGetPthIndexInfo
 功能描述  : 获取线程类型和索引信息
 输入参数  : RCT_INDEXINFO_S *pstIndexInfo  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Task_ReactorGetPthIndexInfo(RCT_INDEXINFO_S *pstIndexInfo)
{
    if ( NULL == g_th_pstReactor
        || NULL == g_th_pstReactor->pstPthCfgCtx )
    {
        return VOS_ERR;
    }

    pstIndexInfo->ulRctType     = g_th_pstReactor->pstPthCfgCtx->stIndexInfo.ulRctType;
    pstIndexInfo->ulPthSubIndex = g_th_pstReactor->pstPthCfgCtx->stIndexInfo.ulPthSubIndex;
    pstIndexInfo->ulArryIndex   = g_th_pstReactor->pstPthCfgCtx->stIndexInfo.ulArryIndex;
    pstIndexInfo->ulMsgEventFd  = g_th_pstReactor->pstPthCfgCtx->stIndexInfo.ulMsgEventFd;

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_Task_ReactorEvtInfoGet
 功能描述  : 获取自己当前线程的通信矩阵信息
 输入参数  : RCT_COMMUTEINFO_S *pstEvtInfo  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Task_ReactorGetCommunInfo(RCT_COMMUTEINFO_S *pstEvtInfo)
{
    if ( NULL == g_th_pstReactor
        || NULL == g_th_pstReactor->pstPthCfgCtx )
    {
        return VOS_ERR;
    }

    pstEvtInfo->ulPid           = g_th_pstReactor->pstPthCfgCtx->stEvtInfo.ulPid;
    pstEvtInfo->ulEventFdMsg    = g_th_pstReactor->pstPthCfgCtx->stEvtInfo.ulEventFdMsg;
    pstEvtInfo->ulEventFdTimer  = g_th_pstReactor->pstPthCfgCtx->stEvtInfo.ulEventFdTimer;
    pstEvtInfo->ulEventFdExpr   = g_th_pstReactor->pstPthCfgCtx->stEvtInfo.ulEventFdExpr;
    pstEvtInfo->pstQueMgrCtx    = g_th_pstReactor->pstPthCfgCtx->stEvtInfo.pstQueMgrCtx;
    
    return VOS_OK;
}

/*EPOLL等待事件发生*/
LONG RCT_Task_ReactorWait(RCT_EVTREACTOR_S *pstRctEvtReactor)
{
    LONG lRet      = 0;
    LONG lEpollID = 0;

    if ( NULL == pstRctEvtReactor )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

    lEpollID = pstRctEvtReactor->pstPthCfgCtx->ulEpollId;

    /*一直等待直到有事件发生*/
    lRet = VOS_EPOLL_Wait(lEpollID, &pstRctEvtReactor->stNetEvtHandler.stEpollWait, VOS_EPOLL_TIME_INFININ);
    if ( VOS_EPOLL_ERR == lRet )
    {
        DbgRctModuleError("SSN_EPOLL_Wait error!");
        return VOS_ERR;
    }
    
    return lRet;
}


/*EPOLL事件分发器*/
LONG RCT_Task_ReactorDispatch(RCT_EVTREACTOR_S *pstRctEvtReactor)
{
    ULONG ulIndex    = 0;
    ULONG ulEvtNum = 0;
    LONG  lSockfd     = 0;
    RCT_REACTOR_NETEVTHANDLER_S *pstNetEvtHandler = NULL;
    RCT_REACTOR_NETEVT_OPT_S      *pstNetEvtOps       = NULL;
    VOS_EPOLL_EVENT_S                   *pstEvent                  = NULL;
    
    if ( NULL == pstRctEvtReactor  )
    {
        DbgRctModuleError("param error");
        return VOS_ERR; 
    }
    
    pstNetEvtHandler = &pstRctEvtReactor->stNetEvtHandler;

    /*获取当前发生的数量*/
    ulEvtNum = pstNetEvtHandler->stEpollWait.ulEvtNum;

    /*处理网络事件*/
    for(ulIndex=0;ulIndex < ulEvtNum; ulIndex++)
    {    
        /*获取单个事件*/
        pstEvent        =  &pstNetEvtHandler->stEpollWait.astEevent[ulIndex];
        lSockfd          =  pstEvent->lSockFd;
        pstNetEvtOps = pstNetEvtHandler->apstEpollEvtOps[lSockfd];

        /*不应该发生: 没有注册的网络事件*/
        if ( NULL == pstNetEvtOps )
        {
            DbgRctModuleError("RCT_Task_ReactorWaitAndDispatch not found register net event function!");
            //There is must be something system wrong!
            continue;
        }

        /*处理对应的事件*/
        if ( pstEvent->ulEventMask & VOS_EPOLL_MASK_POLLIN )
        {
            ((rtm_reactor_netevt_recv_cb)(pstNetEvtOps->stRecv.pvcbFunc))(pstNetEvtOps->stRecv.pvData);
        }
        
        if ( pstEvent->ulEventMask & VOS_EPOLL_MASK_POLLOUT )
        {
            ((rtm_reactor_netevt_send_cb)(pstNetEvtOps->stSend.pvcbFunc))(pstNetEvtOps->stSend.pvData);
        }
    }
    
    return VOS_OK;
}



/*该EPOLL的触发器的主函数*/
LONG RCT_Task_ReactorWaitAndDispatch(RCT_EVTREACTOR_S *pstRctEvtReactor)
{
    ULONG  ulIndex         = 0;
    LONG    lSockfd         = 0;
    ULONG  ulEvtNum      = 0;
    LONG    lEpollID         = 0;
    LONG    lRet              = VOS_ERR;
    RCT_REACTOR_NETEVTHANDLER_S  *pstNetEvtHandler = NULL;
    RCT_REACTOR_NETEVT_OPT_S       *pstNetEvtOps       = NULL;
    VOS_EPOLL_EVENT_S                    *pstEvent              = NULL;
    
    if ( NULL == pstRctEvtReactor  )
    {
        DbgRctModuleError("param error");
        return VOS_ERR; 
    }
    
    lEpollID = pstRctEvtReactor->pstPthCfgCtx->ulEpollId;

    lRet = VOS_EPOLL_Wait(lEpollID, &pstRctEvtReactor->stNetEvtHandler.stEpollWait, VOS_EPOLL_TIME_200S);
    if ( VOS_EPOLL_ERR == lRet )
    {
        DbgRctModuleError("VOS_EPOLL_Wait error!");
        return VOS_ERR;
    }

    pstNetEvtHandler = &pstRctEvtReactor->stNetEvtHandler;
    ulEvtNum            = pstNetEvtHandler->stEpollWait.ulEvtNum;
        
    for(ulIndex=0;ulIndex < ulEvtNum; ulIndex++)
    {    
        pstEvent        = &pstNetEvtHandler->stEpollWait.astEevent[ulIndex];
        lSockfd          = pstEvent->lSockFd;
        pstNetEvtOps = pstNetEvtHandler->apstEpollEvtOps[lSockfd];

        if ( NULL == pstNetEvtOps )
        {
            DbgRctModuleError("RCT_Task_ReactorWaitAndDispatch not found register net event function!Sockfd=[%d]", lSockfd);
            //There is must be something system wrong
            continue;
        }
        
        if ( pstEvent->ulEventMask & VOS_EPOLL_MASK_POLLIN )
        {
            ((rtm_reactor_netevt_recv_cb)(pstNetEvtOps->stRecv.pvcbFunc))(pstNetEvtOps->stRecv.pvData);
        }
        
        if ( pstEvent->ulEventMask & VOS_EPOLL_MASK_POLLOUT )
        {
            ((rtm_reactor_netevt_send_cb)(pstNetEvtOps->stSend.pvcbFunc))(pstNetEvtOps->stSend.pvData);
        }
    }
    
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_TaskPthreadCfgCtxInit
 功能描述  : 线程任务全局配置上下文保存的初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月17日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_TaskPthCfgCtxInit()
{
    ULONG   ulIndex 	= 0;
	ULONG 	ulSubTypeIndex 	= 0;

    /*线程同步完成信号*/
    if( VOS_ERR == VOS_SM_Init(&g_stTaskCompelete) )
    {
        DbgRctModuleError("vos sm init error!");
        return VOS_ERR;
    }

    /*申请线程任务初始化函数的注册数组*/
    g_pstPthMapCfgCtx = (RCT_PTHCTX_INFO_S *)VOS_Malloc(RCT_MID_SID_TSK, sizeof(RCT_PTHCTX_INFO_S));
    if ( NULL == g_pstPthMapCfgCtx )
    {
        DbgRctModuleError("malloc error!");
        return VOS_ERR;
    }

    /*任务初始化*/
    VOS_Mem_Zero((CHAR *)g_pstPthMapCfgCtx, sizeof(RCT_PTHCTX_INFO_S));

	
    for(ulIndex = 0; ulIndex <RCT_PTHMAXNUMS; ulIndex++ )
    {
        g_pstPthMapCfgCtx->pstPthCfgCtx[ulIndex]  = NULL;
    }

    /*初始化一下另外一个业务注册函数数组*/
    for(ulIndex =0; ulIndex<RCT_TYPE_NUMS; ulIndex++ )
    {
        for(ulSubTypeIndex=0;ulSubTypeIndex<RCT_SUBTYPE_MAXNUMS;ulSubTypeIndex++ )
        {
            g_stArryRegInfo.stArryBizCtxInitCb[ulIndex][ulSubTypeIndex].stpfInit.pvcbFunc   = NULL;
            g_stArryRegInfo.stArryBizCtxInitCb[ulIndex][ulSubTypeIndex].stpfInit.pvData     = NULL;
            g_stArryRegInfo.stArryBizCtxInitCb[ulIndex][ulSubTypeIndex].pfUnInit            = NULL;
        }
    }
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_TaskPthreadCfgCtxUninit
 功能描述  : 释放全局的数组
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
*****************************************************************************/
VOID RCT_TaskPthCfgCtxUninit()
{
    ULONG ulIndex = 0;
    
    if ( NULL == g_pstPthMapCfgCtx )
    {
        DbgRctModuleWarning("pthread task global var is null!");
        return;
    }

    /*释放信息*/
    for(ulIndex = 0; ulIndex <RCT_PTHMAXNUMS; ulIndex++ )
    {
        if( NULL != g_pstPthMapCfgCtx->pstPthCfgCtx[ulIndex] )
        {
            VOS_Free((CHAR *)g_pstPthMapCfgCtx->pstPthCfgCtx[ulIndex]);
            g_pstPthMapCfgCtx->pstPthCfgCtx[ulIndex] = NULL;
        }
    }

    VOS_SM_Destroy(&g_stTaskCompelete);
    
    VOS_Free((CHAR *)g_pstPthMapCfgCtx);
    g_pstPthMapCfgCtx = NULL;
}

/*****************************************************************************
 函 数 名  : RCT_TaskPthreadCtxRecord
 功能描述  : 创建每个线程的上下文记录记录信息
 输入参数  : ULONG ulType      
             ULONG ulSubIndex  
             void *pvPthCtx    
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月17日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_TaskPthCtxRecord(RCT_INICFG_CTX_S *pstPthCfgCtx)
{
    ULONG ulArryIndex = 0;

    /*当前的线程初始化索引*/
    ulArryIndex = pstPthCfgCtx->stIndexInfo.ulArryIndex;

    /*保存到每个线程的数组中*/
    if ( NULL != g_pstPthMapCfgCtx->pstPthCfgCtx[ulArryIndex] )
    {
        DbgRctModuleError("system error!");
        return VOS_ERR;
    }

	/*直接保存当前的值*/
    g_pstPthMapCfgCtx->pstPthCfgCtx[ulArryIndex]  = pstPthCfgCtx;

    VOS_InterlockedIncrement(&g_pstPthMapCfgCtx->ulPthNums);

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_TaskPthreadGetNums
 功能描述  : 获取当前的线程数量
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月19日
    作    者   : 蒋康
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_TaskPthGetNums()
{
    if ( NULL == g_pstPthMapCfgCtx )
    {
        return 0;
    }
    
    return g_pstPthMapCfgCtx->ulPthNums;
}


/*****************************************************************************
 函 数 名  : RCT_TaskPthreadSyncMapInfo
 功能描述  : 将所有的线程信息，全部进行同步, 提供全局MAP查询
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月17日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_TaskPthSetSyncMapInfo(ULONG ulRctType, ULONG ulSubIndex, ULONG ulPthIndex )
{
    RCT_COMMUTEINFO_S    *pstEvtInfo     = NULL;

    if ( NULL != g_pstPthMapCfgCtx->pstPthCfgCtx[ulPthIndex] )
    {
        /*注意：初始化的时候，一定是按照配置的相同的顺序来的,否则线程间的通信矩阵是无法对应*/
        pstEvtInfo = &g_pstPthMapCfgCtx->pstPthCfgCtx[ulPthIndex]->stEvtInfo;
        
        /*拷贝的形式保存*/
        g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].ulPid            = pstEvtInfo->ulPid;
        g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].ulEventFdExpr    = pstEvtInfo->ulEventFdExpr;
        g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].ulEventFdMsg     = pstEvtInfo->ulEventFdMsg;
        g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].ulEventFdTimer   = pstEvtInfo->ulEventFdTimer;
        g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].pstQueMgrCtx     = pstEvtInfo->pstQueMgrCtx;
#if 0
        DbgRctModuleEvent("***Communication MapInfo***: RctType=[%d]:[%d] [Timr:%d] [Expir:%d] [Mesg:%d] !",
            ulRctType, ulSubIndex, pstEvtInfo->ulEventFdTimer, pstEvtInfo->ulEventFdExpr, pstEvtInfo->ulEventFdMsg);
#endif
        DbgRctModuleEvent("***Communication MapArry***: Pid:%08x RctType=[%02d:%02d] [Timr:%02d] [Expir:%02d] [Mesg:%02d] !",
            pstEvtInfo->ulPid, ulRctType, ulSubIndex, pstEvtInfo->ulEventFdTimer, pstEvtInfo->ulEventFdExpr, pstEvtInfo->ulEventFdMsg);
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_TaskPthreadGetPthIndexInfo
 功能描述  : 获取当前线程的索引类型信息
 输入参数  : ULONG   *pulRctType            --当前的线程类型
           ULONG *pulSubIndex           --当前的线程子索引
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月19日
    作    者   : 蒋康
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_TaskPthGetPthTypeInfo(ULONG   *pulRctType, ULONG *pulSubIndex)
{
    if ( NULL == pulRctType
        || NULL == pulSubIndex )
    {
        return VOS_ERR;
    }

    *pulRctType  = g_th_pstReactor->pstPthCfgCtx->stIndexInfo.ulRctType;
    *pulSubIndex = g_th_pstReactor->pstPthCfgCtx->stIndexInfo.ulPthSubIndex;

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_TaskPthGetCommunMapInfo
 功能描述  : 获取通信矩阵的信息
 输入参数  :    ULONG ulRctType             --线程索引
            ULONG ulSubIndex            --线程索引
            RCT_COMMUTEINFO_S *pstEvtInfo    --获取的完整信息 
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月19日
    作    者   : 蒋康
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_TaskPthGetCommunMapInfo(ULONG ulRctType, ULONG ulSubIndex, RCT_COMMUTEINFO_S *pstEvtInfo)
{
    if ( 0 == g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].ulPid )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }

    if ( NULL != pstEvtInfo )
    {
        /*拷贝的形式保存*/
        pstEvtInfo->ulPid           = g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].ulPid;
        pstEvtInfo->ulEventFdExpr   = g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].ulEventFdExpr;
        pstEvtInfo->ulEventFdMsg    = g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].ulEventFdMsg;
        pstEvtInfo->ulEventFdTimer  = g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].ulEventFdTimer;
        pstEvtInfo->pstQueMgrCtx    = g_pstPthMapCfgCtx->stMapEventFdInfo[ulRctType][ulSubIndex].pstQueMgrCtx;
        
        DbgRctModuleEvent("Rct task pthread Get sync MapInfo: (pid:%d)[%d:%d] Expir=[%d], Msg=[%d], Timer=[%d]!",
            pstEvtInfo->ulPid, ulRctType, ulSubIndex, pstEvtInfo->ulEventFdExpr, pstEvtInfo->ulEventFdMsg, pstEvtInfo->ulEventFdTimer);
        return VOS_OK;
    }

    return VOS_ERR;
}




/*****************************************************************************
 函 数 名  : RCT_TaskPthreadArrayInitRegister
 功能描述  :     注册外部的线程业务初始化函数
 输入参数  :     VOS_CALLBACK pfInitCb    
             VOID *pvArg              
             ULONG ulTskIndex         
             ULONG ulType             
             VOS_CALLBACK pfUnInitCb  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
*****************************************************************************/
INT32 RCT_TaskArrayBizCtxInitRegister(VOS_CALLBACK pfInitCb, VOID *pvArg, ULONG ulRctType,ULONG ulRctSubType, VOS_CALLBACK pfUnInitCb)
{
    if ( NULL == pfInitCb 
        || NULL == pfUnInitCb )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }   

    if ( ulRctType >= RCT_TYPE_NUMS 
        || ulRctSubType >= RCT_SUBTYPE_MAXNUMS )
    {
        DbgRctModuleError("Rct type index error!");
        return VOS_ERR; 
    }

    /*根据类型进行初始化注册*/
    switch(ulRctType)
    {
        case RCT_TYPE_MP_CLI:
        case RCT_TYPE_MP_AAA:
        case RCT_TYPE_DP_TCP:
        case RCT_TYPE_DP_SSL:
        case RCT_TYPE_DP_UDPS:
        case RCT_TYPE_DP_TWORK:
        case RCT_TYPE_DP_VSNAT:
            /*配置进行赋值操作*/
            g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].stpfInit.pvcbFunc= pfInitCb;
            g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].stpfInit.pvData  = pvArg;
            g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].pfUnInit         = pfUnInitCb;
            g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].ulRctType        = ulRctType;
            g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].ulSubType        = ulRctSubType;
            break;
        default:
            break;
    }
    
    return VOS_OK;

}



/*****************************************************************************
 函 数 名  : RCT_TaskPthreadArrayInitRegister
 功能描述  : 注册外部的线程初始化函数
 输入参数  : VOS_CALLBACK pfInitCb    
             VOID *pvArg              
             ULONG ulTskIndex         
             ULONG ulType             
             VOS_CALLBACK pfUnInitCb  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
*****************************************************************************/
INT32 RCT_TaskArrayBizCtxInitUnRegister(ULONG ulRctType,  ULONG ulRctSubType)
{
    if ( ulRctType >= RCT_TYPE_NUMS
        || ulRctSubType >= RCT_SUBTYPE_MAXNUMS)
    {
        DbgRctModuleError("ulRctType is exceed RCT_TYPE_NUMS!");
        return VOS_ERR; 
    }

    if ( NULL != g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].pfUnInit )
    {
        ((VOS_CALLBACK_UNINIT)g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].pfUnInit)();
    }
    
    g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].stpfInit.pvcbFunc= NULL;
    g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].stpfInit.pvData  = NULL;
    g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].pfUnInit        = NULL;
    g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].ulRctType       = 0;
    g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulRctSubType].ulSubType       = 0;
    
    return VOS_OK;

}




/*****************************************************************************
 函 数 名  : RCT_TaskArrayBizMsgHandlerRegister
 功能描述  :    业务的消息通信注册
 输入参数  :    VOS_CALLBACK pfInitCb    
            VOID *pvArg              
            ULONG ulRctType          
            ULONG ulRctSubType       
            VOS_CALLBACK pfUnInitCb  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32  RCT_TaskArrayBizMsgHandlerRegister(ULONG ulRctType,ULONG ulRctSubType, CHAR *pcModuName, pfComMsgHandlerCallBack pfMsgHandlerCb)
{
    if ( NULL == pfMsgHandlerCb  )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }   

    if ( ulRctType >= RCT_TYPE_NUMS
        || ulRctSubType >= RCT_SUBTYPE_MAXNUMS )
    {
        DbgRctModuleError("Rct type index error!");
        return VOS_ERR; 
    }

    /*根据类型进行初始化注册*/
    switch(ulRctType)
    {
        case RCT_TYPE_MP_CLI:
        case RCT_TYPE_MP_AAA:
        case RCT_TYPE_DP_TCP:
        case RCT_TYPE_DP_SSL:
        case RCT_TYPE_DP_UDPS:
        case RCT_TYPE_DP_TWORK:
        case RCT_TYPE_DP_VSNAT:
            /*配置进行赋值操作*/
            g_stArryRegInfo.stArryBizMsgHandlerCb[ulRctType][ulRctSubType].pfMsgHandlerCb   = pfMsgHandlerCb;
            g_stArryRegInfo.stArryBizMsgHandlerCb[ulRctType][ulRctSubType].ulRctType        = ulRctType;
            g_stArryRegInfo.stArryBizMsgHandlerCb[ulRctType][ulRctSubType].ulSubType        = ulRctSubType;
            VOS_StrCpy_S(g_stArryRegInfo.stArryBizMsgHandlerCb[ulRctType][ulRctSubType].acModuleName, RCT_STR_LEN, pcModuName);
            break;
        default:
            break;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_TaskArryBizMsgHandlerRunCall
 功能描述  : 运行消息注册函数
 输入参数  : ULONG ulRctType     
             ULONG ulRctSubType  
             VOID *pvArg1        
             VOID *pvArg2        
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_TaskArryBizMsgHandlerRunCall(RCT_MQUENODE_S *pstMessage)
{
    if ( NULL == pstMessage )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR; 
    }

    if ( pstMessage->stHead.ulDstRctType >= RCT_TYPE_NUMS
        || pstMessage->stHead.ulDstRctSubType >= RCT_SUBTYPE_MAXNUMS )
    {
        DbgRctModuleError("Rct type index error!");
        return VOS_ERR; 
    }

    if ( NULL != g_stArryRegInfo.stArryBizMsgHandlerCb[pstMessage->stHead.ulDstRctType][pstMessage->stHead.ulDstRctSubType].pfMsgHandlerCb )
    {
        /*直接调用相关消息处理*/
        ((pfComMsgHandlerCallBack)g_stArryRegInfo.stArryBizMsgHandlerCb[pstMessage->stHead.ulDstRctType][pstMessage->stHead.ulDstRctSubType].pfMsgHandlerCb)(
                &pstMessage->stHead, pstMessage->pcMsgData, pstMessage->ulMsgLen);
    }
   
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_TaskArrayBizMsgHandlerUnRegister
 功能描述  : 业务消息通信去注册
 输入参数  : ULONG ulRctType     
             ULONG ulRctSubType  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32   RCT_TaskArrayBizMsgHandlerUnRegister(ULONG ulRctType, ULONG ulRctSubType)
{
    if ( ulRctType >= RCT_TYPE_NUMS
        || ulRctSubType >= RCT_SUBTYPE_MAXNUMS)
    {
        DbgRctModuleError("ulRctType is exceed RCT_TYPE_NUMS!");
        return VOS_ERR; 
    }
    
    g_stArryRegInfo.stArryBizMsgHandlerCb[ulRctType][ulRctSubType].pfMsgHandlerCb   =  NULL;
    g_stArryRegInfo.stArryBizMsgHandlerCb[ulRctType][ulRctSubType].ulRctType        = 0;
    g_stArryRegInfo.stArryBizMsgHandlerCb[ulRctType][ulRctSubType].ulSubType        = 0;
    
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_TaskPthreadArrayInitRun
 功能描述  : 每个线程任务资源初始化，线程下所有业务上下文都需要初始化, 可能有多个初始化函数
 输入参数  : ULONG ulType  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 

*****************************************************************************/
INT32 RCT_TaskPthreadArrayInitRun(ULONG ulRctType)
{
    ULONG ulSubIndex = 0;

    /*根据类型进行初始化注册*/
    if ( ulRctType >= RCT_TYPE_NUMS )
    {
        DbgRctModuleError("ultask index error!");
        return VOS_ERR; 
    }

    /*根据类型进行初始化注册*/
    switch(ulRctType)
    {
        case RCT_TYPE_MP_CLI:
        case RCT_TYPE_MP_AAA:
        case RCT_TYPE_DP_TCP:
        case RCT_TYPE_DP_UDPS:
        case RCT_TYPE_DP_TWORK:
        case RCT_TYPE_DP_VSNAT:
            /*运行该注册的业务初始化函数, 默认只有一个业务情况*/
            if( NULL != g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][RCT_SUBTYPE_SINGLE].stpfInit.pvcbFunc)
            {
                ((VOS_CALLBACK)g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][RCT_SUBTYPE_SINGLE].stpfInit.pvcbFunc)(g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][RCT_SUBTYPE_SINGLE].stpfInit.pvData);
            }
            break;
        /*目前就SSL有多个业务的情况, 做特殊处理即可*/
        case RCT_TYPE_DP_SSL:
            /*线程的业务上下文都需要调用一次*/
            for(ulSubIndex=0; ulSubIndex <RCT_SUBTYPE_SSLNUMS; ulSubIndex++ )
            {
                if( NULL != g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulSubIndex].stpfInit.pvcbFunc)
                {
                    ((VOS_CALLBACK)g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][ulSubIndex].stpfInit.pvcbFunc)(g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][RCT_SUBTYPE_SINGLE].stpfInit.pvData);
                }
            }
            break;
        default:
            break;
    }
    
    return VOS_OK;
}






