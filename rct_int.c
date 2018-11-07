/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_int.c
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年2月14日
  最近修改   :
  功能描述   : 配置和任务初始化
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月14日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
#include <core_def.h>
#include <vos_pub.h>
#include <rct_pub.h>

#if 0
/*静态线程配置: 规定创建多少线程*/
/*确保和类型保持一一对应*/
RCT_INICONF_S g_stRctPthIniConfig[]=
{
    {
        RCT_TYPE_MP_CLI,
        RCT_TYPE_SIGLENUMS,          
        "Manage Plane: [Command]",  /*命令行*/
    },
    {
        RCT_TYPE_MP_AAA,
        RCT_TYPE_SIGLENUMS,          
        "Manage Plane: [AAA]",      /*认证授权*/
    },
    {
        RCT_TYPE_MP_PROXY,
        RCT_TYPE_SIGLENUMS,          
        "Manage Plane: [PROXY]",      /*代理模块:DNS/ARP*/
    },
    {
        RCT_TYPE_DP_TCP,
        RCT_TYPE_SIGLENUMS,          
        "Data Plane: [TCP]",        /*TCP监听*/
    },
    {
        RCT_TYPE_DP_SSL,
        RCT_TYPE_SSL_MUTILNUMS,          
        "Data Plane: [SSL]",        /*SSL业务*/
    },
    {
        RCT_TYPE_DP_UDPS,
        RCT_TYPE_SIGLENUMS,          
        "Data Plane: [UDPS]",       /*UDPS业务*/
    },
    {
        RCT_TYPE_DP_TWORK,
        RCT_TYPE_SIGLENUMS,          
        "Manage Plane: [TWOK]",      /*定时任务业务: 例如DNS\ARP查询等*/
    },
    {
        RCT_TYPE_DP_VSNAT,
        RCT_TYPE_SIGLENUMS,          
        "Manage Plane: [VSNAT]",      /*Tap的SNAT业务*/
    },
};
#else
/* 文件升级服务器的配置: 就创建2种类型 */
RCT_INICONF_S g_stRctPthIniConfig[]=
{
    {
        RCT_TYPE_MP_CLI,
        0,          
        "Manage Plane: [Command]",  /*命令行*/
    },
    {
        RCT_TYPE_MP_AAA,
        0,          
        "Manage Plane: [AAA]",      /*认证授权*/
    },
    {
        RCT_TYPE_MP_PROXY,
        0,          
        "Manage Plane: [PROXY]",      /*代理模块:DNS/ARP*/
    },
    {
        RCT_TYPE_DP_TCP,
        1,          
        "Data Plane: [TCP]",        /*TCP监听*/
    },
    {
        RCT_TYPE_DP_SSL,
        2,          
        "Data Plane: [SSL]",        /*SSL业务*/
    },
    {
        RCT_TYPE_DP_UDPS,
        0,          
        "Data Plane: [UDPS]",       /*UDPS业务*/
    },
    {
        RCT_TYPE_DP_TWORK,
        0,          
        "Manage Plane: [TWOK]",      /*定时任务业务: 例如DNS\ARP查询等*/
    },
    {
        RCT_TYPE_DP_VSNAT,
        0,          
        "Manage Plane: [VSNAT]",      /*Tap的SNAT业务*/
    },
};


#endif
/*****************************************************************************
 函 数 名  : RCT_InitPthCfgGetSubNums
 功能描述  : 获取子线程的配置
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
ULONG RCT_InitPthCfgGetSubNums(ULONG ulRctType)
{
    ULONG ulSubNums = 0;

    if ( ulRctType >= RCT_TYPE_NUMS  )
    {
        return 0;
    }


    ulSubNums = g_stRctPthIniConfig[ulRctType].ulPthRctSubNums;
    
    return ulSubNums;
}

/*****************************************************************************
 函 数 名  : RCT_InitPthreadBaseTimerGetMapInfo
 功能描述  : 获取用于提取所有的Map信息，进行定时器的通信矩阵读写
 输入参数  : RCT_INIT_TIMERBASE_CONFIG_S *pstTimrConfig  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
*****************************************************************************/
VOID RCT_InitPthreadBaseTimerGetMapInfo(RCT_INIT_TIMERBASE_CONFIG_S *pstTimrConfig)
{
    ULONG               ulRctType       = 0;
    ULONG               ulSubIndex      = 0;
    ULONG               ulPthArryIndex  = 0;
    RCT_COMMUTEINFO_S   stEvtInfo       ={0};
    
    if ( NULL == pstTimrConfig )
    {
        DbgRctModuleError("param error!");
        return;
    }
    
    pstTimrConfig->lPthreadNums = RCT_TaskPthGetNums();

    for (ulRctType=0; ulRctType < RCT_TYPE_NUMS; ulRctType++)
    {
        for(ulSubIndex=0; ulSubIndex < g_stRctPthIniConfig[ulRctType].ulPthRctSubNums; ulSubIndex++ )
        {
            if ( VOS_OK == RCT_TaskPthGetCommunMapInfo(ulRctType, ulSubIndex, &stEvtInfo) )
            {
                pstTimrConfig->aulMapExprEvtfd[ulPthArryIndex] =stEvtInfo.ulEventFdExpr;
                pstTimrConfig->aulMapTimrEvtfd[ulPthArryIndex] =stEvtInfo.ulEventFdTimer;
            }
            ulPthArryIndex++;
        }
    }

    pstTimrConfig->lPthreadNums = ulPthArryIndex;

    return;
}

/*****************************************************************************
 函 数 名  : RCT_InitPthreadBaseTimerCycle
 功能描述  : 定时器主循环*
 输入参数  : VOID *pvSleepTime  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
注意: 该定时器线程向所有的本地业务线程发送UDP报文来触发其它线程的定时器
             需要注意的是，因为采用UDP，因此存在一定的丢包可能，
             定时器不会特别的准确
             (后续可以考虑Ack机制，来进一步完善RCT架构内的定时器机制)

*****************************************************************************/
VOID RCT_InitPthreadBaseTimerCycle(VOID *pvSleepTime)
{
    LONG    lRet            = 0;
    ULONG   ulIndex         =0;
    ULONG   ulSleepTime     = 0;
    ULONG  *pulSleepTime    = (ULONG *)pvSleepTime;
    ULONG   uiVal           = 1;    //这个值千万不能为0!!!!!
    RCT_INIT_TIMERBASE_CONFIG_S stTimrConfig = {0};
    
    if ( NULL == pvSleepTime )
    {
        ulSleepTime = RCT_REACTOR_TIMEROUT_US;
        usleep(ulSleepTime);
    }
    else
    {
        ulSleepTime = *pulSleepTime;
    }

    RCT_InitPthreadBaseTimerGetMapInfo(&stTimrConfig);
        
    while(1)
    {
        usleep(ulSleepTime);
        //VOS_Sleep(1);
        for(ulIndex = 0; ulIndex < stTimrConfig.lPthreadNums; ulIndex++)
        {
            if ( stTimrConfig.aulMapExprEvtfd[ulIndex] > 0 )
            {
                lRet = VOS_EventfdWrite(stTimrConfig.aulMapExprEvtfd[ulIndex], &uiVal, sizeof(ULONG));
                if( lRet < 0)
                {
                    DbgRctModuleError("Base Expire write ExpireEventFd=[%d] failed!errno=%d", 
						stTimrConfig.aulMapExprEvtfd[ulIndex], errno);
                }
            }
            
            if ( stTimrConfig.aulMapTimrEvtfd[ulIndex] > 0 )
            {
                
                lRet = VOS_EventfdWrite(stTimrConfig.aulMapTimrEvtfd[ulIndex], &uiVal, sizeof(ULONG));
                if( lRet < 0)
                {
                    DbgRctModuleError("Base Timer write TimerEventFd=[%d] failed!errno=%d", 
						stTimrConfig.aulMapTimrEvtfd[ulIndex], errno);
                }
            }
        }
    }  

    DbgRctModuleError("The base Timer pthread has been END!");
    
    return;
}


/*****************************************************************************
 函 数 名  : RCT_InitPthreadBaseTimer
 功能描述  : 最小粒度的定时器线程初始化，发送给各个线程
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 

*****************************************************************************/
LONG RCT_InitPthreadBaseTimer()
{
    LONG     lRet = 0;
    
    /*默认为500ms定时器最小回收粒度*/
    lRet = VOS_Thread_Create(NULL, RCT_InitPthreadBaseTimerCycle, NULL, 0,0);
    if ( 0 == lRet )
    {
        DbgRctModuleError("pthread_create timer cycle failed!");
        return VOS_ERR;
    }
    
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RCT_InitPthreadTasks
 功能描述  : RCT的所有线程任务创建和初始化
           因此在该运行前，需要将相关的业务进行注册
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
*****************************************************************************/
LONG   RCT_InitPthreadTasksRun()
{
    ULONG           ulIndex         = 0;
    ULONG           ulSubIndex      = 0;
    ULONG           ulPthArryIndex  = 0;
    RCT_INDEXINFO_S stIndexInfo     = {0};
    
    /*进行线程任务的初始化*/
    for (ulIndex=0; ulIndex < RCT_TYPE_NUMS; ulIndex++)
    {
        stIndexInfo.ulRctType = ulIndex;
        /*可能有多个的情况*/
        /*注意：初始化的时候，一定是按照配置的相同的顺序来的,否则线程间的通信矩阵是无法对应*/
        for(ulSubIndex=0; ulSubIndex < g_stRctPthIniConfig[ulIndex].ulPthRctSubNums; ulSubIndex++ )
        {
            stIndexInfo.ulPthSubIndex   = ulSubIndex;
            stIndexInfo.ulArryIndex     = ulPthArryIndex;
            if( 0 == VOS_Thread_Create(NULL, RCT_Task_Main, &stIndexInfo, 256*1024*1024, 0) )
            {
                DbgRctModuleError("Rct init task main error!ulPthArryIndex=%d", ulPthArryIndex);
                return VOS_ERR;
            }
            else
            {   
                RCT_Task_MainCompeleteWaitfor();
            }
            ulPthArryIndex++;
        }
    }

    /*全部初始化完成以后，要全部重新同步, 生成一个Map信息*/
    ulPthArryIndex = 0;
    for (ulIndex=0; ulIndex < RCT_TYPE_NUMS; ulIndex++)
    {
        stIndexInfo.ulRctType = ulIndex;
        for(ulSubIndex=0; ulSubIndex < g_stRctPthIniConfig[ulIndex].ulPthRctSubNums; ulSubIndex++ )
        {
            if ( VOS_ERR == RCT_TaskPthSetSyncMapInfo(stIndexInfo.ulRctType, ulSubIndex, ulPthArryIndex) )
            {
                DbgRctModuleError("Rct init base timer!ulIndex=%d", ulIndex);
                return VOS_ERR;
            }
            ulPthArryIndex++;
        }
   }

    if ( VOS_ERR == RCT_InitPthreadBaseTimer() )
    {
        DbgRctModuleError("Rct init base timer!ulIndex=%d", ulIndex);
        return VOS_ERR;
    }
    
    return VOS_OK;
}

/*专门给RCT测试代码用的**/
VOID RCT_Test_InitPthreadRunSyncMapInfo()
{
    ULONG           ulIndex         = 0;
    ULONG           ulSubIndex      = 0;
    ULONG           ulPthArryIndex  = 0;
    RCT_INDEXINFO_S stIndexInfo     = {0};


     /*全部初始化完成以后，要全部重新同步, 生成一个Map信息*/
     ulPthArryIndex = 0;
     for (ulIndex=0; ulIndex < RCT_TYPE_NUMS; ulIndex++)
     {
         stIndexInfo.ulRctType = ulIndex;
         for(ulSubIndex=0; ulSubIndex < g_stRctPthIniConfig[ulIndex].ulPthRctSubNums; ulSubIndex++ )
         {
             if ( VOS_ERR == RCT_TaskPthSetSyncMapInfo(stIndexInfo.ulRctType, ulSubIndex, ulPthArryIndex) )
             {
                 DbgRctModuleError("Rct init base timer!ulIndex=%d", ulIndex);
                 return;
             }
             ulPthArryIndex++;
         }
    }

     return;
}



/*****************************************************************************
 函 数 名  : RCT_InitPthCfgCtxCreate
 功能描述  : 创建配置上下文保存节点 
 输入参数  : PRCT_INICFG_CTX_S *ppstCfgCtx  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月17日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
INT32 RCT_InitPthCfgCtxCreate(PRCT_INICFG_CTX_S *ppstCfgCtx, RCT_INDEXINFO_S *pstInfo)
{
    PRCT_INICFG_CTX_S pstCtx = NULL;

    if ( NULL == ppstCfgCtx 
        || NULL == pstInfo )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }

    pstCtx = (PRCT_INICFG_CTX_S)malloc(sizeof(RCT_INICFG_CTX_S));
    if ( NULL == pstCtx )
    {
        DbgRctModuleError("malloc error!");
        return VOS_ERR;
    }

    VOS_Mem_Zero((CHAR *)pstCtx, sizeof(RCT_INICFG_CTX_S));
    
    pstCtx->stIndexInfo.ulRctType       = pstInfo->ulRctType;
    pstCtx->stIndexInfo.ulPthSubIndex   = pstInfo->ulPthSubIndex;
    pstCtx->stIndexInfo.ulArryIndex     = pstInfo->ulArryIndex;
    
    /*部分参数初始化*/
    pstCtx->ulEpollWaitNum  = RCT_EPOLL_WAIT_NUMS;

    /*初始化后不变的，直接用系统malloc申请*/
    pstCtx->stEvtInfo.pstQueMgrCtx = (PRCT_MQUEMGRCTX_S)malloc(sizeof(RCT_MQUEMGRCTX_S));
    if ( NULL == pstCtx->stEvtInfo.pstQueMgrCtx )
    {
        DbgRctModuleError("malloc error!");
        free((CHAR *)pstCtx);
        return VOS_ERR;
    }

    VOS_Mem_Zero((CHAR *)pstCtx->stEvtInfo.pstQueMgrCtx, sizeof(RCT_MQUEMGRCTX_S));

    VOS_DLIST_INIT(&pstCtx->stEvtInfo.pstQueMgrCtx->stMsgList);
    VOS_RWLOCK_INIT(pstCtx->stEvtInfo.pstQueMgrCtx->stMsgLock);

    VOS_DLIST_INIT(&pstCtx->stEvtInfo.pstQueMgrCtx->stCheckRlyList);
    VOS_RWLOCK_INIT(pstCtx->stEvtInfo.pstQueMgrCtx->stCheckRlyLock);
    
    *ppstCfgCtx = pstCtx;
        
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RCT_InitPthCfgCtxRelease
 功能描述  : 释放配置保存节点
 输入参数  : RCT_INICFG_CTX_S *pstCtx  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年4月17日
    作    者   : jimk
    修改内容   : 新生成函数

*****************************************************************************/
VOID RCT_InitPthCfgCtxRelease(RCT_INICFG_CTX_S *pstCtx)
{
    RCT_MRLYLOCKNODE_S  *pstMsgLockNode  = NULL;
    RCT_MQUENODE_S      *pstMsgNode      = NULL;
    PRCT_MQUEMGRCTX_S   pstQueMgrCtx  = NULL;
    PVOS_DLIST_NODE_S   thisEntry, nextEntry, ListHead; 
    
    if ( NULL != pstCtx )
    {
        pstQueMgrCtx = pstCtx->stEvtInfo.pstQueMgrCtx;

        VOS_RWLOCK_LOCK(pstQueMgrCtx->stMsgLock);
        if ( VOS_OK != VOS_Node_IsEmpty(&pstQueMgrCtx->stMsgList) )
        {
            ListHead = &pstQueMgrCtx->stMsgList;
    
            for(thisEntry = ListHead->next;
                thisEntry != ListHead;
                thisEntry = nextEntry)
            {
                nextEntry = thisEntry->next;
                pstMsgNode = VOS_DLIST_ENTRY(thisEntry, RCT_MQUENODE_S, stEntry);
                VOS_Node_Remove(thisEntry);
                if ( NULL != pstMsgNode->pcMsgData )
                {
                    VOS_Free(pstMsgNode->pcMsgData);
                    pstMsgNode->pcMsgData = NULL;
                }
                
                VOS_Free((CHAR *)pstMsgNode);
            }
        }
        VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stMsgLock);

        VOS_RWLOCK_LOCK(pstQueMgrCtx->stCheckRlyLock);
        if ( VOS_OK != VOS_Node_IsEmpty(&pstQueMgrCtx->stCheckRlyList) )
        {
            ListHead = &pstQueMgrCtx->stCheckRlyList;
    
            for(thisEntry = ListHead->next;
                thisEntry != ListHead;
                thisEntry = nextEntry)
            {
                nextEntry = thisEntry->next;
                pstMsgLockNode = VOS_DLIST_ENTRY(thisEntry, RCT_MRLYLOCKNODE_S, stEntry);
                VOS_Node_Remove(thisEntry);
                
                VOS_Free((CHAR *)pstMsgLockNode);
            }
        }
        VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);

        free((CHAR *)pstQueMgrCtx);
        
        free((CHAR *)pstCtx);
        
        pstCtx = NULL;
    }
}









