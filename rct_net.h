/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_net.h
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
/*�����¼�*/
typedef struct tagRctReactorNetEvtHandler          RCT_REACTOR_NETEVTHANDLER_S;
typedef struct tagRctReactorNetEvtOpts              RCT_REACTOR_NETEVT_OPT_S;

/****************[NetOps]*************************/
typedef VOID (*rtm_reactor_netevt_recv_cb)(VOID *pvConn);
typedef VOID (*rtm_reactor_netevt_send_cb)(VOID *pvConn);

/*�����¼�������: �����¼��������Ļ���*/
struct tagRctReactorNetEvtHandler
{
   RCT_EVTREACTOR_S                *pstRctEvtReactor;       /*�ܴ�����*/ 
   VOS_EPOLL_WAIT_EVT_S             stEpollWait;            /*��ȡ������EPOLL�¼���Ϣ*/
   RCT_REACTOR_NETEVT_OPT_S       **apstEpollEvtOps;        /*���ݶ�Ӧ��socketfd����������*/
};

/*�����ڵ���Ҫ��������Ϣ*/
struct tagRctReactorNetEvtOpts
{
  LONG                    lSockfd;               /*�����¼���socket��*/
  ULONG                   ulEventMask;       /*�����¼���עEPOLL_IN/ EPOLL_OUT*/
  VOS_CALLBACK_S          stRecv;               /*���յ��¼�����ص������ͽڵ�ָ��*/
  VOS_CALLBACK_S          stSend;               /*���͵��¼�����ص������ͽڵ�ָ��*/
};

/*����ڵ��ʼ��*/
#define RCT_REACTOR_NETEVTOPTS_INIT(pstNetEvtOps_, ifd_, ulEventMask_, pfRecv_, pfSend_, pvHandler_) do{\
    (pstNetEvtOps_)->lSockfd = (ifd_);\
    (pstNetEvtOps_)->ulEventMask = (ulEventMask_);\
    VOS_CALLBACK_INIT(&(pstNetEvtOps_)->stRecv, pfRecv_, pvHandler_);\
    VOS_CALLBACK_INIT(&(pstNetEvtOps_)->stSend, pfSend_, pvHandler_);\
}while(0);

/*��ʱ�ڵ��ʼ��*/
#define RCT_REACTOR_TIMEOPTS_INIT(pstTimeOps_,ulTimeType_, ulTimeOut_, pfTimeCb_,pvTimerConn_) do{\
    (pstTimeOps_)->ulTimeType  =  (ulTimeType_);\
    (pstTimeOps_)->ulTimerOut   = (ulTimeOut_);\
    VOS_CALLBACK_INIT(&(pstTimeOps_)->stTimercb, pfTimeCb_, pvTimerConn_);\
}while(0);

/*�ϻ��ڵ��ʼ��*/
#define RCT_REACTOR_EXPIREOPTS_INIT(pstExpireOps_,pfExpireCb_,pvExpireConn_) do{\
    VOS_DLIST_INIT(&(pstExpireOps_)->stNode);\
    (pstExpireOps_)->ulExpireConfirm = VOS_FALSE;\
    VOS_CALLBACK_INIT(&(pstExpireOps_)->stExpirecb, pfExpireCb_, pvExpireConn_);\
}while(0);


LONG RCT_Net_EventHandlerCreate(RCT_EVTREACTOR_S *pstRctEvtReactor);
LONG RCT_Net_EventHandlerRelease(RCT_REACTOR_NETEVTHANDLER_S *pstNetEvtHandler);

LONG RCT_Net_EventOptsRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_NETEVT_OPT_S *pstEvtOps);
LONG RCT_Net_EventOptsUnRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_NETEVT_OPT_S *pstEvtOps);

LONG RCT_Net_EventEpollMaskModify(RCT_EVTREACTOR_S *pstRctReactor, ULONG ulOption, LONG lSockfd, ULONG *pulEvtMask);



