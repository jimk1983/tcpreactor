/******************************************************************************

                  ��Ȩ���� (C), 2015-2025, ����Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : rct_tim.h
  �� �� ��   : ����
  ��    ��   : jimk
  ��������   : 2016��2��18��
  ����޸�   :
  ��������   : ��ʱ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2016��2��18��
    ��    ��   : jimk
    �޸�����   : �����ļ�

******************************************************************************/
#define RCT_TIMER_RECVBUF_SIZE          256
/*500ms*/
#define RCT_SLEEP_UNIT_US               500000

/*��ʱ������: һ�ζ�ʱ��*/
#define RCT_TIMER_TYPE_ONCE             0
/*��ʱ������:ѭ����ʱ��*/
#define RCT_TIMER_TYPE_RECYLE           1

/*ÿ���̵߳Ķ�ʱ��������*/
#define RCT_TIMER_ITEMS_NUM             200


/**************[TimerOps]***********************/

/*��ʱ�¼�*/
typedef struct tagRctReactorTimerHandler           RCT_REACTOR_TIMERHANDLER_S;
typedef struct tagRctReactorTimerOpts               RCT_REACTOR_TIMER_OPT_S;


#define RCT_REACTOR_TIMER_MAX_NUM       200 
#define RCT_REACTOR_TIMER_UDPBUF        256  
#define RCT_REACTOR_TIMEROUT_US         100000  /*100ms*/

typedef VOID (*rtm_reactor_timerops_cb)(VOID *pvhandler);
/*������ʱ��*/
struct tagRctReactorTimerOpts
{
    /*��ʱ���ڵ�*/
    VOS_DLIST_NODE_S          stTimerNode;
    /*TimerType: 0: once����, 1:recycleѭ��*/
    ULONG                     ulTimeType;
    /*��ʱ������ID */
    ULONG                     ulTimerID;
    /*ulTimerOut=2, example: 500ms * 2 = 1s, RCT_SLEEP_UNIT_US=500ms*/
    ULONG                     ulTimerOut;
    /*��ʱ��ʱ���*/
    ULONG                     ulTimeStamp;
    /*��ʱ���ص�����*/
    VOS_CALLBACK_S            stTimercb;
};

/*��ʱ����������ķ�ʽʵ��*/
struct tagRctReactorTimerHandler
{
    /*������������Լ̳�*/
    RCT_EVTREACTOR_S                    *pstRctEvtReactor;
    /*��ʱ������socket�����յ�����*/
    LONG                                 lTimrEventfd;
    /*��ʱ������*/
    VOS_DLIST_NODE_S                     stTimerList;
    /*ʹ�������¼�ʵ�ָô�����*/
    RCT_REACTOR_NETEVT_OPT_S             stNetEvtOps;
    /*��ʱ��ɨ��ע������*/
    RCT_REACTOR_TIMER_OPT_S            **apstTimerOps; 
};


LONG RCT_Timer_EventHandlerCreate(RCT_EVTREACTOR_S *pstRctReactor);
LONG RCT_Timer_EventHandlerRelease(RCT_REACTOR_TIMERHANDLER_S *pstTimerHandler);


LONG RCT_Timer_EventOptsRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_TIMER_OPT_S *pstTimeOps);
LONG RCT_Timer_EventOptsUnRegister(RCT_EVTREACTOR_S *pstRctReactor, RCT_REACTOR_TIMER_OPT_S *pstTimeOps);




