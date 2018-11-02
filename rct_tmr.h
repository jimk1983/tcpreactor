/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_tim.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年2月18日
  最近修改   :
  功能描述   : 定时器
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月18日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/
#define RCT_TIMER_RECVBUF_SIZE          256
/*500ms*/
#define RCT_SLEEP_UNIT_US               500000

/*定时器类型: 一次定时器*/
#define RCT_TIMER_TYPE_ONCE             0
/*定时器类型:循环定时器*/
#define RCT_TIMER_TYPE_RECYLE           1

/*每个线程的定时器的数量*/
#define RCT_TIMER_ITEMS_NUM             200


/**************[TimerOps]***********************/

/*定时事件*/
typedef struct tagRctReactorTimerHandler           RCT_REACTOR_TIMERHANDLER_S;
typedef struct tagRctReactorTimerOpts               RCT_REACTOR_TIMER_OPT_S;


#define RCT_REACTOR_TIMER_MAX_NUM       200 
#define RCT_REACTOR_TIMER_UDPBUF        256  
#define RCT_REACTOR_TIMEROUT_US         100000  /*100ms*/

typedef VOID (*rtm_reactor_timerops_cb)(VOID *pvhandler);
/*单个定时器*/
struct tagRctReactorTimerOpts
{
    /*定时器节点*/
    VOS_DLIST_NODE_S          stTimerNode;
    /*TimerType: 0: once单次, 1:recycle循环*/
    ULONG                     ulTimeType;
    /*定时器数组ID */
    ULONG                     ulTimerID;
    /*ulTimerOut=2, example: 500ms * 2 = 1s, RCT_SLEEP_UNIT_US=500ms*/
    ULONG                     ulTimerOut;
    /*定时器时间戳*/
    ULONG                     ulTimeStamp;
    /*定时器回调函数*/
    VOS_CALLBACK_S            stTimercb;
};

/*定时器采用数组的方式实现*/
struct tagRctReactorTimerHandler
{
    /*触发器对象可以继承*/
    RCT_EVTREACTOR_S                    *pstRctEvtReactor;
    /*定时器接收socket，接收到数据*/
    LONG                                 lTimrEventfd;
    /*定时器链表*/
    VOS_DLIST_NODE_S                     stTimerList;
    /*使用网络事件实现该触发器*/
    RCT_REACTOR_NETEVT_OPT_S             stNetEvtOps;
    /*定时器扫描注册数组*/
    RCT_REACTOR_TIMER_OPT_S            **apstTimerOps; 
};


LONG RCT_Timer_EventHandlerCreate(RCT_EVTREACTOR_S *pstRctReactor);
LONG RCT_Timer_EventHandlerRelease(RCT_REACTOR_TIMERHANDLER_S *pstTimerHandler);


LONG RCT_Timer_EventOptsRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_TIMER_OPT_S *pstTimeOps);
LONG RCT_Timer_EventOptsUnRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_TIMER_OPT_S *pstTimeOps);




