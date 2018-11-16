/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_net.c
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年5月25日
  最近修改   :
  功能描述   : 网络事件
  函数列表   :
  修改历史   :
  1.日    期   : 2016年5月25日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>



/*****************************************************************************
 函 数 名  : RCT_Net_EventHandlerCreate
 功能描述  : 网络事件处理器的创建
 输入参数  : RCT_EVTREACTOR_S *pstRctEvtReactor  --管理器
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年7月27日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Net_EventHandlerCreate(RCT_EVTREACTOR_S *pstRctEvtReactor)
{
    RCT_REACTOR_NETEVTHANDLER_S *pstNetEvtHandler = NULL;
    ULONG ulSize = 0;
    
    if(NULL == pstRctEvtReactor)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    VOS_Mem_Zero((CHAR *)(&pstRctEvtReactor->stNetEvtHandler), sizeof(RCT_REACTOR_NETEVTHANDLER_S));
    /*网络事件处理器*/
    pstNetEvtHandler = (&pstRctEvtReactor->stNetEvtHandler);    

    /*保存可以继承的触发器*/
    pstNetEvtHandler->pstRctEvtReactor = pstRctEvtReactor;

    /*稍微浪费，其实每次触发仅用了EpollWaitNum个数的OPT，
        或者可以用链表来注册也可以, 以后再测试， 常见的每个线程并发几百左右
       但是这样socketfd直接命中速度快，牺牲空间换取时间*/
    ulSize = sizeof(RCT_REACTOR_NETEVT_OPT_S *)*(VOS_SOCK_FDSIZE_MAX + 1);    
    /*创建所有socketfd注册的回调处理事件, 数据为自己所在的节点*/
    pstNetEvtHandler->apstEpollEvtOps = (RCT_REACTOR_NETEVT_OPT_S **)VOS_Malloc(RCT_MID_SID_EVT, ulSize);
    if ( NULL == pstNetEvtHandler->apstEpollEvtOps )
    {
        DbgRctModuleError("epoll event option malloc error!");
        return VOS_ERR;
    }
    
    VOS_Mem_Zero((CHAR *)pstNetEvtHandler->apstEpollEvtOps, ulSize);
    
    return VOS_OK;
}

/*释放网络管理器的所有事件, 内部事件由业务本身保证*/
LONG RCT_Net_EventHandlerRelease(RCT_REACTOR_NETEVTHANDLER_S *pstNetEvtHandler)
{
    if(NULL == pstNetEvtHandler)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    VOS_Free((CHAR *)pstNetEvtHandler->apstEpollEvtOps);
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_Net_EventOptsRegister
 功能描述  : 提供的RCT模块内部使用的网络事件注册
                 注意: 外部使用的，都需要将参数变为VOID
 输入参数  :    RCT_EVTREACTOR_S *pstRctReactor      
             RCT_REACTOR_NETEVT_OPT_S *pstEvtOps  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年7月27日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_Net_EventOptsRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_NETEVT_OPT_S *pstEvtOps)
{
    RCT_REACTOR_NETEVTHANDLER_S *pstNetEvtHandler = NULL;

    if ( NULL == pstRctReactor
        || NULL == pstEvtOps)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    if ( VOS_SOCK_FDSIZE_MAX < pstEvtOps->lSockfd 
        || VOS_SOCKET_INVALID >= pstEvtOps->lSockfd )
    {
        DbgRctModuleError("system error!");
        return VOS_ERR;
    }
    
    pstNetEvtHandler = &pstRctReactor->stNetEvtHandler;

    /*因为socketfd不可能重复，因此不应该发生该情况
       除非之前没有unRegister进行释放*/
    if ( NULL != pstNetEvtHandler->apstEpollEvtOps[pstEvtOps->lSockfd] )
    {
        DbgRctModuleError("ASSERT() error!");
        //System must be something wrong
        return VOS_ERR;
    }

    /*将网络节点进行注册*/
    pstNetEvtHandler->apstEpollEvtOps[pstEvtOps->lSockfd] = pstEvtOps;

    if ( VOS_EPOLL_ERR == VOS_EPOLL_Ctrl(pstRctReactor->pstPthCfgCtx->ulEpollId,pstEvtOps->lSockfd, VOS_EPOLL_CTL_ADD,pstEvtOps->ulEventMask))
    {
        DbgRctModuleError("RTM_EPOLL_CTL error!");
        return VOS_ERR;
    }
    
    return VOS_OK;
}

/*去注册网络事件*/
LONG RCT_Net_EventOptsUnRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_NETEVT_OPT_S *pstEvtOps)
{
    RCT_REACTOR_NETEVTHANDLER_S *pstNetEvtHandler = NULL;
    
    if ( NULL == pstRctReactor
        || NULL == pstEvtOps)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    pstNetEvtHandler = &pstRctReactor->stNetEvtHandler;

    /*添加重复的去注册保护, 保证优先去注册，否则网络出错时，会产生大量的0事件*/
    if ( NULL == pstNetEvtHandler->apstEpollEvtOps[pstEvtOps->lSockfd] )
    {
        return VOS_OK;
    }

    pstNetEvtHandler->apstEpollEvtOps[pstEvtOps->lSockfd] = NULL;
    
    if ( VOS_EPOLL_ERR == VOS_EPOLL_Ctrl(pstRctReactor->pstPthCfgCtx->ulEpollId,pstEvtOps->lSockfd, VOS_EPOLL_CTL_DEL,pstEvtOps->ulEventMask))
    {
        DbgRctModuleError("RTM_EPOLL_Ctl delfd=[%d] error!", pstEvtOps->lSockfd);
        return VOS_ERR;
    }
    else
    {
        DbgRctModuleEvent("RTM_EPOLL_Ctl unregister the net event OK!!");
        
        VOS_Printf("RTM_EPOLL_Ctl unregister the net event OK! Del Epoll fd=%d", pstEvtOps->lSockfd);
    }
    
    return VOS_OK;
}

/*修改网络事件关心事件掩码*/
LONG RCT_Net_EventEpollMaskModify(RCT_EVTREACTOR_S *pstRctReactor, ULONG ulOption, LONG lSockfd, ULONG *pulEvtMask)
{
    ULONG ulEpollMask = 0;
    RCT_REACTOR_NETEVTHANDLER_S *pstNetEvtHandler = NULL;

    if( NULL == pstRctReactor || VOS_SOCKET_INVALID == lSockfd )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
        
    pstNetEvtHandler = &pstRctReactor->stNetEvtHandler;

    /*添加去注册保护, 一旦已经去注册，所有的事件已经失效了 */
    if ( NULL == pstNetEvtHandler->apstEpollEvtOps[lSockfd] )
    {
        return VOS_OK;
    }

    ulEpollMask = (*pulEvtMask);
    switch(ulOption)
    {
        case VOS_EPOLL_CTRL_INOPEN:
            ulEpollMask |= VOS_EPOLL_MASK_POLLIN;
            break;
        case VOS_EPOLL_CTRL_INCLOSE:
            ulEpollMask &= ~VOS_EPOLL_MASK_POLLIN;
            break;
        case VOS_EPOLL_CTRL_OUTOPEN:
            ulEpollMask |= VOS_EPOLL_MASK_POLLOUT;
            break;
        case VOS_EPOLL_CTRL_OUTCLOSE:
            ulEpollMask &= ~VOS_EPOLL_MASK_POLLOUT;
            break;
        default:
            break;
    }
    
    (*pulEvtMask) = ulEpollMask;
    if( VOS_EPOLL_ERR == VOS_EPOLL_Ctrl(pstRctReactor->pstPthCfgCtx->ulEpollId, lSockfd, VOS_EPOLL_CTL_MOD, ulEpollMask) )
    {
        DbgRctModuleError("SSN_EPOLL_Ctrl fd=[%d] error! errno=%d", lSockfd, errno);
        return VOS_ERR;
    }
    
    return VOS_OK;
} 





