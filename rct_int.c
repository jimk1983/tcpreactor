/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_int.c
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��2��14��
  ����޸�   :
  ��������   : ���ú������ʼ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��2��14��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/
#include <core_def.h>
#include <vos_pub.h>
#include <rct_pub.h>

#if 0
/*��̬�߳�����: �涨���������߳�*/
/*ȷ�������ͱ���һһ��Ӧ*/
RCT_INICONF_S g_stRctPthIniConfig[]=
{
    {
        RCT_TYPE_MP_CLI,
        RCT_TYPE_SIGLENUMS,          
        "Manage Plane: [Command]",  /*������*/
    },
    {
        RCT_TYPE_MP_AAA,
        RCT_TYPE_SIGLENUMS,          
        "Manage Plane: [AAA]",      /*��֤��Ȩ*/
    },
    {
        RCT_TYPE_MP_PROXY,
        RCT_TYPE_SIGLENUMS,          
        "Manage Plane: [PROXY]",      /*����ģ��:DNS/ARP*/
    },
    {
        RCT_TYPE_DP_TCP,
        RCT_TYPE_SIGLENUMS,          
        "Data Plane: [TCP]",        /*TCP����*/
    },
    {
        RCT_TYPE_DP_SSL,
        RCT_TYPE_SSL_MUTILNUMS,          
        "Data Plane: [SSL]",        /*SSLҵ��*/
    },
    {
        RCT_TYPE_DP_UDPS,
        RCT_TYPE_SIGLENUMS,          
        "Data Plane: [UDPS]",       /*UDPSҵ��*/
    },
    {
        RCT_TYPE_DP_TWORK,
        RCT_TYPE_SIGLENUMS,          
        "Manage Plane: [TWOK]",      /*��ʱ����ҵ��: ����DNS\ARP��ѯ��*/
    },
    {
        RCT_TYPE_DP_VSNAT,
        RCT_TYPE_SIGLENUMS,          
        "Manage Plane: [VSNAT]",      /*Tap��SNATҵ��*/
    },
};
#else
/* �ļ�����������������: �ʹ���2������ */
RCT_INICONF_S g_stRctPthIniConfig[]=
{
    {
        RCT_TYPE_MP_CLI,
        0,          
        "Manage Plane: [Command]",  /*������*/
    },
    {
        RCT_TYPE_MP_AAA,
        0,          
        "Manage Plane: [AAA]",      /*��֤��Ȩ*/
    },
    {
        RCT_TYPE_MP_PROXY,
        0,          
        "Manage Plane: [PROXY]",      /*����ģ��:DNS/ARP*/
    },
    {
        RCT_TYPE_DP_TCP,
        1,          
        "Data Plane: [TCP]",        /*TCP����*/
    },
    {
        RCT_TYPE_DP_SSL,
        2,          
        "Data Plane: [SSL]",        /*SSLҵ��*/
    },
    {
        RCT_TYPE_DP_UDPS,
        0,          
        "Data Plane: [UDPS]",       /*UDPSҵ��*/
    },
    {
        RCT_TYPE_DP_TWORK,
        0,          
        "Manage Plane: [TWOK]",      /*��ʱ����ҵ��: ����DNS\ARP��ѯ��*/
    },
    {
        RCT_TYPE_DP_VSNAT,
        0,          
        "Manage Plane: [VSNAT]",      /*Tap��SNATҵ��*/
    },
};


#endif
/*****************************************************************************
 �� �� ��  : RCT_InitPthCfgGetSubNums
 ��������  : ��ȡ���̵߳�����
 �������  : ULONG ulRctType  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��21��
    ��    ��   : ����
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_InitPthreadBaseTimerGetMapInfo
 ��������  : ��ȡ������ȡ���е�Map��Ϣ�����ж�ʱ����ͨ�ž����д
 �������  : RCT_INIT_TIMERBASE_CONFIG_S *pstTimrConfig  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
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
 �� �� ��  : RCT_InitPthreadBaseTimerCycle
 ��������  : ��ʱ����ѭ��*
 �������  : VOID *pvSleepTime  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
ע��: �ö�ʱ���߳������еı���ҵ���̷߳���UDP���������������̵߳Ķ�ʱ��
             ��Ҫע����ǣ���Ϊ����UDP����˴���һ���Ķ������ܣ�
             ��ʱ�������ر��׼ȷ
             (�������Կ���Ack���ƣ�����һ������RCT�ܹ��ڵĶ�ʱ������)

*****************************************************************************/
VOID RCT_InitPthreadBaseTimerCycle(VOID *pvSleepTime)
{
    LONG    lRet            = 0;
    ULONG   ulIndex         =0;
    ULONG   ulSleepTime     = 0;
    ULONG  *pulSleepTime    = (ULONG *)pvSleepTime;
    ULONG   uiVal           = 1;    //���ֵǧ����Ϊ0!!!!!
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
 �� �� ��  : RCT_InitPthreadBaseTimer
 ��������  : ��С���ȵĶ�ʱ���̳߳�ʼ�������͸������߳�
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 

*****************************************************************************/
LONG RCT_InitPthreadBaseTimer()
{
    LONG     lRet = 0;
    
    /*Ĭ��Ϊ500ms��ʱ����С����*/
    lRet = VOS_Thread_Create(NULL, RCT_InitPthreadBaseTimerCycle, NULL, 0,0);
    if ( 0 == lRet )
    {
        DbgRctModuleError("pthread_create timer cycle failed!");
        return VOS_ERR;
    }
    
    return VOS_OK;
}


/*****************************************************************************
 �� �� ��  : RCT_InitPthreadTasks
 ��������  : RCT�������߳����񴴽��ͳ�ʼ��
           ����ڸ�����ǰ����Ҫ����ص�ҵ�����ע��
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
*****************************************************************************/
LONG   RCT_InitPthreadTasksRun()
{
    ULONG           ulIndex         = 0;
    ULONG           ulSubIndex      = 0;
    ULONG           ulPthArryIndex  = 0;
    RCT_INDEXINFO_S stIndexInfo     = {0};
    
    /*�����߳�����ĳ�ʼ��*/
    for (ulIndex=0; ulIndex < RCT_TYPE_NUMS; ulIndex++)
    {
        stIndexInfo.ulRctType = ulIndex;
        /*�����ж�������*/
        /*ע�⣺��ʼ����ʱ��һ���ǰ������õ���ͬ��˳������,�����̼߳��ͨ�ž������޷���Ӧ*/
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

    /*ȫ����ʼ������Ժ�Ҫȫ������ͬ��, ����һ��Map��Ϣ*/
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

/*ר�Ÿ�RCT���Դ����õ�**/
VOID RCT_Test_InitPthreadRunSyncMapInfo()
{
    ULONG           ulIndex         = 0;
    ULONG           ulSubIndex      = 0;
    ULONG           ulPthArryIndex  = 0;
    RCT_INDEXINFO_S stIndexInfo     = {0};


     /*ȫ����ʼ������Ժ�Ҫȫ������ͬ��, ����һ��Map��Ϣ*/
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
 �� �� ��  : RCT_InitPthCfgCtxCreate
 ��������  : �������������ı���ڵ� 
 �������  : PRCT_INICFG_CTX_S *ppstCfgCtx  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��17��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
    
    /*���ֲ�����ʼ��*/
    pstCtx->ulEpollWaitNum  = RCT_EPOLL_WAIT_NUMS;

    /*��ʼ���󲻱�ģ�ֱ����ϵͳmalloc����*/
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
 �� �� ��  : RCT_InitPthCfgCtxRelease
 ��������  : �ͷ����ñ���ڵ�
 �������  : RCT_INICFG_CTX_S *pstCtx  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��17��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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









