/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_tim.c
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��2��17��
  ����޸�   :
  ��������   : ��ʱ���¼�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��2��17��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>


/*****************************************************************************
 �� �� ��  : RCT_Timer_EventHandlerCb
 ��������  : ��ʱ��������
 �������  : VOID *pvHandler  ---���̵߳Ķ�ʱ������������
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��7��27��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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

        /*ҵ��ʱ�����õĳ�ʱʱ��,��ʱ�����Ը�ʱ��Ϊ׼(ulTimeoutΪ����)*/
        /*�ö�ʱ��ʱ���, ÿ�ζ���������¼���, ���ں�����ĳ�ʱʱ����жԱ�*/
        ulTimeStamp = pstTimerOpsTmp->ulTimeStamp;
        
        pfTimercb     =  pstTimerOpsTmp->stTimercb.pvcbFunc;
        /*һ��ʱ������ڳ�ʱʱ�䣬��ô�����ж�ʱ��*/
        if ( (NULL != (pfTimercb) ) && (ulTimeStamp >= pstTimerOpsTmp->ulTimerOut))
        {
            pstTimerOpsTmp->ulTimeStamp = 0;
            /*������һ�ζ�ʱ���ص�����*/
            ((rtm_reactor_timerops_cb)pfTimercb)(pstTimerOpsTmp->stTimercb.pvData);
            if ( RCT_TIMER_TYPE_ONCE == pstTimerOpsTmp->ulTimeType )
            {
                /*Ȼ������ǵ�����ʱ����Ҫ��������ժ�����ȱ���ǰһ���ڵ�*/
                pstTimerOpsSave = VOS_DLIST_ENTRY(pstTimerOpsTmp->stTimerNode.prev, RCT_REACTOR_TIMER_OPT_S, stTimerNode);
                /*Ȼ��ɾ�����ڵ�*/
                VOS_Node_Remove(&pstTimerOpsTmp->stTimerNode);
                /*��������󣬱�����һ�εĽڵ㣬��һ��ѭ���ͻ���뵽��һ���µĽڵ�*/
                pstTimerOpsTmp = pstTimerOpsSave;
                continue;
            }
        }
        pstTimerOpsTmp->ulTimeStamp++;
    }
    return;
}

/*****************************************************************************
 �� �� ��  : RCT_Timer_EventHandlerCreate
 ��������  : ��ʱ���������Ĵ���
 �������  : RCT_EVTREACTOR_S *pstRctReactor  ---����������
                          ULONG ulTimePort                           ---���߳����ڴ������Ķ�ʱ��UDP���ն˿�
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��7��27��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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

    /*��ʱ���ص�����ע�������¼�*/
    RCT_REACTOR_NETEVTOPTS_INIT(
                &pstTimerHandler->stNetEvtOps,
                pstTimerHandler->lTimrEventfd,
                VOS_EPOLL_MASK_POLLIN,
                RCT_Timer_EventHandlerCb,
                NULL,
                pstTimerHandler);

    /*Ϊ��socketע�������¼�*/
    if(VOS_ERR == RCT_Net_EventOptsRegister(pstRctReactor, &pstTimerHandler->stNetEvtOps))
    {
       DbgRctModuleError("RCT_Reactor_NetEvtOptsRegister error!");
       VOS_Free((CHAR *)(pstTimerHandler->apstTimerOps));
       VOS_SOCK_Close(pstTimerHandler->lTimrEventfd);
       return VOS_ERR;
    }

    /*��������*/
    pstRctReactor->pstPthCfgCtx->stEvtInfo.ulEventFdTimer = pstTimerHandler->lTimrEventfd;

    //VOS_Printf("Timer event handler create OK! TimerEventfd=[%d]", pstTimerHandler->lTimrEventfd);
    return VOS_OK;
}


/*�ͷŴ�����*/
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


/*����ע�ᶨʱ��*/
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

    /*��ʱ���Ѿ���*/
    if ( ulTimerID == RCT_TIMER_ITEMS_NUM-1 )
    {
        DbgRctModuleError("this timer option has been max limited!");
        return VOS_ERR;
    }
    
    pstTimeOps->ulTimerID = ulTimerID;
    
    /*���ýڵ��������*/
    pstList = &pstTimerHandler->stTimerList;
    pstNode = &pstTimeOps->stTimerNode;

    VOS_Node_Init(pstNode);
    VOS_Node_TailAdd(pstList, pstNode);
    
    return VOS_OK;
}


/*��ʱ��ȥע��*/
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


