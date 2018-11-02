/******************************************************************************

                  ��Ȩ���� (C), 2017-2020, �������޹�˾

 ******************************************************************************
  �� �� ��   : rct_mgt.h
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2018��4��17��
  ����޸�   :
  ��������   : �˼���Ϣ����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2018��4��17��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/




/**************************************************************/
/************** ���ר�����ں˼�ͨ�ŵ���Ϣ����                ****************/
/**************************************************************/


/*������Ϣ�ڵ�: ע����Ϣ�д���С��Ŀǰ��ֱ��ͨ��Malloc()����ʽ����������*/
typedef struct tagRctMsgQueueNode
{
    VOS_DLIST_NODE_S    stEntry;           /*��Ϣ�ڵ�*/
    RCT_MSG_HEAD_S      stHead;            /*��Ϣͷ��*/
    
    ULONG               ulMsgLen;          /*��Ϣ����*/
    CHAR               *pcMsgData;         /*��Ϣ����*/
    
}RCT_MQUENODE_S, *PRCT_MQUENODE_S;


/*��������Ϣ�����ݼ��ڵ���Ϣ: ֻҪ��������Ƿ�Ϊ0���ͱ�ʾ�ýڵ��Ƿ��Ѿ�����ȫ�ͷ���*/
typedef struct tagRctPeplyLockNode
{
    VOS_DLIST_NODE_S    stEntry;        /*��Ϣ�ڵ�*/
    UINT32              ulLockId;   /*��Ϣ���ID: ����ȷ����ǰ���߳�ID���ڣ�������ڣ�����Ϣ����
                                        ���򣬵��ö�Ӧ����Ϣ�ᵼ�¿�ָ��Dump
                                        0: ��ʾ����Ϣ��>0��ʾ�ظ���Ϣ����Ҫ���*/
    //VOID                *pcLockConn;    /*����ס����Ϣ�����ǽڵ�ָ��,������ϻ��õ������������*/
}RCT_MRLYLOCKNODE_S, *PRCT_MRLYLOCKNODE_S;


/*�������̵߳Ķ���������*/
typedef struct tagRctMsgQueueMgrCtx
{
    /*���̵߳���Ϣ���ն���*/
    VOS_DLIST_NODE_S    stMsgList;          /*���е�ͷ*/
    VOS_RW_LOCK_S       stMsgLock;          /*���е���*/
    ULONG               uiNum;              /*��������Ϣ���ظ���*/

    /*���̵߳�����Ϣ*/
    VOS_DLIST_NODE_S    stCheckRlyList;     /*��鷵�ض��е�ͷ*/
    VOS_RW_LOCK_S       stCheckRlyLock;     /*��鷵�ض��е���*/
    ULONG               uiCheckRlyNum;      /*�ϻ���Ҫ�ͷ��Լ��Ľڵ�*/
    
}RCT_MQUEMGRCTX_S, *PRCT_MQUEMGRCTX_S;

LONG    RCT_Reactor_MgtNormalSend(ULONG ulSrcSubType, ULONG ulDstRctType, ULONG ulDstSubType, ULONG ulDstPthIndex, CHAR *pcData, ULONG ulLen);

LONG    RCT_Reactor_MgtLockySend(            ULONG ulSrcSubType, ULONG ulDstRctType, 
                                             ULONG ulDstSubType, ULONG ulDstPthIndex, 
                                             CHAR *pcData, ULONG ulLen, UINT32 uiLockID);

LONG RCT_Reactor_MgtSendBack(RCT_MSG_HEAD_S            *pstSrcHead,  CHAR *pcData, ULONG ulLen);


VOID    RCT_Reactor_MsgQueLockNodeExpired(VOID *pvConn);


/****************************************************************************/
/*********************************[Mgmt Ops]*********************************/
/*��Ϣ�¼�*/
typedef struct tagRctReactorMgtHandler             RCT_REACTOR_MGT_HANDLER_S;


/*��Ϣ���������*/
struct tagRctReactorMgtHandler
{
    RCT_EVTREACTOR_S                   *pstRctEvtReactor;
    /*ʹ�������¼�ʵ��*/
    RCT_REACTOR_NETEVT_OPT_S            stNetEvtOps;

    LONG                                lMsgEventfd;

    /*�����Ѿ������õĹ�����ָ��, ��Ҫ����Ϣ���еĹ���*/
    PRCT_MQUEMGRCTX_S                   pstMsgQueMgr;    
};


LONG    RCT_Reactor_MgtHandlerCreate(RCT_EVTREACTOR_S *pstRctReactor);

LONG    RCT_Reactor_MgtHandlerRelease(RCT_REACTOR_MGT_HANDLER_S *pstMgtHandler);


