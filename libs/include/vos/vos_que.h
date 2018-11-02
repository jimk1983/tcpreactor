/******************************************************************************

                  版权所有 (C), 2015-2025, 网络科技有限公司

 ******************************************************************************
  文 件 名   : vos_que.h
  版 本 号   : 初稿
  作    者   : jimk
  生成日期   : 2016年5月21日
  最近修改   :
  功能描述   : 简单的环形队列实现
  函数列表   :
  修改历史   :
  1.日    期   : 2016年5月21日
    作    者   : jimk
    修改内容   : 创建文件

******************************************************************************/

/*简单的消息队列设计, 通过UDP触发，该队列设计主要用于核间消息 */
typedef struct tagVosMsgQue
{
    VOS_DLIST_NODE_S     stList;      /*队列的头*/
    VOS_RW_LOCK_S        stLock;      /*队列的锁*/
    UINT32               uiNum;       /*队列内消息挂载个数*/
    UINT32               uiIndex;     /*队列所在的全局线程信息索引, 初始化时确定的索引，便于查找*/
}VOS_MSG_QUE_S;

/*如果内容过大，使用VOS_Malloc()来创建内存，线程间可以访问到，自行发送内存方式*/
#define VOS_MSGQUE_DATA_LEN     1920        /*传递的消息的长度,定义小块内存*/

/*具体传递消息的节点*/
typedef struct tagVosMsgQueNode
{
    VOS_DLIST_NODE_S    stNode;                     
    CHAR                acMsgData[VOS_MSGQUE_DATA_LEN];    /*具体传递的消息*/
}VOS_MSG_QUE_NODE_S;

/*获取队列节点的数据*/
#define VOS_RCTMSGQUENODE_GETDATA(pstMsgNode)       (&pstMsgNode->acMsgData[0])

VOS_MSG_QUE_S *VOS_Rct_MsgQueCreate(UINT32 uiIndex);
VOID VOS_Rct_MSGQue_Release(VOS_MSG_QUE_S *pstMsgQue);

INT32 VOS_Rct_MSGQue_Push(VOS_MSG_QUE_S *pstMsgQue, VOS_MSG_QUE_NODE_S *pstMsgNode);
VOS_MSG_QUE_NODE_S * VOS_Rct_MSGQue_Pop(VOS_MSG_QUE_S *pstMsgQue);

VOS_MSG_QUE_NODE_S *VOS_Rct_MSGQueNode_Create();
VOID VOS_Rct_MSGQueNode_Release(VOS_MSG_QUE_NODE_S *pstQueNode);

BOOL VOS_Rct_MSGQue_IsEmpty(VOS_MSG_QUE_S *pstMsgQue);







