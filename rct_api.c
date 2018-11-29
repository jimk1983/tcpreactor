/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_ctx_api.c
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年2月14日
  最近修改   :
  功能描述   : 对外的rct模块接口
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月14日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>

/*****************************************************************************
 函 数 名  : RCT_API_EnvInit
 功能描述  : 第一步:
                          运行环境初始化
                          (包含VOS内存管理器和配置生成等)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年5月28日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_EnvInit()
{
    if(VOS_ERR == VOS_EnvInit())
    {
        DbgRctModuleError("vos environment init error");
        return VOS_ERR;
    }
    
    /*各个线程配置保存的上下文资源初始化*/
    if ( VOS_ERR == RCT_TaskPthCfgCtxInit() )
    {
        DbgRctModuleError("rct task arry init error");
        VOS_EnvUnInit();
        return VOS_ERR;
    }

    

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_API_EnvRun
 功能描述  : 第二步，
                           开始启动任务，注意启动前各个注册函数也已经需要完成
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年5月28日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_EnvRun()
{
    /*启动线程任务,在线程中运行注册的业务资源初始化函数*/
    if( VOS_ERR ==  RCT_InitPthreadTasksRun() )
    {
        DbgRctModuleError("rct task pthread running...error!");
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_API_EnvUnInit
 功能描述  : 环境资源回收接口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年5月28日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_API_EnvUnInit()
{
    RCT_TaskPthCfgCtxUninit();

    VOS_EnvUnInit();
}


/*****************************************************************************
 函 数 名  : RCT_API_TaskRegister
 功能描述  : 任务初始化注册
 输入参数  :    VOS_CALLBACK pfInitCb   ---需要在该线程做的任务模块上下文初始化，例如SWM/NEM...
                                       这些上下文在rct_def.h中可以定义添加
            VOID *pvArg             ---注册时需要保存的参数
            ULONG ulTskIndex        ---业务的任务类型，单个线程中也可能存在多个业务:SWM/NEM
            ULONG ulType            ---在rct_def.h中定义的类型，直接索引到其数组进行快速注册
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年5月28日
    作    者   : jimk
    修改内容   : 新生成函数

注意:注册函数都需要在环境初始化之前就完成
将ulTskIndex的任务索引注册在ulType类型的线程上进行初始化
*****************************************************************************/
INT32 RCT_API_EnvTaskInitRegister(VOS_CALLBACK pfInitCb, VOID *pvArg, ULONG ulRctType,ULONG ulRctSubType, VOS_CALLBACK pfUnInitCb)
{
    if ( NULL == pfInitCb 
        || ulRctType > RCT_TYPE_NUMS )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

    /*进行快速注册*/
    if ( VOS_ERR == RCT_TaskArrayBizCtxInitRegister(pfInitCb, pvArg, ulRctType,ulRctSubType,  pfUnInitCb) )
    {
        DbgRctModuleError("rct task ctx init register error");
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_API_EnvTaskInitUnRegister
 功能描述  : 去注册
 输入参数  : ULONG ulTskIndex  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年11月7日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_EnvTaskInitUnRegister(ULONG ulRctType, ULONG ulRctSubType)
{
    if ( ulRctType > RCT_TYPE_NUMS)
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

    /*取消注册*/
    if ( VOS_ERR == RCT_TaskArrayBizCtxInitUnRegister(ulRctType, ulRctSubType) )
    {
        DbgRctModuleError("rct task ctx init un register error");
        return VOS_ERR;
    }

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_API_EnvGetPthreadInfo
 功能描述  : 获取当前线程的一些配置信息, 
           (主要是线程间消息发送时候，回包比较方便)
 输入参数  : ULONG    *pulRctType,  获取线程业务类型
           ULONG  *pulPolIndex  获取线程业务类型所在的第几个索引
 输出参数  : 无 
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年6月12日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_API_EnvGetPthConfInfo(  UINT32    *pulRctType, UINT32    *pulSubPthIndex)
{
    RCT_INDEXINFO_S stIndexInfo = {0};
    
    if ( VOS_ERR == RCT_Task_ReactorGetPthIndexInfo(&stIndexInfo)  )
    {
        DbgRctModuleError("rct task ctx get pthread index info error");
        return;
    }

    *pulRctType     = stIndexInfo.ulRctType;
    *pulSubPthIndex = stIndexInfo.ulPthSubIndex;
    
    return;
}

/*****************************************************************************
 函 数 名  : RCT_API_EnvGetTypeCfgSubNums
 功能描述  : 返回当前的配置信息，例如SSL有多个
 输入参数  : ULONG ulRctType  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月21日
    作    者   : 蒋康
    修改内容   : 新生成函数

*****************************************************************************/
ULONG   RCT_API_EnvGetTypeCfgSubNums(ULONG ulRctType)
{
    return RCT_InitPthCfgGetSubNums(ulRctType);
}


/*****************************************************************************
 函 数 名  : RCT_API_MgtLockySend
 功能描述  : 需要锁定节点的消息发送,最后一个参数是节-
                 点数据，返回的时候会自动检查
 输入参数  : ULONG ulSrcSubType   
             ULONG ulDstRctType   
             ULONG ulDstSubType   
             ULONG ulDstPthIndex  
             CHAR *pcData         
             ULONG ulLen          
             VOID *pcLockConn     用户发送的节点
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月21日
    作    者   : 蒋康
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_API_MgtLockySend(ULONG ulSrcSubType, ULONG ulDstRctType, ULONG ulDstSubType, 
                                          ULONG ulDstPthIndex, CHAR *pcData, ULONG ulLen, UINT32 uiLockID)
{
    if ( VOS_ERR == RCT_Reactor_MgtLockySend(ulSrcSubType, ulDstRctType, ulDstSubType, 
                                                  ulDstPthIndex, pcData, ulLen,uiLockID) )
    {
        return VOS_ERR;
    }
    
    return VOS_OK;
}

                                          

/*****************************************************************************
 函 数 名  : RCT_API_MgtNormalSend
 功能描述  : 普通的一次性消息，不需要返回数据的,一般用于转发包
 输入参数  : ULONG ulSrcSubType   
           ULONG ulDstRctType   
           ULONG ulDstSubType   
           ULONG ulDstPthIndex  
           CHAR *pcData         
           ULONG ulLen          
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月21日
    作    者   : 蒋康
    修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_API_MgtNormalSend(ULONG ulSrcSubType,
                                         ULONG ulDstRctType,
                                         ULONG ulDstSubType,
                                         ULONG ulDstPthIndex,
                                         CHAR *pcData,ULONG ulLen)
{
    if ( VOS_ERR == RCT_Reactor_MgtNormalSend(ulSrcSubType,ulDstRctType,ulDstSubType,
                                              ulDstPthIndex, pcData, ulLen) )
    {
        return VOS_ERR;
    }
    
    return VOS_OK;
}

                                         
/*****************************************************************************
函 数 名  : RCT_API_MgtLockySendBack
功能描述  : 回送
输入参数  : RCT_MSG_HEAD_S            *pstSrcHead  
          CHAR *pcData                           
          ULONG ulLen                            
输出参数  : 无
返 回 值  : 
调用函数  : 
被调函数  : 

修改历史      :
1.日    期   : 2018年6月3日
 作    者   : 蒋康
 修改内容   : 新生成函数

*****************************************************************************/
LONG RCT_API_MgtSendBack(RCT_MSG_HEAD_S            *pstSrcHead, CHAR *pcData, ULONG ulLen)
{
    if ( VOS_ERR == RCT_Reactor_MgtSendBack(pstSrcHead, pcData, ulLen) )
    {
       return VOS_ERR;
    }

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_API_MgtHandlerRegister
 功能描述  : 业务模块的消息处理函数
 输入参数  : ULONG ulRctType                   
           ULONG ulSubType                   
           CHAR *pcModuName                  
           pfComMsgHandlerCallBack pfMethod  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月20日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_MgtHandlerRegister(ULONG ulRctType, ULONG ulSubType, const CHAR *pcModuName, pfComMsgHandlerCallBack pfMethod)
{
    if ( NULL == pfMethod
        || NULL == pcModuName )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

    /*业务线程注册函数, 内部封装*/
    if ( VOS_ERR == RCT_TaskArrayBizMsgHandlerRegister(ulRctType, ulSubType, (CHAR *)pcModuName, pfMethod) )
    {
        DbgRctModuleError("rct mgt register error");
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_API_EnvMgtRegister
 功能描述  : 去注册消息处理函数，用于错误处理
 输入参数  : ULONG ulType  
             CHAR *pcName  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年6月4日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_API_MgtHandlerUnRegister(ULONG ulRctType, ULONG ulSubType)
{

    /*业务线程注册函数, 内部封装*/
    if ( VOS_ERR == RCT_TaskArrayBizMsgHandlerUnRegister(ulRctType, ulSubType) )
    {
        DbgRctModuleError("rct mgt register error");
        return;
    }
}

/*****************************************************************************
 函 数 名  : RCT_API_NetOpsEventRegister
 功能描述  : 网络事件对外注册函数
 输入参数  :    RCT_NETOPS_EVENT_S *pstNetOps   ----自身的网络事件节点
                             LONG lSockfd                                  ----该网络事件的sockfd
                             ULONG ulEMask                               ----关注的EPOLLIN | EPOLLOUT
                                                                                        (VOS_EPOLL_MASK_POLLIN | VOS_EPOLL_MASK_POLLOUT)
                             VOS_CALLBACK pfRecv                     ----对应的接收处理函数
                             VOS_CALLBACK pfSend                     ----对应的发送处理函数
                             VOID *pvConn                                ----保存自身的节点数据
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年5月29日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_NetOpsEventRegister(RCT_NETOPT_EVENT_S *pstNetOps,  
                                                LONG lSockfd,  
                                                ULONG ulEMask, 
                                                VOS_CALLBACK pfRecv, 
                                                VOS_CALLBACK pfSend,
                                                VOID *pvConn)
{
    if ( NULL == pstNetOps
        || VOS_SOCKET_INVALID >= lSockfd )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

    if ( NULL == g_th_pstReactor )
    {
        DbgRctModuleError("system error");
        return VOS_ERR;
    }
    
    //register network
    RCT_REACTOR_NETEVTOPTS_INIT(
                    pstNetOps,
                    lSockfd,
                    ulEMask,
                    pfRecv,
                    pfSend,
                    pvConn);
    
    if(VOS_ERR == RCT_Net_EventOptsRegister(g_th_pstReactor, (RCT_REACTOR_NETEVT_OPT_S *)pstNetOps))
    {
        DbgRctModuleError("RCT_Net_EventOptsRegister error!");
        return VOS_ERR;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_API_NetOpsEventUnRegister
 功能描述  : 去注册网络事件，允许重复去注册
 输入参数  : RCT_NETOPT_EVENT_S *pstNetOps  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年6月4日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_NetOpsEventUnRegister(RCT_NETOPT_EVENT_S *pstNetOps)
{
    if ( NULL == pstNetOps )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }
    
    if(VOS_ERR == RCT_Net_EventOptsUnRegister(g_th_pstReactor, (RCT_REACTOR_NETEVT_OPT_S *)pstNetOps))
    {
        DbgRctModuleError("RCT_Net_EventOptsUnRegister error!");
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_API_NetOpsEventCtrl
 功能描述  : 发送和接收事件的修改
 输入参数  :   RCT_NETOPT_EVENT_S *pstNetOps    --注册的网络事件
                            LONG lSockfd                                   --网络socket
                            ULONG ulEpollMask                           --掩码
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年6月6日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_NetOpsEventCtrl(RCT_NETOPT_EVENT_S *pstNetOps, ULONG ulEpollMask)
{
    
    if ( NULL == pstNetOps )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }
    if ( VOS_ERR == RCT_Net_EventEpollMaskModify(g_th_pstReactor,ulEpollMask, pstNetOps->lSockfd, &pstNetOps->ulEventMask))
    {
        DbgRctModuleError("RCT_Reactor_NetEvtEpollMaskModify error!");
        return VOS_ERR;
    }
    
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_API_ExpireOpsEventRegister
 功能描述  : 老化注册函数
 输入参数  :     RCT_EXPIROPT_EVENT_S *pstExpireOps      ----自身的老化事件
             VOS_CALLBACK pfRecv                    ----老化处理函数
             VOID *pvConn                           ----自身的节点数据
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年5月29日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_ExpireOpsEventRegister(RCT_EXPIROPT_EVENT_S *pstExpireOps, 
                                                VOS_CALLBACK pfRecv, 
                                                VOID *pvConn)
{
    if ( NULL == pfRecv 
        || NULL == pstExpireOps )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }
    //register expire 
    RCT_REACTOR_EXPIREOPTS_INIT(
                pstExpireOps, 
                pfRecv, 
                pvConn);

    if ( VOS_ERR == RCT_Expire_EventOptsRegister(g_th_pstReactor, (RCT_REACTOR_EXPIRE_OPT_S  *)pstExpireOps) )
    {
        DbgRctModuleError("RCT_Reactor_ExpireOptsRegister error!");
        return VOS_ERR;
    } 

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_API_ExpireOpsEventRegister
 功能描述  : 老化去注册
 输入参数  : RCT_EXPIROPT_EVENT_S *pstExpireOps  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年6月4日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_ExpireOpsEventUnRegister(RCT_EXPIROPT_EVENT_S *pstExpireOps)
{
    if ( NULL == pstExpireOps )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }
    
    if ( VOS_ERR == RCT_Expire_EventOptsUnRegister(g_th_pstReactor, (RCT_REACTOR_EXPIRE_OPT_S  *)pstExpireOps) )
    {
        DbgRctModuleError("RCT_Expire_EventOptsUnRegister error!");
        return VOS_ERR;
    } 

    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_API_TimerOpsEventRegister
 功能描述  : 定时器的注册函数
 输入参数  :    RCT_TIMEROPT_EVENT_S *pstTimerOps   ----自身的定时器事件
             ULONG ulTimerType                  ----定时器类型: 单次或循环
             ULONG ulTimerOut                   ----定时超时时间:毫秒
             VOS_CALLBACK pfRecv                ----定时器回调函数
             VOID *pvConn                       ----自身节点的数据
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年5月29日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_TimerOpsEventRegister(RCT_TIMEROPT_EVENT_S *pstTimerOps, 
                                                ULONG ulTimerType, 
                                                ULONG ulTimerOut, 
                                                VOS_CALLBACK pfRecv, 
                                                VOID *pvConn)
{
    if ( NULL == pfRecv 
        || NULL == pstTimerOps )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }
    
    //register timer
    RCT_REACTOR_TIMEOPTS_INIT(
                pstTimerOps,
                ulTimerType,
                ulTimerOut,     
                pfRecv, 
                pvConn);
        
    if ( VOS_ERR == RCT_Timer_EventOptsRegister(g_th_pstReactor, (RCT_REACTOR_TIMER_OPT_S *)pstTimerOps) )
    {
        DbgRctModuleError("RCT_Timer_EventOptsRegister error!");
        
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_API_TimerOpsEventUnRegister
 功能描述  : 去注册
 输入参数  : RCT_TIMEROPT_EVENT_S *pstTimerOps  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年6月4日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_API_TimerOpsEventUnRegister(RCT_TIMEROPT_EVENT_S *pstTimerOps)
{
    if ( NULL ==  pstTimerOps )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

     if ( VOS_ERR == RCT_Timer_EventOptsUnRegister(g_th_pstReactor, (RCT_REACTOR_TIMER_OPT_S *)pstTimerOps) )
    {
        DbgRctModuleError("RCT_Timer_EventOptsUnRegister error!");

        return VOS_ERR;
    }
     
    return VOS_OK;
}

