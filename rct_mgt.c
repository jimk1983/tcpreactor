/******************************************************************************

                  ��Ȩ���� (C), 2018-2028, �������޹�˾

 ******************************************************************************
  �� �� ��   : rct_mgt.c
  �� �� ��   : ����
  ��    ��   : ����
  ��������   : 2018��4��19��
  ����޸�   :
  ��������   : ��Ϣͨ��
  �����б�   :
              RCT_Reactor_MgtHandlerCreate
              RCT_Reactor_MgtHandlerRelease
  �޸���ʷ   :
  1.��    ��   : 2018��4��19��
    ��    ��   : ����
    �޸�����   : �����ļ�
ע��
    ����һ������Bug, ��Ҫ����һ�ֶ��û����ݽ�����ס�����, ���ν����ع���
******************************************************************************/
#include <vos_pub.h>
#include <rct_pub.h>


/*****************************************************************************
 �� �� ��  : RCT_Reactor_MsgQueLockNodePush
 ��������  : ���ڵ����
 �������  : PRCT_MQUEMGRCTX_S pstQueMgrCtx  
             RCT_MRLYLOCKNODE_S *pstMsgNode  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID RCT_Reactor_MsgQueLockNodePush(PRCT_MQUEMGRCTX_S pstQueMgrCtx, RCT_MRLYLOCKNODE_S *pstMsgNode)
{
    if ( NULL == pstQueMgrCtx )
    {
        return;
    }
    
    VOS_RWLOCK_LOCK(pstQueMgrCtx->stCheckRlyLock);
    VOS_DLIST_ADD_TAIL(&pstQueMgrCtx->stCheckRlyList, &pstMsgNode->stEntry);
    VOS_InterlockedIncrement(&pstQueMgrCtx->uiCheckRlyNum);
    VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);

    VOS_Printf("Lock Push NodeInfo: pstLockMsgNode=%p, lockid=%d", 
            &pstMsgNode->stEntry, pstMsgNode->ulLockId);
    
    return;
}

/*****************************************************************************
 �� �� ��  : RCT_Reactor_MsgQueLockNodePop
 ��������  : ���������ݵĶ���
 �������  : PRCT_MQUEMGRCTX_S pstQueMgrCtx  
 �������  : ��
 �� �� ֵ  : RCT_MRLYLOCKNODE_S
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
RCT_MRLYLOCKNODE_S *RCT_Reactor_MsgQueLockNodePop(PRCT_MQUEMGRCTX_S pstQueMgrCtx)
{
    PVOS_DLIST_NODE_S    thisEntry, nextEntry, ListHead; 
    RCT_MRLYLOCKNODE_S  *pstMsgNode= NULL;
    
    if ( NULL == pstQueMgrCtx )
    {
        return NULL;
    }

    VOS_RWLOCK_LOCK(pstQueMgrCtx->stCheckRlyLock);
    if ( VOS_OK == VOS_Node_IsEmpty(&pstQueMgrCtx->stCheckRlyList) )
    {
        VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);
        return NULL;    
    }
    
    ListHead = &pstQueMgrCtx->stCheckRlyList;

    for(thisEntry = ListHead->next;
        thisEntry != ListHead;
        thisEntry = nextEntry)
    {
        nextEntry = thisEntry->next;
        pstMsgNode = VOS_DLIST_ENTRY(thisEntry, RCT_MRLYLOCKNODE_S, stEntry);
        VOS_Node_Remove(thisEntry);
        VOS_InterlockedDecrement(&pstQueMgrCtx->uiCheckRlyNum);
        break;
    }
    VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);
        
    return pstMsgNode;
}


/*****************************************************************************
 �� �� ��  : RCT_Reactor_MsgQueLockNodeExpired
 ��������  : ���ݽڵ��ϻ��Ķ��������Լ��Ŀɿ��������д���
 �������  : VOID *pvConn   --Ŀǰ��ܣ�������������ݱ�����conn�Ľڵ㣬��Ϊ�ϻ����õ�������ڵ㣬�������ϻ���
                           
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID RCT_Reactor_MsgQueLockNodeExpired(VOID *pvConn)
{
    PVOS_DLIST_NODE_S           thisEntry, nextEntry, ListHead; 
    RCT_COMMUTEINFO_S           stSrcEvntInfo   = {0};
    RCT_MRLYLOCKNODE_S         *pstMsgNode      = NULL;
    ULONG                       ulLockRplyId = 0;

    ulLockRplyId = (ULONG)pvConn;
    
    /*��ȡ��ǰ�̵߳���Ϣ*/
    if ( VOS_ERR == RCT_Task_ReactorGetCommunInfo(&stSrcEvntInfo) )
    {
        DbgRctModuleError("Get our pthread communication map info failed!");
        return;
    }

    if ( NULL == stSrcEvntInfo.pstQueMgrCtx )
    {
        return;
    }

    VOS_RWLOCK_LOCK(stSrcEvntInfo.pstQueMgrCtx->stCheckRlyLock);
    if ( VOS_OK == VOS_Node_IsEmpty(&stSrcEvntInfo.pstQueMgrCtx->stCheckRlyList) )
    {
        VOS_RWLOCK_UNLOCK(stSrcEvntInfo.pstQueMgrCtx->stCheckRlyLock);
        return;    
    }
    
    ListHead = &stSrcEvntInfo.pstQueMgrCtx->stCheckRlyList;

    for(thisEntry = ListHead->next;
        thisEntry != ListHead;
        thisEntry = nextEntry)
    {
        nextEntry = thisEntry->next;
        pstMsgNode = VOS_DLIST_ENTRY(thisEntry, RCT_MRLYLOCKNODE_S, stEntry);
        if ( ulLockRplyId == pstMsgNode->ulLockId )
        {
            VOS_Printf("RCT_Reactor_MsgQueLockNodeExpired OK!, ulLockId=%08x", pstMsgNode->ulLockId);
            VOS_Node_Remove(thisEntry);
            VOS_InterlockedDecrement(&stSrcEvntInfo.pstQueMgrCtx->uiCheckRlyNum);
            break;
        }
    }
    VOS_RWLOCK_UNLOCK(stSrcEvntInfo.pstQueMgrCtx->stCheckRlyLock);
    
    return;
}


/*****************************************************************************
 �� �� ��  : RCT_Reactor_MsgQueLockNodeIsExist
 ��������  : �ýڵ��Ƿ���
 �������  : VOID *pvConn  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
BOOL RCT_Reactor_MsgQueLockNodeCheckAndExist(PRCT_MQUEMGRCTX_S pstQueMgrCtx, UINT32 ulLockyId)
{
    PVOS_DLIST_NODE_S    thisEntry, nextEntry, ListHead; 
    RCT_MRLYLOCKNODE_S  *pstLockNode      = NULL;
    
    if ( NULL == pstQueMgrCtx
        || 0 == ulLockyId )
    {
        return VOS_FALSE;
    }

    VOS_RWLOCK_LOCK(pstQueMgrCtx->stCheckRlyLock);
    if ( VOS_OK == VOS_Node_IsEmpty(&pstQueMgrCtx->stCheckRlyList) )
    {
        VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);
        return VOS_FALSE;    
    }
    
    ListHead = &pstQueMgrCtx->stCheckRlyList;

    for(thisEntry = ListHead->next;
        thisEntry != ListHead;
        thisEntry = nextEntry)
    {
        nextEntry = thisEntry->next;
        pstLockNode = VOS_DLIST_ENTRY(thisEntry, RCT_MRLYLOCKNODE_S, stEntry);

        /*ID���ϼ���ʾ����ڵ㻹���ڣ���ôɾ�����ڵ㣬Ȼ�����Ϣ�������*/
        if ( ulLockyId == pstLockNode->ulLockId )
        {
            VOS_Node_Remove(thisEntry);
            VOS_InterlockedDecrement(&pstQueMgrCtx->uiCheckRlyNum);
            VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);
            return VOS_TRUE;
        }
    }
        
    VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stCheckRlyLock);
    
    return VOS_FALSE;
}


/*****************************************************************************
 �� �� ��  : RCT_Reactor_MsgQueNodePush
 ��������  : ���ڵ����ͽ���
 �������  : PRCT_MQUEMGRCTX_S pstMegQueMgr  
             RCT_MQUENODE_S *pstMsgNode      
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
VOID RCT_Reactor_MsgQueNodePush(PRCT_MQUEMGRCTX_S pstQueMgrCtx, RCT_MQUENODE_S *pstMsgNode)
{
    if ( NULL == pstQueMgrCtx )
    {
        return;
    }
    
    VOS_RWLOCK_LOCK(pstQueMgrCtx->stMsgLock);
    VOS_DLIST_ADD_TAIL(&pstQueMgrCtx->stMsgList, &pstMsgNode->stEntry);
    VOS_InterlockedIncrement(&pstQueMgrCtx->uiNum);
    VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stMsgLock);
    return;
}


/*****************************************************************************
 �� �� ��  : RCT_Reactor_MsgQueNodePop
 ��������  : ���ڵ��ó�
 �������  : PRCT_MQUEMGRCTX_S pstMegQueMgr  
 �������  : ��
 �� �� ֵ  : RCT_MQUENODE_S
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
RCT_MQUENODE_S *RCT_Reactor_MsgQueNodePop(PRCT_MQUEMGRCTX_S pstQueMgrCtx)
{
    PVOS_DLIST_NODE_S  thisEntry, nextEntry, ListHead; 
    RCT_MQUENODE_S    *pstMsgNode= NULL;
    
    if ( NULL == pstQueMgrCtx )
    {
        return NULL;
    }

    VOS_RWLOCK_LOCK(pstQueMgrCtx->stMsgLock);
    if ( VOS_OK == VOS_Node_IsEmpty(&pstQueMgrCtx->stMsgList) )
    {
        VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stMsgLock);
        return NULL;    
    }
    
    ListHead = &pstQueMgrCtx->stMsgList;

    for(thisEntry = ListHead->next;
        thisEntry != ListHead;
        thisEntry = nextEntry)
    {
        nextEntry = thisEntry->next;
        pstMsgNode = VOS_DLIST_ENTRY(thisEntry, RCT_MQUENODE_S, stEntry);
        VOS_Node_Remove(thisEntry);
        VOS_InterlockedDecrement(&pstQueMgrCtx->uiNum);
        break;
    }
    VOS_RWLOCK_UNLOCK(pstQueMgrCtx->stMsgLock);
        
    return pstMsgNode;
}


/*****************************************************************************
 �� �� ��: RCT_Reactor_MgtNormalSend
 ��������  :   һ������Ϣ�����践�صĽ���ģ�������Ϣ���ڴ��ɶԷ��ͷ�
 �������  :   ULONG ulSrcSubType  --����ӿ����ڶԷ��յ���Ϣ�󣬿���ֱ�ӻ�ȡSSL�µ�SWM\NEM\TCM֮��ģ����㷵��
                                 �ṩ������ӿڣ��������û����з�װ
           ULONG ulRctType     --Ŀ�ĵ�RctType
           ULONG ulSubType     --Ŀ�ĵ�SubType
           ULONG ulDstPthIndex --����SSL���֣��ж���̵߳ģ���Ҫ��֪Ŀ���߳�Nums
           CHAR *pcData        --���͵���Ϣָ�룬����ҪVOS_Malloc()����
           ULONG ulLen         --����Ϣ�ĳ���
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���
ע�⣺��Ϊ�Ƿǿ������첽���ģʽ��
      ������е�pcData, ����Ҫ��̬��VOS_Malloc()������, �����þֲ�����������

*****************************************************************************/
LONG RCT_Reactor_MgtNormalSend(ULONG ulSrcSubType,ULONG ulDstRctType,ULONG ulDstSubType,ULONG ulDstPthIndex,CHAR *pcData,ULONG ulLen)
{
    PRCT_MQUEMGRCTX_S   pstQueMgrCtx    = NULL;
    RCT_COMMUTEINFO_S   stDstEvntInfo   = {0};
    PRCT_MQUENODE_S     pstMgtNode      = NULL;
    RCT_INDEXINFO_S     stSrcIndexInfo  = {0};
    ULONG               uiVal           = 1;
    LONG                lRet            = 0;

    /*��׼��������Ϣ�����͵���Ӧ����Ϣ������*/
    /*����ҵ�������ͺ�ҵ������߳������ţ�����ȡͨ�ž�����Ϣ*/
    if ( VOS_ERR == RCT_TaskPthGetCommunMapInfo(ulDstRctType, ulDstPthIndex, &stDstEvntInfo)  )
    {
        DbgRctModuleError("Get communication map info failed!");
        return VOS_ERR;
    }
    
    /*��ȡ����Ŀ����Ϣ���й�����*/
    pstQueMgrCtx = stDstEvntInfo.pstQueMgrCtx;
    if ( NULL == pstQueMgrCtx )
    {
        DbgRctModuleError("Dst commuicate quemgr has NULL!");
        return VOS_ERR;
    }

    /*��ȡ�Լ����߳�������Ϣ*/
    if ( VOS_ERR == RCT_Task_ReactorGetPthIndexInfo(&stSrcIndexInfo)  )
    {
        DbgRctModuleError("Get pthread index info failed!");
        return VOS_ERR;
    }

    /*ֱ����VOS_Malloc()���ɣ���Ϊ���ڴ��������Ӱ���ٶ�*/
    pstMgtNode= (RCT_MQUENODE_S *)VOS_Malloc(RCT_MID_SID_EVT, sizeof(RCT_MQUENODE_S));
    if ( NULL == pstMgtNode )
    {
        DbgRctModuleError("mgt node malloc failed!");
        return VOS_ERR;
    }
    VOS_Mem_Zero((CHAR *)pstMgtNode, sizeof(RCT_MQUENODE_S));

    VOS_DLIST_INIT(&pstMgtNode->stEntry);

    /*��д��Ϣͷ����Ϣ*/
    //SRC
    pstMgtNode->stHead.ulSrcRctSubType  = ulSrcSubType;                 /*SSL�µ�SWM\NEM\TCM��������, �����û��Լ���д*/
    pstMgtNode->stHead.ulSrcRctType     = stSrcIndexInfo.ulRctType;     /*������:CLI\SSL\AAA\...*/
    pstMgtNode->stHead.ulSrcPthIndex    = stSrcIndexInfo.ulPthSubIndex; /*SSL����̣߳��߳�����*/
    pstMgtNode->stHead.ulSrcMsgEventFd  = stSrcIndexInfo.ulMsgEventFd;  /*��Ϣ*/
    //DST
    pstMgtNode->stHead.ulDstRctType     = ulDstRctType;
    pstMgtNode->stHead.ulDstRctSubType  = ulDstSubType;
    pstMgtNode->stHead.ulDstPthIndex    = ulDstPthIndex;
    /*����Ҫ: �����Need����ô��Ҫ���������ǽ��뵽���û����ݵ��ж�*/
    //pstMgtNode->stHead.ulLockRplyFlag   = RCT_MSGLOCKPLY_NONEED;
    /*����Back��Ϣ*/
    pstMgtNode->stHead.ulBackFlag       = 0;
    
    pstMgtNode->pcMsgData   = pcData;
    pstMgtNode->ulMsgLen    = ulLen;

    RCT_Reactor_MsgQueNodePush(pstQueMgrCtx, pstMgtNode);

    /*����ͨ�ž���, ������Ӧ����Ϣ�߳�*/
    lRet = VOS_EventfdWrite(stDstEvntInfo.ulEventFdMsg,  &uiVal, sizeof(ULONG));
    if( lRet < 0)
    {
        DbgRctModuleError("message action eventfd=[%d] failed!", stDstEvntInfo.ulEventFdMsg);
    }

    //VOS_Printf("NormalSend: [%d:%d:%d] --> [%d:%d:%d]-%d ,QueMgrCtx=%p,MsgNode=%p,",
    //    stSrcIndexInfo.ulRctType, ulSrcSubType, stSrcIndexInfo.ulPthSubIndex, 
    //    ulDstRctType, ulDstSubType, ulDstPthIndex, stDstEvntInfo.ulEventFdMsg, pstQueMgrCtx, pstMgtNode);
    
    return VOS_OK;
}


/*****************************************************************************
 �� �� ��  : RCT_Reactor_MgtLockyDataSend
 ��������  :    ������������,
            �ṩ��Ϣ��Ҫ���صģ�������������Ҫ���������Ƿ�ɿ�
            ��Ϊ������ɿ�����ô������⣬�������첽ģʽ���޷��жϳ��ýڵ��Ƿ��Ѿ����ͷ�
            ����ͨ��һ��lock�Ļ��ƣ�����֤����첽ģʽ��ܵĿɿ���
 �������  : ULONG ulSrcSubType   
             ULONG ulDstRctType   
             ULONG ulDstSubType   
             ULONG ulDstPthIndex  
             CHAR *pcData         
             ULONG ulLen          
             *pcLockConn          --���������û����ݣ�һ����һ���ڵ���Ϣ
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
LONG RCT_Reactor_MgtLockySend(ULONG ulSrcSubType, ULONG ulDstRctType, ULONG ulDstSubType, 
                                          ULONG ulDstPthIndex, CHAR *pcData, ULONG ulLen, UINT32 uiLockID)
{
    PRCT_MQUEMGRCTX_S   pstQueMgrCtx    = NULL;
    RCT_COMMUTEINFO_S   stDstEvntInfo   = {0};
    PRCT_MQUENODE_S     pstMgtNode      = NULL;
    PRCT_MRLYLOCKNODE_S pstLockNode     = NULL;
    RCT_INDEXINFO_S     stSrcIndexInfo  = {0};
    ULONG               uiVal           = 1;
    LONG                lRet            = 0;

    /*��׼��������Ϣ�����͵���Ӧ����Ϣ������*/
    /*����ҵ�������ͺ�ҵ������߳������ţ�����ȡͨ�ž�����Ϣ*/
    if ( VOS_ERR == RCT_TaskPthGetCommunMapInfo(ulDstRctType, ulDstPthIndex, &stDstEvntInfo)  )
    {
        DbgRctModuleError("Get communication map info failed!");
        return VOS_ERR;
    }
    
    /*��ȡ����Ŀ����Ϣ���й�����*/
    pstQueMgrCtx = stDstEvntInfo.pstQueMgrCtx;
    if ( NULL == pstQueMgrCtx )
    {
        DbgRctModuleError("Dst commuicate quemgr has NULL!");
        return VOS_ERR;
    }

    /*��ȡ�Լ����߳�������Ϣ*/
    if ( VOS_ERR == RCT_Task_ReactorGetPthIndexInfo(&stSrcIndexInfo)  )
    {
        DbgRctModuleError("Get pthread index info failed!");
        return VOS_ERR;
    }

    /*ֱ����VOS_Malloc()���ɣ���Ϊ���ڴ��������Ӱ���ٶ�*/
    pstMgtNode= (RCT_MQUENODE_S *)VOS_Malloc(RCT_MID_SID_EVT, sizeof(RCT_MQUENODE_S));
    if ( NULL == pstMgtNode )
    {
        DbgRctModuleError("mgt node malloc failed!");
        return VOS_ERR;
    }
    VOS_Mem_Zero((CHAR *)pstMgtNode, sizeof(RCT_MQUENODE_S));

    VOS_DLIST_INIT(&pstMgtNode->stEntry);

    /*��д��Ϣͷ����Ϣ*/
    //SRC
    pstMgtNode->stHead.ulSrcRctType     = stSrcIndexInfo.ulRctType;     /*������:CLI\SSL\AAA\...*/
    pstMgtNode->stHead.ulSrcRctSubType  = ulSrcSubType;                 /*SSL�µ�SWM\NEM\TCM��������*/
    pstMgtNode->stHead.ulSrcPthIndex    = stSrcIndexInfo.ulPthSubIndex; /*SSL����̣߳��߳�����*/
    pstMgtNode->stHead.ulSrcMsgEventFd  = stSrcIndexInfo.ulMsgEventFd;  /*������Ϣ����*/
    
    //DST
    pstMgtNode->stHead.ulDstRctType     = ulDstRctType;
    pstMgtNode->stHead.ulDstRctSubType  = ulDstSubType;
    pstMgtNode->stHead.ulDstPthIndex    = ulDstPthIndex;

    pstMgtNode->pcMsgData   = pcData;
    pstMgtNode->ulMsgLen    = ulLen;

    /*����Back��Ϣ*/
    pstMgtNode->stHead.ulBackFlag       = 0;
    
    //pstMgtNode->stHead.ulLockRplyFlag   = RCT_MSGLOCKPLY_NEED;
    /*��Ҫ�����Ľڵ㣬���ڷ��ص�ʱ����*/
    pstMgtNode->stHead.ulLockRplyId     = uiLockID;

    /*���͵��Է��Ķ�����*/
    RCT_Reactor_MsgQueNodePush(pstQueMgrCtx, pstMgtNode);
    

    /*****************************************************************************/
    /*****************************�����ڱ�����************************************/

    /*�û������Ľڵ�*/
    pstLockNode= (RCT_MRLYLOCKNODE_S *)VOS_Malloc(RCT_MID_SID_EVT, sizeof(RCT_MRLYLOCKNODE_S));
    if ( NULL == pstLockNode )
    {
        DbgRctModuleError("mgt node malloc failed!");
        return VOS_ERR;
    }
    
    VOS_Mem_Zero((CHAR *)pstLockNode, sizeof(RCT_MRLYLOCKNODE_S));

    VOS_DLIST_INIT(&pstLockNode->stEntry);

    //pstLockNode->pcLockConn       = pcLockConn;
    pstLockNode->ulLockId     = uiLockID;

    /*����ڱ�����*/
    RCT_Reactor_MsgQueLockNodePush(g_th_pstReactor->pstPthCfgCtx->stEvtInfo.pstQueMgrCtx, pstLockNode);    
    
    /*����ͨ�ž���, ������Ӧ����Ϣ�߳�*/
    lRet = VOS_EventfdWrite(stDstEvntInfo.ulEventFdMsg,  &uiVal, sizeof(ULONG));
    if( lRet < 0)
    {
        DbgRctModuleError("message action eventfd=[%d] failed!", stDstEvntInfo.ulEventFdMsg);
    }
    
    return VOS_OK;
}


/*****************************************************************************
�� �� ��  : RCT_Reactor_MgtNormalSendBack
��������  : ����Ϣ����ԭ�ȵ�ͷ����ԭ·����
�������  : RCT_MSG_HEAD_S            *pstBackHead  
           CHAR *pcData                            
           ULONG ulLen                             
           VOID *pcLockConn                        
�������  : ��
�� �� ֵ  : 
���ú���  : 
��������  : 

�޸���ʷ      :
1.��    ��   : 2018��6��5��
  ��    ��   : jimk
  �޸�����   : �����ɺ���

*****************************************************************************/
LONG RCT_Reactor_MgtSendBack(RCT_MSG_HEAD_S            *pstHead, CHAR *pcData, ULONG ulLen)
{
  PRCT_MQUEMGRCTX_S   pstQueMgrCtx    = NULL;
  RCT_COMMUTEINFO_S   stDstEvntInfo   = {0};
  PRCT_MQUENODE_S     pstMgtNode      = NULL;
  ULONG               uiVal           = 1;
  LONG                lRet            = 0;

  /*��׼��������Ϣ�����͵���Ӧ����Ϣ������*/
  /*����ҵ�������ͺ�ҵ������߳������ţ�����ȡͨ�ž�����Ϣ*/
  if ( VOS_ERR == RCT_TaskPthGetCommunMapInfo(pstHead->ulSrcRctType, pstHead->ulSrcPthIndex, &stDstEvntInfo)  )
  {
      DbgRctModuleError("Get communication map info failed!");
      return VOS_ERR;
  }
  
  /*��ȡ����Ŀ����Ϣ���й�����*/
  pstQueMgrCtx = stDstEvntInfo.pstQueMgrCtx;
  if ( NULL == pstQueMgrCtx )
  {
      DbgRctModuleError("Dst commuicate quemgr has NULL!");
      return VOS_ERR;
  }

  /*ֱ����VOS_Malloc()���ɣ���Ϊ���ڴ��������Ӱ���ٶ�*/
  pstMgtNode= (RCT_MQUENODE_S *)VOS_Malloc(RCT_MID_SID_EVT, sizeof(RCT_MQUENODE_S));
  if ( NULL == pstMgtNode )
  {
      DbgRctModuleError("mgt node malloc failed!");
      return VOS_ERR;
  }
  VOS_Mem_Zero((CHAR *)pstMgtNode, sizeof(RCT_MQUENODE_S));

  VOS_DLIST_INIT(&pstMgtNode->stEntry);

  /*��д��Ϣͷ����Ϣ*/
  //SRC
  pstMgtNode->stHead.ulSrcRctType     = pstHead->ulDstRctType;     /*������:CLI\SSL\AAA\...*/
  pstMgtNode->stHead.ulSrcRctSubType  = pstHead->ulDstRctSubType;  /*SSL�µ�SWM\NEM\TCM��������*/
  pstMgtNode->stHead.ulSrcPthIndex    = pstHead->ulDstPthIndex;    /*SSL����̣߳��߳�����*/
  
  //DST
  pstMgtNode->stHead.ulDstRctType     = pstHead->ulSrcRctType;
  pstMgtNode->stHead.ulDstRctSubType  = pstHead->ulSrcRctSubType;
  pstMgtNode->stHead.ulDstPthIndex    = pstHead->ulSrcPthIndex;

  pstMgtNode->pcMsgData   = pcData;
  pstMgtNode->ulMsgLen    = ulLen;

  /*****************************************************************************/
  //pstMgtNode->stHead.ulLockRplyFlag   = RCT_MSGLOCKPLY_NEED;
  pstMgtNode->stHead.ulLockRplyId     = pstHead->ulLockRplyId;
  pstMgtNode->stHead.ulBackFlag       = 1;

  RCT_Reactor_MsgQueNodePush(pstQueMgrCtx, pstMgtNode);

  if ( stDstEvntInfo.ulEventFdMsg != pstHead->ulSrcMsgEventFd  )
  {
      /*��ʾ��Ϣ��ID�������⣬ͨ�ž������Ϣ����*/
      DbgRctModuleError("rct reactor message eventfd=[%d] not equal the srcmsgeventfd=[%d]!", 
      stDstEvntInfo.ulEventFdMsg, pstHead->ulSrcMsgEventFd);
  }
  
  /*����ͨ�ž���, ������Ӧ����Ϣ�߳�*/
  lRet = VOS_EventfdWrite(stDstEvntInfo.ulEventFdMsg,  &uiVal, sizeof(ULONG));
  if( lRet < 0)
  {
      DbgRctModuleError("message action eventfd=[%d] failed!", stDstEvntInfo.ulEventFdMsg);
  }
  
  return VOS_OK;
}

/*****************************************************************************
 �� �� ��  :    RCT_Reactor_MessageMainHandler
 ��������  :     ��ȡ������Ϣ���д���
 �������  :     PRCT_MQUEMGRCTX_S  pstQueMgrCtx --������        
             RCT_MQUENODE_S     *pstMsgNode  --ȡ������Ϣ
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
LONG RCT_Reactor_MessageMainHandler(PRCT_MQUEMGRCTX_S pstQueMgrCtx, RCT_MQUENODE_S              *pstMsgNode)
{
    ULONG       ulDropFlag = VOS_FALSE;
    
    if ( NULL== pstQueMgrCtx
        || NULL == pstMsgNode
        || NULL == pstMsgNode->pcMsgData )
    {
        DbgRctModuleError("param error");
        return VOS_ERR;
    }

    /*�鿴��Ϣ�Ƿ�Ϊ����ʱ����Ҫ���ģ��������ݵ���Ϣ, ��Ϊ0����ʾ����������Ϣ*/
    if ( pstMsgNode->stHead.ulBackFlag == 1 && pstMsgNode->stHead.ulLockRplyId != 0 )
    {
        /*������Ϣ����������*/
        if ( VOS_FALSE == RCT_Reactor_MsgQueLockNodeCheckAndExist(pstQueMgrCtx, pstMsgNode->stHead.ulLockRplyId)  )
        {
            /*������ʱ�ڵ��Ѿ����û��ϻ����ˣ�����Ϣ����*/
            ulDropFlag = VOS_TRUE;
        }    
    }
    
    /*����Ϣ����, �����д�����, �������Ҳ�㴦��ɹ�*/
    if ( VOS_TRUE == ulDropFlag  )
    {
        DbgRctModuleError("message drop handle!");
        VOS_Free((CHAR *)pstMsgNode->pcMsgData);
            
        /*�ɱ��߳��ͷŵ�����Ϣ�ڵ�*/
        VOS_Free((CHAR *)pstMsgNode);
        return VOS_OK;
    }

    /*������Ϣ������ע���ҵ��ص�����*/
    if ( VOS_ERR == RCT_TaskArrayBizMsgHandlerRunCall(pstMsgNode) )
    {
        DbgRctModuleError("message drop handle!");
        VOS_Free((CHAR *)pstMsgNode->pcMsgData);
        /*�ɱ��߳��ͷŵ�����Ϣ�ڵ�*/
        VOS_Free((CHAR *)pstMsgNode);
        return VOS_ERR;
    }
    
    VOS_Free((CHAR *)pstMsgNode->pcMsgData);
        
    /*�ɱ��߳��ͷŵ�����Ϣ�ڵ�*/
    VOS_Free((CHAR *)pstMsgNode);
        
    return VOS_OK;
}


/*****************************************************************************
 �� �� ��: RCT_Reactor_MgtHandlerCb
 ��������  :    ���̵߳�Ԫ����Ϣ�����ص�����
 �������  :    VOID *pvHandler            ---���õ���Ϣ�ص�
 �������  :    ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
LONG RCT_Reactor_MgtHandlerCb(VOID *pvHandler)
{
    RCT_REACTOR_MGT_HANDLER_S   *pstMgtHandler  = NULL;
    RCT_MQUENODE_S              *pstMsgNode     = NULL;
    LONG                        lRet            = 0;
    ULONG                       uiVal           = 0;
    RCT_COMMUTEINFO_S           stSrcEvntInfo   = {0};
    
    pstMgtHandler = (RCT_REACTOR_MGT_HANDLER_S *)pvHandler;

    /*��ȡ������������*/
    lRet = VOS_EventfdRead(pstMgtHandler->lMsgEventfd, &uiVal, sizeof(ULONG));
    if( VOS_ERR == lRet)
    {
        DbgRctModuleError("Mgt: event fd:[%d] read error!ret=%d", pstMgtHandler->lMsgEventfd, lRet);
        return VOS_ERR;
    }
    
    /*��ȡ��ǰ�̵߳���Ϣ*/
    if ( VOS_ERR == RCT_Task_ReactorGetCommunInfo(&stSrcEvntInfo) )
    {
        DbgRctModuleError("Get our pthread communication map info failed!");
        return VOS_ERR;
    }

    //VOS_Printf("NormalRecv: Src:%d, pid=[%08x], QueMgrCtx=%p ",
    //   pstMgtHandler->lMsgEventfd,  stSrcEvntInfo.ulPid, stSrcEvntInfo.pstQueMgrCtx);

    if ( NULL == stSrcEvntInfo.pstQueMgrCtx )
    {
        DbgRctModuleError("Get our pthread que mgr ctx failed!");
        return VOS_ERR;
    }

    /*ѭ���������������Ϣ*/
    while(1)
    {
        /*�����ж��Ƿ�Ϊ��*/
        VOS_RWLOCK_LOCK(stSrcEvntInfo.pstQueMgrCtx->stMsgLock);
        if ( VOS_OK == VOS_Node_IsEmpty(&stSrcEvntInfo.pstQueMgrCtx->stMsgList) )
        {
            VOS_RWLOCK_UNLOCK(stSrcEvntInfo.pstQueMgrCtx->stMsgLock);   
            break;
        }
        VOS_RWLOCK_UNLOCK(stSrcEvntInfo.pstQueMgrCtx->stMsgLock);  

        /*ȡ����Ϣ���д���, �����Ϣ�ڵ��Ѿ������ˣ����ڶ�������*/
        pstMsgNode = RCT_Reactor_MsgQueNodePop(stSrcEvntInfo.pstQueMgrCtx);

        //VOS_Printf("NormalRecv: RCT_Reactor_MsgQueNodePop=%p ",pstMsgNode);
        if (VOS_ERR == RCT_Reactor_MessageMainHandler(stSrcEvntInfo.pstQueMgrCtx, pstMsgNode))
        {
            /*��������ˣ�Ҳ�˳�*/
            return VOS_ERR;
        }
    }
    
    return VOS_OK;
}





/*****************************************************************************
 �� �� ��  : RCT_Reactor_MgtHandlerCreate
 ��������  : �˼���Ϣ������
 �������  : RCT_EVTREACTOR_S *pstRctReactor  
             ULONG ulMgtPort                  
             VOS_MSG_QUE_S  *pstMgtQue        
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
*****************************************************************************/
LONG RCT_Reactor_MgtHandlerCreate(RCT_EVTREACTOR_S *pstRctReactor)
{
   
    RCT_REACTOR_MGT_HANDLER_S   *pstMgtHandler  = NULL;
    
    if ( NULL == pstRctReactor 
        || NULL == pstRctReactor->pstPthCfgCtx )
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    VOS_Mem_Zero((CHAR *)(&pstRctReactor->stMgtHandler), sizeof(RCT_REACTOR_MGT_HANDLER_S));
    
    pstMgtHandler = (&pstRctReactor->stMgtHandler);
    
    pstMgtHandler->pstRctEvtReactor = pstRctReactor;
    
    /*��֮ǰ�����õ��������ݱ��棬���ڲ�������Ϣ���е���Ϣ
      ͨ��ͨ�ž���Ҳ���Ի�ȡ*/
    pstMgtHandler->pstMsgQueMgr     = pstRctReactor->pstPthCfgCtx->stEvtInfo.pstQueMgrCtx;

    pstMgtHandler->lMsgEventfd = VOS_EventfdCreate(0);
    if ( VOS_ERR ==  pstMgtHandler->lMsgEventfd )
    {
        DbgRctModuleError("eventfd create error!");
        return VOS_ERR;
    }

    /*��¼��ͨ�ž���*/
    pstRctReactor->pstPthCfgCtx->stEvtInfo.ulEventFdMsg     = pstMgtHandler->lMsgEventfd;
    /*��¼����Ϣ����*/
    pstRctReactor->pstPthCfgCtx->stIndexInfo.ulMsgEventFd   = pstMgtHandler->lMsgEventfd;

    /*Init MgtHandler NetEvent */
    RCT_REACTOR_NETEVTOPTS_INIT(
                &pstMgtHandler->stNetEvtOps,
                pstMgtHandler->lMsgEventfd,
                VOS_EPOLL_MASK_POLLIN,
                RCT_Reactor_MgtHandlerCb,
                NULL,
                pstMgtHandler
                );

    /*register the network epoll events*/
    if(VOS_ERR == RCT_Net_EventOptsRegister(pstRctReactor, &pstMgtHandler->stNetEvtOps))
    {
        DbgRctModuleError("RCT_Net_EventOptsRegister error!");
        VOS_EventfdClose(pstMgtHandler->lMsgEventfd);
        return VOS_ERR;
    }
    
    DbgRctModuleEvent("Rct msg Handler create OK!");
    
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : RCT_Reactor_MgtHandlerRelease
 ��������  : �ͷź˼���Ϣ������
 �������  : RCT_REACTOR_MGT_HANDLER_S *pstMgtHandler  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��4��20��
    ��    ��   : jimk
    �޸�����   : �����ɺ���

*****************************************************************************/
LONG RCT_Reactor_MgtHandlerRelease(RCT_REACTOR_MGT_HANDLER_S *pstMgtHandler)
{
    if(NULL == pstMgtHandler)
    {
        DbgRctModuleError("param error!");
        return VOS_ERR;
    }
    
    (VOID)RCT_Net_EventOptsUnRegister(pstMgtHandler->pstRctEvtReactor, &pstMgtHandler->stNetEvtOps);
    
    VOS_EventfdClose(pstMgtHandler->lMsgEventfd);
    
    return VOS_OK;
}
