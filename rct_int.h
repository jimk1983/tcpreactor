/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_int.h
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��2��14��
  ����޸�   :
  ��������   : ��ʼ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��2��14��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/



/*��Ϣ��Ҫʹ�õ��߳���Ϣ��������Ϣ���ͺͽ���*/
/*Ҫ��������ͣ����Կ��ٵ��ҵ���eventfd��Ϣ, ͨ����ά����ֱ�Ӳ���*/
/*�̵߳�ͨ�ž������Ϣ*/
typedef struct tagRctPthCommuteInfo
{
    ULONG               ulPid;          /*�߳�ID*/
    LONG                ulEventFdMsg;   /*��Ϣ��EventFd��Ϣ*/
    LONG                ulEventFdTimer; /*��ʱ����EventFd��Ϣ*/
    LONG                ulEventFdExpr;  /*�ϻ���EventFd��Ϣ*/
    PRCT_MQUEMGRCTX_S   pstQueMgrCtx;  /*�˼���Ϣ����Ϣ����, RCT�ܹ�����Ϣ����*/
    
}RCT_COMMUTEINFO_S, *PRCT_COMMUTEINFO_S;



/*�߳����������*/
typedef struct tagRctIniConfig
{
    ULONG       ulPthRctType;     /*�߳�ҵ������*/
    ULONG       ulPthRctSubNums;  /*���̴߳�������: Ĭ����1��������SSL�����ж��*/
    const char  *pcPthName;       /*���߳�����*/
}RCT_INICONF_S;


/*��ǰ�̵߳�������������Ϣ*/
typedef struct tagRctInitConfigCtxInfo
{   
    RCT_INDEXINFO_S     stIndexInfo;    /*������߳�������Ϣ*/
    ULONG               ulEpollWaitNum; /*�߳�EPOLL�����¼�����*/
    LONG                ulEpollId;      /*���¼���EPOLL ID */
    
    RCT_COMMUTEINFO_S   stEvtInfo;      /*����Event��Ϣ��¼�������µ��ܵ�ӳ��� ���ڿ��ٲ���*/
    
}RCT_INICFG_CTX_S, *PRCT_INICFG_CTX_S;

LONG    RCT_InitPthreadTasksRun();

INT32   RCT_InitPthCfgCtxCreate(PRCT_INICFG_CTX_S *ppstCfgCtx, RCT_INDEXINFO_S *pstInfo);

VOID    RCT_InitPthCfgCtxRelease(RCT_INICFG_CTX_S *pstCtx);

ULONG   RCT_InitPthCfgGetSubNums(ULONG ulRctType);

/**************************************************************/
typedef struct tagRctInitTimerBaseConfig
{
    ULONG lPthreadNums;                       /*���ٸ��߳�*/
    LONG aulMapExprEvtfd[RCT_PTHMAXNUMS];     /*��Ҫ���̷߳��͵��ϻ��˿�*/
    LONG aulMapTimrEvtfd[RCT_PTHMAXNUMS];     /*��Ҫ���̷߳��͵Ķ�ʱ���˿�*/

}RCT_INIT_TIMERBASE_CONFIG_S;


VOID RCT_Test_InitPthreadRunSyncMapInfo();



