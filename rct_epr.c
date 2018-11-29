/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_epr.c
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��5��26��
  ����޸�   :
  ��������   : �ϻ��¼�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��5��26��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>

/*****************************************************************************
 �� �� ��  : RCT_Expire_EventHandlerCb
 ��������  : �ϻ�����ص���ͨ�������¼������뵽�ûص��������ϻ��ص���������
 �������  : VOID *pvHandler  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
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
    
    /*�ȼ���ϻ������ǲ���Ϊ��*/
    pstList = &pstExpireHandler->stExpireList;
    
    if ( VOS_OK == VOS_Node_IsEmpty(pstList) )
    {
        return;
    }    

    /*��������ע���ϻ��Ļص�����*/
    #if 1
    VOS_DLIST_FOR_EACH_ENTRY(pstExpireOpsTmp, &pstExpireHandler->stExpireList, RCT_REACTOR_EXPIRE_OPT_S, stNode)
    {
        if ( NULL == pstExpireOpsTmp )
        {
            DbgRctModuleError("ASSERT error!");
            break;
        }

        pfExpirecb = (rtm_reactor_expireops_cb)pstExpireOpsTmp->stExpirecb.pvcbFunc;

        /*������ϻ��ڵ��Ѿ��������ϻ���ǣ���ôִ���ϻ��ص�*/
        if ( (NULL != (pfExpirecb) ) && (pstExpireOpsTmp->ulExpireConfirm == VOS_TRUE) )
        {   
            /*ע�ⲻ��ֱ������,�ȱ����ýڵ����һ���ڵ�*/
            pstExpireOpsSave = VOS_DLIST_ENTRY(pstExpireOpsTmp->stNode.prev, RCT_REACTOR_EXPIRE_OPT_S, stNode);
             
            /*ֱ�ӽ����ϻ��ڵ��Ƚ�������*/
            VOS_Node_Remove((&pstExpireOpsTmp->stNode));
            
            /*��ִ���ϻ���������ֹ�ϻ��ڵ����Ƚ�������*/
            ((rtm_reactor_expireops_cb)pfExpirecb)(pstExpireOpsTmp->stExpirecb.pvData);

            /*���пɿ��ԵĽڵ��ϻ�*/
            RCT_Reactor_MsgQueLockNodeExpired(pstExpireOpsTmp->stExpirecb.pvData);
            
            /*��������󣬼�����һ���ڵ�ѭ��*/
            pstExpireOpsTmp = pstExpireOpsSave;
            continue;
        }
    }
    #else /*ʵ�ֺ�����һ����Ч��*/
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

        /*������ϻ��ڵ��Ѿ��������ϻ���ǣ���ôִ���ϻ��ص�,����������*/
        if ( (NULL != (pfExpirecb) ) && (pstExpireOpsTmp->ulExpireConfirm == VOS_TRUE) )
        {   
            /*��ִ���ϻ���������ֹ�ϻ��ڵ����Ƚ�������*/
            ((rtm_reactor_expireops_cb)pfExpirecb)(pstExpireOpsTmp->stExpirecb.pvData);
            
            /*ע�ⲻ��ֱ������,�ȱ����ýڵ����һ���ڵ�*/
            pstExpireOpsSave = VOS_DLIST_ENTRY(pstExpireOpsTmp->stNode.next, RCT_REACTOR_EXPIRE_OPT_S, stNode);
             
            /*Ȼ��ֱ�ӽ����ϻ��ڵ��Ƚ�������*/
            VOS_Node_Remove((&pstExpireOpsTmp->stNode));

            pstExpireOpsTmp = pstExpireOpsSave;
        }
    }while(pstExpireOpsTmp != &pstExpireHandler->stExpireList);
    

    #endif

    
    return;
}


/*****************************************************************************
 �� �� ��  : RCT_Expire_EventHandlerCreate
 ��������  : �����ϻ��¼�������
 �������  : RCT_EVTREACTOR_S *pstRctReactor  
             ULONG ulExpirePort               
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 

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
    
    /*�������ϻ���������socket*/
    pstExpireHandler->lExprEventfd = VOS_EventfdCreate(0);
    if ( VOS_SOCKET_INVALID ==  pstExpireHandler->lExprEventfd )
    {
        DbgRctModuleError("Expire eventfd create error!");
        return VOS_ERR;
    }

    /*��ʼ���ϻ�����*/
    VOS_Node_Init(&pstExpireHandler->stExpireList);

    /*���ϻ��ص�ע�ᵽ�����¼���*/
    RCT_REACTOR_NETEVTOPTS_INIT(
                &pstExpireHandler->stNetEvtOps,
                pstExpireHandler->lExprEventfd,
                VOS_EPOLL_MASK_POLLIN,
                RCT_Expire_EventHandlerCb,
                NULL,
                pstExpireHandler);

    /*�����ϻ�socket�ص���ע�������¼���*/
    if(VOS_ERR == RCT_Net_EventOptsRegister(pstRctReactor, &pstExpireHandler->stNetEvtOps))
    {
       DbgRctModuleError("RCT_Net_EventOptsRegister error!");
       VOS_EventfdClose(pstExpireHandler->lExprEventfd);
       return VOS_ERR;
    }

    
    /*��������*/
    pstRctReactor->pstPthCfgCtx->stEvtInfo.ulEventFdExpr  = pstExpireHandler->lExprEventfd;

    //DbgRctModuleEvent("Expire event handler create OK!");
    DbgRctModuleEvent("Expire event handler create OK!eventfd=[%d]", pstRctReactor->pstPthCfgCtx->stEvtInfo.ulEventFdExpr);
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : RCT_Expire_EventHandlerRelease
 ��������  : �ϻ��������ڴ��ͷ�
 �������  : RCT_REACTOR_EXPIREHANDLER_S *pstExpireHandler  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
*****************************************************************************/
LONG RCT_Expire_EventHandlerRelease(RCT_REACTOR_EXPIREHANDLER_S *pstExpireHandler)
{
    if(NULL == pstExpireHandler)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }

    (VOID)RCT_Net_EventOptsUnRegister(pstExpireHandler->pstRctEvtReactor, &pstExpireHandler->stNetEvtOps);

    /*�ͷű��������¼���socket*/
    VOS_EventfdClose(pstExpireHandler->lExprEventfd);
    
    return VOS_OK;
}


/*****************************************************************************
 �� �� ��  : RCT_Expire_EventOptsRegister
 ��������  : �ṩ�ڲ�ʹ�õ��ϻ�ע�ắ��
 �������  : RCT_EVTREACTOR_S *pstRctReactor          
             RCT_REACTOR_EXPIRE_OPT_S  *pstExpireOps  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
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
    
    /*���ڵ���ӵ��ϻ�������*/
    pstList = &pstExpireHandler->stExpireList;
    VOS_Node_TailAdd(pstList, pstNode);
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : RCT_Expire_EventOptsUnRegister
 ��������  : �����ڵ�ȥע���ϻ�
 �������  : RCT_EVTREACTOR_S *pstRctReactor          
             RCT_REACTOR_EXPIRE_OPT_S  *pstExpireOps  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
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





