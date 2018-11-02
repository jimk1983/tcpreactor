/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_pevt.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年11月24日
  最近修改   :
  功能描述   : 仿照EPOLL的事件队列通知机制，不是socket，而是事件方式
  函数列表   :
  修改历史   :
  1.日    期   : 2016年11月24日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/

/*该队列名称长度*/
#define VOS_PTHEVT_NAME_LEN              32

/*最多创建的队列数量*/
#define VOS_PTHEVT_MAX_NUMS             256

/*事件队列单次处理的最大数量*/
#define VOS_PTHEVT_DPTCH_NUMS         64

/*写入事件队列，并通知*/
typedef LONG (*VOS_PTHEVT_WRITE)(UINT32 uiQueID, UINT32 uiEvtype, VOID *pcData);

/*读取队列链表*/
typedef LONG (*VOS_PTHEVT_READ)(UINT32 uiQueID, VOS_DLIST_NODE_S *pstEvts);

#if VOS_PLAT_WIN
typedef  HANDLE      VOS_PTHEVT_T;
#else 
typedef  UINT32       VOS_PTHEVT_T;
#endif


/*线程之间的消息通信队列，事件通知方式
该设计在客户端尤为重要*/
typedef struct tagVosPthEvtQue
{
    CHAR        acQueName[VOS_PTHEVT_NAME_LEN];     /*该队列的名称*/
    UINT32     uiQueID;                                               /*该队列的ID, 也即数组索引*/
    UINT32     uiFreeFlag;                                            /*队列空闲标记,用于队列的读写确认
                                                                                 *只有当为0时候，才可以写入*/
    UINT32     uiEventNums;                                        /*当前队列内部事件的数量，
                                                                                 *最大不能超过VOS_PTHEVT_DPTCH_NUMS(64)*/
    VOS_PTHEVT_T         stEventHandle;                      /*该队列的时间句柄*/
                                                                                 
    VOS_DLIST_NODE_S  stEvtList;                               /*事件队列中的事件链表*/
                                                                                 
    VOS_PTHEVT_WRITE pfEvtWrite;                            /*事件通知*/
                                                                                 
    VOS_PTHEVT_READ   pfEvtRead;                            /*事件读取*/
                                                                                 
}VOS_PTHEVT_QUE_S;



/*************************用户根据如下事件类型来注册相应的处理函数**************************************/
#define VOS_PTHEVT_TYPE_PACKIN 0x00000001              /*数据包事件*/
#define VOS_PTHEVT_TYPE_TIMER  0x00000002              /*定时器事件*/

/*如下对应的数组也要修改*/
typedef enum
{
    VOS_PTHEVT_TYPE_INDEX_PACKIN =0,
    VOS_PTHEVT_TYPE_INDEX_TIMER,

    
}VOS_PTHEVT_TYPE_E;

    
/*每个链表的事件节点*/
typedef struct tagVosPthEvtNode
{
    VOS_DLIST_NODE_S stNode;            /*链表节点*/
    
    UINT32          uiEvttype;                  /*事件类型*/

    VOID*           pvData;                     /*用户数据*/
    
}VOS_PTHEVT_NODE_S;


