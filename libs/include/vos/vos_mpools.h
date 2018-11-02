/******************************************************************************

                  版权所有 (C), 2018-2028, 网络有限公司

 ******************************************************************************
  文 件 名   : vos_mpools.h
  版 本 号   : 初稿
  作    者   : 蒋康
  生成日期   : 2018年4月29日
  最近修改   :
  功能描述   : 动态内存池的设计
  函数列表   :
  修改历史   :
  1.日    期   : 2018年4月29日
    作    者   : 蒋康
    修改内容   : 创建文件

******************************************************************************/

/* 默认创建的队列长度*/ 
#define     VOS_LOOKSIDE_NUMS       100

/*最大的队列阀值: 超过阀值丢包*/
#define     VOS_THRED_NUMS          10000

/* 内存池的设计 */
typedef struct VOSMPoolLookSideList
{   
    UINT32           uiEntrySize;        /*该内池的内存大小*/

    VOS_RW_LOCK_S    stPoolListLock;     /*该内存池的锁*/

    VOS_DLIST_NODE_S stPoolListFree;     /*空闲的存放*/

    UINT32           uiNums;             /*当前该池中的数量*/
    
}VOS_PLookSideList, *PVOS_PLookSideList;



LONG    VOS_LookSideListInit(VOS_PLookSideList **ppstLookSideList,UINT32 uiEntrySize);

VOID    VOS_LookSideListUnInit(VOS_PLookSideList *pstLookSideList);

VOID   *VOS_AllcateFromLookSideList(VOS_PLookSideList *pstLookSideList);

VOID    VOS_FreeToLookSideList(VOS_PLookSideList *pstLookSideList, VOID *pvEntry);




