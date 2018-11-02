/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_epr.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年5月26日
  最近修改   :
  功能描述   : 
  函数列表   :
  修改历史   :
  1.日    期   : 2016年5月26日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/

/*接收缓存*/
#define RCT_EXPIRE_RECVBUF_SIZE         256

/***************[Expire Ops]*************************/

/*老化事件*/
typedef struct tagRctReactorExpireHandler          RCT_REACTOR_EXPIREHANDLER_S;
typedef struct tagRctReactorExpireOpts              RCT_REACTOR_EXPIRE_OPT_S;

typedef VOID (*rtm_reactor_expireops_cb)(VOID *pvhandler);

/*单个老化节点: 老化链表挂载以及处理老化回调函数*/
struct tagRctReactorExpireOpts
{
    VOS_DLIST_NODE_S            stNode; 
    VOS_CALLBACK_S              stExpirecb;
    ULONG                       ulExpireConfirm;
};

/*老化处理器: 老化链表*/
struct tagRctReactorExpireHandler
{
    RCT_EVTREACTOR_S                    *pstRctEvtReactor;
    /*Current Max ssl connect node num is 200 for one epollReactor, 
         if >200, change to the hash table instand of the expirelist.*/
    VOS_DLIST_NODE_S                    stExpireList;
    /*也是基于网络事件*/
    RCT_REACTOR_NETEVT_OPT_S            stNetEvtOps;
    /*老化处理器的Socketfd*/
    LONG                                lExprEventfd;
};


VOID RCT_Expire_EventHandlerCb(VOID *pvHandler);

LONG RCT_Expire_EventHandlerCreate(RCT_EVTREACTOR_S *pstRctReactor);
LONG RCT_Expire_EventHandlerRelease(RCT_REACTOR_EXPIREHANDLER_S *pstExpireHandler);

LONG RCT_Expire_EventOptsRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_EXPIRE_OPT_S  *pstExpireOps);
LONG RCT_Expire_EventOptsUnRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_EXPIRE_OPT_S  *pstExpireOps);

