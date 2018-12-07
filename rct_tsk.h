/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_tsk.h
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��2��15��
  ����޸�   :
  ��������   : �����񴴽�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��2��15��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/

/*ҵ�����ͳ�ʼ������ע��*/
typedef struct tagRctTaskPfCallback
{
    ULONG                       ulRctType;      /*Rct���߳�����*/
    ULONG                       ulSubType;      /*Rct������������*/
    VOS_CALLBACK_S              stpfInit;       /*ע���ҵ�����Ͳ���*/
    VOS_CALLBACK_UNINIT         pfUnInit;       /*��ҵ���߳��������Դ�ͷţ�����������*/
}RCT_TASKCB_S;


/*ҵ��������Ϣ������ע��*/
typedef struct tagRctTaskMsgHandlerCallback
{
    ULONG                       ulRctType;                  /*Rct���߳�����*/
    ULONG                       ulSubType;                  /*Rct������������*/
    pfComMsgHandlerCallBack     pfMsgHandlerCb;             /*ҵ����Ϣ������*/
    CHAR                        acModuleName[RCT_STR_LEN];  /*ģ������*/
}RCT_TASKMSGCB_S;


/*ÿ���̵߳����͵�������Ϣ���ṩע����Ϣ*/
typedef struct tagRctPthTskConfigInfo
{
	/*ҵ�������ĳ�ʼ��ע�ắ��, SSL��Ϊһ���߳����ж��ҵ�����࣬������Ҫ���ע��*/

    RCT_TASKCB_S        stArryBizCtxInitCb[RCT_TYPE_NUMS][RCT_SUBTYPE_MAXNUMS];    
    RCT_TASKMSGCB_S     stArryBizMsgHandlerCb[RCT_TYPE_NUMS][RCT_SUBTYPE_MAXNUMS];
}RCT_TASKCONF_INFO_S, *PRCT_TASKCONF_INFO_S;
/*******************************************************/

/*ȫ���߳����������ü�¼, �����ȫ�ֵ�����, ������Ͳ����޸�, ��������*/
typedef struct tagRctPthCtxInfo
{
    /*��������¼ÿ���̵߳��Լ���������Ϣ�� һ���߳̾�һ����Ϣ*/
    RCT_INICFG_CTX_S    *pstPthCfgCtx[RCT_PTHMAXNUMS];  
    /*�߳�������Ҳ��Ϊ��������Ҫ+1*/
    ULONG               ulPthNums;                      

    /*��������߳����ʱ����Ҫͬ��һ�Σ���֤������������ȷ��, ����ȫ�ֿ��ٲ���
      ����ͬ�����浽�ó�Ա��*/
    RCT_COMMUTEINFO_S   stMapEventFdInfo[RCT_TYPE_NUMS][RCT_TYPE_PTHMAXNUMS];
    
}RCT_PTHCTX_INFO_S, *PRCT_PTHCTX_INFO_S;

/*******************************************************/

/*******************************************************/

extern __thread  RCT_EVTREACTOR_S * g_th_pstReactor;

/*�ܵĴ�������������*/
struct tagRctEvtReactor
{

    RCT_EVTREACTOR_S               *pstRctEvtReactor;

    RCT_REACTOR_NETEVTHANDLER_S     stNetEvtHandler;

    RCT_REACTOR_TIMERHANDLER_S      stTimerHandler; 

    RCT_REACTOR_EXPIREHANDLER_S     stExpireHandler;

    RCT_REACTOR_MGT_HANDLER_S       stMgtHandler;  
    
    /*�߳�������Ϣ*/
    PRCT_INICFG_CTX_S               pstPthCfgCtx;        /*���̵߳�������Ϣ*/    
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




