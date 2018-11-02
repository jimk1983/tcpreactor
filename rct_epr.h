/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_epr.h
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��5��26��
  ����޸�   :
  ��������   : 
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��5��26��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/

/*���ջ���*/
#define RCT_EXPIRE_RECVBUF_SIZE         256

/***************[Expire Ops]*************************/

/*�ϻ��¼�*/
typedef struct tagRctReactorExpireHandler          RCT_REACTOR_EXPIREHANDLER_S;
typedef struct tagRctReactorExpireOpts              RCT_REACTOR_EXPIRE_OPT_S;

typedef VOID (*rtm_reactor_expireops_cb)(VOID *pvhandler);

/*�����ϻ��ڵ�: �ϻ���������Լ������ϻ��ص�����*/
struct tagRctReactorExpireOpts
{
    VOS_DLIST_NODE_S            stNode; 
    VOS_CALLBACK_S              stExpirecb;
    ULONG                       ulExpireConfirm;
};

/*�ϻ�������: �ϻ�����*/
struct tagRctReactorExpireHandler
{
    RCT_EVTREACTOR_S                    *pstRctEvtReactor;
    /*Current Max ssl connect node num is 200 for one epollReactor, 
         if >200, change to the hash table instand of the expirelist.*/
    VOS_DLIST_NODE_S                    stExpireList;
    /*Ҳ�ǻ��������¼�*/
    RCT_REACTOR_NETEVT_OPT_S            stNetEvtOps;
    /*�ϻ���������Socketfd*/
    LONG                                lExprEventfd;
};


VOID RCT_Expire_EventHandlerCb(VOID *pvHandler);

LONG RCT_Expire_EventHandlerCreate(RCT_EVTREACTOR_S *pstRctReactor);
LONG RCT_Expire_EventHandlerRelease(RCT_REACTOR_EXPIREHANDLER_S *pstExpireHandler);

LONG RCT_Expire_EventOptsRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_EXPIRE_OPT_S  *pstExpireOps);
LONG RCT_Expire_EventOptsUnRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_EXPIRE_OPT_S  *pstExpireOps);

