/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_net.h
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
/*网络事件*/
typedef struct tagRctReactorNetEvtHandler          RCT_REACTOR_NETEVTHANDLER_S;
typedef struct tagRctReactorNetEvtOpts              RCT_REACTOR_NETEVT_OPT_S;

/****************[NetOps]*************************/
typedef VOID (*rtm_reactor_netevt_recv_cb)(VOID *pvConn);
typedef VOID (*rtm_reactor_netevt_send_cb)(VOID *pvConn);

/*网络事件处理器: 所有事件触发器的基础*/
struct tagRctReactorNetEvtHandler
{
   RCT_EVTREACTOR_S                *pstRctEvtReactor;       /*总触发器*/ 
   VOS_EPOLL_WAIT_EVT_S             stEpollWait;            /*获取发生的EPOLL事件信息*/
   RCT_REACTOR_NETEVT_OPT_S       **apstEpollEvtOps;        /*根据对应的socketfd号所建立的*/
};

/*单个节点需要包含的信息*/
struct tagRctReactorNetEvtOpts
{
  LONG                    lSockfd;               /*网络事件的socket号*/
  ULONG                   ulEventMask;       /*网络事件关注EPOLL_IN/ EPOLL_OUT*/
  VOS_CALLBACK_S          stRecv;               /*接收的事件处理回调函数和节点指针*/
  VOS_CALLBACK_S          stSend;               /*发送的事件处理回调函数和节点指针*/
};

/*网络节点初始化*/
#define RCT_REACTOR_NETEVTOPTS_INIT(pstNetEvtOps_, ifd_, ulEventMask_, pfRecv_, pfSend_, pvHandler_) do{\
    (pstNetEvtOps_)->lSockfd = (ifd_);\
    (pstNetEvtOps_)->ulEventMask = (ulEventMask_);\
    VOS_CALLBACK_INIT(&(pstNetEvtOps_)->stRecv, pfRecv_, pvHandler_);\
    VOS_CALLBACK_INIT(&(pstNetEvtOps_)->stSend, pfSend_, pvHandler_);\
}while(0);

/*定时节点初始化*/
#define RCT_REACTOR_TIMEOPTS_INIT(pstTimeOps_,ulTimeType_, ulTimeOut_, pfTimeCb_,pvTimerConn_) do{\
    (pstTimeOps_)->ulTimeType  =  (ulTimeType_);\
    (pstTimeOps_)->ulTimerOut   = (ulTimeOut_);\
    VOS_CALLBACK_INIT(&(pstTimeOps_)->stTimercb, pfTimeCb_, pvTimerConn_);\
}while(0);

/*老化节点初始化*/
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



