/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_net.c
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��5��25��
  ����޸�   :
  ��������   : �����¼�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��5��25��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>



/*****************************************************************************
 �� �� ��  : RCT_Net_EventHandlerCreate
 ��������  : �����¼��������Ĵ���
 �������  : RCT_EVTREACTOR_S *pstRctEvtReactor  --������
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��7��27��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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
    /*�����¼�������*/
    pstNetEvtHandler = (&pstRctEvtReactor->stNetEvtHandler);    

    /*������Լ̳еĴ�����*/
    pstNetEvtHandler->pstRctEvtReactor = pstRctEvtReactor;

    /*��΢�˷ѣ���ʵÿ�δ���������EpollWaitNum������OPT��
        ���߿�����������ע��Ҳ����, �Ժ��ٲ��ԣ� ������ÿ���̲߳�����������
       ��������socketfdֱ�������ٶȿ죬�����ռ任ȡʱ��*/
    ulSize = sizeof(RCT_REACTOR_NETEVT_OPT_S *)*(VOS_SOCK_FDSIZE_MAX + 1);    
    /*��������socketfdע��Ļص������¼�, ����Ϊ�Լ����ڵĽڵ�*/
    pstNetEvtHandler->apstEpollEvtOps = (RCT_REACTOR_NETEVT_OPT_S **)VOS_Malloc(RCT_MID_SID_EVT, ulSize);
    if ( NULL == pstNetEvtHandler->apstEpollEvtOps )
    {
        DbgRctModuleError("epoll event option malloc error!");
        return VOS_ERR;
    }
    
    VOS_Mem_Zero((CHAR *)pstNetEvtHandler->apstEpollEvtOps, ulSize);
    
    return VOS_OK;
}

/*�ͷ�����������������¼�, �ڲ��¼���ҵ����֤*/
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
 �� �� ��  : RCT_Net_EventOptsRegister
 ��������  : �ṩ��RCTģ���ڲ�ʹ�õ������¼�ע��
                 ע��: �ⲿʹ�õģ�����Ҫ��������ΪVOID
 �������  :    RCT_EVTREACTOR_S *pstRctReactor      
             RCT_REACTOR_NETEVT_OPT_S *pstEvtOps  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��7��27��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

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

    /*��Ϊsocketfd�������ظ�����˲�Ӧ�÷��������
       ����֮ǰû��unRegister�����ͷ�*/
    if ( NULL != pstNetEvtHandler->apstEpollEvtOps[pstEvtOps->lSockfd] )
    {
        DbgRctModuleError("ASSERT() error!");
        //System must be something wrong
        return VOS_ERR;
    }

    /*������ڵ����ע��*/
    pstNetEvtHandler->apstEpollEvtOps[pstEvtOps->lSockfd] = pstEvtOps;

    if ( VOS_EPOLL_ERR == VOS_EPOLL_Ctrl(pstRctReactor->pstPthCfgCtx->ulEpollId,pstEvtOps->lSockfd, VOS_EPOLL_CTL_ADD,pstEvtOps->ulEventMask))
    {
        DbgRctModuleError("RTM_EPOLL_CTL error!");
        return VOS_ERR;
    }
    
    return VOS_OK;
}

/*ȥע�������¼�*/
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

    /*����ظ���ȥע�ᱣ��, ��֤����ȥע�ᣬ�����������ʱ�������������0�¼�*/
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

/*�޸������¼������¼�����*/
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

    /*���ȥע�ᱣ��, һ���Ѿ�ȥע�ᣬ���е��¼��Ѿ�ʧЧ�� */
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





