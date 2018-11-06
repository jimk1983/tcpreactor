/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_ctx_api.c
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��2��14��
  ����޸�   :
  ��������   : �����rctģ��ӿ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��2��14��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>

/*****************************************************************************
 �� �� ��  : RCT_API_EnvInit
 ��������  : ��һ��:
                          ���л�����ʼ��
                          (����VOS�ڴ���������������ɵ�)
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��5��28��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
INT32 RCT_API_EnvInit()
{
    if(VOS_ERR == VOS_EnvInit())
    {
        DbgRctModuleError("vos environment init error");
        return VOS_ERR;
    }
    
    /*�����߳����ñ������������Դ��ʼ��*/
    if ( VOS_ERR == RCT_TaskPthCfgCtxInit() )
    {
        DbgRctModuleError("rct task arry init error");
        VOS_EnvUnInit();
        return VOS_ERR;
    }

    

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : RCT_API_EnvRun
 ��������  : �ڶ�����
                           ��ʼ��������ע������ǰ����ע�ắ��Ҳ�Ѿ���Ҫ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��5��28��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
INT32 RCT_API_EnvRun()
{
    /*�����߳�����,���߳�������ע���ҵ����Դ��ʼ������*/
    if( VOS_ERR ==  RCT_InitPthreadTasksRun() )
    {
        DbgRctModuleError("rct task pthread running...error!");
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : RCT_API_EnvUnInit
 ��������  : ������Դ���սӿ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��5��28��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID RCT_API_EnvUnInit()
{
    RCT_TaskPthCfgCtxUninit();

    VOS_EnvUnInit();
}


/*****************************************************************************
 �� �� ��  : RCT_API_TaskRegister
 ��������  : �����ʼ��ע��
 �������  :    VOS_CALLBACK pfInitCb   ---��Ҫ�ڸ��߳���������ģ�������ĳ�ʼ��������SWM/NEM...
                                       ��Щ��������rct_def.h�п��Զ������
            VOID *pvArg             ---ע��ʱ��Ҫ����Ĳ���
            ULONG ulTskIndex        ---ҵ����������ͣ������߳���Ҳ���ܴ��ڶ��ҵ��:SWM/NEM
            ULONG ulType            ---��rct_def.h�ж�������ͣ�ֱ����������������п���ע��
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��5��28��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

ע��:ע�ắ������Ҫ�ڻ�����ʼ��֮ǰ�����
��ulTskIndex����������ע����ulType���͵��߳��Ͻ��г�ʼ��
*****************************************************************************/
INT32 RCT_API_EnvTaskInitRegister(VOS_CALLBACK pfInitCb, VOID *pvArg, ULONG ulRctType,ULONG ulRctSubType, VOS_CALLBACK pfUnInitCb)
{
    if ( NULL == pfInitCb 
        || ulRctType > RCT_TYPE_NUMS )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

    /*���п���ע��*/
    if ( VOS_ERR == RCT_TaskArrayBizCtxInitRegister(pfInitCb, pvArg, ulRctType,ulRctSubType,  pfUnInitCb) )
    {
        DbgRctModuleError("rct task ctx init register error");
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : RCT_API_EnvTaskInitUnRegister
 ��������  : ȥע��
 �������  : ULONG ulTskIndex  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��11��7��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
INT32 RCT_API_EnvTaskInitUnRegister(ULONG ulRctType, ULONG ulRctSubType)
{
    if ( ulRctType > RCT_TYPE_NUMS)
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

    /*ȡ��ע��*/
    if ( VOS_ERR == RCT_TaskArrayBizCtxInitUnRegister(ulRctType, ulRctSubType) )
    {
        DbgRctModuleError("rct task ctx init un register error");
        return VOS_ERR;
    }

    return VOS_OK;
}


/*****************************************************************************
 �� �� ��  : RCT_API_EnvGetPthreadInfo
 ��������  : ��ȡ��ǰ�̵߳�һЩ������Ϣ, 
           (��Ҫ���̼߳���Ϣ����ʱ�򣬻ذ��ȽϷ���)
 �������  : ULONG    *pulRctType,  ��ȡ�߳�ҵ������
           ULONG  *pulPolIndex  ��ȡ�߳�ҵ���������ڵĵڼ�������
 �������  : �� 
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��6��12��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_API_EnvGetTypeCfgSubNums
 ��������  : ���ص�ǰ��������Ϣ������SSL�ж��
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
ULONG   RCT_API_EnvGetTypeCfgSubNums(ULONG ulRctType)
{
    return RCT_InitPthCfgGetSubNums(ulRctType);
}


/*****************************************************************************
 �� �� ��  : RCT_API_MgtLockySend
 ��������  : ��Ҫ�����ڵ����Ϣ����,���һ�������ǽ�-
                 �����ݣ����ص�ʱ����Զ����
 �������  : ULONG ulSrcSubType   
             ULONG ulDstRctType   
             ULONG ulDstSubType   
             ULONG ulDstPthIndex  
             CHAR *pcData         
             ULONG ulLen          
             VOID *pcLockConn     �û����͵Ľڵ�
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��21��
    ��    ��   : ����
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_API_MgtNormalSend
 ��������  : ��ͨ��һ������Ϣ������Ҫ�������ݵ�,һ������ת����
 �������  : ULONG ulSrcSubType   
           ULONG ulDstRctType   
           ULONG ulDstSubType   
           ULONG ulDstPthIndex  
           CHAR *pcData         
           ULONG ulLen          
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��21��
    ��    ��   : ����
    �޸�����   : �����ɺ���

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
�� �� ��  : RCT_API_MgtLockySendBack
��������  : ����
�������  : RCT_MSG_HEAD_S            *pstSrcHead  
          CHAR *pcData                           
          ULONG ulLen                            
�������  : ��
�� �� ֵ  : 
���ú���  : 
��������  : 

�޸���ʷ      :
1.��    ��   : 2018��6��3��
 ��    ��   : ����
 �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_API_MgtHandlerRegister
 ��������  : ҵ��ģ�����Ϣ������
 �������  : ULONG ulRctType                   
           ULONG ulSubType                   
           CHAR *pcModuName                  
           pfComMsgHandlerCallBack pfMethod  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
INT32 RCT_API_MgtHandlerRegister(ULONG ulRctType, ULONG ulSubType, CHAR *pcModuName, pfComMsgHandlerCallBack pfMethod)
{
    if ( NULL == pfMethod
        || NULL == pcModuName )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

    /*ҵ���߳�ע�ắ��, �ڲ���װ*/
    if ( VOS_ERR == RCT_TaskArrayBizMsgHandlerRegister(ulRctType, ulSubType, pcModuName, pfMethod) )
    {
        DbgRctModuleError("rct mgt register error");
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : RCT_API_EnvMgtRegister
 ��������  : ȥע����Ϣ�����������ڴ�����
 �������  : ULONG ulType  
             CHAR *pcName  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��6��4��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID RCT_API_MgtHandlerUnRegister(ULONG ulRctType, ULONG ulSubType)
{

    /*ҵ���߳�ע�ắ��, �ڲ���װ*/
    if ( VOS_ERR == RCT_TaskArrayBizMsgHandlerUnRegister(ulRctType, ulSubType) )
    {
        DbgRctModuleError("rct mgt register error");
        return;
    }
}

/*****************************************************************************
 �� �� ��  : RCT_API_NetOpsEventRegister
 ��������  : �����¼�����ע�ắ��
 �������  :    RCT_NETOPS_EVENT_S *pstNetOps   ----����������¼��ڵ�
                             LONG lSockfd                                  ----�������¼���sockfd
                             ULONG ulEMask                               ----��ע��EPOLLIN | EPOLLOUT
                                                                                        (VOS_EPOLL_MASK_POLLIN | VOS_EPOLL_MASK_POLLOUT)
                             VOS_CALLBACK pfRecv                     ----��Ӧ�Ľ��մ�����
                             VOS_CALLBACK pfSend                     ----��Ӧ�ķ��ʹ�����
                             VOID *pvConn                                ----��������Ľڵ�����
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��5��29��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_API_NetOpsEventUnRegister
 ��������  : ȥע�������¼��������ظ�ȥע��
 �������  : RCT_NETOPT_EVENT_S *pstNetOps  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��6��4��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_API_NetOpsEventCtrl
 ��������  : ���ͺͽ����¼����޸�
 �������  :   RCT_NETOPT_EVENT_S *pstNetOps    --ע��������¼�
                            LONG lSockfd                                   --����socket
                            ULONG ulEpollMask                           --����
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��6��6��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_API_ExpireOpsEventRegister
 ��������  : �ϻ�ע�ắ��
 �������  :     RCT_EXPIROPT_EVENT_S *pstExpireOps      ----������ϻ��¼�
             VOS_CALLBACK pfRecv                    ----�ϻ�������
             VOID *pvConn                           ----����Ľڵ�����
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��5��29��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_API_ExpireOpsEventRegister
 ��������  : �ϻ�ȥע��
 �������  : RCT_EXPIROPT_EVENT_S *pstExpireOps  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��6��4��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_API_TimerOpsEventRegister
 ��������  : ��ʱ����ע�ắ��
 �������  :    RCT_TIMEROPT_EVENT_S *pstTimerOps   ----����Ķ�ʱ���¼�
             ULONG ulTimerType                  ----��ʱ������: ���λ�ѭ��
             ULONG ulTimerOut                   ----��ʱ��ʱʱ��:����
             VOS_CALLBACK pfRecv                ----��ʱ���ص�����
             VOID *pvConn                       ----����ڵ������
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��5��29��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
 �� �� ��  : RCT_API_TimerOpsEventUnRegister
 ��������  : ȥע��
 �������  : RCT_TIMEROPT_EVENT_S *pstTimerOps  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��6��4��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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

