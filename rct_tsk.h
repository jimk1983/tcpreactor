/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : rct_tsk.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年2月15日
  最近修改   :
  功能描述   : 主任务创建
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月15日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/

/*业务类型初始化函数注册*/
typedef struct tagRctTaskPfCallback
{
    ULONG                       ulRctType;      /*Rct的线程类型*/
    ULONG                       ulSubType;      /*Rct的子类型类型*/
    VOS_CALLBACK_S              stpfInit;       /*注册的业务函数和参数*/
    VOS_CALLBACK_UNINIT         pfUnInit;       /*该业务线程任务的资源释放，不带参数了*/
}RCT_TASKCB_S;


/*业务类型消息处理函数注册*/
typedef struct tagRctTaskMsgHandlerCallback
{
    ULONG                       ulRctType;                  /*Rct的线程类型*/
    ULONG                       ulSubType;                  /*Rct的子类型类型*/
    pfComMsgHandlerCallBack     pfMsgHandlerCb;             /*业务消息处理函数*/
    CHAR                        acModuleName[RCT_STR_LEN];  /*模块名称*/
}RCT_TASKMSGCB_S;


/*每个线程的类型的配置信息，提供注册信息*/
typedef struct tagRctPthTskConfigInfo
{
	/*业务上下文初始化注册函数, SSL因为一个线程中有多个业务种类，所以需要多个注册*/

    RCT_TASKCB_S        stArryBizCtxInitCb[RCT_TYPE_NUMS][RCT_SUBTYPE_MAXNUMS];    
    RCT_TASKMSGCB_S     stArryBizMsgHandlerCb[RCT_TYPE_NUMS][RCT_SUBTYPE_MAXNUMS];
}RCT_TASKCONF_INFO_S, *PRCT_TASKCONF_INFO_S;
/*******************************************************/

/*全局线程上下文配置记录, 这个是全局的配置, 启动后就不再修改, 按序启动*/
typedef struct tagRctPthCtxInfo
{
    /*数组来记录每个线程的自己上下文信息， 一个线程就一个信息*/
    RCT_INICFG_CTX_S    *pstPthCfgCtx[RCT_PTHMAXNUMS];  
    /*线程总数，也作为索引，需要+1*/
    ULONG               ulPthNums;                      

    /*最后所有线程完成时候，需要同步一次，保证所有索引是正确的, 用于全局快速查找
      最终同步保存到该成员中*/
    RCT_COMMUTEINFO_S   stMapEventFdInfo[RCT_TYPE_NUMS][RCT_TYPE_PTHMAXNUMS];
    
}RCT_PTHCTX_INFO_S, *PRCT_PTHCTX_INFO_S;

/*******************************************************/

/*******************************************************/

extern __thread  RCT_EVTREACTOR_S * g_th_pstReactor;

/*总的触发管理器对象*/
struct tagRctEvtReactor
{

    RCT_EVTREACTOR_S               *pstRctEvtReactor;

    RCT_REACTOR_NETEVTHANDLER_S     stNetEvtHandler;

    RCT_REACTOR_TIMERHANDLER_S      stTimerHandler; 

    RCT_REACTOR_EXPIREHANDLER_S     stExpireHandler;

    RCT_REACTOR_MGT_HANDLER_S       stMgtHandler;  
    
    /*线程索引信息*/
    PRCT_INICFG_CTX_S               pstPthCfgCtx;        /*本线程的配置信息*/    
};
/***************************************************************************/


LONG    RCT_TaskPthCfgCtxInit();

VOID    RCT_TaskPthCfgCtxUninit();

INT32   RCT_TaskPthreadArrayInitRun(ULONG ulRctType);

INT32   RCT_TaskPthCtxRecord(RCT_INICFG_CTX_S *pstPthCfgCtx);

INT32   RCT_TaskPthGetNums();

LONG    RCT_TaskPthGetPthTypeInfo(ULONG   *pulRctType, ULONG *pulSubIndex);

LONG    RCT_TaskPthSetSyncMapInfo(ULONG ulRctType, ULONG ulSubIndex,  ULONG ulPthIndex);

LONG    RCT_TaskPthGetCommunMapInfo(ULONG ulRctType, ULONG ulSubIndex, RCT_COMMUTEINFO_S *pstEvtInfo);

VOID    RCT_Task_MainCompeleteWaitfor();

INT32   RCT_TaskArrayBizCtxInitRegister(VOS_CALLBACK pfInitCb, VOID *pvArg, ULONG ulRctType,ULONG ulRctSubType, VOS_CALLBACK pfUnInitCb);

INT32   RCT_TaskArrayBizCtxInitUnRegister(ULONG ulRctType, ULONG ulRctSubType);

INT32   RCT_TaskArrayBizMsgHandlerRegister(ULONG ulRctType,ULONG ulRctSubType, CHAR *pcModuName, pfComMsgHandlerCallBack pfMsgHandlerCb);

INT32   RCT_TaskArrayBizMsgHandlerUnRegister(ULONG ulRctType, ULONG ulRctSubType);

LONG    RCT_TaskArrayBizMsgHandlerRunCall(RCT_MQUENODE_S *pstMessage);
    

/************************************************************************************/

VOID *RCT_Task_Main(VOID *pstRctInfo);

LONG RCT_Task_ReactorCreate(RCT_INICFG_CTX_S *pstRctCfgInfo);

LONG RCT_Task_ReactorRelease();

LONG RCT_Task_ReactorWait(RCT_EVTREACTOR_S *pstRctEvtReactor);

LONG RCT_Task_ReactorDispatch(RCT_EVTREACTOR_S *pstRctEvtReactor);

LONG RCT_Task_ReactorWaitAndDispatch(RCT_EVTREACTOR_S *pstRctEvtReactor);

LONG RCT_Task_ReactorGetCommunInfo(RCT_COMMUTEINFO_S *pstEvtInfo);

LONG RCT_Task_ReactorGetPthIndexInfo(RCT_INDEXINFO_S *pstIndexInfo);




