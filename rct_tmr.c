/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_tim.c
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年2月17日
  最近修改   :
  功能描述   : 定时器事件
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月17日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>


/*****************************************************************************
 函 数 名  : RCT_Timer_EventHandlerCb
 功能描述  : 定时器处理器
 输入参数  : VOID *pvHandler  ---该线程的定时管理器处理函数
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年7月27日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_Timer_EventHandlerCb(VOID *pvHandler)
{
    RCT_REACTOR_TIMERHANDLER_S      *pstTimerHandler    = NULL;
    RCT_REACTOR_TIMER_OPT_S         *pstTimerOpsTmp     = NULL;
    RCT_REACTOR_TIMER_OPT_S         *pstTimerOpsSave    = NULL;
    rtm_reactor_timerops_cb          pfTimercb          = NULL;
    LONG                lRet            = 0;
    ULONG               ulTimeStamp     = 0;
    ULONG               uiVal           = 0;
    
    if ( NULL == pvHandler )
    {
        DbgRctModuleError("param error!");
        return;
    }

    pstTimerHandler = (RCT_REACTOR_TIMERHANDLER_S *)pvHandler;
    
    lRet = VOS_EventfdRead(pstTimerHandler->lTimrEventfd, &uiVal, sizeof(ULONG));
    if( VOS_ERR == lRet)
    {
        DbgRctModuleError("Timer: event fd:[%d] read error!ret=%d",
            pstTimerHandler->lTimrEventfd, lRet);
        return;
    }
    
    if ( VOS_OK == VOS_Node_IsEmpty(&pstTimerHandler->stTimerList) )
    {
        //DbgRctModuleError("pstTimerHandler->stTimerList=[%p], next[%p] is mepty!", pstList, pstTimerHandler->stTimerList.next);
        return;
    }    
    
    VOS_DLIST_FOR_EACH_ENTRY(pstTimerOpsTmp, &pstTimerHandler->stTimerList, RCT_REACTOR_TIMER_OPT_S, stTimerNode)
    {
        if ( NULL == pstTimerOpsTmp )
        {
            DbgRctModuleError("continue!");
            break;
        }

        /*业务定时器设置的超时时间,超时计算以该时间为准(ulTimeout为次数)*/
        /*该定时器时间戳, 每次都会进行重新计算, 用于和上面的超时时间进行对比*/
        ulTimeStamp = pstTimerOpsTmp->ulTimeStamp;
        
        pfTimercb     =  pstTimerOpsTmp->stTimercb.pvcbFunc;
        /*一旦时间戳大于超时时间，那么就运行定时器*/
        if ( (NULL != (pfTimercb) ) && (ulTimeStamp >= pstTimerOpsTmp->ulTimerOut))
        {
            pstTimerOpsTmp->ulTimeStamp = 0;
            /*先运行一次定时器回调函数*/
            ((rtm_reactor_timerops_cb)pfTimercb)(pstTimerOpsTmp->stTimercb.pvData);
            if ( RCT_TIMER_TYPE_ONCE == pstTimerOpsTmp->ulTimeType )
            {
                /*然后如果是单个定时器需要从链表中摘除，先保存前一个节点*/
                pstTimerOpsSave = VOS_DLIST_ENTRY(pstTimerOpsTmp->stTimerNode.prev, RCT_REACTOR_TIMER_OPT_S, stTimerNode);
                /*然后删除本节点*/
                VOS_Node_Remove(&pstTimerOpsTmp->stTimerNode);
                /*脱离链表后，保留上一次的节点，下一次循环就会进入到下一个新的节点*/
                pstTimerOpsTmp = pstTimerOpsSave;
                continue;
            }
        }
        pstTimerOpsTmp->ulTimeStamp++;
    }
    return;
}

/*****************************************************************************
 函 数 名  : RCT_Timer_EventHandlerCreate
 功能描述  : 定时器触发器的创建
 输入参数  : RCT_EVTREACTOR_S *pstRctReactor  ---触发管理器
                          ULONG ulTimePort                           ---该线程所在触发器的定时器UDP接收端口
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年7月27日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Timer_EventHandlerCreate(RCT_EVTREACTOR_S *pstRctReactor)
{
    RCT_REACTOR_TIMERHANDLER_S *pstTimerHandler = NULL;
    ULONG                       ulSize          = 0;
    
    if(NULL == pstRctReactor)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    VOS_Mem_Zero((CHAR *)(&pstRctReactor->stTimerHandler), sizeof(RCT_REACTOR_TIMERHANDLER_S));
    
    pstTimerHandler = (&pstRctReactor->stTimerHandler);
    
    pstTimerHandler->pstRctEvtReactor = pstRctReactor;
    
    ulSize = sizeof(RCT_REACTOR_TIMER_OPT_S *) * (RCT_TIMER_ITEMS_NUM + 1);
    
    pstTimerHandler->apstTimerOps = (RCT_REACTOR_TIMER_OPT_S **)VOS_Malloc(RCT_MID_SID_EVT, ulSize);
    if ( NULL == pstTimerHandler->apstTimerOps )
    {
        DbgRctModuleError("Rct timer handler event option malloc error!");
        return VOS_ERR;
    }
    
    VOS_Mem_Zero((CHAR *)pstTimerHandler->apstTimerOps, ulSize);

    pstTimerHandler->lTimrEventfd = VOS_EventfdCreate(0);
    if ( VOS_ERR ==  pstTimerHandler->lTimrEventfd )
    {
        DbgRctModuleError("eventfd create error!");
        VOS_Free((CHAR *)(pstTimerHandler->apstTimerOps));
        return VOS_ERR;
    }

    VOS_Node_Init(&pstTimerHandler->stTimerList);

    /*定时器回调函数注册网络事件*/
    RCT_REACTOR_NETEVTOPTS_INIT(
                &pstTimerHandler->stNetEvtOps,
                pstTimerHandler->lTimrEventfd,
                VOS_EPOLL_MASK_POLLIN,
                RCT_Timer_EventHandlerCb,
                NULL,
                pstTimerHandler);

    /*为该socket注册网络事件*/
    if(VOS_ERR == RCT_Net_EventOptsRegister(pstRctReactor, &pstTimerHandler->stNetEvtOps))
    {
       DbgRctModuleError("RCT_Reactor_NetEvtOptsRegister error!");
       VOS_Free((CHAR *)(pstTimerHandler->apstTimerOps));
       VOS_SOCK_Close(pstTimerHandler->lTimrEventfd);
       return VOS_ERR;
    }

    /*更新配置*/
    pstRctReactor->pstPthCfgCtx->stEvtInfo.ulEventFdTimer = pstTimerHandler->lTimrEventfd;

    //VOS_Printf("Timer event handler create OK! TimerEventfd=[%d]", pstTimerHandler->lTimrEventfd);
    return VOS_OK;
}


/*释放处理器*/
LONG RCT_Timer_EventHandlerRelease(RCT_REACTOR_TIMERHANDLER_S *pstTimerHandler)
{
    
    if(NULL == pstTimerHandler)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    (VOID)RCT_Net_EventOptsUnRegister(pstTimerHandler->pstRctEvtReactor, &pstTimerHandler->stNetEvtOps);
    
    VOS_Free((CHAR *)(pstTimerHandler->apstTimerOps));

    VOS_EventfdClose(pstTimerHandler->lTimrEventfd);
    
    return VOS_OK;
}


/*单个注册定时器*/
LONG RCT_Timer_EventOptsRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_TIMER_OPT_S *pstTimeOps)
{
    RCT_REACTOR_TIMERHANDLER_S *pstTimerHandler = NULL;
    VOS_DLIST_NODE_S *pstList      = NULL;
    VOS_DLIST_NODE_S *pstNode   = NULL;
    ULONG                     ulTimerID = 0;
    
    
    if ( NULL == pstRctReactor 
        || NULL == pstTimeOps )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    pstTimerHandler = (&pstRctReactor->stTimerHandler);

    for( ulTimerID=0; ulTimerID < RCT_TIMER_ITEMS_NUM; ulTimerID++ )
    {
        if ( pstTimerHandler->apstTimerOps[ulTimerID] == NULL )
        {
            pstTimerHandler->apstTimerOps[ulTimerID] = pstTimeOps;
            
            break;
        }
    }

    /*定时器已经满*/
    if ( ulTimerID == RCT_TIMER_ITEMS_NUM-1 )
    {
        DbgRctModuleError("this timer option has been max limited!");
        return VOS_ERR;
    }
    
    pstTimeOps->ulTimerID = ulTimerID;
    
    /*将该节点加入链表*/
    pstList = &pstTimerHandler->stTimerList;
    pstNode = &pstTimeOps->stTimerNode;

    VOS_Node_Init(pstNode);
    VOS_Node_TailAdd(pstList, pstNode);
    
    return VOS_OK;
}


/*定时器去注册*/
LONG RCT_Timer_EventOptsUnRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_TIMER_OPT_S *pstTimeOps)
{
    RCT_REACTOR_TIMERHANDLER_S *pstTimerHandler = NULL;
    VOS_DLIST_NODE_S *                  pstNode     = NULL;
    ULONG                                       ulTimerID   = 0;
    
    if(NULL == pstRctReactor || NULL == pstTimeOps )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }

    pstTimerHandler = (&pstRctReactor->stTimerHandler);

    ulTimerID = pstTimeOps->ulTimerID;

    pstTimerHandler->apstTimerOps[ulTimerID] = NULL;

    pstNode = &pstTimeOps->stTimerNode;
    
    VOS_Node_Remove(pstNode);
    
    return VOS_OK;
}


