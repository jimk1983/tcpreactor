/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_tsk.c
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��2��15��
  ����޸�   :
  ��������   : �����񴴽�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��2��15��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>


/*ÿ���߳��Լ��Ĵ�����*/
__thread  RCT_EVTREACTOR_S      *g_th_pstReactor = NULL;

/*ÿ���̵߳���Ϣ���棬��Ҫ����Ϣӳ�䣬���ڿ��ٲ�ѯ*/
static PRCT_PTHCTX_INFO_S         g_pstPthMapCfgCtx = NULL;

/*ȫ����Ϣ: ע���ҵ���������Ļص���ʼ����Ϣ*/
static RCT_TASKCONF_INFO_S       g_stArryRegInfo;

/*ÿ���߳����ʱ��ģ�֪ͨͬ��, �߳�һ�����İ����ʼ��*/
VOS_SM_T    g_stTaskCompelete = {0};


/*****************************************************************************
 �� �� ��  : RCT_Task_MainCompeleteNotify
 ��������  : ͬ��֪ͨ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��17��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID RCT_Task_MainCompeleteNotify()
{
    VOS_SM_V(&g_stTaskCompelete);
}

/*****************************************************************************
 �� �� ��  : RCT_Task_MainCompeleteWaitfor
 ��������  : �̵߳ȴ����
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��17��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID RCT_Task_MainCompeleteWaitfor()
{
    VOS_SM_P(&g_stTaskCompelete,0);
}

/*****************************************************************************
 �� �� ��  : RCT_Task_Main
 ��������  : ÿ���̵߳Ĵ�������������ѭ��
 �������  : VOID *pstConf  --�������������б��̵߳����������ĳ�ʼ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��7��26��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
    
    /*����Epoll����������*/
    if ( VOS_ERR == RCT_Task_ReactorCreate(pstCfgCtx) )
    { 
        DbgRctModuleError("RCT_Reactor_Create error!");
        return;
    }

    /*�����ñ��浽ȫ�ֵļ�¼��Ϣ��*/
    if ( VOS_ERR == RCT_TaskPthCtxRecord(pstCfgCtx)  )
    {
        DbgRctModuleError("RCT_PthreadTaskInitRun error!");
        (VOID)RCT_Task_ReactorRelease();
        return;
    }
    
    /*�߳������ʼ��, ע���ҵ���ʼ������*/
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
 �� �� ��  : RCT_Task_ReactorCreate
 ��������  : RCTÿ���̶߳���Ҫ��������EPOLL
 �������  : RCT_INICFG_CTX_S *pstRctCfgInfo  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��7��26��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
LONG RCT_Task_ReactorCreate(RCT_INICFG_CTX_S *pstRctCfgInfo)
{
    RCT_EVTREACTOR_S *pstRctReactor = NULL;

    if ( NULL == pstRctCfgInfo )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }

    /*�ȴ���һ���ܹ�������Ҳ����EPOLL*/
    pstRctReactor = (RCT_EVTREACTOR_S *)VOS_Malloc(RCT_MID_SID_TSK, sizeof(RCT_EVTREACTOR_S));
    if(NULL == pstRctReactor)
    {
        DbgRctModuleError("malloc error!");
        return VOS_ERR;
    }

    VOS_Mem_Zero((CHAR *)pstRctReactor, sizeof(RCT_EVTREACTOR_S));
    /*���浽�̱߳���, ��Ҫ����Ϣ������, ��ȡ������Ϣ*/
    pstRctReactor->pstPthCfgCtx     = pstRctCfgInfo;
    /*��������Լ̳�*/
    pstRctReactor->pstRctEvtReactor = pstRctReactor;

    /*���̵߳�EPOLL, ÿ���̶߳���һ��EPOLL, �����ĵ��ι����¼���*/
    pstRctCfgInfo->ulEpollId  = VOS_EPOLL_Create(pstRctCfgInfo->ulEpollWaitNum);
    if ( VOS_EPOLL_INVALID == pstRctCfgInfo->ulEpollId )
    {
        VOS_Free((CHAR *)pstRctReactor);
        DbgRctModuleError("VOS_EPOLL_Create error!");
        return VOS_ERR;
    }
    
    pstRctCfgInfo->stEvtInfo.ulPid             = VOS_GetSelfPhreadID();

    /*1 . ����EPOLL�¼�,�����¼��Ļ���*/
    if(VOS_ERR == RCT_Net_EventHandlerCreate(pstRctReactor) )
    {
        DbgRctModuleError("RCT_Net_EventHandlerCreate error!");
        VOS_EPOLL_Release(pstRctCfgInfo->ulEpollId);
        VOS_Free((CHAR *)pstRctReactor);
        return VOS_ERR;
    }
    
    /*2. ��ʱ�� �¼�*/
    if ( VOS_ERR == RCT_Timer_EventHandlerCreate(pstRctReactor) )
    {
        DbgRctModuleError("RCT_Timer_EventHandlerCreate error!");
        RCT_Net_EventHandlerRelease(&pstRctReactor->stNetEvtHandler);
        VOS_EPOLL_Release(pstRctCfgInfo->ulEpollId);
        VOS_Free((CHAR *)pstRctReactor);
        return VOS_ERR;
    }
  
    /*3.�ϻ��¼�*/
    if ( VOS_ERR == RCT_Expire_EventHandlerCreate(pstRctReactor) )
    {
        DbgRctModuleError("RCT_Expire_EventHandlerCreate error!");
        RCT_Net_EventHandlerRelease(&pstRctReactor->stNetEvtHandler);
        RCT_Timer_EventHandlerRelease(&pstRctReactor->stTimerHandler);
        VOS_EPOLL_Release(pstRctCfgInfo->ulEpollId);
        VOS_Free((CHAR *)pstRctReactor);
        return VOS_ERR;
    }

    /*4. �̼߳���Ϣ: ���߳�Ҳ��һ��socket*/
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


/*��EPOLL���������ڵ��߳���Դ�ͷ�*/
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
 �� �� ��  : RCT_Task_ReactorGetPthIndexInfo
 ��������  : ��ȡ�߳����ͺ�������Ϣ
 �������  : RCT_INDEXINFO_S *pstIndexInfo  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_Task_ReactorEvtInfoGet
 ��������  : ��ȡ�Լ���ǰ�̵߳�ͨ�ž�����Ϣ
 �������  : RCT_COMMUTEINFO_S *pstEvtInfo  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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

/*EPOLL�ȴ��¼�����*/
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

    /*һֱ�ȴ�ֱ�����¼�����*/
    lRet = VOS_EPOLL_Wait(lEpollID, &pstRctEvtReactor->stNetEvtHandler.stEpollWait, VOS_EPOLL_TIME_INFININ);
    if ( VOS_EPOLL_ERR == lRet )
    {
        DbgRctModuleError("SSN_EPOLL_Wait error!");
        return VOS_ERR;
    }
    
    return lRet;
}


/*EPOLL�¼��ַ���*/
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

    /*��ȡ��ǰ����������*/
    ulEvtNum = pstNetEvtHandler->stEpollWait.ulEvtNum;

    /*���������¼�*/
    for(ulIndex=0;ulIndex < ulEvtNum; ulIndex++)
    {    
        /*��ȡ�����¼�*/
        pstEvent        =  &pstNetEvtHandler->stEpollWait.astEevent[ulIndex];
        lSockfd          =  pstEvent->lSockFd;
        pstNetEvtOps = pstNetEvtHandler->apstEpollEvtOps[lSockfd];

        /*��Ӧ�÷���: û��ע��������¼�*/
        if ( NULL == pstNetEvtOps )
        {
            DbgRctModuleError("RCT_Task_ReactorWaitAndDispatch not found register net event function!");
            //There is must be something system wrong!
            continue;
        }

        /*�����Ӧ���¼�*/
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



/*��EPOLL�Ĵ�������������*/
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
 �� �� ��  : RCT_TaskPthreadCfgCtxInit
 ��������  : �߳�����ȫ�����������ı���ĳ�ʼ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��17��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
LONG RCT_TaskPthCfgCtxInit()
{
    ULONG   ulIndex 	= 0;
	ULONG 	ulSubTypeIndex 	= 0;

    /*�߳�ͬ������ź�*/
    if( VOS_ERR == VOS_SM_Init(&g_stTaskCompelete) )
    {
        DbgRctModuleError("vos sm init error!");
        return VOS_ERR;
    }

    /*�����߳������ʼ��������ע������*/
    g_pstPthMapCfgCtx = (RCT_PTHCTX_INFO_S *)VOS_Malloc(RCT_MID_SID_TSK, sizeof(RCT_PTHCTX_INFO_S));
    if ( NULL == g_pstPthMapCfgCtx )
    {
        DbgRctModuleError("malloc error!");
        return VOS_ERR;
    }

    /*�����ʼ��*/
    VOS_Mem_Zero((CHAR *)g_pstPthMapCfgCtx, sizeof(RCT_PTHCTX_INFO_S));
	
    for( ulIndex = 0; ulIndex < RCT_PTHMAXNUMS; ulIndex++ )
    {
        g_pstPthMapCfgCtx->pstPthCfgCtx[ulIndex]  = NULL;
    }

    /*��ʼ��һ������һ��ҵ��ע�ắ������*/
    for(ulIndex =0; ulIndex < RCT_TYPE_NUMS; ulIndex++ )
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
 �� �� ��  : RCT_TaskPthreadCfgCtxUninit
 ��������  : �ͷ�ȫ�ֵ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
*****************************************************************************/
VOID RCT_TaskPthCfgCtxUninit()
{
    ULONG ulIndex = 0;
    
    if ( NULL == g_pstPthMapCfgCtx )
    {
        DbgRctModuleWarning("pthread task global var is null!");
        return;
    }

    /*�ͷ���Ϣ*/
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
 �� �� ��  : RCT_TaskPthreadCtxRecord
 ��������  : ����ÿ���̵߳������ļ�¼��¼��Ϣ
 �������  : ULONG ulType      
             ULONG ulSubIndex  
             void *pvPthCtx    
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��17��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
INT32 RCT_TaskPthCtxRecord(RCT_INICFG_CTX_S *pstPthCfgCtx)
{
    ULONG ulArryIndex = 0;

    /*��ǰ���̳߳�ʼ������*/
    ulArryIndex = pstPthCfgCtx->stIndexInfo.ulArryIndex;

    /*���浽ÿ���̵߳�������*/
    if ( NULL != g_pstPthMapCfgCtx->pstPthCfgCtx[ulArryIndex] )
    {
        DbgRctModuleError("system error!");
        return VOS_ERR;
    }

	/*ֱ�ӱ��浱ǰ��ֵ*/
    g_pstPthMapCfgCtx->pstPthCfgCtx[ulArryIndex]  = pstPthCfgCtx;

    VOS_InterlockedIncrement(&g_pstPthMapCfgCtx->ulPthNums);

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : RCT_TaskPthreadGetNums
 ��������  : ��ȡ��ǰ���߳�����
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��19��
    ��    ��   : ����
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_TaskPthreadSyncMapInfo
 ��������  : �����е��߳���Ϣ��ȫ������ͬ��, �ṩȫ��MAP��ѯ
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��17��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
LONG RCT_TaskPthSetSyncMapInfo(ULONG ulRctType, ULONG ulSubIndex, ULONG ulPthIndex )
{
    RCT_COMMUTEINFO_S    *pstEvtInfo     = NULL;

    if ( NULL != g_pstPthMapCfgCtx->pstPthCfgCtx[ulPthIndex] )
    {
        /*ע�⣺��ʼ����ʱ��һ���ǰ������õ���ͬ��˳������,�����̼߳��ͨ�ž������޷���Ӧ*/
        pstEvtInfo = &g_pstPthMapCfgCtx->pstPthCfgCtx[ulPthIndex]->stEvtInfo;
        
        /*��������ʽ����*/
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
 �� �� ��  : RCT_TaskPthreadGetPthIndexInfo
 ��������  : ��ȡ��ǰ�̵߳�����������Ϣ
 �������  : ULONG   *pulRctType            --��ǰ���߳�����
           ULONG *pulSubIndex           --��ǰ���߳�������
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��19��
    ��    ��   : ����
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_TaskPthGetCommunMapInfo
 ��������  : ��ȡͨ�ž������Ϣ
 �������  :    ULONG ulRctType             --�߳�����
            ULONG ulSubIndex            --�߳�����
            RCT_COMMUTEINFO_S *pstEvtInfo    --��ȡ��������Ϣ 
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��19��
    ��    ��   : ����
    �޸�����   : �����ɺ���

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
        /*��������ʽ����*/
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
 �� �� ��  : RCT_TaskPthreadArrayInitRegister
 ��������  :     ע���ⲿ���߳�ҵ���ʼ������
 �������  :     VOS_CALLBACK pfInitCb    
             VOID *pvArg              
             ULONG ulTskIndex         
             ULONG ulType             
             VOS_CALLBACK pfUnInitCb  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
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

    /*�������ͽ��г�ʼ��ע��*/
    switch(ulRctType)
    {
        case RCT_TYPE_MP_CLI:
        case RCT_TYPE_MP_AAA:
        case RCT_TYPE_DP_TCP:
        case RCT_TYPE_DP_SSL:
        case RCT_TYPE_DP_UDPS:
        case RCT_TYPE_DP_TWORK:
        case RCT_TYPE_DP_VSNAT:
            /*���ý��и�ֵ����*/
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
 �� �� ��  : RCT_TaskPthreadArrayInitRegister
 ��������  : ע���ⲿ���̳߳�ʼ������
 �������  : VOS_CALLBACK pfInitCb    
             VOID *pvArg              
             ULONG ulTskIndex         
             ULONG ulType             
             VOS_CALLBACK pfUnInitCb  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
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
 �� �� ��  : RCT_TaskArrayBizMsgHandlerRegister
 ��������  :    ҵ�����Ϣͨ��ע��
 �������  :    VOS_CALLBACK pfInitCb    
            VOID *pvArg              
            ULONG ulRctType          
            ULONG ulRctSubType       
            VOS_CALLBACK pfUnInitCb  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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

    /*�������ͽ��г�ʼ��ע��*/
    switch(ulRctType)
    {
        case RCT_TYPE_MP_CLI:
        case RCT_TYPE_MP_AAA:
        case RCT_TYPE_DP_TCP:
        case RCT_TYPE_DP_SSL:
        case RCT_TYPE_DP_UDPS:
        case RCT_TYPE_DP_TWORK:
        case RCT_TYPE_DP_VSNAT:
            /*���ý��и�ֵ����*/
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
 �� �� ��  : RCT_TaskArryBizMsgHandlerRunCall
 ��������  : ������Ϣע�ắ��
 �������  : ULONG ulRctType     
             ULONG ulRctSubType  
             VOID *pvArg1        
             VOID *pvArg2        
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
        /*ֱ�ӵ��������Ϣ����*/
        ((pfComMsgHandlerCallBack)g_stArryRegInfo.stArryBizMsgHandlerCb[pstMessage->stHead.ulDstRctType][pstMessage->stHead.ulDstRctSubType].pfMsgHandlerCb)(
                &pstMessage->stHead, pstMessage->pcMsgData, pstMessage->ulMsgLen);
    }
   
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : RCT_TaskArrayBizMsgHandlerUnRegister
 ��������  : ҵ����Ϣͨ��ȥע��
 �������  : ULONG ulRctType     
             ULONG ulRctSubType  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_TaskPthreadArrayInitRun
 ��������  : ÿ���߳�������Դ��ʼ�����߳�������ҵ�������Ķ���Ҫ��ʼ��, �����ж����ʼ������
 �������  : ULONG ulType  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 

*****************************************************************************/
INT32 RCT_TaskPthreadArrayInitRun(ULONG ulRctType)
{
    ULONG ulSubIndex = 0;

    /*�������ͽ��г�ʼ��ע��*/
    if ( ulRctType >= RCT_TYPE_NUMS )
    {
        DbgRctModuleError("ultask index error!");
        return VOS_ERR; 
    }

    /*�������ͽ��г�ʼ��ע��*/
    switch(ulRctType)
    {
        case RCT_TYPE_MP_CLI:
        case RCT_TYPE_MP_AAA:
        case RCT_TYPE_DP_TCP:
        case RCT_TYPE_DP_UDPS:
        case RCT_TYPE_DP_TWORK:
        case RCT_TYPE_DP_VSNAT:
            /*���и�ע���ҵ���ʼ������, Ĭ��ֻ��һ��ҵ�����*/
            if( NULL != g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][RCT_SUBTYPE_SINGLE].stpfInit.pvcbFunc)
            {
                ((VOS_CALLBACK)g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][RCT_SUBTYPE_SINGLE].stpfInit.pvcbFunc)(g_stArryRegInfo.stArryBizCtxInitCb[ulRctType][RCT_SUBTYPE_SINGLE].stpfInit.pvData);
            }
            break;
        /*Ŀǰ��SSL�ж��ҵ������, �����⴦����*/
        case RCT_TYPE_DP_SSL:
            /*�̵߳�ҵ�������Ķ���Ҫ����һ��*/
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






