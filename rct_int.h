/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_int.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年2月14日
  最近修改   :
  功能描述   : 初始化
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月14日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/



/*消息需要使用的线程信息，用于消息发送和接收*/
/*要求根据类型，可以快速的找到该eventfd信息, 通过二维数组直接查找*/
/*线程的通信矩阵的信息*/
typedef struct tagRctPthCommuteInfo
{
    ULONG               ulPid;          /*线程ID*/
    LONG                ulEventFdMsg;   /*消息的EventFd信息*/
    LONG                ulEventFdTimer; /*定时器的EventFd信息*/
    LONG                ulEventFdExpr;  /*老化的EventFd信息*/
    PRCT_MQUEMGRCTX_S   pstQueMgrCtx;  /*核间消息的消息队列, RCT架构的消息队列*/
    
}RCT_COMMUTEINFO_S, *PRCT_COMMUTEINFO_S;



/*线程任务的配置*/
typedef struct tagRctIniConfig
{
    ULONG       ulPthRctType;     /*线程业务类型*/
    ULONG       ulPthRctSubNums;  /*子线程创建个数: 默认是1个，但是SSL可能有多个*/
    const char  *pcPthName;       /*该线程名称*/
}RCT_INICONF_S;


/*当前线程的上下文配置信息*/
typedef struct tagRctInitConfigCtxInfo
{   
    RCT_INDEXINFO_S     stIndexInfo;    /*保存的线程索引信息*/
    ULONG               ulEpollWaitNum; /*线程EPOLL监听事件数量*/
    LONG                ulEpollId;      /*该事件的EPOLL ID */
    
    RCT_COMMUTEINFO_S   stEvtInfo;      /*各种Event信息记录，最后更新到总的映射表， 用于快速查找*/
    
}RCT_INICFG_CTX_S, *PRCT_INICFG_CTX_S;

LONG    RCT_InitPthreadTasksRun();

INT32   RCT_InitPthCfgCtxCreate(PRCT_INICFG_CTX_S *ppstCfgCtx, RCT_INDEXINFO_S *pstInfo);

VOID    RCT_InitPthCfgCtxRelease(RCT_INICFG_CTX_S *pstCtx);

ULONG   RCT_InitPthCfgGetSubNums(ULONG ulRctType);

/**************************************************************/
typedef struct tagRctInitTimerBaseConfig
{
    ULONG lPthreadNums;                       /*多少个线程*/
    LONG aulMapExprEvtfd[RCT_PTHMAXNUMS];     /*需要向线程发送的老化端口*/
    LONG aulMapTimrEvtfd[RCT_PTHMAXNUMS];     /*需要向线程发送的定时器端口*/

}RCT_INIT_TIMERBASE_CONFIG_S;


VOID RCT_Test_InitPthreadRunSyncMapInfo();



